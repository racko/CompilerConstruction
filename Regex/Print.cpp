#include "Regex/Print.h"

#include <sstream>

std::ostream& showImpl<std::vector<bool>>::Show(std::ostream& s, const std::vector<bool>& v) {
    unsigned int i = 0;

    while (i < v.size() && !v[i])
        i++;

    if (i == v.size()) {
        s << "{}";
        return s;
    }
    s << "{ " << i;
    for (i++; i < v.size(); i++)
        if (v[i])
            s << ", " << i;

    s << " }";
    return s;
}

std::ostream& showImpl<std::vector<char>>::Show(std::ostream& s, const std::vector<char>& v) {
    if (v.empty()) {
        s << "[]";
        return s;
    }
    s << "[ " << showCharEscaped(v[0]);
    for (unsigned int i = 1; i < v.size(); i++)
        s << ", " << showCharEscaped(v[i]);

    s << " ]";
    return s;
}

std::ostream& operator<<(std::ostream& s, const showChar sc) {
    const auto c = sc.c_;
    // TODO: Put all 256 values into an array?
    static const char* const controlChars[] = {"EOF", "NUL", "SOH", "STX", "ETX", "EOT", "ENQ",  "ACK", "BEL",
                                               "BS",  "HT",  "LF",  "VT",  "FF",  "CR",  "SO",   "SI",  "DLE",
                                               "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",  "CAN", "EM",
                                               "SUB", "ESC", "FS",  "GS",  "RS",  "US",  "SPACE"};
    if (c < -1 || c > 127)
        return s << c;
    else if (c < 33)
        return s << controlChars[c + 1];
    else if (c == 127)
        return s << "DEL";
    else
        return s << static_cast<char>(c);
}

std::string print(int c) {
    std::stringstream s;
    s << showChar(c);
    return s.str();
}

std::ostream& operator<<(std::ostream& s, const showCharEscaped sc) {
    const auto c = sc.c_;
    // TODO: Put all 256 values into an array?
    static const char* const controlChars[] = {"EOF", "NUL", "SOH", "STX", "ETX", "EOT", "ENQ",  "ACK", "BEL",
                                               "BS",  "HT",  "LF",  "VT",  "FF",  "CR",  "SO",   "SI",  "DLE",
                                               "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",  "CAN", "EM",
                                               "SUB", "ESC", "FS",  "GS",  "RS",  "US",  "SPACE"};
    if (c < -1 || c > 127)
        return s << c;
    else if (c < 33)
        return s << controlChars[c + 1];
    else if (c == 127)
        return s << "DEL";
    else if (c == '"')
        return s << "\\\"";
    else if (c == '\\')
        return s << "BSL";
    else
        return s << static_cast<char>(c);
}

std::string printEscaped(int c) {
    std::stringstream s;
    s << showCharEscaped(c);
    return s.str();
}
