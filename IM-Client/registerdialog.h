#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "global.h"


namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

public slots:
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);


private slots:
    void on_varify_btn_clicked();
    void on_confirm_btn_clicked();
    void on_cancel_btn_clicked();

signals:
    void sigSwitchLogin();

private:
    Ui::RegisterDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    QMap<TipErr, QString> _tip_errs;
    // QTimer* _timer;
    // int _countDown;

    void initHttpHandlers();
    void showTip(QString str, bool b_state);
    void addTipErr(TipErr te, QString str);
    void delTipErr(TipErr te);
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkConfirmValid();
    bool checkVarifyValid();

    void return_log_dlg();
};

#endif // REGISTERDIALOG_H
