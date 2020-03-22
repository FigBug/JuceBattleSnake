Game::Game (Snake* snake_)
{
    snake.reset (snake_);
}

var Game::start (var json)
{
    auto obj = new DynamicObject();
    obj->setProperty ("color", snake->colour);
    obj->setProperty ("headType", snake->head);
    obj->setProperty ("tailType", snake->tail);
    
    return var (obj);
}

var Game::move (var json)
{
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
