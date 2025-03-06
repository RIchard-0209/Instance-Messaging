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

 private:
  void showSearch(bool b_search);
  void addChatUserList();
 private:
  Ui::ChatDialog *ui;
  ChatUIMode _mode;
  // ChatUIMode _state;
  bool _b_loading;
};



#endif  // CHATDIALOG_H
