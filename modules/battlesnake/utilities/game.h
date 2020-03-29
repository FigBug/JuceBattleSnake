class Snake;
class Game
{
public:
    Game (Snake* snake);
    
    struct Body
    {
        String id;
        String name;
        int health = 0;
        bool me = false;
        Array<Point<int>> pos;
        
        Rectangle<int> getDangerZone();
        
        Point<int> getHead()    { return pos[0];        }
        int getLength()         { return pos.size();    }
    };
    
    struct Food
    {
        Point<int> pos;
    };
    
    int getBoardWidth()                 { return width;     }
    int getBoardHeight()                { return height;    }
    
    bool isOnBoard (Point<int> p);
    Point<int> getPoint (Point<int> p, Direction d);
    
    const OwnedArray<Food>& getFood()   { return food;      }
    const OwnedArray<Body>& getBodies() { return bodies;    }
    
    std::optional<Point<int>> getNearestFood();
    Body* getNearestPrey();
        
    var start (var obj);
    var move (var obj);
    
    bool isSquareBlocked (Point<int> p);
    Array<Point<int>> possibleNextMoves (Body& b);
    
private:
    void updateGameState (var obj);
    
    String id;
    std::unique_ptr<Snake> snake;
    
    int width = 0;
    int height = 0;
    
    OwnedArray<Food> food;
    OwnedArray<Body> bodies;
    
    std::map<std::pair<int, int>, bool> blockedSquares;
};
