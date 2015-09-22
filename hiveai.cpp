#include "hiveai.h"
#include "game.h"
#include <stdlib.h>
#include <time.h>
#include <tuple>


extern Game *game;
HiveAI::HiveAI()
{
}


void HiveAI::GetMoveList()
{
    moveList.clear();//clear current move list

    //get list of moves that involve placing a tile near the opponent queen
    std::deque<Tile*>::iterator tileIter;
    std::set<copair>::iterator moveIter;

    Tile *tile;
    bool adjacent;

    for(tileIter=(game->tileDeck).begin(); tileIter != (game->tileDeck).end(); tileIter++)
    {
        adjacent = false;
        tile = *tileIter;
        int i,j;

        //check if tile is already next to opponent queen
        for(i = 0;i<6;i++)
        {
            if(tile->getNeighbor(i)==game->GetQueen(1-game->GetTurn()))
            {
                adjacent = true;
                break;
            }
        }

        if(adjacent)
            continue;


        for(moveIter = (tile->possibleMoves).begin(); moveIter != (tile->possibleMoves).end(); moveIter++)
        {
            copair move = *moveIter;

            for(j=0;j<6;j++)
            {
                if(move == game->GetQueen(1-game->GetTurn())->getSpace(j))
                    moveList.push_back(std::make_pair(tile,*moveIter));

            }
        }
    }


}


void QueenSeeker::DecideMove()
{

    float exploreChance = 2; //percent probability that queen seeking move isn't favored
    float roll = (rand()%100);
    Tile *tile;
    copair move;
    int N, M = rand();

    std::deque<std::pair<Tile*,copair>>::iterator deckIter;



    if(roll > exploreChance && moveList.size())
    {
        M = M%(moveList.size());
        deckIter = moveList.begin()+=M;

        tile = (*deckIter).first;
        move = (*deckIter).second;
        game->makeMove(tile,move);
        return;
    }







    do
    {
        N = rand();
        tile = game->tileDeck[N%(game->tileDeck.size())];
    }while(tile->possibleMoves.size()==0);

    std::set<copair>::iterator iter;
    iter = tile->possibleMoves.begin();
    M=M%(tile->possibleMoves.size());
    for(int i=1;i<M;i++)
    {
        iter++;
    }
    move = *iter;

    game->makeMove(tile,move);
}

QueenSeeker::QueenSeeker(int p)
{
    srand(time(NULL));
    playerNumber = p;
}

void QueenSeeker::play()
{
    GetMoveList();
    DecideMove();
}

