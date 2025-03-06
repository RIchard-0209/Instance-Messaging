#include "chatdialog.h"

#include <QAction>

#include "appsources.h"
#include "ui_chatdialog.h"

ChatDialog::ChatDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::ChatDialog) {
  ui->setupUi(this);
  ui->add_btn->SetState("normal", "hover", "press");
  ui->add_btn->setProperty("state", "normal");

  /* 搜索框设置 */
  // 添加一个搜索Icon
  QAction* searchAction = new QAction(ui->search_edit);
  searchAction->setIcon(QIcon(AppResources::getSearchIconPath()));
  ui->search_edit->addAction(searchAction,
                             QLineEdit::LeadingPosition);  // 头部设置
  ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));

  // 创建一个清除动作并设置icon
  QAction* clearAction = new QAction(ui->search_edit);
  // 初始时不显示清除图标
  clearAction->setIcon(QIcon(AppResources::getCloseTransparentIconPath()));
  ui->search_edit->addAction(clearAction, QLineEdit::TrailingPosition);
  // 当需要显示清除图标时，更改为实际的清除图标
  connect(
      ui->search_edit, &QLineEdit::textChanged,
      [clearAction](const QString& text) {
        if (!text.isEmpty()) {
          clearAction->setIcon(QIcon(AppResources::getCloseSearchIconPath()));
        } else {
          clearAction->setIcon(
              QIcon(AppResources::getCloseTransparentIconPath()));
        }
      });

  // 连接清除动作
  connect(clearAction, &QAction::triggered, [this, clearAction]() {
    ui->search_edit->clear();
    clearAction->setIcon(QIcon(AppResources::getCloseTransparentIconPath()));
    ui->search_edit->clearFocus();
  });

  ui->search_edit->setMaxLength(15);
}

ChatDialog::~ChatDialog() { delete ui; }
