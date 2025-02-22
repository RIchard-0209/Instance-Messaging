#include "registerdialog.h"
#include "ui_registerdialog.h"


#include <QRegularExpression>
#include <QDebug>
#include <QLabel>

#include "global.h"
#include "httpmgr.h"
#include "clickedlabel.h"


RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{

    ui->setupUi(this);

    // // 进入用户注册设置focus
    // ui->user_lineEdit->setFocus();
    // 设置密码不显示
    ui->psw_lineEdit->setEchoMode(QLineEdit::Password);
    ui->psw_confirm_lineEdit->setEchoMode(QLineEdit::Password); // PasswordEchoOnEdit输入状态的时候，是正常显示字符，输入结束后，隐藏

    ui->psw_lineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);    //屏蔽输入法
    ui->psw_confirm_lineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);

    connect(HttpMgr::GetInstance().get(), &HttpMgr::sign_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);

    initHttpHandlers();
    ui->err_tip->clear();

    connect(ui->user_lineEdit,&QLineEdit::editingFinished,this,[this](){
        checkUserValid();
    });

    connect(ui->mail_lineEdit, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });

    connect(ui->psw_lineEdit, &QLineEdit::editingFinished, this, [this](){
        checkPassValid();
    });

    connect(ui->psw_lineEdit, &QLineEdit::editingFinished, this, [this]() {
        if (ui->psw_confirm_lineEdit->text().length() != 0)
            checkConfirmValid();
    });

    connect(ui->psw_confirm_lineEdit, &QLineEdit::editingFinished, this, [this](){
        checkConfirmValid();
    });

    connect(ui->varify_lineEdit, &QLineEdit::editingFinished, this, [this](){
        checkVarifyValid();
    });

    //设置浮动显示手形状
    ui->pass_visible->setCursor(Qt::PointingHandCursor);
    ui->confirm_visible->setCursor(Qt::PointingHandCursor);

    ui->pass_visible->SetState("unvisible","unvisible_hover","","visible",
                               "visible_hover","");

    ui->confirm_visible->SetState("unvisible","unvisible_hover","","visible",
                                  "visible_hover","");
    //连接点击事件
    connect(ui->pass_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->pass_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->psw_lineEdit->setEchoMode(QLineEdit::Password);
        }else{
            ui->psw_lineEdit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
    });

    connect(ui->confirm_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->confirm_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->psw_confirm_lineEdit->setEchoMode(QLineEdit::Password);
        }else{
            ui->psw_confirm_lineEdit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
    });

}

RegisterDialog::~RegisterDialog()
{
    qDebug() << "RegisterDialog destructed";
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

    // 感觉这块怪怪的
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

    QJsonObject json_obj;
    json_obj["user"] = ui->user_lineEdit->text();
    json_obj["email"] = ui->mail_lineEdit->text();
    json_obj["passwd"] = encrypt(ui->psw_lineEdit->text());
    json_obj["confirm"] = encrypt(ui->psw_confirm_lineEdit->text());
    json_obj["varifycode"] = ui->varify_lineEdit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_register"),
                                        json_obj, ReqId::ID_REG_USER,Modules::REGISTERMOD);


}

void RegisterDialog::on_cancel_btn_clicked()
{

    emit sigSwitchLogin();
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
        showTip(tr("用户注册成功，系统将自动返回"), true);
        qDebug() << "email is " << email ;

        return_log_dlg();
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

void RegisterDialog::addTipErr(TipErr te, QString str)
{
    _tip_errs[te] = str;
    showTip(str, false);
}

void RegisterDialog::delTipErr(TipErr te) {
    _tip_errs.remove(te);
    if (_tip_errs.empty()) {
        ui->err_tip->clear();
        return ;
    }
    showTip(_tip_errs.first(), false);
}

bool RegisterDialog::checkUserValid()
{
    if(ui->user_lineEdit->text().length() <= 0){
        addTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }

    delTipErr(TipErr::TIP_USER_ERR);
    return true;
}


bool RegisterDialog::checkPassValid()
{
    auto pass = ui->psw_lineEdit->text();

    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        addTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*_]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*_]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        addTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;;
    }

    delTipErr(TipErr::TIP_PWD_ERR);

    return true;
}

bool RegisterDialog::checkConfirmValid()
{
    auto psw = ui->psw_lineEdit->text();
    auto psw_confirm = ui->psw_confirm_lineEdit->text();

    if (psw != psw_confirm) {
        addTipErr(TipErr::TIP_PWD_CONFIRM, tr("确认密码与密码不一致"));
        return false;
    }
    delTipErr(TipErr::TIP_PWD_CONFIRM);
    return true;
}

bool RegisterDialog::checkEmailValid() {
    QString email = ui->mail_lineEdit->text().trimmed();

    // 提前校验基础条件
    if (email.isEmpty()) {
        addTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱不能为空"));
        return false;
    }
    if (email.length() > 320) {
        addTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址过长"));
        return false;
    }
    if (!email.contains('@')) {
        addTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱必须包含@符号"));
        return false;
    }

    const QRegularExpression emailRegex(
        R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)"
        );

    QRegularExpressionMatch match = emailRegex.match(email);
    if (!match.hasMatch()) {
        addTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }

    delTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool RegisterDialog::checkVarifyValid()
{
    auto pass = ui->varify_lineEdit->text();
    if(pass.isEmpty()){
        addTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }

    delTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

void RegisterDialog::return_log_dlg()
{

    QTimer::singleShot(3000, this, [this]() {
        emit sigSwitchLogin();
        });
}
