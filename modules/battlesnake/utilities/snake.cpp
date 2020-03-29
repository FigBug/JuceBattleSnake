
Snake::Snake()
{
}

String Snake::toString (Direction d)
{
    switch (d)
    {
        case up:    return "Up";
        case left:  return "Left";
        case down:  return "Down";
        case right: return "Right";
    }
}

Game::Body* Snake::getOwnBody()
{
    for (auto b : game->getBodies())
        if (b->me)
            return b;
    
    return nullptr;
}

Point<int> Snake::getHead()
{
    if (auto b = getOwnBody())
        return b->pos.getFirst();
    
    return {};
}

int Snake::getLength()
{
    if (auto b = getOwnBody())
        return b->getLength();
    
    return 0;
}

int Snake::getHealth()
{
    if (auto b = getOwnBody())
        return b->health;
    
    return {};
}

void Snake::turnLeft()
{
    switch (currentDirection)
    {
        case up:    currentDirection = left;    break;
        case left:  currentDirection = down;    break;
        case down:  currentDirection = right;   break;
        case right: currentDirection = up;      break;
    }
}

void Snake::turnRight()
{
    switch (currentDirection)
    {
        case up:    currentDirection = right;   break;
        case left:  currentDirection = up;      break;
        case down:  currentDirection = left;    break;
        case right: currentDirection = down;    break;
    }
}

Array<bs::Direction> Snake::possibleDirections()
{
    switch (currentDirection)
    {
        case up:
            return { left, up, right };
        case left:
            return { down, left, up };
        case down:
            return { right, down, left };
        case right:
            return { up, right, down };
        default:
            jassertfalse;
            return {};
    }
}

std::optional<Point<int>> Snake::getNearestFood()
{
    auto head = getHead().toDouble();
    double delta = std::numeric_limits<double>::max();
    std::optional<Point<int>> res;
    
    for (auto f : game->getFood())
    {
        double newDelta = f->pos.toDouble().getDistanceFrom (head);
        if (newDelta < delta)
        {
            delta = newDelta;
            res = f->pos;
        }
    }
    
    return res;
}

Game::Body* Snake::getNearestPrey()
{
    auto head = getHead().toDouble();
    double delta = std::numeric_limits<double>::max();
    Game::Body* res = nullptr;
    
    for (auto b : game->getBodies())
    {
        if (b->me && b->getLength() >= getLength())
            continue;
        
        double newDelta = b->getHead().toDouble().getDistanceFrom (head);
        if (newDelta < delta)
        {
            delta = newDelta;
            res = b;
        }
    }
    
    return res;
}

void Snake::moveTowards (Point<int> p)
{
    
}

void Snake::avoidWalls()
{
    auto w = game->getBoardWidth();
    auto h = game->getBoardHeight();
    
    auto head = getHead();
    
    auto x = head.x;
    auto y = head.y;
    
    if (currentDirection == up)
    {
        if (y < 2)
            currentDirection = (x > w / 2) ? left : right;
    }
    else if (currentDirection == down)
    {
        if (y >= h - 2)
            currentDirection = (x > w / 2) ? left : right;;
    }
    else if (currentDirection == left)
    {
        if (x < 2)
            currentDirection = (y > h / 2) ? up : down;
    }
    else if (currentDirection == right)
    {
        if (x >= w - 2)
            currentDirection = (y > h / 2) ? up : down;
    }
}

Point<int> Snake::unitVector (Direction d)
{
    switch (d)
    {
        case up:    return { 0, 1 };
        case left:  return { 1, 0 };
        case down:  return { 0, -1 };
        case right: return { -1, 0 };
        default:
            jassertfalse;
            return {};
    }
}

int Snake::distanceToObstacle (Direction d)
{
    auto w = game->getBoardWidth();
    auto h = game->getBoardHeight();

    auto uv = unitVector (d);
    auto head = getHead();
    
    auto x = head.x;
    auto y = head.y;
    
    int distance = 0;

    while (true)
    {
        x -= uv.x;
        y -= uv.y;

        distance++;
        
        if (x < 0 || x >= w || y < 0 || y >= h)
            return distance;

        for (auto b : game->getBodies())
        {
            for (auto p : b->pos)
                if (p == Point<int> (x, y))
                    return distance;
        }
    }
}

std::optional<int> Snake::distanceToDangerZone (Direction d)
{
    auto w = game->getBoardWidth();
    auto h = game->getBoardHeight();

    auto uv = unitVector (d);
    auto head = getHead();
    
    auto x = head.x;
    auto y = head.y;
    
    int distance = 0;

    while (true)
    {
        x -= uv.x;
        y -= uv.y;

        distance++;
        
        if (x < 0 || x >= w || y < 0 || y >= h)
            return {};

        for (auto b : game->getBodies())
        {
            if (! b->me)
                if (b->getDangerZone().contains (Point<int> (x, y)))
                    return distance;
        }
    }
}

std::optional<int> Snake::distanceToFood (Direction d)
{
    auto w = game->getBoardWidth();
    auto h = game->getBoardHeight();

    auto uv = unitVector (d);
    auto head = getHead();
    
    auto x = head.x;
    auto y = head.y;
    
    int distance = 0;

    while (true)
    {
        x -= uv.x;
        y -= uv.y;

        distance++;
        
        if (x < 0 || x >= w || y < 0 || y >= h)
            return {};

        for (auto f : game->getFood())
            if (f->pos == Point<int> (x, y))
                return distance;
    }
}

