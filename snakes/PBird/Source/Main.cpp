#include <JuceHeader.h>
#include "PBird.h"

int main()
{
    bs::Webserver::Options o;
    o.ports = {7778};
    
    auto snakePit = std::make_unique<bs::SnakePit>();
    snakePit->addSnakeEggs ([] () { return new PBird(); });
    snakePit->start (o);
    
    while (true)
    {
        Thread::sleep (100);
    }
    
    return 0;
}
