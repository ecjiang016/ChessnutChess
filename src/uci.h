#pragma once
#include "game.h"
#include <sstream>

using std::istringstream;

namespace UCI {
    void go(istringstream &stream, Chess game, Color color);
    void position(istringstream &stream, Chess &game, Color &color);
}