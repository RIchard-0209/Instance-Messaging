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
    ID_RESET_PWD = 1003,
    ID_LOGIN_USER = 1004,
    ID_CHAT_LOGIN = 1005, //登陆聊天服务器
    ID_CHAT_LOGIN_RSP= 1006, //登陆聊天服务器回包
};

enum Modules {
    REGISTERMOD = 0,
    RESETMOD = 1,
    LOGINMOD = 2,
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


struct ServerInfo{
    QString Host;
    QString Port;
    QString Token;
    int Uid;
};

enum class ChatRole
{

    Self,
    Other
};

struct MsgInfo{
    QString msgFlag;//"text,image,file"
    QString content;//表示文件和图像的url,文本信息
    QPixmap pixmap;//文件和图片的缩略图
};

//聊天界面几种模式
enum ChatUIMode{
    SearchMode, //搜索模式
    ChatMode, //聊天模式
};

//自定义QListWidgetItem的几种类型
enum ListItemType{
    ChatUserItem, //聊天用户
    ContactUserItem, //联系人用户
    SearchUserItem, //搜索到的用户
    AddUserTipItem, //提示添加用户
    InvalidItem,  //不可点击条目
};

//申请好友标签输入框最低长度
const int MIN_APPLY_LABEL_ED_LEN = 40;
#endif // GLOBAL_H
