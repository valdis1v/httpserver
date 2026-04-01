#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
/***
 * 0 Grün
 * 1 Info
 * 2 Warnung
 * 3 Fehler
 */
void write_log(const char* msg, int sev) noexcept;
/***
 * 0 Grün
 * 1 Info
 * 2 Warnung
 * 3 Fehler
 */
void write_log(const std::string msg, int sev) noexcept;


#endif
