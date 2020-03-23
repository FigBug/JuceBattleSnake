class SnakePit
{
public:
    ~SnakePit();
    
    void start (const Webserver::Options&);
    void stop();
    
    void addSnakeEggs (std::function<Snake* ()> eggs);
    
private:
    CriticalSection lock;
    
    String getUid (var json);
    
    std::unique_ptr<Webserver> webserver;
    
    std::function<Snake* ()> eggs;
    std::map<String, std::unique_ptr<Game>> games;
};
