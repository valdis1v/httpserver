#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
void log(const char* msg, int sev) noexcept;

void log(const std::string msg, int sev) noexcept;


#endif
