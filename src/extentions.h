#ifndef EXTENTIONS_H
#define EXTENTIONS_H
#include <ctime>
#include <iostream>

std::ostream& operator<<(std::ostream& os, const std::tm& tm);

namespace df
{
std::string formatPgTimeInterval(const std::string &interval);
}

#endif // EXTENTIONS_H
