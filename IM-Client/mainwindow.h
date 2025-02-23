#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "logindialog.h"
#include "registerdialog.h"
#include "resetdialog.h"
/******************************************************************************
 *
 * @file       mainwindow.h
 * @brief      XXXX Function
 *
 * @author     2ein
 * @date       2025/02/17
 * @history
 *****************************************************************************/


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void SlotSwitchLog();
//    void SlotSwitchLogFromReset();  // 从重置界面返回
    void SlotSwitchReg();
    void SlotSwitchResetPwd();

private:
    Ui::MainWindow *ui;
    LoginDialog *_log_dlg;
    RegisterDialog* _reg_dlg;
    ResetDialog* _reset_dlg;
};
#endif // MAINWINDOW_H
