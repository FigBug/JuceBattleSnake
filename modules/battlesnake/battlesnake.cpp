/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

extern "C"
{
#include "3rdparty/civet/include/civetweb.h"
}

// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.
#include "AppConfig.h"

#include "battlesnake.h"

//==============================================================================
namespace bs
{
#include "utilities/webserver.cpp"
#include "utilities/snake.cpp"
#include "utilities/game.cpp"
#include "utilities/snakepit.cpp"
}
