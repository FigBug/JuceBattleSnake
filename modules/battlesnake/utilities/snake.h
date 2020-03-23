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
    
    String toString (Direction d);
    
    Snake();
    virtual ~Snake() = default;
    
    virtual Direction getMove() = 0;
    
    Game::Body* getOwnBody();
    Point<int> getHead();
    int getHealth();
    
    void turnLeft();
    void turnRight();
    
    Array<Direction> possibleDirections();
    
    Point<int> unitVector (Direction d);
    
    int distanceToObstacle (Direction d);
    std::optional<int> distanceToFood (Direction d);
    
    void moveTowards (Point<int> p);
    void avoidWalls();
    
    std::optional<Point<int>> getNearestFood();
    
    Game* game = nullptr;
    
    Direction currentDirection = up;
    
    String colour   = "0xffff00";
    String head     = "shades";
    String tail     = "small-rattle";
};
