Rectangle<int> Game::Body::getDangerZone()
{
    auto p = pos.getFirst();
    return { p.x - 1, p.y - 1, 3, 3 };
}

Game::Game (Snake* snake_)
{
    snake.reset (snake_);
    snake->game = this;
}

Point<int> Game::getPoint (Point<int> p, Direction d)
{
    switch (d)
    {
        case up:    return { p.x, p.y - 1 };
        case left:  return { p.x - 1, p.y };
        case down:  return { p.x, p.y + 1 };
        case right: return { p.x + 1, p.y };
    }
}

bool Game::isOnBoard (Point<int> p)
{
    return
        p.x >= 0    &&
        p.y >= 0    &&
        p.x < width &&
        p.y < height;
}

var Game::start (var json)
{
    updateGameState (json);
    
    auto obj = new DynamicObject();
    obj->setProperty ("color", snake->colour);
    obj->setProperty ("headType", snake->head);
    obj->setProperty ("tailType", snake->tail);
    
    return var (obj);
}

var Game::move (var json)
{
    updateGameState (json);
    
    auto toString = [] (bs::Direction d)
    {
        switch (d)
        {
            case bs::left:  return "left";
            case bs::right: return "right";
            case bs::up:    return "up";
            case bs::down:  return "down";
        }
    };
    
    auto obj = new DynamicObject();
    obj->setProperty ("move", toString (snake->getMove()));
    return var (obj);
}

void Game::updateGameState (var json)
{
    String youId;
    auto youObj = json.getProperty ("you", {});
    if (youObj.isObject())
        youId = youObj.getProperty ("id", "");
    
    auto boardObj = json.getProperty ("board", {});
    if (boardObj.isObject())
    {
        width  = boardObj.getProperty ("width", 0);
        height = boardObj.getProperty ("height", 0);
        
        auto foodArr = boardObj.getProperty ("food", {});
        if (foodArr.isArray())
        {
            food.clear();
            
            for (auto foodObj : *foodArr.getArray())
            {
                if (foodObj.isObject())
                {
                    auto f = new Food();
                    f->pos.x = foodObj.getProperty ("x", 0);
                    f->pos.y = foodObj.getProperty ("y", 0);
                    
                    food.add (f);
                }
            }
        }
        
        auto snakesArr = boardObj.getProperty ("snakes", {});
        if (snakesArr.isArray())
        {
            bodies.clear();
            
            for (auto snakeObj : *snakesArr.getArray())
            {
                auto body = new Body();
                
                body->health = snakeObj.getProperty ("health", 0);
                body->name   = snakeObj.getProperty ("name", "");
                body->id     = snakeObj.getProperty ("id", "");
                body->me     = body->id == youId;
                
                auto bodyArr = snakeObj.getProperty ("body", {});
                if (bodyArr.isArray())
                {
                    for (auto bodyObj : *bodyArr.getArray())
                    {
                        int x = bodyObj.getProperty ("x", 0);
                        int y = bodyObj.getProperty ("y", 0);
                        
                        body->pos.add ({x, y});
                    }
                }
                
                bodies.add (body);
            }
        }
    }
    
    blockedSquares.clear();
    for (auto b : bodies)
    {
        if (! b->me && b->getLength() >= snake->getLength())
        {
            auto h = b->getHead ();
            blockedSquares[{h.x - 1, h.y}] = true;
            blockedSquares[{h.x + 1, h.y}] = true;
            blockedSquares[{h.x, h.y - 1}] = true;
            blockedSquares[{h.x, h.y + 1}] = true;
        }
        
        for (auto p : b->pos)
            blockedSquares[{p.x, p.y}] = true;
    }
}

bool Game::isSquareBlocked (Point<int> p)
{
    auto itr = blockedSquares.find ({p.x, p.y});
    if (itr == blockedSquares.end())
        return false;
    
    return true;
}

Array<Point<int>> Game::possibleNextMoves (Body& b)
{
    Array<Point<int>> res;
    auto h = b.getHead ();
    res.add ({h.x - 1, h.y});
    res.add ({h.x + 1, h.y});
    res.add ({h.x, h.y - 1});
    res.add ({h.x, h.y + 1});

    for (int i = res.size(); --i >= 0;)
    {
        auto p = res[i];
        if (! isOnBoard (p) || isSquareBlocked (p))
            res.remove (i);
    }
    
    return res;
}
