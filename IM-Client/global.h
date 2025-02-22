#ifndef GLOBAL_H
#define GLOBAL_H

#include <functional>
#include <iostream>
#include <memory>
#include <mutex>


#include <QWidget>
#include <QRegularExpression>
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <QDir>
#include <QSettings>

#include "QStyle"


extern std::function<void(QWidget*)> repolish;
extern QString encrypt(const QString& str);


enum ReqId {
    ID_GET_VARIFY_CODE = 1001, // 获取验证码
    ID_REG_USER = 1002,
};

enum Modules {
    REGISTERMOD = 0,
};

// 有些公司不会把错误码发给用户，防止用户爆破
enum ErrorCodes {
    SUCCESS = 0,
    ERR_JSON = 1, // json 解析失败
    ERR_NETWORK = 2, // 网络错误
};


enum TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6
};

enum ClickLbState{
    Normal = 0,
    Selected = 1
};

extern QString gate_url_prefix;

#endif // GLOBAL_H
