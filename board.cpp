#include "board.h"
#include "game.h"
#include <QDebug>

extern Game *game;
Board::Board(QGraphicsView *parent):QGraphicsScene(parent)
{


}


void Board::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    event->accept();

    int i,j,x,y;
    float xs,ys,xb,yb;

    //mouse position in scene coordinates
    xs = event->scenePos().x();
    ys = event->scenePos().y();

    //mouse position in board coordinates
    xb = (xs-xc)/(qSqrt(3)*HEX_SCALE);
    yb =  -(ys-yc)/HEX_SCALE;

    //rounded values;
    int xr = round(xb);
    int yr = round(yb);

    //find hex that point clicked is closest to
    float dist,currDist;
    dist = HEX_SCALE*5;
    for(i = xr-1;i<xr+2;i++)
    {
        for(j=yr-1;j<yr+2;j++)
        {
            if ((i+j)%2==0)
            {
                currDist = qSqrt(qPow((xs-(i*(qSqrt(3)*HEX_SCALE)+xc)),2)+qPow((ys-(-j*HEX_SCALE+yc)),2));

                if (currDist<dist)
                {
                    dist = currDist;
                    x = i;
                    y = j;

                }
            }
        }
    }

    //qDebug()<<"xs: " << xs << " ys: " << ys;
    game->handleClick(x,y);




}
