SnakePit::~SnakePit()
{
    stop();
}

String SnakePit::getUid (var json)
{
    String gameId;
    String snakeId;
    
    auto game = json.getProperty ("game", {});
    if (game.isObject())
        gameId = game.getProperty ("id", {});
    
    auto you = json.getProperty ("you", {});
    if (you.isObject())
        snakeId = you.getProperty ("id", {});
    
    jassert (gameId.isNotEmpty());
    jassert (snakeId.isNotEmpty());
    
    return gameId + "/" + snakeId;
}

void SnakePit::start (const Webserver::Options& o)
{
    webserver = std::make_unique<Webserver> (o);
    
    webserver->addHandler ("/ping", [this] (Webserver::Request& req)
    {
        ScopedLock sl (lock);
        
        req.contentType = "application/json";
        req.response    = "";

        return 200;
    });
    
    webserver->addHandler ("/start", [this] (Webserver::Request& req)
    {
        ScopedLock sl (lock);
        
        var json = JSON::parse (req.body);
        
        if (json.isObject())
        {
            auto game = new Game (eggs());
            
            req.contentType = "application/json";
            req.response    = JSON::toString (game->start (json));
            
            games[getUid (json)] = std::unique_ptr<Game> (game) ;
        }
        
        return 200;
    });
    
    webserver->addHandler ("/move", [this] (Webserver::Request& req)
    {
        ScopedLock sl (lock);
        
        var json = JSON::parse (req.body);
        
        if (json.isObject())
        {
            auto itr = games.find (getUid (json));
            if (itr != games.end())
            {
                req.contentType = "application/json";
                req.response    = JSON::toString (itr->second->move (json));
            }
            else
            {
                jassertfalse;
            }
        }
        
        return 200;
    });
    
    webserver->addHandler ("/end", [this] (Webserver::Request& req)
    {
        ScopedLock sl (lock);
        
        var json = JSON::parse (req.body);
        
        if (json.isObject())
        {
            auto id = getUid (json);
            games.erase (id);
            
            req.contentType = "application/json";
            req.response    = "";
        }
        
        return 200;
    });
}

void SnakePit::stop()
{
}

void SnakePit::addSnakeEggs (std::function<Snake* ()> eggs_)
{
    eggs = eggs_;
}
