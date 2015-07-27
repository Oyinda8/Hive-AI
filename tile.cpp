#include "tile.h"
#include "QPolygonF"
#include "QPointF"
#include <qmath.h>
#include <math.h>
#include <QDebug>
#include "game.h"


extern Game *game;
Tile::Tile(QGraphicsItem *parent):QGraphicsPixmapItem(parent)
{
    //set points for hexagon
    QVector<QPointF> points;
    points << QPointF(-0.25,-sqrt(3)/4) << QPointF(-0.5,0) << QPointF(-0.25,qSqrt(3)/4)
           << QPointF(0.25,qSqrt(3)/4) << QPointF(0.5,0) << QPointF(0.25,-sqrt(3)/4);

    //scale hexagon
    double HEX_SCALE = 60;
    for(size_t i = 0,n = points.size();i<n;i++)
    {
        points[i]*=HEX_SCALE;
    }

    //set hexagon
    hex = new QGraphicsPolygonItem(QPolygonF(points),this);


    //placeholder pixmap
    setPixmap(QPixmap(":/Stuff/scarab.PNG"));

    //center hexagon
    QPointF tileCenter(x+1*boundingRect().width()/2,
                       y+1*boundingRect().height()/2);

    hex->setPos(tileCenter);


}

copair Tile::getSpace(int m)
{
    m = m%6;//to allow wrap around

    switch(m)
    {
    case 0:
        return copair(x,y+2);//north
    case 1:
        return copair(x+1,y+1);//northeast
    case 2:
        return copair(x+1,y-1);//southeast
    case 3:
        return copair(x,y-2);//south
    case 4:
        return copair(x-1,y-1);//southwest
    case 5:
        return copair(x-1,y+1);//northwest
    default:
        return copair(1,0);//this shouldn't be reachable throws a bad copair if so
    }

}

copair Tile::getSpace(int m,copair possibleSpace)
{
    m = m%6;//to allow wrap around
    int px = possibleSpace.first, py = possibleSpace.second;
    switch(m)
    {
    case 0:
        return copair(px,py+2);//north
    case 1:
        return copair(px+1,py+1);//northeast
    case 2:
        return copair(px+1,py-1);//southeast
    case 3:
        return copair(px,py-2);//south
    case 4:
        return copair(px-1,py-1);//southwest
    case 5:
        return copair(px-1,py+1);//northwest
    default:
        return copair(1,0);//this shouldn't be reachable throws a bad copair if so
    }

}



Tile* Tile::getNeighbor(int m)
{
    copair space = getSpace(m);
    if (game->tileMap.find(space)!=game->tileMap.end())
    {
        return game->tileMap[space];
    }
    return nullptr;
}

Tile* Tile::getNeighbor(int m, copair possibleSpace)
{
    copair space = getSpace(m,possibleSpace);
    if (game->tileMap.find(space)!=game->tileMap.end())
    {
        return game->tileMap[space];
    }
    return nullptr;


}

bool Tile::isConnected(std::map<copair, Tile *> possibleTileMap)
{
    std::set<copair> reachable;
    std::map<copair,Tile*>::iterator mapIter = possibleTileMap.begin();
    reachable.insert(mapIter->first);

    std::set<copair>::iterator setIter = reachable.begin();

    int i;
    copair space;
    while(setIter != reachable.end())
    {
        for(i=0;i<6;i++)
        {
            space = getSpace(i);
            if (possibleTileMap.find(space)!=possibleTileMap.end())
            {
                reachable.insert(space);
            }
        }
        setIter++;
    }

    return(reachable.size()==possibleTileMap.size());



}


void Tile::GetQueenMoves()
{

    copair possibleSpace;//coordinates of space queen might occupy
    std::map<copair,Tile*> possibleTileMap;//tile map for computing whether possible moves are legal

    for(int i=1;i<6;i++)
    {
        if(!(getNeighbor(i)) && (getNeighbor(i+1)==nullptr ^ getNeighbor(i-1)==nullptr))
        {
            possibleSpace = getSpace(i);
            possibleTileMap=game->tileMap;
            possibleTileMap[possibleSpace] = this;
            possibleTileMap.erase(copair(x,y));
            if(isConnected(possibleTileMap))
                possibleMoves.insert(movePair(this,possibleSpace));
        }
    }

}

void Tile::GetSpiderMoves()
{
    copair possibleFirstSpace,possibleSecondSpace,possibleSpace;//coordinates of space queen might occupy
    std::map<copair,Tile*> possibleTileMap;//tile map for computing whether possible moves are legal



    int i,j,k;
    for(i=0;i<6;i++)
    {
        if(!(getNeighbor(i)) && (getNeighbor(i+1)==nullptr ^ getNeighbor(i-1)==nullptr))
        {
            possibleTileMap=game->tileMap;
            possibleTileMap.erase(copair(x,y));
            possibleFirstSpace = getSpace(i);
            possibleTileMap[possibleFirstSpace] = this;

            if(isConnected(possibleTileMap))
            {
                for(j=0;j<6;j++)
                {
                    possibleSecondSpace = getSpace(j,possibleFirstSpace);
                    if(!(getNeighbor(j,possibleFirstSpace))  && possibleSecondSpace != possibleFirstSpace
                            &&(getNeighbor(j+1,possibleFirstSpace)==nullptr ^ getNeighbor(j-1,possibleFirstSpace)==nullptr))
                    {
                        possibleTileMap.erase(possibleFirstSpace);
                        possibleTileMap[possibleSecondSpace] = this;
                        if(isConnected(possibleTileMap))
                        {
                            for(k=0;k<6;k++)
                            {
                                possibleSpace = getSpace(k,possibleSecondSpace);
                                if(!(getNeighbor(k,possibleSecondSpace))  && possibleSpace != possibleSecondSpace
                                        &&(getNeighbor(k+1,possibleSecondSpace)==nullptr ^ getNeighbor(k-1,possibleSecondSpace)==nullptr))
                                {
                                    possibleTileMap.erase(possibleSecondSpace);
                                    possibleTileMap[possibleSpace] = this;
                                    if(isConnected(possibleTileMap))
                                        possibleMoves.insert(movePair(this,possibleSpace));
                                }

                            }
                        }

                    }
                }
            }
        }
    }
}
