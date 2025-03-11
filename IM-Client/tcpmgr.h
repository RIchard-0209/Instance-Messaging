#ifndef TCPMGR_H
#define TCPMGR_H

#include <QJsonArray>
#include <QObject>
#include <QTcpSocket>
#include <functional>

#include "global.h"
#include "singleton.h"
#include "userdata.h"

// 采用单例模式和可被分享类(防止对象被多次析构)，管理TCP连接
class TCPMgr : public QObject,
               public Singleton<TCPMgr>,
               public std::enable_shared_from_this<TCPMgr> {
  Q_OBJECT

 public:
  ~TCPMgr();
  friend class Singleton<TCPMgr>;

 signals:
  void sign_conn_success(bool b_success);
  void sign_send_data(ReqId id, QString data);
  void sign_switch_chatdlg();
  void sign_login_failed(int err);
 void sign_load_apply_list(QJsonArray array);
 void sign_user_search(std::shared_ptr<SearchInfo>);
 void sign_friend_apply(std::shared_ptr<AddFriendApply>);
//  void sign_add_auth_friend(std::shared_ptr<AuthInfo>);
//  void sign_auth_rsp(std::shared_ptr<AuthRsp>);

 public slots:
  void slot_tcp_connect(ServerInfo);
  void slot_send_data(ReqId reqId, QString data);

 private:
  explicit TCPMgr();
  void initHandlers();
  void handleMsg(ReqId id, int len, QByteArray data);

  QTcpSocket _socket;
  QString _host;
  uint16_t _port;  // 固定16位宽度
  QByteArray _buffer;
  bool _b_recv_pending;
  quint16 _message_id;
  quint16 _message_len;
  QMap<ReqId, std::function<void(ReqId id, int len, QByteArray data)>>
      _handlers;
};

#endif  // TCPMGR_H
