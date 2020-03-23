#pragma once

#include <JuceHeader.h>

class Miranda : public bs::Snake
{
public:
    struct Choice
    {
        Direction direction;
        int score = 0;
    };
    
    struct Comp
    {
        static int compareElements (const Choice& first, const Choice& second)
        {
            if (first.score < second.score)
                return -1;
            else if (first.score == second.score)
                return 0;
            else
                return +1;
        }
    };
    
    Direction getMove()
    {
        int health = getHealth();
        DBG("=======================================================");
        DBG("Moving " + toString (currentDirection) + ":");
        Array<Choice> choices;
        
        auto dirs = possibleDirections();
        for (auto d : dirs)
        {
            DBG("  " + toString (d) + ":");
            
            Choice c;
            c.direction = d;
            if (d == currentDirection)
                c.score += 4;
            
            auto food = distanceToFood (d);
            auto danger = distanceToObstacle (d);
            
            DBG(String::formatted ("    food: %d   danger: %d", food.has_value() ? * food : -1, danger));
            
            if (health < 20 && food.has_value() && *food < danger && *food < 6)
            {
                c.score += (6 - *food) * 3;
            }
            else
            {
                if (danger < 2)
                    c.score -= danger * 5;
            }
            
            DBG(String::formatted ("    score: %d", c.score));
            
            choices.add (c);
        }
        
        Comp cmp;
        choices.sort (cmp);
        
        currentDirection = choices.getLast().direction;
        
        DBG("  picked: " + toString (currentDirection));
        
        return currentDirection;
    }
};
