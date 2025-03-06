#include "mainwindow.h"

#include <QLayout>

#include "tcpmgr.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // 创建一个CentralWidget，并将其设置为MainWindow的中心部件
  _log_dlg = new LoginDialog(this);

  setCentralWidget(_log_dlg);
  // _log_dlg->show();

  // 创建和注册信息的连接
  connect(_log_dlg, &LoginDialog::switchRegister, this,
          &MainWindow::SlotSwitchReg);
  _log_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

  connect(_log_dlg, &LoginDialog::switchResetPwd, this,
          &MainWindow::SlotSwitchResetPwd);

  // 连接创建聊天界面信号
  connect(TCPMgr::GetInstance().get(), &TCPMgr::sign_switch_chatdlg, this,
          &MainWindow::SlotSwitchChat);

  // 测试
  emit TCPMgr::GetInstance() -> sign_switch_chatdlg();
}

MainWindow::~MainWindow() {
  delete ui;

  // 构建的时候设置父组件，Qt会自己回收
  // if (_log_dlg) {
  //     delete _log_dlg;
  //     _log_dlg = nullptr;
  // }

  // if (_reg_dlg) {
  //     delete _reg_dlg;
  //     _reg_dlg = nullptr;
  // }
}

void MainWindow::SlotSwitchReg() {
  _reg_dlg = new RegisterDialog(this);
  _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

  connect(_reg_dlg, &RegisterDialog::sigSwitchLogin, this,
          &MainWindow::SlotSwitchLog);
  setCentralWidget(_reg_dlg);
  _log_dlg->hide();
  _reg_dlg->show();
}

void MainWindow::SlotSwitchResetPwd() {
  _reset_dlg = new ResetDialog();
  _reset_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

  setCentralWidget(_reset_dlg);
  // 进入reset界面后，把resetDlg的切换信号和MainWindow的切换回登录界面槽函数连接
  connect(_reset_dlg, &ResetDialog::switchLogin, this,
          &MainWindow::SlotSwitchLog);
  _log_dlg->hide();
  _reset_dlg->show();
}

void MainWindow::SlotSwitchChat() {
  _chat_dlg = new ChatDialog();
  _chat_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
  setCentralWidget(_chat_dlg);
  _chat_dlg->show();
  _log_dlg->hide();

  this->setMinimumSize(QSize(1050, 900));
  this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}

void MainWindow::SlotSwitchLog() {
  _log_dlg = new LoginDialog(this);
  _log_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

  setCentralWidget(_log_dlg);
  _log_dlg->show();

  // 连接登录界面注册信号
  connect(_log_dlg, &LoginDialog::switchRegister, this,
          &MainWindow::SlotSwitchReg);
  // 连接登录界面重置密码信号
  connect(_log_dlg, &LoginDialog::switchResetPwd, this,
          &MainWindow::SlotSwitchResetPwd);
}

// void MainWindow::SlotSwitchLogFromReset()
//{
//     _log_dlg = new LoginDialog(this);
//     _log_dlg->setWindowFlags(Qt::CustomizeWindowHint |
//     Qt::FramelessWindowHint);

//    setCentralWidget(_log_dlg);
//    _log_dlg->show();

//    // 连接登录界面注册信号
//    connect(_log_dlg, &LoginDialog::switchRegister, this,
//    &MainWindow::SlotSwitchReg);

//}
