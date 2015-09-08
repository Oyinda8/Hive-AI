#ifndef GAME_H
#define GAME_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <stdint.h>
#include <set>
#include "tile.h"
#include "tileoutline.h"
#include <QPushButton>
#include <QMouseEvent>
#include "board.h"
#include <ostream>





struct game_state;
//class TileOutline;





class Game : public QGraphicsView
{
public:
    Game(QWidget* parent=0);

    //MEMBER ATTRIBUTES
    game_state *currentGameState;
    game_state *prevGameState;
    float currentValue = .5;
    float prevValue = .5;

    bool noMoves = true;

    std::set<Tile*> tileSet;//reference to every placed tile
    std::map<copair,std::deque<Tile*>> tileMap;// map coordinates to tiles

    std::set<copair> placeableSpots;//set of all places a new tile can be placed
    std::set<TileOutline*> tileOutlineSet;//container for tileOutlines

    Tile* lastTile = nullptr;//last tile moved

    //METHODS
    void UpdateGameState();
    void handleClick(int x, int y);
    void makeMove(Tile* tile, copair xy);
    void selectTile(copair xy);
    void clearOutlines();

private:
    //MEMBER ATTRIBUTES
    Board *Scene;
    int8_t turn = 0;//indicates which player's turn it is.
    int8_t turnNumber = 0;//indicates which turn it is up to turn 5.


    //flags for whether queens have been placed
    bool Queen0Placed = false;
    bool Queen1Placed = false;


    int iconsTop, iconsLeft, iconsRight;//parameters for location of new tile icons

    Tile* selectedTile=nullptr;


    //new tile icons
    Tile *queenIcon0, *queenIcon1, *spiderIcon0, *spiderIcon1,
    *grasshopperIcon0, *grasshopperIcon1, *antIcon0, *antIcon1,
    *beetleIcon0, *beetleIcon1, *mosquitoIcon0, *mosquitoIcon1,
    *ladybugIcon0, *ladybugIcon1, *pillbugIcon0, *pillbugIcon1;

    //number of each tile left
    int nSpider0=2, nGrasshopper0=3, nAnt0=3, nBeetle0=2, nMosquito0=1, nLadybug0=1, nPillbug0=1,
     nSpider1=2, nGrasshopper1=3, nAnt1=3, nBeetle1=2, nMosquito1=1, nLadybug1=1, nPillbug1=1;

    Tile *queen0=nullptr, *queen1=nullptr; //references to queens
    //methods
    void start();
    void getPlaceable();//get list of places a new tile can be placed
    void endTurn();
    void drawScene();
    void deselect();
    void showPossibleMoves();
    void endGame(bool, bool);

    //operator overloads
    friend QDebug operator<< (QDebug d, std::map<copair,std::deque<Tile*>> &tilemap);

};

QDebug operator <<(QDebug d, std::map<copair,std::deque<Tile*>>  &tilemap);
QDebug operator <<(QDebug d, copair xy);

QTextStream& operator <<(QTextStream& stream, std::map<copair,std::deque<Tile*>> &tilemap);
QTextStream& operator <<(QTextStream& stream, copair &xy);


/*enum direction
{
    north,
    northe,
    southe,
    south,
    southw,
    northw
};*/


struct game_state{
    game_state():T0(0), T1(0), T2(0), T3(0), T4(0), G0(0), G1(0), G2(0), C0(0),
        C1(0), C2(0), C3(0), C4(0), C5(0), C6(0), Q0(0), Q1(0), O1(0), O2(0){}

    // Tile order
    int16_t T0;
    int16_t T1;
    int16_t T2;
    int16_t T3;
    int16_t T4;

    // Gaps and line ends
    int16_t G0;
    int16_t G1;
    int16_t G2;

    //connections
    int8_t C0;
    int8_t C1;
    int8_t C2;
    int8_t C3;
    int8_t C4;
    int8_t C5;
    int8_t C6;

    //QMLP order and beetle stacking
    int16_t Q0;
    int16_t Q1;

    //ownershp of tiles
    int16_t O1;
    int8_t O2;

};




#endif // GAME_H
