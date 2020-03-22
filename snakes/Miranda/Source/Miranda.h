#pragma once

#include <JuceHeader.h>

class Miranda : public bs::Snake
{
public:
    Direction getMove()
    {
        switch (++count % 4)
        {
            case 0: return left;
            case 1: return down;
            case 2: return right;
            case 3: return up;
            default:
                return left;
        }
    };
    
    int count = 0;
};
