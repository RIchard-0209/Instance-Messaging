#include "httpmgr.h"

HttpMgr::~HttpMgr()
{

}

HttpMgr::HttpMgr() {
    connect(this, &HttpMgr::sign_http_finish, this, &HttpMgr::slot_http_finish);
}

void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    QByteArray data = QJsonDocument(json).toJson();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));

    auto self = shared_from_this(); // 使类A继承enable_share_from_this，然后通过其成员函数share_from_this()返回当指向自身的share_ptr
    QNetworkReply *reply = _manager.post(request, data);

    // 触发回调之前这个Http Manager不能被删除，所以采用智能指针
    QObject::connect(reply, &QNetworkReply::finished, [self, reply, req_id, mod]() {
        // 处理错误
        qDebug() << "reply->error() is " << reply->error();
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            // 发送信号通知错误
            emit self->sign_http_finish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater();
            return ;
        }

        // 无错误
        QString res = reply->readAll();

        // 发送信息通知完成
        emit self->sign_http_finish(req_id, res, ErrorCodes::SUCCESS, mod);
        reply->deleteLater();
        return ;

    });
}

void HttpMgr::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod)
{
    if (mod == Modules::REGISTERMOD) {
        // 发送信号通知指定模块http的响应结束了
        emit sign_reg_mod_finish(id, res, err);
    }
    if(mod == Modules::RESETMOD){
        //发送信号通知指定模块http响应结束
        emit sign_reset_mod_finish(id, res, err);
    }

    if(mod == Modules::LOGINMOD){
        emit sign_login_mod_finish(id, res, err);
    }
}
