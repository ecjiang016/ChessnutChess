#include "game.h"
#include "uci.h"
#include <iostream>
#include <string>
#include <sstream>

using std::string, std::skipws;

int main() {
    Chess game;
    Color color = WHITE;

    string cmd, fcmd; //fcmd is the first word in the command (cmd)
    do {
        if (!getline(std::cin, cmd))
            cmd = "quit"; //Break on EOF

        std::istringstream stream(cmd);
        stream >> skipws >> fcmd;

        if (fcmd == "go") UCI::go(stream, game, color);
        else if (fcmd == "position") UCI::position(stream, game, color);
        else if (fcmd == "d") game.print();

        stream.clear();

    } while (cmd != "quit");
}