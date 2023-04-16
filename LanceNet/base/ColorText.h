// Author : Lance (Lang Chen) @ nwpu
#ifndef LanceNet_BASE_CONSOLE_COLOR_H
#define LanceNet_BASE_CONSOLE_COLOR_H

#include "LanceNet/base/StringPiece.h"
#include "LanceNet/base/copyable.h"
#include "LanceNet/net/Buffer.h"

// Use the ANSI Escape color code to control the character color output to the console
// \033[0;%dm  set  color
// \033[0m    clear color
// Refer to : https://en.wikipedia.org/wiki/ANSI_escape_code
namespace LanceNet
{
class ColorText : copyable
{
public:
    enum TextColor
    {
        RED,
        GREEN,
        YELLOW,
        NONE_COLOR,
        NUM_COLORS
    };
    static const char* colorCntlCode[TextColor::NUM_COLORS];
public:
    explicit ColorText(StringPiece str, TextColor color);

    // implicit cvt to string
    // to support stream-style output eg.
    //  1. cout << ConsoleColor(..)
    //  2. LogStream << ConsoleColor(...)
    std::string asString();

private:
    net::Buffer buf_;
    TextColor color_;
};

}      // namespace LanceNet

// stream-style output support
template <typename ostream>
ostream& operator<<(ostream& os, LanceNet::ColorText consoleColor)
{
    os << consoleColor.asString();
    return os;
}

#endif //  LanceNet_BASE_CONSOLE_COLOR_H


