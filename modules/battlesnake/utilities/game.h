class Game
{
public:
    Game (Snake* snake);
    
    String getID()      { return id; }
    
    var start (var obj);
    var move (var obj);
    
private:
    void updateGameState (var obj);
    
    struct Body
    {
        Array<Point<int>> pos;
    };
    
    struct Food
    {
        Point<int> pos;
    };
    
    String id;
    std::unique_ptr<Snake> snake;
    
    OwnedArray<Food> food;
    OwnedArray<Body> bodies;
};
