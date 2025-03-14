#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H

#include <QWidget>
#include <QPaintEvent>
#include "global.h"

class ListItemBase : public QWidget {
  Q_OBJECT
 public:
  explicit ListItemBase(QWidget* parent = nullptr);
  void setItemType(ListItemType itemType);

  ListItemType getItemType();
 protected:
  void paintEvent(QPaintEvent* event) override;
 private:
  ListItemType _itemType;

 public slots:

 signals:
 
};

#endif  // LISTITEMBASE_H
