#include "resetdialog.h"
#include "ui_resetdialog.h"
#include "httpmgr.h"


ResetDialog::ResetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResetDialog)
{
    ui->setupUi(this);

    ui->err_tip->setProperty("state","normal");
    repolish(ui->err_tip);
    ui->err_tip->clear();

    connect(ui->user_lineEdit,&QLineEdit::editingFinished,this,[this](){
        checkUserValid();
    });

    connect(ui->email_lineEdit, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });

    connect(ui->psw_lineEdit, &QLineEdit::editingFinished, this, [this](){
        checkPswValid();
    });

    connect(ui->varify_lineEdit, &QLineEdit::editingFinished, this, [this](){
         checkVarifyValid();
    });

    initHandlers();

    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reset_mod_finish, this,
            &ResetDialog::slot_reset_mod_finish);
}

ResetDialog::~ResetDialog()
{
    delete ui;
}

void ResetDialog::on_cancel_btn_clicked()
{
    qDebug() << "cancel btn clicked";
    emit switchLogin();
}
/**
 * @brief ResetDialog::on_confirm_btn_clicked
 * 自动连接
    如果槽函数名称严格遵循 on_<对象名>_<信号名> 的格式（如 on_pushButton_clicked()），Qt 会自动将信号与槽连接。
    无需手动调用 connect()。
 */

void ResetDialog::on_confirm_btn_clicked()
{
    // 发送逻辑
    if (!checkUserValid()) return ;
    if (!checkEmailValid()) return ;
    if (!checkPswValid()) return ;
    if (!checkVarifyValid()) return ;

    qDebug() << "点击确认按钮，并通过验证";
    QJsonObject jsonObj;
    jsonObj["user"] = ui->user_lineEdit->text();
    jsonObj["email"] = ui->email_lineEdit->text();
    jsonObj["passwd"] = encrypt(ui->psw_lineEdit->text());
    jsonObj["varifycode"] = ui->varify_lineEdit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/reset_pwd"),
                     jsonObj, ReqId::ID_RESET_PWD,Modules::RESETMOD);
}

void ResetDialog::slot_reset_mod_finish(ReqId id, QString str, ErrorCodes err)
{
    if (err != ErrorCodes::SUCCESS) {
        showTip(tr("网络请求错误"), false);
        return ;
    }
    // 解析JSON字符串， str需要转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8());

    // json解析失败
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        showTip(tr("json解析错误"), false);
        return;
    }

    // 根据id回调，对应逻辑
    _handlers[id](jsonDoc.object());
}

void ResetDialog::on_varify_btn_clicked()
{
    qDebug()<<"receive varify btn clicked ";
    auto email = ui->email_lineEdit->text();

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

bool ResetDialog::checkUserValid()
{
    if (ui->user_lineEdit->text() == "") {
        addTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }
    delTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool ResetDialog::checkPswValid()
{
    auto pass = ui->psw_lineEdit->text();
    if (pass.length() < 6 || pass.length() > 15) {
        addTipErr(TipErr::TIP_EMAIL_ERR, tr("密码长度应为6~15"));
        return false;
    }

    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    if (!regExp.match(pass).hasMatch()) {
        addTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;
    }
    delTipErr(TipErr::TIP_PWD_ERR);
    return true;
}

bool ResetDialog::checkEmailValid()
{
    auto email = ui->email_lineEdit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(!match){
        //提示邮箱不正确
        addTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }
    delTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool ResetDialog::checkVarifyValid()
{
    auto pass = ui->varify_lineEdit->text();
    if(pass.isEmpty()){
        addTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }

    delTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

void ResetDialog::addTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void ResetDialog::delTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
      ui->err_tip->clear();
      return;
    }
    showTip(_tip_errs.first(), false);
}

void ResetDialog::showTip(QString str, bool b_ok)
{
    if(b_ok){
         ui->err_tip->setProperty("state","normal");
    }else{
        ui->err_tip->setProperty("state","error");
    }

    ui->err_tip->setText(str);

    repolish(ui->err_tip);
}

void ResetDialog::initHandlers()
{
    // 获取验证码逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject jsonObj) {
       int error = jsonObj["error"].toInt();
       if (error != ErrorCodes::SUCCESS) {
           showTip(tr("参数错误"), false);
           return ;
       }

        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱，注意查收"), true);
        qDebug() << "email is " << email;
    });

    _handlers.insert(ReqId::ID_RESET_PWD, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("重置成功,点击返回登录"), true);
        qDebug()<< "email is " << email ;
        qDebug()<< "user uuid is " <<  jsonObj["uuid"].toString();
    });
}
