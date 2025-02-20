#include "mainwindow.h"

#include <QApplication>
#include <QFile>

#include "global.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // QFile 添加样式
    QFile qss(":/src/style/stylesheet.qss");

    if (qss.open(QFile::ReadOnly)) {
        qDebug("Open success\n");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    } else {
        qDebug("Open failed\n");
    }


    // 初始化配置
    QString fileName = "config.ini";
    // 获取当前应用程序的路径
    QString app_path = QCoreApplication::applicationDirPath();
    QString config_path = QDir::toNativeSeparators(app_path + QDir::separator() + fileName);
    QSettings settings(config_path, QSettings::IniFormat);

    /* 没有复制过去 */
    qDebug() << config_path;

    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    if (gate_host.isEmpty()) {
        qDebug() << "Host is empty, check config file.";
    }
    if (gate_port.isEmpty()) {
        qDebug() << "Port is empty, check config file.";
    }

    gate_url_prefix = "http://" + gate_host + ":" + gate_port;
    qDebug() << "Gate URL Prefix:" << gate_url_prefix;

    MainWindow w;
    w.show();
    return a.exec();
}
