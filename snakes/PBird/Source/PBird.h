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
        Grid (PBird& m, bs::Game& g)
            : game (g), me (m)
        {
            // set all to a average value
            for (int x = 0; x < w; x++)
                for (int y = 0; y < h; y++)
                    area.setPixelAt (x, y, Colour (0xff808080));
            
            // don't go near the walls if possible
            for (int x = 0; x < w; x++)
            {
                area.setPixelAt (x, 0, Colour (0xff606060));
                area.setPixelAt (x, h - 1, Colour (0xff606060));
            }
            for (int y = 0; y < h; y++)
            {
                area.setPixelAt (0, y, Colour (0xff606060));
                area.setPixelAt (w - 1, y, Colour (0xff606060));
            }

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
            
            seek();
        }
         
        void seek()
        {
            Array<Point<int>> seekPoints;
            
            // seek to something
            auto food = me.getNearestFood();
            if (food.has_value())
                seekPoints.add (*food);
            
            auto prey = me.getNearestPrey();
            if (prey != nullptr && me.getHealth() > 50)
                seekPoints.addArray (game.possibleNextMoves (*prey));
            
            if (seekPoints.size() > 0)
            {
                struct Comp
                {
                    Comp (Point<int> o) : origin (o.toDouble()) {}
                    
                    int compareElements (const Point<int>& first, const Point<int>& second)
                    {
                        double d1 = origin.getDistanceFrom (first.toDouble());
                        double d2 = origin.getDistanceFrom (second.toDouble());
                        
                        if (d1 < d2)
                            return -1;
                        else if (d1 == d2)
                            return 0;
                        else
                            return +1;
                    }
                    
                    Point<double> origin;
                };
                
                Comp cmp (me.getHead());
                seekPoints.sort (cmp);
                auto seekPoint = seekPoints[0];
                
                Image backup = area;
                
                auto head = me.getHead();
                int dx = seekPoint.x > head.x ? +1 : -1;
                int dy = seekPoint.y > head.y ? +1 : -1;
                
                int x = head.x;
                int y = head.y;
                while (x != seekPoint.x)
                {
                    x += dx;
                    
                    if (game.isSquareBlocked ({x, y}))
                    {
                        area = backup;
                        break;
                    }
                    
                    area.setPixelAt (x, y, Colour (0xffffffff));
                }
                
                while (y != seekPoint.y)
                {
                    if (game.isSquareBlocked ({x, y}))
                    {
                        area = backup;
                        break;
                    }

                    y += dy;
                    area.setPixelAt (x, y, Colour (0xffffffff));
                }
            }
        }
        
        float get (int x, int y)
        {
            return area.getPixelAt (x, y).getRed() / 255.0;
        }
        
        bs::Game& game;
        PBird& me;
        int w = game.getBoardWidth();
        int h = game.getBoardHeight();
        Image area {Image::RGB, game.getBoardWidth(), game.getBoardHeight(), true};
    };
    
    bs::Direction getMove() override
    {
        Grid grid (*this, *game);
        
        Array<Choice> choices;

        auto dirs = possibleDirections();
        for (auto d : dirs)
        {
            DBG("  " + toString (d) + ":");
           
            Choice c;
            c.direction = d;
            
            auto p = game->getPoint (getHead(), d);
            c.score = grid.get (p.x, p.y);
            
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
