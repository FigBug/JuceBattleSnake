#pragma once

#include <JuceHeader.h>

class Miranda : public bs::Snake
{
public:
    Direction getMove()
    {
        avoidWalls();
        
        return currentDirection;
    }
};