game_state HiveAI::GetGameState(std::map<copair, std::deque<Tile *> > tilemap)
{
    std::map<int,std::map<int,std::deque<Tile*>>> xMap;
    std::map<int,std::deque<Tile*>> yMap;
    std::deque<Tile*> tileDeck;
    Tile* tile;

    int i; //for looping over all 12 possible orientations
    int x,y,xr,yr; //integers related to calculating relaive x and y values for each orientation
    int n; //tile counter
    int t; //used in computing T
    int g; //used in computing G
    int c; //used in computing C
    int m0,l0,p0,mult0,b0,m1,l1,p1,mult1,b1; //used in computing Q

    copair xy;

    std::map<copair,std::deque<Tile*>>::iterator mapIter;
    std::map<int,std::map<int,std::deque<Tile*>>>::iterator xIter;
    std::map<int,std::deque<Tile*>>::iterator yIter;
    std::deque<Tile*>::iterator deckIter;

    game_state blankGameState;
    game_state gameState;
    game_state minGameState;


    for(i=0;i<12;i++)
    {
        qDebug()<<"";

        gameState = blankGameState;
        xMap.clear();

        //reset tile counter and qmlp integers
        n = -1;
        m0 = 1; l0 = 2; p0 = 3; mult0 = 1; b0 = 0;
        m1 = 1; l1 = 2; p1 = 3; mult1 = 1; b1 = 0;

        //sort tile-stacks by relative x and relative y's
        for(mapIter=tilemap.begin();mapIter!=tilemap.end();mapIter++)
        {
            xy = mapIter->first;
            x = xy.first;
            y = xy.second;

            //calculate relative x and relative y
            switch(i%3)
            {
            case 0:
                xr = y;
                yr = (x-y)/2;
                break;
            case 1:
                xr = (x+y)/2;
                yr = -y;
                break;
            case 2:
                xr = (x-y)/2;
                yr = -(x+y)/2;
                break;

            }

            if(i%2)
                xr = -xr;
            if((i%4)>2)
                yr = -yr;


            //add map for current realtive x value if it doesn't yet exist
            if(xMap.find(xr) != xMap.end())
                xMap[xr] = yMap;

            //map current relative y value to corresponding tile-stack
            xMap[xr][yr] = mapIter->second;

        }

        //iterate through all relative x's in order
        for(xIter = xMap.begin();xIter!=xMap.end();xIter++)
        {

            //iterate through all corresponding relative y's in order;
            yMap = xIter->second;
            for(yIter = yMap.begin();yIter!=yMap.end();yIter++)
            {

                tileDeck = yIter->second;
                for(deckIter = tileDeck.begin(); deckIter != tileDeck.end(); deckIter++)
                {
                    tile = *deckIter;
                    n++;


                    //compute T
                    switch(tile->type)
                    {
                    case mosquito: case ladybug: case pillbug:
                        t = 0;
                        break;
                    default:
                        t = tile->type;
                        break;

                    }


                    switch(n/6)
                    {
                    case 0:
                        gameState.T0 += t*pow(5,n);
                        break;
                    case 1:
                        gameState.T1 += t*pow(5,n-6);
                        break;
                    case 2:
                        gameState.T2 += t*pow(5,n-12);
                        break;
                    case 3:
                        gameState.T3 += t*pow(5,n-18);
                        break;
                    case 4:
                        gameState.T4 += t*pow(5,n-24);
                        break;

                    }


                    //compute G
                    g = 0;
                    if(deckIter == tileDeck.begin())//only compute gap/linefeed for top tile in tile-stack
                    {

                        if(yMap.end()-- == yIter && --xMap.end() != xIter)//if last y value for given x and not last x value in xMap
                        {
                            g = 2;
                        }else if(yMap.find(yIter->first+1)!= yMap.end()){ //if next y value is present for given x
                            g = 1;
                        }

                        switch(n/7)
                        {
                        case 0:
                            gameState.G0 += g*pow(3,n);
                            break;
                        case 1:
                            gameState.G1 += g*pow(3,n-7);
                            break;
                        case 2:
                            gameState.G2 += g*pow(3,n-14);
                        }
                    }


                    //compute C
                    c = 0;
                    if(deckIter == tileDeck.begin())//only compute connections for top tile in tile-stack
                    {
                        yr = xIter->first;
                        if(xIter != xMap.begin())
                        {
                            xIter--;
                            if((xIter->second).find(yr) != (xIter->second).end())
                            {
                                c += 1;
                            }
                            xIter++;
                        }

                        if(xIter != --(xMap.end()))
                        {
                            xIter++;
                            if((xIter->second).find(yr) != (xIter->second).end())
                            {
                                c += 2;
                            }
                            xIter--;
                        }

                        switch(n/4)
                        {
                        case 0:
                            gameState.C0 += c*pow(4,n);
                            break;
                        case 1:
                            gameState.C1 += c*pow(4,n-4);
                            break;
                        case 2:
                            gameState.C2 += c*pow(4,n-8);
                            break;
                        case 3:
                            gameState.C3 += c*pow(4,n-12);
                            break;
                        case 4:
                            gameState.C4 += c*pow(4,n-16);
                            break;
                        case 5:
                            gameState.C5 += c*pow(4,n-20);
                            break;
                        case 6:
                            gameState.C6 += c*pow(4,n-24);
                            break;
                        }
                    }




                    //compute Q
                    if(tile->owner == playerNumber)
                    {
                        switch(tile->type)
                        {
                        case queen:
                            m0--;
                            l0--;
                            p0--;
                            mult0 = mult0==1?4:12;
                            break;

                        case mosquito:
                            gameState.Q0 += mult0*m0;
                            l0--;
                            p0--;
                            mult0 = mult0==1?4:12;

                            //check if on top of tile
                            if(deckIter+1 != tileDeck.end())
                            {
                                gameState.Q0 += 24*b0;
                            }

                            b0++;
                            break;

                        case ladybug:
                            p0--;
                            gameState.Q0 += mult0*l0;
                            break;

                        case pillbug:
                            gameState.Q0 += mult0*p0;
                            break;

                        case beetle:
                            if(deckIter+1 != tileDeck.end())
                            {
                                gameState.Q0 += 24*b0;
                            }
                            b0++;
                            break;
                        default:;
                        }
                    }else{
                        switch(tile->type)
                        {
                        case queen:
                            m1--;
                            l1--;
                            p1--;
                            mult1 = mult1==1?4:12;
                            break;

                        case mosquito:
                            gameState.Q1 += mult1*m1;
                            l1--;
                            p1--;
                            mult1 = mult1==1?4:12;

                            //check if on top of tile
                            if(deckIter+1 != tileDeck.end())
                            {
                                gameState.Q1 += 24*b1;
                            }

                            b1++;
                            break;

                        case ladybug:
                            p1--;
                            gameState.Q1 += mult1*l1;
                            break;

                        case pillbug:
                            gameState.Q1 += mult1*p1;
                            break;

                        case beetle:
                            if(deckIter+1 != tileDeck.end())
                            {
                                gameState.Q1 += 24*b1;
                            }
                            b1++;
                            break;
                        default:;
                        }
                    }


                    //compute O
                    switch(n/16)
                    {
                    case 0:
                        if (tile->owner == playerNumber)
                            gameState.O0 += pow(2,n);
                    case 1:
                        if (tile->owner == playerNumber)
                            gameState.O1 += pow(2,n-16);
                    }


                }


            }
        }


        //check if gameState computed in current orientaiton yields minimal T
        if( i == 0 || gameState < minGameState)
            minGameState = gameState;



    }

    qDebug()<<"T0: " << minGameState.T0;
    return minGameState;



}





bool operator <(const game_state& x, const game_state & y)
{
    return std::tie(x.Q0,x.Q1,x.C6,x.T4,x.C5,x.T3,x.T2,x.T1,x.T0,x.C4,x.C3,x.C2,x.C1,x.C0,x.G2,x.G1,x.G0,x.O1,x.O0)
            <
            std::tie(y.Q0,y.Q1,y.C6,y.T4,y.C5,y.T3,y.T2,y.T1,y.T0,y.C4,y.C3,y.C2,y.C1,y.C0,y.G2,y.G1,y.G0,y.O1,y.O0);
}


GameStateTester::GameStateTester(int p)
{
    playerNumber = p;
}

void GameStateTester::play()
{
    //qDebug() << "updating game state";
    prevGameState = currentGameState;
    currentGameState = GetGameState(game->tileMap);
    game->UpdateGameState();
}

void GameStateTester::DecideMove()
{

}
