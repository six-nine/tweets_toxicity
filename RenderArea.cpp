#include <QPainter>
#include "RenderArea.h"

RenderArea::RenderArea(QVector<QPair<QPolygon, QColor>> &polygons, QWidget *parent) {
  this->setFixedSize(QSize(1300, 700));
  this->polygons = polygons;
}
QSize RenderArea::sizeHint() const {
  return QSize(1400, 1200);
}
void RenderArea::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  for (auto & polygon : polygons){
    painter.setBrush(QBrush(polygon.second));
    painter.setPen(QPen(polygon.second));
    painter.drawPolygon(polygon.first);
  }
}
QSize RenderArea::minimumSizeHint() const {
  return QSize(800, 600);
}
