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
    };
    
    struct Food
    {
        Point<int> pos;
    };
    
    int getBoardWidth()                 { return width;     }
    int getBoardHeight()                { return height;    }
    
    bool isOnBoard (Point<int> p);
    
    const OwnedArray<Food>& getFood()   { return food;      }
    const OwnedArray<Body>& getBodies() { return bodies;    }
        
    var start (var obj);
    var move (var obj);
    
private:
    void updateGameState (var obj);
    
    String id;
    std::unique_ptr<Snake> snake;
    
    int width = 0;
    int height = 0;
    
    OwnedArray<Food> food;
    OwnedArray<Body> bodies;
};
