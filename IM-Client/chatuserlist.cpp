#include "chatuserlist.h"

ChatUserList::ChatUserList(QWidget *parent) {
  Q_UNUSED(parent);
  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  // 安装事件过滤器
  this->viewport()->installEventFilter(this);
}

bool ChatUserList::eventFilter(QObject *watched, QEvent *event) {
  // 检查事件： 鼠标是否悬浮进入或离开
  if (watched == this->viewport()) {
    if (event->type() == QEvent::Enter) {
      // 鼠标悬浮，显示滚动条
      this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    } else if (event->type() == QEvent::Leave) {
      // 鼠标离开，隐藏滚动条
      this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
  }

  // 检查事件：鼠标是否滚动
  if (watched == this->viewport() && event->type() == QEvent::Wheel) {
    QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
    int numDegrees = wheelEvent->angleDelta().y() / 8;
    int numSteps = numDegrees / 15; // 计算滚动步数
    QScrollBar *scrollBar = this->verticalScrollBar();
    // 设置滚动幅度
    scrollBar->setValue(scrollBar->value() - numSteps);

    // 检查是否滚动到底部
    int maxScrollValue = scrollBar->maximum();
    int currenValue = scrollBar->value();

    if (maxScrollValue - currenValue <= 0) {
      // 滚动到底部，加载
      qDebug() << "load more chat contact";
      emit sign_loading_chat_user();
    }

    return true;
  }

  return QListWidget::eventFilter(watched, event);
}
