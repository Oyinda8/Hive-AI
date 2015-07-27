#include "game.h"
#include "tile.h"

Game::Game()
{
    Scene = new QGraphicsScene(this);
    setScene(Scene);
    Scene->setSceneRect(0,0,800,600);
    Tile *tile1 = new Tile();
    Scene->addItem(tile1);
    tile1->setPos(200,200);




}
