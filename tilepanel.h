#ifndef TILEPANEL_H
#define TILEPANEL_H

#include <QGraphicsView>
#include <QGraphicsScene>

class TilePanel: public QGraphicsScene
{
public:
    TilePanel(int p, QWidget *parent = 0);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    int owner;
};

#endif // TILEPANEL_H
