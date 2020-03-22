class Game
{
public:
    Game (Snake* snake);
    
    String getID()      { return id; }
    
    var start (var obj);
    var move (var obj);
    
private:
    String id;
    std::unique_ptr<Snake> snake;
};
