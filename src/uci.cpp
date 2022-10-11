#include "uci.h"
#include "perft.h"
#include <string>
#include <sstream>
#include <iostream>

using std::string, std::istringstream, std::skipws, std::cout;

namespace UCI {
    void go(istringstream &stream, Chess game, Color color) {
        string arg, sdepth;
        stream >> skipws >> arg;
        if (arg == "perft") {
            stream >> skipws >> sdepth;
            try {
                perft(game, color, std::stoi(sdepth));
            } catch (std::invalid_argument) {
                cout << "Invalid depth. \"" << sdepth << "\" was recived.";
            }
        }
    }

    void position(istringstream &stream, Chess &game, Color &color) {
        string arg;
        string fen, fen_end;
        stream >> skipws >> arg;
        if (arg == "startpos") {
            game = Chess();
            color = WHITE;
        } else if (arg == "fen") {
            stream >> skipws >> fen;
            std::getline(stream, fen_end);
            game = Chess();
            color = game.setFen(fen + fen_end);
        } else {
            cout << "Unable to parse position input \"" << arg << "\".\n";
        }
    }
}