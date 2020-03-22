SnakePit::~SnakePit()
{
    stop();
}

void SnakePit::start (const Webserver::Options& o)
{
    webserver = std::make_unique<Webserver> (o);
    
    webserver->addHandler ("/ping", [this] (Webserver::Request& req)
    {
        return 200;
    });
    webserver->addHandler ("/start", [this] (Webserver::Request& req)
    {
        return 200;
    });
    webserver->addHandler ("/move", [this] (Webserver::Request& req)
    {
        return 200;
    });
    webserver->addHandler ("/end", [this] (Webserver::Request& req)
    {
        return 200;
    });
}

void SnakePit::stop()
{
}

void SnakePit::addSnake (Snake& snake)
{
    snakes.add (&snake);
}
