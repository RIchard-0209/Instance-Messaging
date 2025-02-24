#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDebug>
#include "httpmgr.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

     ui->user_lineEdit->setFocus();
    // 设置密码不显示
    ui->psw_lineEdit->setEchoMode(QLineEdit::Password);
    ui->psw_lineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);    //屏蔽输入法

    ui->forget_label->SetState("normal","hover","","selected","selected_hover","");
    ui->forget_label->setCursor(Qt::PointingHandCursor);

    connect(ui->reg_btn, &QPushButton::clicked, this, &LoginDialog::switchRegister);
    connect(ui->forget_label, &ClickedLabel::clicked, this, &LoginDialog::slot_forget_psw);

    ui->err_tip->setProperty("state","normal");
    repolish(ui->err_tip);
    ui->err_tip->clear();

    connect(ui->user_lineEdit,&QLineEdit::textEdited,this,[this](){
        checkUserValid();
    });

    connect(ui->psw_lineEdit, &QLineEdit::textEdited, this, [this]() {
       checkPwdValid();
    });

    initHttpHandlers();
    //连接登录回包信号
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sign_login_mod_finish, this,
            &LoginDialog::slot_login_mod_finish);
}

LoginDialog::~LoginDialog()
{
    qDebug() << "LoginDialog destructed";
    delete ui;
}

void LoginDialog::slot_forget_psw()
{
    qDebug() << "slot forget pwd";
    emit switchResetPwd();
}

void LoginDialog::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"),false);
        return;
    }

    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    //json解析错误
    if(jsonDoc.isNull()){
        showTip(tr("json解析错误"),false);
        return;
    }

    //json解析错误
    if(!jsonDoc.isObject()){
        showTip(tr("json解析错误"),false);
        return;
    }


    //调用对应的逻辑,根据id回调。
    _handlers[id](jsonDoc.object());

    return;
}

void LoginDialog::on_login_btn_clicked()
{
    qDebug() << "login btn clicked";
    if (!checkUserValid()) return ;
    if (!checkPwdValid()) return ;

    auto user = ui->user_lineEdit->text();
    auto pwd = ui->psw_lineEdit->text();


    // 发送http请求
    QJsonObject jsonObj;
    jsonObj["user"] = user;
    jsonObj["passwd"] = encrypt(pwd);
    qDebug() << "发送登录信息";
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_login"),
                                        jsonObj, ReqId::ID_LOGIN_USER, Modules::LOGINMOD);

}

bool LoginDialog::checkUserValid()
{
    auto user = ui->user_lineEdit->text();
    if (user.isEmpty()) {
        qDebug() << "User empty";
        addTipErr(TipErr::TIP_USER_ERR, tr("用户名不可为空"));
        return false;
    }
    delTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool LoginDialog::checkPwdValid()
{
    auto pwd = ui->psw_lineEdit->text();
    if (pwd.isEmpty()) {
        qDebug() << "Password empty";
        addTipErr(TipErr::TIP_USER_ERR, tr("密码不可为空"));
        return false;
    }
    delTipErr(TipErr::TIP_USER_ERR);
    return true;
}


void LoginDialog::addTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void LoginDialog::delTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
      ui->err_tip->clear();
      return;
    }
    showTip(_tip_errs.first(), false);
}

void LoginDialog::showTip(QString str, bool b_ok)
{
    if(b_ok){
         ui->err_tip->setProperty("state","normal");
    }else{
        ui->err_tip->setProperty("state","error");
    }

    ui->err_tip->setText(str);

    repolish(ui->err_tip);
}

void LoginDialog::initHttpHandlers()
{
    //注册获取登录回包逻辑
    _handlers.insert(ReqId::ID_LOGIN_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }

        auto user = jsonObj["user"].toString();
        ServerInfo si;
        si.Uid = jsonObj["uid"].toInt();
        si.Host = jsonObj["host"].toString();
        si.Port = jsonObj["port"].toString();
        si.Token = jsonObj["token"].toString();

        _uid = si.Uid;
        _token = si.Token;
        qDebug()<< "user is " << user << " uid is " << si.Uid <<" host is "
                << si.Host << " Port is " << si.Port << " Token is " << si.Token;

        showTip(tr("登录成功"), true);
        qDebug()<< "user is " << user ;
    });
}
