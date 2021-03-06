#ifndef GBMUSCREEN_H
#define GBMUSCREEN_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QPixmap>
#include <iostream>
#include <memory>

#define GB_WIDTH 160
#define GB_HEIGTH 144
#define FIT_VIEW_W 0
#define FIT_VIEW_H 28
#define SKIP_TOOLBAR 18

class Helper;

class GbmuScreen : public QGraphicsView {
  Q_OBJECT

 private:
  QWidget *_parent;
  std::unique_ptr<QGraphicsScene> _scene;
  bool _do_resize = false;
 public slots:
  void updateGbScreen(void);

 public:
  GbmuScreen(QWidget *parent);
  void do_resize() { _do_resize = true; }
};

#endif
