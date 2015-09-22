#include "tile.h"
#include "QPolygonF"
#include "QPointF"
#include <qmath.h>
#include <math.h>
#include "game.h"
#include <algorithm>
#include <QDebug>
#include <QBitmap>



extern Game *game;
Tile::Tile(int Owner,Tiletype Type, copair xy,QGraphicsItem *parent):QGraphicsPolygonItem(parent),owner(Owner),type(Type)
{
    debugStream.setString(&debugString);//bind debug string to its stream;

    QBrush brush;
    if(owner)
        brush.setColor(Qt::green);
    else
        brush.setColor(Qt::yellow);

    brush.setStyle(Qt::SolidPattern);
    setBrush(brush);




    //qDebug() <<"tile created";
    //set points for hexagon
    QVector<QPointF> points;
    points << QPointF(-1.0/(qSqrt(3)),-1) << QPointF(1.0/(qSqrt(3)),-1) << QPointF(2.0/(qSqrt(3)),0)
           << QPointF(1.0/(qSqrt(3)),1) << QPointF(-1.0/(qSqrt(3)),1) << QPointF(-2.0/(qSqrt(3)),0);

    //scale hexagon
    for(size_t i = 0,n = points.size();i<n;i++)
    {
        points[i]*=HEX_SCALE;
    }

    //set hexagon
    setPolygon(QPolygonF(points));


    //set scale and picture based on type of bug
    float bugScale;
    QPixmap pic;

    switch(type)
    {
    case queen:
        pic.load(":/Stuff/bee.png");
        bugScale = .38;
        break;
    case spider:
        pic.load(":/Stuff/spider.png");
        bugScale = 0.4;
        break;
    case grasshopper:
        pic.load(":/Stuff/grasshopper.png");
        bugScale = 0.35;
        break;
    case ant:
        pic.load(":/Stuff/ant.png");
        bugScale = 0.4;
        break;
    case beetle:
        pic.load(":/Stuff/beetle.png");
        bugScale = 0.5;
        break;
    case mosquito:
        pic.load(":/Stuff/mosquito.png");
        bugScale = 0.37;
        break;
    case ladybug:
        pic.load(":/Stuff/ladybug.png");
        bugScale = 0.5;
        break;
    case pillbug:
        pic.load(":/Stuff/pillbug.png");
        bugScale = 0.5;
        break;
    }
    pic.setMask(pic.createMaskFromColor(Qt::white));


    QGraphicsPixmapItem *picItem = new QGraphicsPixmapItem(pic,this);//add picture of bug as a child of the tile

    picItem->setScale(bugScale);//scale picture of bug

    //center picture of bug
    QPointF picCenter(-picItem->boundingRect().width()/2,-picItem->boundingRect().height()/2);
    picItem->setPos(picCenter*bugScale);

    int xb=xy.first,yb=xy.second;//board coordinates
    float xs,ys; //screen coordinates

    xs = HEX_SCALE*qSqrt(3)*xb+xc;
    ys = HEX_SCALE*(-yb)+yc;

    //qDebug()<<"tile created at "<<xs<<","<<ys;

    setPos(xs,ys);

    x = xb;
    y = yb;



}

