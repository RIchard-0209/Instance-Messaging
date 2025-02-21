#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 创建一个CentralWidget，并将其设置为MainWindow的中心部件
    _log_dlg = new LoginDialog(this);
    setCentralWidget(_log_dlg);
    // _log_dlg->show();

    // 创建和注册信息的连接
    connect(_log_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    _log_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

}

MainWindow::~MainWindow()
{
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

void MainWindow::SlotSwitchReg()
{
    _reg_dlg = new RegisterDialog(this);
    _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    connect(_reg_dlg, &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLog);
    setCentralWidget(_reg_dlg);
    _log_dlg->hide();
    _reg_dlg->show();
}

void MainWindow::SlotSwitchLog()
{
    _log_dlg = new LoginDialog(this);
    _log_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setCentralWidget(_log_dlg);
    _log_dlg->show();

    // 连接登录界面注册信号
    connect(_log_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);

}
