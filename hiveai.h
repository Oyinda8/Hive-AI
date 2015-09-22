#ifndef HIVEAI_H
#define HIVEAI_H


#include <stdlib.h>
#include <tile.h>
#include <stdint.h>


struct game_state{

    // Tile order
    int16_t T0=0;
    int16_t T1=0;
    int16_t T2=0;
    int16_t T3=0;
    int16_t T4=0;

    // Gaps and line ends
    int16_t G0=0;
    int16_t G1=0;
    int16_t G2=0;

    //connections
    int8_t C0=0;
    int8_t C1=0;
    int8_t C2=0;
    int8_t C3=0;
    int8_t C4=0;
    int8_t C5=0;
    int8_t C6=0;

    //QMLP order and beetle stacking
    int16_t Q0=0;
    int16_t Q1=0;

    //ownershp of tiles
    int16_t O0=0;
    int8_t O1=0;

};

class HiveAI
{
public:
    HiveAI();

    //MEMBER ATTRIBUTES
    std::deque<std::pair<Tile*,copair>> moveList;//list of all move available to A.I.

    int8_t playerNumber; //which player slot is taken by this A.I.

    game_state currentGameState;
    game_state prevGameState;

    //METHODS
    void GetMoveList();
    virtual void DecideMove(){}
    virtual void play(){}
    game_state GetGameState(std::map<copair,std::deque<Tile*>> tilemap);

};

class QueenSeeker: public HiveAI
{
public:
    QueenSeeker(int);
    void DecideMove();
    void play();
};

class GameStateTester: public HiveAI
{
public:
    GameStateTester(int);
    void play();
    void DecideMove();
};




bool operator <(const game_state&, const game_state&);

#endif // HIVEAI_H
