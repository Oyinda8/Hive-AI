#include "game.h"
#include "tile.h"
#include <QDebug>
#include <qmath.h>
#include <iostream>


Game::Game()
{
    drawScene();
    start();


}



void Game::getPlaceable()
{


    if (turnNumber==0)
    {
        placeableSpots.insert(copair(0,0));
        return;
    }else if(turnNumber == 1)
    {
        placeableSpots.insert(copair(0,2));
        return;
    }

    int debugInt = 0;
    std::set<Tile*>::iterator iter;
    int i;


    //add all the positions adjacent to the current player's tiles
    for(iter = tileSet.begin();iter!=tileSet.end();iter++)
    {
        if ((*iter)->owner == turn)
        {
            for(i=0;i<6;i++)
            {
                if(!(*iter)->getNeighbor(i))
                    placeableSpots.insert((*iter)->getSpace(i));
            }
        }
    }

    std::set<copair>::iterator iter2;
    iter2 = placeableSpots.begin();
    bool removeSpot;
    Tile *tile;

    //remove all positions adjacent to the other player's tiles
    while(iter2!=placeableSpots.end())
    {
        debugInt++;
        removeSpot = false;
        for(i=0;i<6;i++)
        {

            tile = tile->getNeighbor(i,*iter2);
            //qDebug()<<*tile;


            if (tile&&tile->owner != turn)
            {
                removeSpot = true;
                break;
            }
        }

        if (removeSpot)
        {
            iter2 = placeableSpots.erase(iter2);
        }else{
            iter2++;
        }

    }


}

void Game::deselect()
{
    //delete tile if not placed
    if(selectedTile && tileSet.find(selectedTile)==tileSet.end())
        delete selectedTile;

    //remove outlines
    clearOutlines();
    tileOutlineSet.clear();

    selectedTile = nullptr;
    qDebug() << tileMap;
}
void Game::clearOutlines()
{

    std::set<TileOutline*>::iterator iter = tileOutlineSet.begin();
    //delete outlines
    while(iter!= tileOutlineSet.end())
    {
        delete (*iter);
        iter++;
    }
    tileOutlineSet.clear();

}

void Game::start()
{
    getPlaceable();



}

void Game::endTurn()
{

    //qDebug()<<"\n\n"<<tileMap;

    clearOutlines();
    deselect();
    turn = 1-turn;//toggle turn

    //only first 6 turns matter for queen placement
    if (turnNumber<6)
        turnNumber++;

    //get placeable spots for next turn
    placeableSpots.clear();
    getPlaceable();


    Tile *tile;

    //get tile moves for next turn
    std::set<Tile*>::iterator iter=tileSet.begin();
    while(iter!=tileSet.end())
    {
        tile = *iter;

        tile->possibleMoves.clear();
        tile->debugString.clear();

        iter++;
    }

    //if((turnNumber==4 && nQueen0) ||(turnNumber==5 && nQueen1) )
    // return;

    iter = tileSet.begin();


    while(iter!=tileSet.end())
    {
        tile = *iter;
        //no moves if under another tile or last tile moves
        if((tileMap[copair(tile->x,tile->y)]).front()==tile && tile->owner==turn && tile != lastTile)
            tile->GetMoves();
        iter++;
    }



}




void Game::showPossibleMoves()
{
    clearOutlines();
    TileOutline *tileOutline;

    std::set<copair>::iterator iter = (selectedTile->possibleMoves).begin();

    while(iter!= (selectedTile->possibleMoves).end())
    {
        tileOutline = new TileOutline(*iter);
        Scene->addItem(tileOutline);
        tileOutlineSet.insert(tileOutline);
        iter++;
    }



}

void Game::handleClick(int x, int y)
{
    //if move in the set of selected tile, make move
    if(selectedTile && selectedTile->possibleMoves.find(copair(x,y))!=selectedTile->possibleMoves.end())
    {
        try
        {
            makeMove(selectedTile,copair(x,y));
        }catch(int &excep){
            qDebug()<< "exception " <<excep<< " in handleClick";
            throw(excep);

        }
    }else{
        selectTile(copair(x,y));
    }
    // if no selected tile or not in the the move set of selected tile, try to select another tile
}

