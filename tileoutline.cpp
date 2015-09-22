#include "tileoutline.h"
#include "tile.h"
#include <qmath.h>
#include <QDebug>
#include <QGraphicsItem>


TileOutline::TileOutline(QGraphicsItem *parent):QGraphicsPolygonItem(parent)
{

    QPen pen(Qt::blue,2);
    setPen(pen);

    //set points for hexagon
    QVector<QPointF> points;
    points << QPointF(-1.0/(qSqrt(3)),-1) << QPointF(1.0/(qSqrt(3)),-1) << QPointF(2.0/(qSqrt(3)),0)
           << QPointF(1.0/(qSqrt(3)),1) << QPointF(-1.0/(qSqrt(3)),1) << QPointF(-2.0/(qSqrt(3)),0);

    //scale hexagon
    for(size_t i = 0,n = points.size();i<n;i++)
    {
        points[i]*=HEX_SCALE;
    }

    setPolygon(QPolygonF(points));

}



TileOutline::TileOutline(copair xy, QGraphicsItem *parent):QGraphicsPolygonItem(parent)
{

    QPen pen(Qt::blue,2);
    setPen(pen);

    if((xy.first+xy.second)%2)
        throw(1);

    //set points for hexagon
    QVector<QPointF> points;
    points << QPointF(-1.0/(qSqrt(3)),-1) << QPointF(1.0/(qSqrt(3)),-1) << QPointF(2.0/(qSqrt(3)),0)
           << QPointF(1.0/(qSqrt(3)),1) << QPointF(-1.0/(qSqrt(3)),1) << QPointF(-2.0/(qSqrt(3)),0);

    //scale hexagon
    for(size_t i = 0,n = points.size();i<n;i++)
    {
        points[i]*=HEX_SCALE;
    }

    setPolygon(QPolygonF(points));

    int xb=xy.first,yb=xy.second;//board coordinates
    float xs,ys; //screen coordinates

    xs = HEX_SCALE*qSqrt(3)*xb+xc;
    ys = HEX_SCALE*(-yb)+yc;

    //qDebug()<<"tile created at "<<xs<<","<<ys;

    setPos(xs,ys);
}

TileOutline::TileOutline(Qt::GlobalColor color, QGraphicsItem *parent):QGraphicsPolygonItem(parent)
{
    QPen pen(color,2);
    setPen(pen);

    //set points for hexagon
    QVector<QPointF> points;
    points << QPointF(-1.0/(qSqrt(3)),-1) << QPointF(1.0/(qSqrt(3)),-1) << QPointF(2.0/(qSqrt(3)),0)
           << QPointF(1.0/(qSqrt(3)),1) << QPointF(-1.0/(qSqrt(3)),1) << QPointF(-2.0/(qSqrt(3)),0);

    //scale hexagon
    for(size_t i = 0,n = points.size();i<n;i++)
    {
        points[i]*=HEX_SCALE;
    }

    setPolygon(QPolygonF(points));
}


TileOutline::TileOutline(Qt::GlobalColor color, copair xy, QGraphicsItem *parent):QGraphicsPolygonItem(parent)
{
    QPen pen(color,2);
    setPen(pen);

    if((xy.first+xy.second)%2)
        throw(1);

    //set points for hexagon
    QVector<QPointF> points;
    points << QPointF(-1.0/(qSqrt(3)),-1) << QPointF(1.0/(qSqrt(3)),-1) << QPointF(2.0/(qSqrt(3)),0)
           << QPointF(1.0/(qSqrt(3)),1) << QPointF(-1.0/(qSqrt(3)),1) << QPointF(-2.0/(qSqrt(3)),0);

    //scale hexagon
    for(size_t i = 0,n = points.size();i<n;i++)
    {
        points[i]*=HEX_SCALE;
    }

    setPolygon(QPolygonF(points));

    int xb=xy.first,yb=xy.second;//board coordinates
    float xs,ys; //screen coordinates

    xs = HEX_SCALE*qSqrt(3)*xb+xc;
    ys = HEX_SCALE*(-yb)+yc;

    //qDebug()<<"tile created at "<<xs<<","<<ys;

    setPos(xs,ys);
}




