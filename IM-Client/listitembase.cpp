#include "listitembase.h"
#include <QStyleOption>
#include <QPainter>

ListItemBase::ListItemBase(QWidget *parent) {}

void ListItemBase::setItemType(ListItemType itemType) { _itemType = itemType; }

ListItemType ListItemBase::getItemType() { return _itemType; }

void ListItemBase::paintEvent(QPaintEvent *event) {
  QStyleOption opt;
  opt.init(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