void Game::selectTile(copair xy)
{
    int x = xy.first, y = xy.second;
    //deselect any tiles that were already selected
    if(selectedTile)
        deselect();

    //check if clicked spot is a placed tile
    if (tileMap.find(xy)!=tileMap.end())
    {
        selectedTile = (tileMap[xy]).front();
        qDebug() << selectedTile->debugString;
        showPossibleMoves();
    }

    //check if clicked spot is an icon
    if((turn==0 && x == iconsLeft) || (turn==1 && x == iconsRight))
    {


        switch((iconsTop-y)/2)
        {
        case 0:

            if ((!queen0 && !turn) || (!queen1 && turn))
                selectedTile = new Queen(turn,copair(x,y));
            break;
        case 1:
            if((nSpider0 && !turn) || (nSpider1 && turn))
                selectedTile = new Spider(turn,copair(x,y));
            break;
        case 2:
            if ((nGrasshopper0 && !turn) || (nGrasshopper1 && turn))
                selectedTile = new Grasshopper(turn,copair(x,y));
            break;
        case 3:
            if ((nAnt0 && !turn) || (nAnt1 && turn))
                selectedTile = new Ant(turn,copair(x,y));
            break;
        case 4:
            if ((nBeetle0 && !turn) || (nBeetle1 && turn))
                selectedTile = new Beetle(turn,copair(x,y));
            break;
        case 5:
            if ((nMosquito0 && !turn) || (nMosquito1 && turn))
                selectedTile = new Mosquito(turn,copair(x,y));
            break;
        case 6:
            if ((nLadybug0 && !turn) || (nLadybug1 && turn))
                selectedTile = new Ladybug(turn,copair(x,y));
            break;
        case 7:
            if ((nPillbug0 && !turn) || (nPillbug1 && turn))
                selectedTile = new Pillbug(turn,copair(x,y));
            break;
        default:;

        }
        if(selectedTile)
        {
            Scene->addItem(selectedTile);
            selectedTile->possibleMoves = placeableSpots;
        }
    }

    if(selectedTile)
    {

        // qDebug()<< "Selected tile:\nowner: " <<selectedTile->owner
        //       <<"\ntype: "<< static_cast<int>(selectedTile->type);
        showPossibleMoves();
    }

}

void Game::makeMove(Tile *tile, copair xy)
{

    //throw exception if move isn't legal
    if (tile->possibleMoves.find(xy) == tile->possibleMoves.end())
        throw 2;


    //add tile to set
    tileSet.insert(tile);


    //update tileMap
    tile->moveInTileMap(xy,tileMap);

    //move tile image
    tile->moveTo(xy);

    //label as last tile
    lastTile = tile;

    endTurn();

}



