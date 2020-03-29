class Snake
{
public:
    String toString (Direction d);
    
    Snake();
    virtual ~Snake() = default;
    
    virtual Direction getMove() = 0;
    
    Game::Body* getOwnBody();
    Point<int> getHead();
    int getLength();
    int getHealth();
    
    void turnLeft();
    void turnRight();
    
    Array<Direction> possibleDirections();
    
    Point<int> unitVector (Direction d);
    
    int distanceToObstacle (Direction d);
    std::optional<int> distanceToDangerZone (Direction d);
    std::optional<int> distanceToFood (Direction d);
    
    void moveTowards (Point<int> p);
    void avoidWalls();
    
    std::optional<Point<int>> getNearestFood();
    Game::Body* getNearestPrey (double within = 1000.0);

    Game* game = nullptr;
    
    Direction currentDirection = up;
    
    String colour   = "0xffff00";
    String head     = "shades";
    String tail     = "small-rattle";
};
