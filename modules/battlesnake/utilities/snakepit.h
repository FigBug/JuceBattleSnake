class SnakePit
{
public:
    ~SnakePit();
    
    void start (const Webserver::Options&);
    void stop();
    
    void addSnake (Snake& snake);
    
private:
    std::unique_ptr<Webserver> webserver;
    
    OwnedArray<Snake> snakes;
};
