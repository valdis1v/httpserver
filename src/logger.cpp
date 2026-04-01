

#include <cstring>
#include <mutex>
#include <string>
#include <unistd.h>


static std::mutex lock_q;
#define rot "\033[31m"
#define gelb "\033[33m"
#define gruen "\033[32m"
#define blau "\033[34m"
#define reset_c "\033[0m"


/***
 * 0 Grün
 * 1 Info
 * 2 Warnung
 * 3 Fehler
 */
void log(const char* msg, int sev) noexcept {
    const char* b = reset_c;
    switch(sev) {
        case 0:
            b = gruen;
            break;
        case 1:
            b = blau "[Info]\n";
            break;
        case 2:
            b = gelb "[Warning]\n";
            break;
        case 3:
            b = rot "[Error]\n";
            break;
    }

    std::lock_guard<std::mutex> lock(lock_q);
    // 1 linux default stdout fd
    write(1, b, 5);
    write(1, msg, strlen(msg));
    write(1, reset_c "\n", 5);
}

void log(const std::string msg, int sev) noexcept {
    const char* b = reset_c;
    int blen = 0;
    switch(sev) {
        case 0:
            b = gruen;
            break;
        case 1:
            b = blau "[Info]\n";
            blen = 13;
            break;
        case 2:
            b = gelb "[Warning]\n";
            blen = 16;
            break;
        case 3:
            b = rot "[Error]\n";
            blen = 14;
            break;
    }

    std::lock_guard<std::mutex> lock(lock_q);
    write(1, b, blen);
    write(1, msg.data(), msg.length());
    write(1, reset_c "\n", 5);
}
