#ifndef BOARD_H
#define BOARD_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <qmath.h>
#include <QDebug>




class Board: public QGraphicsScene
{

public:
    Board(QGraphicsView *parent = 0);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

};

#endif // BOARD_H
