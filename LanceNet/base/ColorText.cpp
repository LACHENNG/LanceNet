#include "LanceNet/base/StringPiece.h"
#include <LanceNet/base/ColorText.h>
#include <iostream>
namespace LanceNet
{

const char* ColorText::colorCntlCode[ColorText::TextColor::NUM_COLORS] =
{
    // "\033[31m",
    // "\033[32m",
    // "\033[33m",
    // "\033[0m",
    "\033[91m",
    "\033[92m",
    "\033[93m",
    "\033[0m",
};

ColorText::ColorText(StringPiece str, TextColor color)
  : buf_(64, 12),
    color_(color)
{
    buf_.append(str);

    // prepend color control code
    const char* cntl = ColorText::colorCntlCode[color];
    buf_.preappend(cntl, strlen(cntl));

    // clear color
    buf_.append("\033[0m", strlen("\033[0m")) ;
    const char* non_color= ColorText::colorCntlCode[ColorText::NONE_COLOR];
    buf_.append(non_color, strlen(non_color));
}


std::string ColorText::asString()
{
    std::string str(buf_.retrieveAllAsString());
    // std::cout << "\033[0masString: " << str << std::endl;
    return str;
}


} // namespace LanceNet
