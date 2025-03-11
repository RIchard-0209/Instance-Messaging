#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include <QString>
#include <vector>

#include "global.h"


namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog {
  Q_OBJECT

 public:
  explicit ChatDialog(QWidget *parent = nullptr);
  ~ChatDialog();
 protected:
  bool eventFilter(QObject *watched, QEvent *event) override;
  void handleGlobalMousePress(QMouseEvent *event);

 private:
  void showSearch(bool b_search);
  void addChatUserList();

 private:
  Ui::ChatDialog *ui;
  bool _b_loading;
  ChatUIMode _mode;
  // ChatUIMode _state;
};

#endif  // CHATDIALOG_H
