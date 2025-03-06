#ifndef MESSAGETEXTEDIT_H
#define MESSAGETEXTEDIT_H

#include <QApplication>
#include <QDrag>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QMimeData>
#include <QMouseEvent>
#include <QObject>
#include <QPainter>
#include <QTextEdit>
#include <QVector>

#include "global.h"

class MessageTextEdit : public QTextEdit {
  Q_OBJECT
 public:
  explicit MessageTextEdit(QWidget* parent = nullptr);
  ~MessageTextEdit();

  QVector<MsgInfo> getMsgList();

  void insertFileFromUrl(const QStringList& urls);

 signals:
  void send();

 protected:
  void dragEnterEvent(QDragEnterEvent* event);
  void dropEvent(QDropEvent* event);
  void keyPressEvent(QKeyEvent* event);

 private:
  void insertImages(const QString& url);
  void insertTextFile(const QString& url);
  void insertFromMimeData(const QMimeData* source);
  bool canInsertFromMimeData(const QMimeData* source) const;

  bool isImage(QString url);
  void insertMsgList(QVector<MsgInfo>& list, QString flag, QString text,
                     QPixmap pix);
  QStringList getUrl(QString url);
  QPixmap getFileIconPixmap(
      const QString& url);  // 获取文件图标及大小信息，转化成图片
  QString getFileSize(qint64 size);

 private slots:
  void textEditChanged();

 private:
  QVector<MsgInfo> mMsgList;
  QVector<MsgInfo> mGetMsgList;
};

#endif  // MESSAGETEXTEDIT_H
