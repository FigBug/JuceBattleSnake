Game::Game (Snake* snake_)
{
    snake.reset (snake_);
    snake->game = this;
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
    
    auto toString = [] (Snake::Direction d)
    {
        switch (d)
        {
            case Snake::left:  return "left";
            case Snake::right: return "right";
            case Snake::up:    return "up";
            case Snake::down:  return "down";
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
}
