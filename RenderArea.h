#pragma once

#include <QWidget>

class RenderArea : public QWidget {
 public:
  RenderArea(QVector<QPair<QPolygon, QColor>> &polygons, QWidget *parent = nullptr);
  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;
 protected:
  void paintEvent(QPaintEvent *event) override;
 private:
  QVector<QPair<QPolygon, QColor>> polygons;
};
