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
    connect(_log_dlg, &LoginDialog::switchRegister, this, &MainWindow::SoltSwitchReg);

    _reg_dlg = new RegisterDialog(this);

    _log_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    _reg_dlg->hide();

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

void MainWindow::SoltSwitchReg()
{
    setCentralWidget(_reg_dlg);
    _log_dlg->hide();
    _reg_dlg->show();
}
