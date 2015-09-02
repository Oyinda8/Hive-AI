#ifndef TILE_H
#define TILE_H

#include <QGraphicsItem>
#include <QGraphicsObject>
#include <QGraphicsPolygonItem>
#include <QGraphicsPixmapItem>
#include <stdlib.h>
#include <deque>
#include <set>
#include <QDebug>
#include <QPainter>
#include <QTextStream>

const float HEX_SCALE(25); //scale of tiles
const float xc(400); //x center position
const float yc(400); //y center position

class Tile;
typedef std::pair<int,int> copair;
enum Tiletype
{
    queen = 0,
    spider,
    grasshopper,
    ant,
    beetle,
    mosquito,
    ladybug,
    pillbug
};


class Tile: public QGraphicsPolygonItem
{


public:
    Tile(int Owner, Tiletype type, copair xy, QGraphicsItem *parent=0);


    //MEMBER ATTRIBUTES
    int owner;//indicates who owns the tile
    Tiletype type;//type of tile

    // x and ycoordinate relative to the board
    int x;
    int y;

    //x and y coordinates based on possible board orientations for calculating game state
    int xRelative;
    int yRelative;


    std::set<copair> possibleMoves;//a container of all possible moves a tile can make



    QGraphicsPolygonItem *hex;//for image of tile


    QString debugString;
    QTextStream debugStream;



    //METHODS
    copair getSpace(int);//used to get coordinates of adjacent spaces
    copair getSpace(int,copair);//used to get coordinates of spaces adjacent to specified copair
    Tile* getNeighbor(int);//used to get pointer to neighboring tile
    Tile* getNeighbor(int,copair);//used to get pointer to neighboring tile if current tile has been moved to specified copair

    //used to get height of a given space
    int getHeight();
    int getHeight(copair);
    int getHeight(std::map<copair,std::deque<Tile*>> &tilemap);
    int getHeight(copair, std::map<copair,std::deque<Tile*>> &tilemap);

    bool isConnected(std::map<copair,std::deque<Tile*>>);

    void moveTo(copair xy);

    void moveInTileMap(copair space, std::map<copair,std::deque<Tile*>> &tilemap);
    //GetMoves() calls the appropriate function based on tiletype or, in the case of the mosquito, tiletypes of neighbors
    virtual void GetMoves(){};

    void GetQueenMoves();
    void GetSpiderMoves();
    void GetBeetleMoves();
    void GetGrasshopperMoves();
    void GetAntMoves();
    void GetLadybugMoves();
    void GetPillbugMoves();



    friend QDebug operator <<(QDebug d, Tile &tile);
    friend QTextStream & operator<<(QTextStream &stream, Tile &tile);
};







class Queen: public Tile
{
public:
    Queen(int Owner, copair xy, QGraphicsItem *parent=0):Tile(Owner,queen,xy,parent){}
    void GetMoves();
};

class Spider: public Tile
{
    public:
    Spider(int Owner, copair xy, QGraphicsItem *parent=0):Tile(Owner,spider,xy,parent){}
    void GetMoves();
};



class Grasshopper: public Tile
{
    public:
    Grasshopper(int Owner, copair xy, QGraphicsItem *parent=0):Tile(Owner,grasshopper,xy,parent){}
    void GetMoves();
};

class Ant: public Tile
{
    public:
    Ant(int Owner, copair xy, QGraphicsItem *parent=0):Tile(Owner,ant,xy,parent){}
    void GetMoves();
};

class Beetle: public Tile
{
    public:
    Beetle(int Owner, copair xy, QGraphicsItem *parent=0):Tile(Owner,beetle,xy,parent){}
    void GetMoves();

};

class Mosquito: public Tile
{
    public:
    Mosquito(int Owner, copair xy, QGraphicsItem *parent=0):Tile(Owner,mosquito,xy,parent){}
    void GetMoves();
};

class Ladybug: public Tile
{
    public:
    Ladybug(int Owner, copair xy, QGraphicsItem *parent=0):Tile(Owner,ladybug,xy,parent){}
    void GetMoves();
};

class Pillbug: public Tile
{
    public:
    Pillbug(int Owner, copair xy, QGraphicsItem *parent=0):Tile(Owner,pillbug,xy,parent){}
    void GetMoves();
};



#endif // TILE_H
