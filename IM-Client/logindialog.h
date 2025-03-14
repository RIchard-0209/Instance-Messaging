#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

#include "global.h"


namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog {
  Q_OBJECT

 public:
  explicit LoginDialog(QWidget *parent = nullptr);
  ~LoginDialog();

 private slots:
  void slot_forget_psw();
  void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);
  void on_login_btn_clicked();
  void slot_tcp_conn_finish(bool);
  void slot_login_failed(int);

 signals:
  void switchRegister();
  void switchResetPwd();
  void sign_connect_tcp(ServerInfo);

 private:
  bool checkUserValid();
  bool checkPwdValid();
  void addTipErr(TipErr te, QString tips);
  void delTipErr(TipErr te);
  void showTip(QString str, bool b_ok);
  void initHttpHandlers();
  void enableBtn(bool);

 private:
  Ui::LoginDialog *ui;
  QMap<TipErr, QString> _tip_errs;
  QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;
  int _uid;
  QString _token;
};

#endif  // LOGINDIALOG_H
