#include "chatdialog.h"

#include <QAction>
#include <QRandomGenerator>

#include "appsources.h"
#include "chatuserwid.h"
#include "ui_chatdialog.h"

ChatDialog::ChatDialog(QWidget* parent)
    : QDialog(parent),
      ui(new Ui::ChatDialog),
      _b_loading(false),
      _mode(ChatUIMode::ChatMode) {
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

  /* 添加联系人界面 */
  addChatUserList();

  showSearch(false);
}

ChatDialog::~ChatDialog() { delete ui; }

void ChatDialog::showSearch(bool b_search) {
  if (b_search) {
    ui->chat_user_list->hide();
    // ui->con_user_list->hide();
    ui->search_list->show();
    _mode = ChatUIMode::SearchMode;
  } else {
    ui->chat_user_list->show();
    ui->search_list->hide();
    _mode = ChatUIMode::ChatMode;
  }
}

void ChatDialog::addChatUserList() {
  std::vector<QString> strs = {
      "hello world !", "nice to meet u", "New year，new life",
      "You have to love yourself",
      "My love is written in the wind ever since the whole world is you"};

  std::vector<QString> heads = {":/src/img/head_1.jpg", ":/src/img/head_2.jpg",
                                ":/src/img/head_3.jpg", ":/src/img/head_4.jpg",
                                ":/src/img/head_5.jpg", ":/src/img/head_6.jpg",
                                ":/src/img/head_7.jpg"};

  std::vector<QString> names = {"ichat", "2ein",   "golang", "cpp",
                                "java",  "nodejs", "python", "rust"};
  for (int i = 0; i < 13; i++) {
    int randomValue = QRandomGenerator::global()->bounded(100);
    int str_i = randomValue % strs.size();
    int head_i = randomValue % heads.size();
    int name_i = randomValue % names.size();

    auto* chat_user_wid = new ChatUserWid();
    chat_user_wid->SetInfo(names[name_i], heads[head_i], strs[str_i]);

    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->addItem(item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
  }
}