void Game::drawScene()
{

    //specify parameters
    float sceneWidth = 1340, sceneHeight = 1340;
    float panelWidth = 120;
    iconsTop = 10, iconsLeft = -8, iconsRight = 20;


    //create scene
    Scene = new Board(this);
    setScene(Scene);
    Scene->setSceneRect(0,0,sceneWidth,sceneHeight);


    //create panels for holding icons for unplayed tiles
    QGraphicsRectItem *leftPanel = new QGraphicsRectItem(0,0,panelWidth,sceneHeight);
    QGraphicsRectItem *rightPanel = new QGraphicsRectItem(sceneWidth-panelWidth,0,panelWidth,sceneHeight);
    Scene->addItem(leftPanel);
    Scene->addItem(rightPanel);


    //create icons for unplayed tiles
    queenIcon0 = new Queen(0,copair(iconsLeft,iconsTop));
    spiderIcon0 = new Spider(0,copair(iconsLeft,iconsTop-2));
    grasshopperIcon0 = new Grasshopper(0,copair(iconsLeft,iconsTop-4));
    antIcon0 = new Ant(0,copair(iconsLeft,iconsTop-6));
    beetleIcon0 = new Beetle(0,copair(iconsLeft,iconsTop-8));
    mosquitoIcon0 = new Mosquito(0,copair(iconsLeft,iconsTop-10));
    ladybugIcon0 = new Ladybug(0,copair(iconsLeft,iconsTop-12));
    pillbugIcon0 = new Pillbug(0,copair(iconsLeft,iconsTop-14));
    queenIcon1 = new Queen(1,copair(iconsRight,iconsTop));
    spiderIcon1 = new Spider(1,copair(iconsRight,iconsTop-2));
    grasshopperIcon1 = new Grasshopper(1,copair(iconsRight,iconsTop-4));
    antIcon1 = new Ant(1,copair(iconsRight,iconsTop-6));
    beetleIcon1 = new Beetle(1,copair(iconsRight,iconsTop-8));
    mosquitoIcon1 = new Mosquito(1,copair(iconsRight,iconsTop-10));
    ladybugIcon1 = new Ladybug(1,copair(iconsRight,iconsTop-12));
    pillbugIcon1 = new Pillbug(1,copair(iconsRight,iconsTop-14));

    Scene->addItem(queenIcon0);
    Scene->addItem(spiderIcon0);
    Scene->addItem(grasshopperIcon0);
    Scene->addItem(beetleIcon0);
    Scene->addItem(antIcon0);
    Scene->addItem(mosquitoIcon0);
    Scene->addItem(ladybugIcon0);
    Scene->addItem(pillbugIcon0);
    Scene->addItem(queenIcon1);
    Scene->addItem(spiderIcon1);
    Scene->addItem(grasshopperIcon1);
    Scene->addItem(beetleIcon1);
    Scene->addItem(antIcon1);
    Scene->addItem(mosquitoIcon1);
    Scene->addItem(ladybugIcon1);
    Scene->addItem(pillbugIcon1);

}


QDebug operator <<(QDebug d, std::map<copair,std::deque<Tile*>> &tilemap)
{

    Tile *tile;
    copair xy;

    d<<"Tilemap:\n";
    std::map<copair,std::deque<Tile*>>::iterator  mapIter = tilemap.begin();
    std::deque<Tile*>::iterator dequeIter;

    if (mapIter == tilemap.end())
    {
        d.nospace()<< "no tiles placed.";
        return d;
    }

    //loop through the tile map and output each copair and tile type/owner
    while(mapIter!=tilemap.end())
    {

        xy = mapIter->first;
        d.nospace()<<"{"<<xy;
        dequeIter = (mapIter->second).begin();
        while(dequeIter != (mapIter->second).end())
        {

            tile = *dequeIter;
            d.nospace()<<" "<<*tile;
            dequeIter++;
        }

        d.nospace()<<"}";
        mapIter++;

    }


    return d;

}

QDebug operator <<(QDebug d, copair xy)
{
    //output the coordinate pair
    d.nospace()<<"("<<xy.first<<","<<xy.second<<")";
    return d;
}



QTextStream & operator <<(QTextStream & stream, std::map<copair,std::deque<Tile*>> &tilemap)
{

    Tile *tile;
    copair xy;

    stream<<"Tilemap:\n";
    std::map<copair,std::deque<Tile*>>::iterator  mapIter = tilemap.begin();
    std::deque<Tile*>::iterator dequeIter;

    if (mapIter == tilemap.end())
    {
        stream<< "no tiles placed.";
        return stream;
    }

    //loop through the tile map and output each copair and tile type/owner
    while(mapIter!=tilemap.end())
    {


        xy = mapIter->first;


        stream<<"{"<<xy;
        dequeIter = (mapIter->second).begin();
        while(dequeIter != (mapIter->second).end())
        {
            tile = *dequeIter;
            stream<<" "<<*tile;
            dequeIter++;
        }

        mapIter++;

    }
    return stream;

}

QTextStream & operator <<(QTextStream &stream, copair &xy)
{
    stream << "(" <<xy.first << "," << xy.second << ")";
    return stream;
}
