#include "LanceNet/base/StringPiece.h"
#include <LanceNet/base/ColorText.h>
#include <iostream>
namespace LanceNet
{

const char* ColorText::colorCntlCode[ColorText::TextColor::NUM_COLORS] =
{
    "\033[91m", // RED
    "\033[92m", // GREEN
    "\033[93m", // YELLOW
    "\033[0m",  // NONE
};

ColorText::ColorText(StringPiece str, TextColor color)
  : color_(color)
{
    // prepend color control code
    const char* cntl = ColorText::colorCntlCode[color];
    buf_.append(cntl, strlen(cntl));

    buf_.append(str.data());

    // clear color
    const char* noneColor = colorCntlCode[TextColor::NONE_COLOR];
    buf_.append(noneColor, strlen(noneColor)) ;
}


std::string ColorText::asString() const
{
    // std::cout << "\033[0masString: " << str << std::endl;
    return buf_;
}


} // namespace LanceNet
