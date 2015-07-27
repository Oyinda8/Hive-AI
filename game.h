#ifndef GAME_H
#define GAME_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <stdint.h>
#include <set>
#include "tile.h"


struct game_state;


class Game : public QGraphicsView
{
public:
    Game();

    //MEMBER ATTRIBUTES
    game_state *currentGameState;
    game_state *prevGameState;
    float currentValue = .5;
    float prevValue = .5;

    std::set<Tile*> tileSet;//reference to every placed tile
    std::map<copair,Tile*> tileMap;// map coordinates to tiles

    //METHODS
    void UpdateGameState();

private:
    //MEMBER ATTRIBUTES
    QGraphicsScene *Scene;
    bool player1Turn;

    //methods
    void start();

};

enum direction
{
    north,
    northe,
    southe,
    south,
    southw,
    northw
};


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
