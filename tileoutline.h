#ifndef TILEOUTLINE_H
#define TILEOUTLINE_H

#include <QVector>
#include <QPointF>
#include <QGraphicsPolygonItem>
#include <QGraphicsItem>
#include <QPolygonF>
#include <QPainter>
#include "tile.h"



class TileOutline:public QGraphicsPolygonItem
{
public:
    TileOutline(QGraphicsItem *parent=0);
    TileOutline(copair,QGraphicsItem *parent=0);
    TileOutline(Qt::GlobalColor color, QGraphicsItem *parent);
    TileOutline(Qt::GlobalColor color, copair,QGraphicsItem *parent=0);

};

#endif // TILEOUTLINE_H
