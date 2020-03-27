
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
    if (game->getFood().size() == 0)
        return {};
    
    double delta = 10000;
    Point<int> best = { 10000, 10000 };
    
    auto head = getHead().toDouble();
    
    for (auto food : game->getFood())
    {
        double newDelta = Line<double> (head, food->pos.toDouble()).getLength();
        if (newDelta < delta)
        {
            delta = newDelta;
            best = food->pos;
        }
    }
    
    return best;
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

