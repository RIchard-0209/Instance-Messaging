#ifndef CUSTOMIZEEDIT_H
#define CUSTOMIZEEDIT_H

#include <QDebug>
#include <QLineEdit>

class CustomizeEdit : public QLineEdit {
  Q_OBJECT
 public:
  CustomizeEdit(QWidget* parent = nullptr);
  void setMaxLength(int max_len);

 signals:
  void sign_foucus_out();

 protected:
  void focusOutEvent(QFocusEvent* event) override;

 private:
  void limitTextLength(QString text);
  int _max_len;
};

#endif  // CUSTOMIZEEDIT_H
