#pragma once

#include <JuceHeader.h>
#include "stackblur.h"

class PBird : public bs::Snake
{
public:
    struct Choice
    {
        bs::Direction direction;
        float score = 0;
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
    
    struct Grid
    {
        Grid (bs::Game& g)
            : game (g)
        {
            // set all to a average value
            for (int x = 0; x < w; x++)
                for (int y = 0; y < h; y++)
                    area.setPixelAt (x, y, Colour (0xff808080));
            
            // food is great
            for (auto f : game.getFood())
                area.setPixelAt (f->pos.x, f->pos.y, Colour (0xffffffff));
            
            // snakes are bad, mkay
            for (auto s : game.getBodies())
                for (auto p : s->pos)
                    area.setPixelAt (p.x, p.y, Colour (0xff000000));
            
            applyStackBlur (area, 5);
            
            // snakes are bad
            for (auto s : game.getBodies())
            {
                if (! s->me)
                {
                    auto h = s->pos[0];
                    area.setPixelAt (h.x - 1, h.y, Colour (0xff404040));
                    area.setPixelAt (h.x + 1, h.y, Colour (0xff404040));
                    area.setPixelAt (h.x, h.y - 1, Colour (0xff404040));
                    area.setPixelAt (h.x, h.y + 1, Colour (0xff404040));
                }
                
                for (auto p : s->pos)
                    area.setPixelAt (p.x, p.y, Colour (0xff000000));
            }
        }
        
        float get (int x, int y)
        {
            return area.getPixelAt (x, y).getRed() / 255.0;
        }
        
        bs::Game& game;
        int w = game.getBoardWidth();
        int h = game.getBoardHeight();
        Image area {Image::RGB, game.getBoardWidth(), game.getBoardHeight(), true};
    };
    
    bs::Direction getMove() override
    {
        Grid grid (*game);
        
        Array<Choice> choices;

        auto dirs = possibleDirections();
        for (auto d : dirs)
        {
            DBG("  " + toString (d) + ":");
           
            Choice c;
            c.direction = d;
            
            auto p = game->getPoint (getHead(), d);
            c.score = grid.get (p.x, p.y);
            
            //if (d == currentDirection)
            //    c.score += 0.05;
            
            DBG (String::formatted ("   %d %d %f", p.x, p.y, c.score));
            
            choices.add (c);
        }

        Comp cmp;
        choices.sort (cmp);

        currentDirection = choices.getLast().direction;

        DBG("  picked: " + toString (currentDirection));
        
        return currentDirection;
    }
};
