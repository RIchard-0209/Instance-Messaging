#include "registerdialog.h"
#include "ui_registerdialog.h"
#include <QRegularExpression>
#include "global.h"
#include "httpmgr.h"


RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    // 设置密码不显示
    ui->setupUi(this);
    ui->psw_confirm_lineEdit->setEchoMode(QLineEdit::Password);
    ui->psw_confirm_lineEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    /*
     * PasswordEchoOnEdit输入状态的时候，是正常显示字符，输入结束后，隐藏
     */

    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);

    connect(HttpMgr::GetInstance().get(), &HttpMgr::sign_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);

    initHttpHandlers();
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_varify_btn_clicked()
{
    auto email = ui->mail_lineEdit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();

    if (match) {
        // 发送http验证码
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/get_varifycode"),
                                            json_obj, ReqId::ID_GET_VARIFY_CODE,Modules::REGISTERMOD);
    } else {
        showTip(tr("邮箱地址不正确"), false);
    }

}

void RegisterDialog::on_confirm_btn_clicked()
{
    if(ui->user_lineEdit->text() == ""){
        showTip(tr("用户名不能为空"), false);
        return;
    }

    if(ui->mail_lineEdit->text() == ""){
        showTip(tr("邮箱不能为空"), false);
        return;
    }

    if(ui->psw_lineEdit->text() == ""){
        showTip(tr("密码不能为空"), false);
        return;
    }

    if(ui->psw_confirm_lineEdit->text() == ""){
        showTip(tr("确认密码不能为空"), false);
        return;
    }

    if(ui->psw_confirm_lineEdit->text() != ui->psw_lineEdit->text()){
        showTip(tr("密码和确认密码不匹配"), false);
        return;
    }

    if(ui->varify_lineEdit->text() == ""){
        showTip(tr("验证码不能为空"), false);
        return;
    }

    //day11 发送http请求注册用户
    QJsonObject json_obj;
    json_obj["user"] = ui->user_lineEdit->text();
    json_obj["email"] = ui->mail_lineEdit->text();
    json_obj["passwd"] = ui->psw_lineEdit->text();
    json_obj["confirm"] = ui->psw_confirm_lineEdit->text();
    json_obj["varifycode"] = ui->varify_lineEdit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_register"),
                                        json_obj, ReqId::ID_REG_USER,Modules::REGISTERMOD);
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if (err != ErrorCodes::SUCCESS) {
        showTip(tr("网络请求错误"), false);
        return ;
    }

    // 解析JSon字符串， res 转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        showTip(tr("json解析失败"), false);
        return ;
    }


    _handlers[id](jsonDoc.object());
    return ;

}

void RegisterDialog::initHttpHandlers()
{
    // 注册获取验证码回包的逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](const QJsonObject& jsonObj) {
        int error = jsonObj["error"].toInt();

        if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"), false);
            return;
        }

        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已经发送到邮箱，注意查收"), true);
        qDebug() << "email is " << email;
    });

    _handlers.insert(ReqId::ID_REG_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("用户注册成功"), true);
        qDebug() << "user uuid is " << jsonObj["uuid"].toString();
        qDebug() << "email is " << email ;

    });
}

void RegisterDialog::showTip(QString str, bool b_state)
{
    if (b_state) {
        ui->err_tip->setProperty("state", "normal");
    } else {
        ui->err_tip->setProperty("state", "error");
    }
    ui->err_tip->setText(str);

    repolish(ui->err_tip);
}

