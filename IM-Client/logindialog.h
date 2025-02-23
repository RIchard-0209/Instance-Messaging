#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void slot_forget_psw();

signals:
    void switchRegister();
    void switchResetPwd();
private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
