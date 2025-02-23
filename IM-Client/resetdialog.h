#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();


private slots:
    void on_cancel_btn_clicked();
    void on_confirm_btn_clicked();
    void slot_reset_mod_finish(ReqId id, QString str, ErrorCodes err);
    void on_varify_btn_clicked();

signals:
    void switchLogin();

private:
    bool checkUserValid();
    bool checkPswValid();
    bool checkEmailValid();
    bool checkVarifyValid();

    void addTipErr(TipErr, QString str);
    void delTipErr(TipErr);
    void showTip(QString, bool);

    void initHandlers();
private:
    Ui::ResetDialog *ui;
    QMap<TipErr, QString> _tip_errs;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;

};

#endif // RESETDIALOG_H
