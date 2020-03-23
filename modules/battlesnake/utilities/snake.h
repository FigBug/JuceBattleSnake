class Snake
{
public:
    enum Direction
    {
        up,
        down,
        left,
        right,
    };
    
    Snake();
    virtual ~Snake() = default;
    
    virtual Direction getMove() = 0;
    
    Game::Body* getOwnBody();
    Point<int> getHead();
    
    void turnLeft();
    void turnRight();
    
    void moveTowards (Point<int> p);
    void avoidWalls();
    
    std::optional<Point<int>> getNearestFood();
    
    Game* game = nullptr;
    
    Direction currentDirection = up;
    
    String colour   = "0xffff00";
    String head     = "shades";
    String tail     = "small-rattle";
};
