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

private:
    Ui::RegisterDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;

    void initHttpHandlers();
    void showTip(QString str, bool b_state);
};

#endif // REGISTERDIALOG_H
