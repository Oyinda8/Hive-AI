#ifndef TILE_H
#define TILE_H

#include <QGraphicsItem>
#include <QGraphicsObject>
#include <QGraphicsPolygonItem>
#include <QGraphicsPixmapItem>
#include <stdlib.h>
#include <set>


class Tile;
typedef std::pair<int,int> copair;
typedef std::pair<Tile*,copair> movePair;

class Tile: public QGraphicsPixmapItem
{


public:
    Tile(QGraphicsItem *parent=0);

    //MEMBER ATTRIBUTES

    // x and ycoordinate relative to the board
    int x;
    int y;

    //x and y coordinates based on possible board orientations for calculating game state
    int xRelative;
    int yRelative;


    std::set<movePair> possibleMoves;//a container of all possible moves a tile can make
    bool movesCurrent;//a flag for whether the possible moves have been computed since the last move made


    bool immobile = false;//flag for whether immobilized by pillbug



    //METHODS
    copair getSpace(int);//used to get coordinates of adjacent spaces
    copair getSpace(int,copair);//used to get coordinates of spaces adjacent to specified copair
    Tile* getNeighbor(int);//used to get pointer to neighboring tile
    Tile* getNeighbor(int,copair);//used to get pointer to neighboring tile if current tile has been moved to specified copair


    bool isConnected(std::map<copair,Tile*>);


    //GetMoves() calls the appropriate function based on tiletype or, in the case of the mosquito, tiletypes of neighbors
    virtual void GetMoves(){};

    void GetQueenMoves();
    void GetSpiderMoves();
    void GetBeetleMoves();
    void GetGrasshopperMoves();
    void GetAntMoves();
    void GetLadybugMoves();
    void GetPillbugMoves();

    QGraphicsPolygonItem *hex;

};


class Queen: public Tile
{

};

class Spider: public Tile
{

};

class Beetle: public Tile
{
public:
    Tile *tileBeneath = nullptr;//
};

class Grasshopper: public Tile
{

};

class Ant: public Tile
{

};

class Mosquito: public Tile
{

};

class Ladybug: public Tile
{

};

class Pillbug: public Tile
{

};



#endif // TILE_H
