#include "tcpmgr.h"

TCPMgr::TCPMgr() : _host(""), _port(0), _b_recv_pending(false), _message_id(0),
    _message_len(0)
{
    connect(&_socket, &QTcpSocket::connected, [&]() {
        qDebug() << "Connected to server!";

        emit sign_conn_success(true);
    });

    connect(&_socket, &QTcpSocket::readyRead, [&]() {
        // 当存在数据可读的时候，读取所有数据
        // 读取所有数据并追加到缓冲区
        _buffer.append(_socket.readAll());

        QDataStream stream(&_buffer, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_0); // 设置版本

        forever {
            // 解析头部
            if (!_b_recv_pending) {
                // 当sizeof 里面是变量名的时候可以省略括号， 若是类型名则需要加括号
                // 检查缓冲区中的数据是否足够解析出一个消息头， 消息ID+消息长度
                // 数据不够，等待更多的数据
                if (_buffer.size() <static_cast<int>(sizeof(quint16) * 2) )
                    return ;

                // 预读消息ID和消息长度，但不从缓冲区中移除
                stream >> _message_id >> _message_len;

                // 将buffer 中的前四个字节移除
                // QByteArray.mid() 从某个位置截取某段字符串
                _buffer = _buffer.mid(sizeof(quint16) * 2);

                // 输出读取的数据
                qDebug() << "Message ID is :" << _message_id
                         << "Message Length is :" << _message_len;
            }

            // buffer 剩余长度是否满足消息体的长度，不满足继续等待更多数据
            if (_buffer.size() < _message_len) {
                _b_recv_pending = true;
                return;
            }

            _b_recv_pending = false;

            // 读取消息体
            QByteArray messageBody = _buffer.mid(0, _message_len);
            qDebug() << "Receive body msg is :" << messageBody;

            _buffer = _buffer.mid(_message_len);
            handleMsg(ReqId(_message_id), _message_len, messageBody);

        }
    });

        connect(&_socket,
                QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
                [&](QAbstractSocket::SocketError socketError) {
            Q_UNUSED(socketError) // Q_UNUSED = void
            qDebug() << "Error:" << _socket.errorString();
            switch (socketError) {
                case QTcpSocket::ConnectionRefusedError:
                    qDebug() << "Connection Refused!";
                    emit sign_conn_success(false);
                    break;
                case QTcpSocket::RemoteHostClosedError:
                    qDebug() << "Remote Host Closed Connection!";
                    break;
                case QTcpSocket::HostNotFoundError:
                    qDebug() << "Host Not Found!";
                    emit sign_conn_success(false);
                    break;
                case QTcpSocket::SocketTimeoutError:
                    qDebug() << "Connection Timeout!";
                    emit sign_conn_success(false);
                    break;
                case QTcpSocket::NetworkError:
                    qDebug() << "Network Error!";
                    break;
                default:
                    qDebug() << "Other Error!";
                    break;
                    }
        });

        // 处理连接断开
        connect(&_socket, &QTcpSocket::disconnected, [&]() {
            qDebug() << "Disconnected from Server: ";
        });

        // 连接发送信号用来发送数据
        connect(this, &TCPMgr::sign_send_data, this, &TCPMgr::slot_send_data);

        // 注册消息
        initHandlers();

}

void TCPMgr::initHandlers()
{
    // auto self = shared_from_this(); 应该在类初始化之后才能使用，而initHandlers在构造函数之中

    _handlers.insert(ID_CHAT_LOGIN_RSP, [this](ReqId id, int len, QByteArray data) {
       // Q_UNUSED(len) 是 Qt 框架提供的一个宏，其核心作用是显式标记未使用的变量或参数，避免编译器生成“未使用变量”的警告
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << "data is " << data;

        // 将QByteArray转换成QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument";
            return ;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error") ) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Login Failed, error is JSON Parse ERROR " << err;
            emit sign_login_failed(err);
            return ;
        }

//        UserMgr::GetInstance()->SetUid(jsonObj["uid"].toInt());
//        UserMgr::GetInstance()->SetName(jsonObj["name"].toString());
//        UserMgr::GetInstance()->SetToken(jsonObj["token"].toString());
        emit sign_switch_chatdlg();
    });

}

void TCPMgr::handleMsg(ReqId id, int len, QByteArray data)
{
    auto find_it = _handlers.find(id);
    if (find_it == _handlers.end()) {
        qDebug() << "not found od [" << id << "] to handle";
        return ;
    }
    // 找到了那个函数，直接调用
    find_it.value()(id, len, data);
}

void TCPMgr::slot_tcp_connect(ServerInfo si)
{
    qDebug()<< "receive tcp connect signal";
    // 尝试连接到服务器
    qDebug() << "Connecting to server...";
    _host = si.Host;
    _port = static_cast<uint16_t>(si.Port.toUInt());
    _socket.connectToHost(si.Host, _port);
}

void TCPMgr::slot_send_data(ReqId reqId, QString data)
{
    uint16_t id = reqId;

    // 将字符串转换为UTF-8编码的字节数组
    QByteArray dataBytes = data.toUtf8();

    // 计算长度（使用网络字节序转换）
    quint16 len = static_cast<quint16>(data.size());

    // 创建一个QByteArray用于存储要发送的所有数据
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    // 设置数据流使用网络字节序（即大端）
    out.setByteOrder(QDataStream::BigEndian);

    // 写入ID和长度
    out << id << len;

    // 添加字符串数据
    block.append(data);

    // 发送数据
    _socket.write(block);
}



TCPMgr::~TCPMgr() {

}
