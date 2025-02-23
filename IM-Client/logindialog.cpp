#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDebug>
#include "global.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    ui->forget_label->SetState("normal","hover","","selected","selected_hover","");
    ui->forget_label->setCursor(Qt::PointingHandCursor);

    connect(ui->reg_btn, &QPushButton::clicked, this, &LoginDialog::switchRegister);
    connect(ui->forget_label, &ClickedLabel::clicked, this, &LoginDialog::slot_forget_psw);

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
