#include "global.h"


#include <QMessageAuthenticationCode>
#include <QString>
#include <QByteArray>

std::function<void(QWidget*)> repolish = [](QWidget* w) {
    w->style()->unpolish(w);
    w->style()->polish(w);
};

QString gate_url_prefix = "";

QString encrypt(const QString& str) {

    QByteArray byteArray = str.toUtf8();
    QString res = QMessageAuthenticationCode::hash(byteArray, "key", QCryptographicHash::Sha256).toHex();
    qDebug() << res ;
    return res;
}