copair Tile::getSpace(int m)
{
    //allow for wrap around
    if(m<0)
        return getSpace(m+6);
    if(m>5)
        return getSpace(m-6);

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
    //allow for wrap around
    if (m<0)
        return getSpace(m+6,possibleSpace);
    if (m>5)
        return getSpace(m-6,possibleSpace);

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

int Tile::getHeight()
{
    return getHeight(copair(x,y),game->tileMap);
}

int Tile::getHeight(copair xy)
{
    return getHeight(xy,game->tileMap);
}

int Tile::getHeight(std::map<copair,std::deque<Tile*>> &tilemap)
{
    return getHeight(copair(x,y),tilemap);
}

int Tile::getHeight(copair space, std::map<copair,std::deque<Tile*>> &tilemap)
{
    if(tilemap.find(space)!=tilemap.end())
        return tilemap[space].size();
    else
        return 0;

}



Tile* Tile::getNeighbor(int m)
{
    copair space = getSpace(m);
    if (game->tileMap.find(space)!=game->tileMap.end())
    {
        return game->tileMap[space].front();
    }
    return nullptr;
}

Tile* Tile::getNeighbor(int m, copair possibleSpace)
{
    copair space = getSpace(m,possibleSpace);
    if (game->tileMap.find(space)!=game->tileMap.end())
    {
        return game->tileMap[space].front();
    }
    return nullptr;
}

Tile* Tile::getNeighbor(int m, copair possibleSpace, std::map<copair,std::deque<Tile*>> &tilemap)
{
    copair space = getSpace(m,possibleSpace);
    if (tilemap.find(space)!=tilemap.end())
    {
        return tilemap[space].front();
    }
    return nullptr;
}



bool Tile::isConnected(std::map<copair,std::deque<Tile*>> possibleTileMap)
{


    //deque is used for bredth-first search
    //set is required to check what has already been queued to be searched
    std::deque<copair> reachableDeque;
    std::set<copair> reachableSet;

    //seed the search
    std::map<copair,std::deque<Tile*>>::iterator mapIter = possibleTileMap.begin();
    reachableDeque.push_back(mapIter->first);
    reachableSet.insert(mapIter->first);

    std::deque<copair>::iterator dequeIter = reachableDeque.begin();

    int i;
    copair space;
    while(dequeIter != reachableDeque.end())
    {
        for(i=0;i<6;i++)
        {
            space = getSpace(i,*dequeIter);
            if (possibleTileMap.find(space)!=possibleTileMap.end() && reachableSet.find(space)==reachableSet.end())
            {
                reachableDeque.push_back(space);
                reachableSet.insert(space);
            }
        }
        dequeIter++;
    }

    return(reachableSet.size()==possibleTileMap.size());



}



void Tile::GetQueenMoves()
{

    copair possibleSpace;//coordinates of space queen might occupy
    std::map<copair,std::deque<Tile*>> possibleTileMap;//tile map for computing whether possible moves are legal

    //check if each move is valid
    for(int i=0;i<6;i++)
    {

        //verify space is empty
        if(getNeighbor(i)!=nullptr)
        {
            debugStream  << i << ": occupied\n";
            continue; //end verification of current move
        }

        //verify freedom to move
        if ((getNeighbor(i+1)!=nullptr) && (getNeighbor(i-1)!=nullptr))
        {
            debugStream << i << ": freedom to move violation\n";
            continue; //end verification of current move

        }

        //verify hive not broken in transit
        if ((getNeighbor(i+1)==nullptr) && (getNeighbor(i-1)==nullptr))
        {
            debugStream << i << ": hive broken in transit\n";
            continue; //end verification of current move

        }

        //verify hive not broken
        possibleSpace = getSpace(i);
        possibleTileMap=game->tileMap;
        moveInTileMap(possibleSpace,possibleTileMap);
        if(!isConnected(possibleTileMap))
        {
            debugStream << i << ": hive broken\n";
            continue; //end verification of current move
        }

        debugStream << i << ": valid\n";
        possibleMoves.insert(possibleSpace);


    }

}




void Tile::GetSpiderMoves()
{

    copair possibleFirstSpace,possibleSecondSpace,possibleSpace;//coordinates of space queen might occupy
    std::map<copair,std::deque<Tile*>> possibleTileMap;//tile map for computing whether possible moves are legal

    int nMoves = 0;


    int i,j,k;//indices for first, secind, and third step of spider move

    //check if each move is valid
    for(i=0;i<6;i++)
    {
        possibleTileMap=game->tileMap;
        //verify space is empty
        if(getNeighbor(i)!=nullptr)
        {
            debugStream  << i << ": occupied\n";
            continue; //end verification of current move
        }

        //verify freedom to move
        if (getHeight(getSpace(i-1),possibleTileMap) && getHeight(getSpace(i+1),possibleTileMap))
        {
            debugStream << i << ": freedom to move violation\n";
            continue; //end verification of current move

        }

        //verify hive not broken in transit
        if ((getNeighbor(i+1)==nullptr) && (getNeighbor(i-1)==nullptr))
        {
            debugStream << i << ": hive broken in transit\n";
            continue; //end verification of current move

        }

        //verify hive not broken

        possibleFirstSpace = getSpace(i);
        moveInTileMap(possibleFirstSpace,possibleTileMap);

        if(!isConnected(possibleTileMap))
        {
            debugStream << i << ": hive broken\n";
            continue; //end verification of current move
        }

        for(j=0;j<6;j++)
        {


            //verify second step isn't starting spot
            if(abs(i-j)==3)
            {
                debugStream << i << j <<": backtrack\n";
                continue; //end verification of current move
            }

            //verify second step isn't occupied
            if(getNeighbor(j,possibleFirstSpace)!=nullptr)
            {
                debugStream << i << j << ": occupied\n";
                continue; //end verification of current move
            }

            //verify freedom to move
            if(getHeight(getSpace(j-1,possibleFirstSpace),possibleTileMap) && getHeight(getSpace(j+1,possibleFirstSpace),possibleTileMap))
            {
                debugStream << i << j <<": freedom to move violation\n";
                continue; //end verification of current move
            }

            //verify hive not broken in transit
            if(getNeighbor(j+1,possibleFirstSpace,possibleTileMap)==nullptr && getNeighbor(j-1,possibleFirstSpace,possibleTileMap)==nullptr)
            {
                debugStream << i << j <<": hive broken in transit\n";
                continue; //end verification of current move
            }

            //verify hive not broken
            possibleSecondSpace = getSpace(j,possibleFirstSpace);
            moveInTileMap(possibleSecondSpace,possibleTileMap);
            if(!isConnected(possibleTileMap))
            {
                debugStream << i << j << ": hive broken\n";
                continue; //end verification of current move
            }

            for(k=0;k<6;k++)
            {

                //verify third step isn't second step
                if (abs(j-k)==3)
                {
                    debugStream << i << j << k << ": backtrack\n";
                    continue; //end verification of current move
                }

                //verify third step isn't starting spot
                possibleSpace = getSpace(k,possibleSecondSpace);
                if(possibleSpace == copair(x,y))
                {
                    debugStream << i << j << k << ": same as starting spot\n";
                    continue; //end verification of current move
                }

                //verify third step isn't occupied
                if(getNeighbor(k,possibleSecondSpace)!=nullptr)
                {
                    debugStream << i << j << k << ": occupied\n";
                    continue; //end verification of current move
                }

                //verify freedom of movement
                if(getHeight(getSpace(k-1,possibleSecondSpace),possibleTileMap) && getHeight(getSpace(k+1,possibleSecondSpace),possibleTileMap))
                {
                    debugStream << i << j << k << ": freedom of movement violated\n";
                    continue; //end verification of current move
                }

                //verify hive not broken in transit
                if((getNeighbor(k+1,possibleSecondSpace,possibleTileMap)==nullptr) && (getNeighbor(k-1,possibleSecondSpace,possibleTileMap)==nullptr))
                {
                    debugStream << i << j << k << ": hive broken in transit\n";
                    continue; //end verification of current move
                }

                //verify hive not broken
                moveInTileMap(possibleSpace,possibleTileMap);
                if(!isConnected(possibleTileMap))
                {
                    debugStream << i << j << k << ": hive broken in\n";
                    continue; //end verification of current move
                }

                debugStream << i << j << k << ": valid\n";
                possibleMoves.insert(possibleSpace);
                nMoves++;

            }
        }
    }

    debugStream <<"number of spider moves: "<<nMoves;
}

void Tile::GetGrasshopperMoves()
{
    std::map<copair,std::deque<Tile*>> possibleTileMap;//tile map for computing whether possible moves are legal
    copair space;
    int i;

    //check if that grasshopper can move without breaking hive
    possibleTileMap = game->tileMap;
    possibleTileMap.erase(copair(x,y));
    if(isConnected(possibleTileMap))

    {
        //loop over all 6 directions
        for(i=0;i<6;i++)
        {
            //only allow movement across a neighbor
            if(getNeighbor(i)==nullptr)
                continue;

            space = copair(x,y);
            while (getNeighbor(i,space)!=nullptr)
            {
                space = getSpace(i,space);//crawl in direction until empty space is reached
            }
            space = getSpace(i,space);

            possibleMoves.insert(space);

        }
    }
}


void Tile::GetAntMoves()
{



    std::map<copair,std::deque<Tile*>> possibleTileMap;//tile map for computing whether possible moves are legal
    std::deque<copair> moveDeck;//growing set of possible ant-moves
    moveDeck.push_back(copair(x,y));//seed set of ant-moves with current location
    std::set<copair> antMoves;//set of ant moves needs to be distinct from total set of moves for mosquito

    copair space, possibleSpace;
    std::deque<copair>::iterator iter = moveDeck.begin();

    int i;
    while(iter!=moveDeck.end())
    {

        space = *iter;
        for(i=0;i<6;i++)
        {

            if(!(getNeighbor(i,space)) && ((getNeighbor(i+1,space)==nullptr) ^ (getNeighbor(i-1,space)==nullptr)))
            {
                possibleSpace = getSpace(i,space);
                possibleTileMap=game->tileMap;
                moveInTileMap(possibleSpace,possibleTileMap);
                if(isConnected(possibleTileMap))
                {
                    if(antMoves.find(possibleSpace)==antMoves.end())
                    {
                        antMoves.insert(possibleSpace);
                        possibleMoves.insert(possibleSpace);
                        moveDeck.push_back(possibleSpace);
                    }
                }

            }
        }
        iter++;
    }




}



void Tile::GetBeetleMoves()
{
    copair possibleSpace;//coordinates of space Beetle might occupy
    std::map<copair,std::deque<Tile*>> possibleTileMap;//tile map for computing whether possible moves are legal
    int effectiveHeight,flankHeight;



    for(int i=0;i<6;i++)
    {
        possibleTileMap = game->tileMap;
        possibleSpace = getSpace(i);
        moveInTileMap(possibleSpace,possibleTileMap);




        //verify freedom to move
        effectiveHeight = std::max(getHeight(),getHeight(possibleSpace,possibleTileMap));
        flankHeight = std::min(getHeight(getSpace(i+1),possibleTileMap),getHeight(getSpace(i-1),possibleTileMap));
        if (flankHeight>=effectiveHeight)
        {
            debugStream << i << ": freedom to move violation\n";
            continue; //end verification of current move
        }

        //verify not broken hive
        if (!isConnected(possibleTileMap))
        {
            debugStream << i << ": hive broken\n";
            continue; //end verification of current move
        }



        possibleMoves.insert(possibleSpace);
        debugStream << i << ": valid\n" <<possibleTileMap<<"\n";
    }

}




void Tile::GetLadybugMoves()
{
    copair firstMove,secondMove,possibleSpace;//coordinates of space queen might occupy
    std::map<copair,std::deque<Tile*>> possibleTileMap;//tile map for computing whether possible moves are legal

    int i,j,k;//indices for first, secind, and third step of spider move

    //check if each move is valid
    for(i=0;i<6;i++)
    {
        possibleTileMap = game->tileMap;
        firstMove = getSpace(i);

        //verify first move is on top of hive
        if (getHeight(firstMove)==0)
        {
            debugStream << i << ": not on top of hive\n";
            continue; //end verification for current move
        }

        //verify freedom to move
        if(getHeight(firstMove)<std::min(getHeight(getSpace(i-1)),getHeight(getSpace(i+1))))
        {
            debugStream << i << ": freedom to move violation\n";
            continue; //end verification for current move
        }



        //verify not broken hive
        moveInTileMap(firstMove,possibleTileMap);
        if (!isConnected(possibleTileMap))
        {
            debugStream << i << ": hive broken\n";
            continue; //end verification for current move
        }

        for(j=0;j<6;j++)
        {
            secondMove = getSpace(j,firstMove);

            //verify second move on top of hive
            if (getHeight(secondMove)==0)
            {
                debugStream << i << j << ": not on top of hive\n";
                continue; // end verification for current move
            }


            //verify freedom to move
            if(std::max(getHeight(secondMove),getHeight(firstMove)+1)<=std::min(getHeight(getSpace(i-1,firstMove)),getHeight(getSpace(i+1,firstMove))))
            {
                debugStream << i << j << ": freedom to move violation\n";
                continue; //end verification of current move
            }

            //verify second move not starting spot
            if(abs(i-j)==3)
            {
                debugStream << i << j << ": same as starting spot\n";
                continue; //end verification of current move
            }

            for(k=0;k<6;k++)
            {
                possibleSpace = getSpace(k,secondMove);

                //verify space is not on top of hive
                if(getHeight(possibleSpace)!=0)
                {
                    debugStream << i << j << k << ": on top of hive";
                    continue; //end verification of current move
                }

                //verify space is not starting spot
                if(possibleSpace == copair(x,y))
                {
                    debugStream << i << j << k << ": same as starting spot";
                    continue; //end verification of current move
                }

                //verify freedom to move
                if(getHeight(secondMove)<std::min(getHeight(getSpace(i-1,secondMove)),getHeight(getSpace(i+1,secondMove))))
                {
                    debugStream << i << j << k << ": freedom to move violation\n";
                    continue; //end verification of current move
                }


                debugStream << i << j << k << ": valid\n";
                possibleMoves.insert(possibleSpace);


            }
        }
    }
}

void Tile::GetPillbugMoves()
{

    std::map<copair,std::deque<Tile*>> possibleTileMap;//tile map for computing whether possible moves are legal
    int i;
    Tile* neighbor;
    GetQueenMoves();//get all standard moves

    std::set<Tile*> moveableNeighbors;//set of all neighbors that can be moved by pillbug move;
    std::set<copair> moveableSpaces; //set of all possible destinations for pillbug move;
    std::set<Tile*>::iterator tileIter;
    std::set<copair>::iterator spaceIter;

    //get moveable neighbors
    for(i=0;i<6;i++)
    {
        neighbor = getNeighbor(i);

        //verify freedom to move
        if(std::min(getHeight(getSpace(i-1)),getHeight(getSpace(i+1)))>1)
        {
            debugStream<< i << ": freedom to move violation\n";
            continue;
        }

        //check if neighbor or empty space
        if(neighbor)
        {

            //verify neighbor wasn't just moved
            if(neighbor == game->lastTile)
            {
                debugStream << i << ": neighbor is immobile\n";
                continue;
            }

            //verify hive not broken
            possibleTileMap = game->tileMap;
            neighbor->moveInTileMap(copair(x,y),possibleTileMap);
            if(!isConnected(possibleTileMap))
            {
                debugStream << i << ": hive broken\n";
                continue;
            }
            debugStream << i << ": valid neighbor\n";
            moveableNeighbors.insert(neighbor);
        }else{
            debugStream << i << ": valid space\n";
            moveableSpaces.insert(getSpace(i));
        }


    }

    //add moves to neighbors move lists
    for(tileIter = moveableNeighbors.begin();tileIter!= moveableNeighbors.end();tileIter++)
    {
        for(spaceIter = moveableSpaces.begin();spaceIter != moveableSpaces.end();spaceIter++)
        {
            (*tileIter)->possibleMoves.insert(*spaceIter);
        }
    }






}

void Queen::GetMoves()
{


    GetQueenMoves();

}

void Spider::GetMoves()
{

    GetSpiderMoves();
}

void Grasshopper::GetMoves()
{

    GetGrasshopperMoves();

}

void Ant::GetMoves()
{

    GetAntMoves();
}

void Beetle::GetMoves()
{

    GetBeetleMoves();
}

void Mosquito::GetMoves()
{


    if (getHeight()>1)
    {
        GetBeetleMoves();
        return;
    }

    for(int i=0;i<6;i++)
        if(getNeighbor(i))
        {
            switch(getNeighbor(i)->type)
            {
            case queen:
                GetQueenMoves();

                break;
            case spider:
                GetSpiderMoves();
                break;
            case grasshopper:
                GetGrasshopperMoves();
                break;
            case ant:
                GetAntMoves();
                break;
            case beetle:
                GetBeetleMoves();
                break;
            case ladybug:
                GetLadybugMoves();
                break;
            case pillbug:
                GetPillbugMoves();
                break;
            case mosquito:;

            }
        }


}

void Ladybug::GetMoves()
{
    //qDebug() << "getLadybugMoves";
    GetLadybugMoves();
}

void Pillbug::GetMoves()
{
    //qDebug() << "getPillbugMoves";
    GetPillbugMoves();
}


void Tile::moveTo(copair xy)
{
    x = xy.first;
    y = xy.second;

    float xs = HEX_SCALE*qSqrt(3)*x+xc;
    float ys = HEX_SCALE*(-y)+yc;

    setPos(xs,ys);


}

void Tile::moveInTileMap(copair space, std::map<copair,std::deque<Tile*>> &tilemap)
{
    // if moving to same space, do nothing
    if(tilemap.find(space) != tilemap.end()  && tilemap[space].front() == this)
        return;


    std::map<copair,std::deque<Tile*>>::iterator mapIter = tilemap.begin();
    std::deque<Tile*>::iterator deckIter;



    while(mapIter != tilemap.end())
    {

        //check if tile is already in tilemap
        if(((mapIter)->second).front()==this)
        {

            ((mapIter)->second).pop_front();//remove tile from current place



            if(((mapIter)->second).size()==0)
                tilemap.erase(mapIter);//if nothing left in tile stack, delete entry from tile map
            break;
        }

        //if tile is already in tilemap but in the middle of tile stack, throw exception
        deckIter = (mapIter->second).begin();

        while(deckIter!=(mapIter->second).end())
        {

            if(*deckIter==this)
            {

                qDebug()<< ((mapIter->second).front()==this);
                qDebug()<<*(mapIter->second).front();
                qDebug()<<*this;
                qDebug()<<tilemap;
                throw 3;
            }

            deckIter++;

        }
        mapIter++;
    }





    if(tilemap.find(space)!=tilemap.end())
    {
        (tilemap[space]).push_front(this);
    }else{
        std::deque<Tile*> tileDeck;
        tileDeck.push_front(this);

        tilemap[space] = tileDeck;
    }



}

QDebug operator <<(QDebug d, Tile &tile)
{
    switch(tile.type)
    {
    case queen:
        d<<"Q";
        break;
    case spider:
        d<<"S";
        break;
    case beetle:
        d<<"B";
        break;
    case grasshopper:
        d<<"G";
        break;
    case ant:
        d<<"A";
        break;
    case mosquito:
        d<<"M";
        break;
    case ladybug:
        d<<"L";
        break;
    case pillbug:
        d<<"P";
        break;
    }
    d<<tile.owner;
    return d;
}


QTextStream & operator <<(QTextStream &stream, Tile &tile)
{
    switch(tile.type)
    {
    case queen:
        stream<<"Q";
        break;
    case spider:
        stream<<"S";
        break;
    case beetle:
        stream<<"B";
        break;
    case grasshopper:
        stream<<"G";
        break;
    case ant:
        stream<<"A";
        break;
    case mosquito:
        stream<<"M";
        break;
    case ladybug:
        stream<<"L";
        break;
    case pillbug:
        stream<<"P";
        break;
    }
    stream<<tile.owner;
    return stream;
}
