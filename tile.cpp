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

int Tile::getHeight(Tile*)
{
    if(tileBeneath)
    {
        return tileBeneath->getHeight()+1;
    }
    return 1;
}

int Tile::getHeight()
{
    return getHeight(this);
}

int Tile::getHeight(copair space)
{
    if(game->tileMap.find(space)!=game->tileMap.end())
        return getHeight(game->tileMap[space]);
    else
        return 0;

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

int debugOwner = owner;
Tiletype debugTileType = type;

    //deque is used for bredth-first search
    //set is required to check what has already been queued to be searched
    std::deque<copair> reachableDeque;
    std::set<copair> reachableSet;

    //seed the search
    std::map<copair,Tile*>::iterator mapIter = possibleTileMap.begin();
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
    std::map<copair,Tile*> possibleTileMap;//tile map for computing whether possible moves are legal

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
    std::map<copair,Tile*> possibleTileMap;//tile map for computing whether possible moves are legal



    int i,j,k;//indices for first, secind, and third step of spider move

    //check if each move is valid
    for(i=0;i<6;i++)
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
        possibleTileMap=game->tileMap;
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
            if(getNeighbor(j+1,possibleFirstSpace)!=nullptr && getNeighbor(j-1,possibleFirstSpace)!=nullptr)
            {
                debugStream << i << j <<": freedom to move violation\n";
                continue; //end verification of current move
            }

            //verify hive not broken in transit
            if(getNeighbor(j+1,possibleFirstSpace)==nullptr && getNeighbor(j-1,possibleFirstSpace)==nullptr)
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
                if((getNeighbor(k+1,possibleSecondSpace)!=nullptr) && (getNeighbor(k-1,possibleSecondSpace)!=nullptr))
                {
                    debugStream << i << j << k << ": freedom of movement violated\n";
                    continue; //end verification of current move
                }

                //verify hive not broken in transit
                if((getNeighbor(k+1,possibleSecondSpace)==nullptr) && (getNeighbor(k-1,possibleSecondSpace)==nullptr))
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

            }
        }
    }
}

void Tile::GetGrasshopperMoves()
{
    std::map<copair,Tile*> possibleTileMap;//tile map for computing whether possible moves are legal
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
    std::map<copair,Tile*> possibleTileMap;//tile map for computing whether possible moves are legal
    std::deque<copair> moveDeck;//growing set of possible ant-moves
    moveDeck.push_back(copair(x,y));//seed set of ant-moves with current location

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
                possibleTileMap[possibleSpace] = this;
                possibleTileMap.erase(copair(x,y));
                if(isConnected(possibleTileMap))
                {
                    if(possibleMoves.find(possibleSpace)==possibleMoves.end())
                    {
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
    std::map<copair,Tile*> possibleTileMap;//tile map for computing whether possible moves are legal
    int effectiveHeight,flankHeight;

    Tile *tileBeneathCopy = tileBeneath; //to restore tileBeneath after use in moveInTileMap

    for(int i=0;i<6;i++)
    {
        possibleTileMap = game->tileMap;
        possibleSpace = getSpace(i);
        moveInTileMap(possibleSpace,possibleTileMap);
        tileBeneath = tileBeneathCopy;



       //verify freedom to move
        effectiveHeight = std::max(getHeight(),getHeight(possibleSpace));
        flankHeight = std::min(getHeight(getSpace(i+1)),getHeight(getSpace(i-1)));
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
    tileBeneath = tileBeneathCopy;
}




void Tile::GetLadybugMoves()
{
    copair firstMove,secondMove,possibleSpace;//coordinates of space queen might occupy
    std::map<copair,Tile*> possibleTileMap;//tile map for computing whether possible moves are legal

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

    std::map<copair,Tile*> possibleTileMap;//tile map for computing whether possible moves are legal
    int i,j;
    Tile* neighbor;
    GetQueenMoves();//get all standard moves

    for(i=0;i<6;i++)
    {
        neighbor = getNeighbor(i);
        if(neighbor)
        {
            for(j=0;j<6;j++)
            {
                if(i!=j && !getNeighbor(j))
                {
                    possibleTileMap=game->tileMap;
                    possibleTileMap[getSpace(j)]=neighbor;
                    possibleTileMap.erase(getSpace(i));
                    if (isConnected(possibleTileMap))
                        neighbor->possibleMoves.insert(getSpace(j));

                }
            }
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
            case spider:
                GetSpiderMoves();
            case grasshopper:
                GetGrasshopperMoves();
            case ant:
                GetAntMoves();
            case beetle:
                GetBeetleMoves();
            case ladybug:
                GetLadybugMoves();
            case pillbug:
                GetPillbugMoves();
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

void Tile::moveInTileMap(copair space, std::map<copair,Tile*> &tilemap)
{
    //if moving to same space, do nothing
    if(tilemap.find(space) != tilemap.end() && tilemap[space] == this)
        return;

    std::map<copair,Tile*>::iterator iter = tilemap.begin(), iter2 =tilemap.end() ;



    while(iter != tilemap.end())
    {

        //if tile is already in tile map, remove it from it's current space
        //revealing the tile beneath it
        if(iter->second == this)
        {

            if(tileBeneath != nullptr)
            {
                iter->second = tileBeneath;
            }else{
                iter2 = iter;
            }

        }

        //if space is already occupied, place the tile there beneath the current tile
        if(iter->first == space)
        {
            tileBeneath = iter->second;
        }
        iter++;
    }

    if(iter2 != tilemap.end())
        tilemap.erase(iter2);

    //place tile in current space
    tilemap[space] = this;


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
