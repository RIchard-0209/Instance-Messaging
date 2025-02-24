#ifndef HTTPMGR_H
#define HTTPMGR_H

#include "singleton.h"
#include "global.h"

#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>


// C++ CRTP
class HttpMgr: public QObject, public Singleton<HttpMgr>,
                public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT


public:
    // 因为Singleton中的智能指针析构会调用这个析构，所以置成public
    ~HttpMgr();

    // 回调时候参数的透传
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);

private:
    // 将基类声明为友元，此时基类就可以访问子类的构造函数了
    friend class Singleton<HttpMgr>;
    HttpMgr();

    QNetworkAccessManager _manager;


public slots:
    void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);

signals:
    void sign_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
    //注册模块http相关请求完成发送此信号
    void sign_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    void sign_reset_mod_finish(ReqId id, QString res, ErrorCodes err);
    void sign_login_mod_finish(ReqId id, QString res, ErrorCodes err);
};

#endif // HTTPMGR_H
