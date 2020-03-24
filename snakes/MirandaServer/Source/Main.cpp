#include <JuceHeader.h>
#include "Miranda.h"

int main()
{
    bs::Webserver::Options o;
    o.ports = {7777};
    
    auto snakePit = std::make_unique<bs::SnakePit>();
    snakePit->addSnakeEggs ([] () { return new Miranda(); });
    snakePit->start (o);
    
    while (true)
    {
        Thread::sleep (100);
    }
    
    return 0;
}
