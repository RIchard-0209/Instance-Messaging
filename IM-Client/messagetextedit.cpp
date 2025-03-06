#include "messagetextedit.h"

MessageTextEdit::MessageTextEdit(QWidget *parent) {
  this->setMaximumHeight(60);
}

MessageTextEdit::~MessageTextEdit() {}

QVector<MsgInfo> MessageTextEdit::getMsgList() {
  mGetMsgList.clear();

  QString doc = this->document()->toPlainText();
  QString text = "";
  int indexUrl = 0;
  int count = mMsgList.size();
}

void MessageTextEdit::insertFileFromUrl(const QStringList &urls) {}

void MessageTextEdit::dragEnterEvent(QDragEnterEvent *event) {}

void MessageTextEdit::dropEvent(QDropEvent *event) {}

void MessageTextEdit::keyPressEvent(QKeyEvent *event) {}

void MessageTextEdit::insertImages(const QString &url) {}

void MessageTextEdit::insertTextFile(const QString &url) {}

void MessageTextEdit::insertFromMimeData(const QMimeData *source) {}

bool MessageTextEdit::canInsertFromMimeData(const QMimeData *source) const {}

bool MessageTextEdit::isImage(QString url) {}

void MessageTextEdit::insertMsgList(QVector<MsgInfo> &list, QString flag,
                                    QString text, QPixmap pix) {}

QStringList MessageTextEdit::getUrl(QString url) {}

QPixmap MessageTextEdit::getFileIconPixmap(const QString &url) {}

QString MessageTextEdit::getFileSize(qint64 size) {}

void MessageTextEdit::textEditChanged() {}
