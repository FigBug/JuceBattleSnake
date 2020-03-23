# JuceBattleSnake

A framework for writing snakes in C++ with JUCE for https://play.battlesnake.com/

Easy to use:

Inherit from the class `Snake` and implement the function `virtual Direction getMove() = 0;`

The `getMove` function should update the `currentDirection` member variable and then return it.

To build a snake:

Set the port for your webserver:
```
bs::Webserver::Options o;
o.ports = {7777};
```
    
Make a snake pit:
```
snakePit = std::make_unique<bs::SnakePit>();
```

Add some snake eggs to the snake pit. Every time a new match starts your snake egg should create a new instance of your snake. My snake is named Miranda.
```
snakePit->addSnakeEggs ([] () { return new Miranda(); });
```

Then start the webserver:
```
snakePit->start (o);
```
