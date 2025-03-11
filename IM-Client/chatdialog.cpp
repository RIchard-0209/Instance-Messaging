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

  /* 侧边栏 */
  QPixmap pixmap(":/src/img/head_1.jpg");  // 加载图片
  ui->side_head_lb->setPixmap(pixmap);  // 将图片设置到QLabel上
  QPixmap scaledPixmap =
      pixmap.scaled(ui->side_head_lb->size(),
                    Qt::KeepAspectRatio);  // 将图片缩放到label的大小
  ui->side_head_lb->setPixmap(scaledPixmap);  // 将缩放后的图片设置到QLabel上
  ui->side_head_lb->setScaledContents(
      true);  // 设置QLabel自动缩放图片内容以适应大小

  ui->side_chat_lb->setProperty("state", "normal");

  ui->side_chat_lb->SetState("normal", "hover", "pressed", "selected_normal",
                             "selected_hover", "selected_pressed");

  ui->side_contact_lb->SetState("normal", "hover", "pressed", "selected_normal",
                                "selected_hover", "selected_pressed");


  /* 标签 */

  // 链接搜索框输入变化
  connect(ui->search_edit, &QLineEdit::textChanged, this,
          &ChatDialog::slot_text_changed);

  showSearch(false);

  // 检测鼠标点击位置判断是否要清空搜索框
  this->installEventFilter(this);  // 安装事件过滤器

  // 设置聊天label选中状态
  ui->side_chat_lb->SetSelected(true);
}

ChatDialog::~ChatDialog() { delete ui; }

void ChatDialog::showSearch(bool b_search) {
  if (bsearch) {
    ui->chat_user_list->hide();
    ui->con_user_list->hide();
    ui->search_list->show();
    _mode = ChatUIMode::SearchMode;
  } else if (_state == ChatUIMode::ChatMode) {
    ui->chat_user_list->show();
    ui->con_user_list->hide();
    ui->search_list->hide();
    _mode = ChatUIMode::ChatMode;
    ui->search_list->CloseFindDlg();
    ui->search_edit->clear();
    ui->search_edit->clearFocus();
  } else if (_state == ChatUIMode::ContactMode) {
    ui->chat_user_list->hide();
    ui->search_list->hide();
    ui->con_user_list->show();
    _mode = ChatUIMode::ContactMode;
    ui->search_list->CloseFindDlg();
    ui->search_edit->clear();
    ui->search_edit->clearFocus();
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

bool ChatDialog::eventFilter(QObject* watched, QEvent* event) {
  if (event->type() == QEvent::MouseButtonPress) {
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    handleGlobalMousePress(mouseEvent);
  }
  return QDialog::eventFilter(watched, event);
}

void ChatDialog::handleGlobalMousePress(QMouseEvent* event) {
  // 实现点击位置的判断和处理逻辑
  // 先判断是否处于搜索模式，如果不处于搜索模式则直接返回
  if (_mode != ChatUIMode::SearchMode) {
    return;
  }

  // 将鼠标点击位置转换为搜索列表坐标系中的位置
  QPoint posInSearchList = ui->search_list->mapFromGlobal(event->globalPos());
  // 判断点击位置是否在聊天列表的范围内
  if (!ui->search_list->rect().contains(posInSearchList)) {
    // 如果不在聊天列表内，清空输入框
    ui->search_edit->clear();
    showSearch(false);
  }
}
