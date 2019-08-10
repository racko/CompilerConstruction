#include "Print.h"

template <>
std::ostream& showVector(const std::vector<bool>& v, std::ostream& s) {
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

template <>
std::ostream& showVector(const std::vector<std::pair<unsigned int,unsigned int>>& v, std::ostream& s) {
    if (v.empty()) {
        s << "[]";
        return s;
    }
    s << "[ " << show(v[0]);
    for (unsigned int i = 1; i < v.size(); i++)
        s << ", " << show(v[i]);

    s << " ]";
    return s;
}

template <>
std::ostream& showVector(const std::vector<char>& v, std::ostream& s) {
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

std::string print(int c) {
    // TODO: Put all 256 values into an array?
    static const char* const controlChars[] = { "EOF", "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", "BS", "HT", "LF", "VT", "FF", "CR", "SO", "SI", "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB", "CAN", "EM", "SUB", "ESC", "FS", "GS", "RS", "US", "SPACE" };
    if (c < -1 || c > 127)
        return std::to_string(c);
    else if (c < 33)
        return controlChars[c + 1];
    else if (c == 127)
        return "DEL";
    else
        return std::string(1, static_cast<char>(c));
}

//std::ostream& operator<<(std::ostream& s, const showChar c) {
//    static const char* const controlChars[] = { "EOF", "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", "BS", "HT", "LF", "VT", "FF", "CR", "SO", "SI", "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB", "CAN", "EM", "SUB", "ESC", "FS", "GS", "RS", "US", "SPACE" };
//    if (c < -1 || c > 127) {
//        s << std::to_string(c);
//    } else if (c < 33) {
//        s << controlChars[c + 1];
//    } else if (c == 127) {
//        s << "DEL";
//    } else {
//        s << std::string(1, static_cast<char>(c));
//    }
//    return s;
//}

std::string printEscaped(int c) {
    // TODO: Put all 256 values into an array?
    static const char* const controlChars[] = { "EOF", "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", "BS", "HT", "LF", "VT", "FF", "CR", "SO", "SI", "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB", "CAN", "EM", "SUB", "ESC", "FS", "GS", "RS", "US", "SPACE" };
    if (c < -1 || c > 127)
        return std::to_string(c);
    else if (c < 33)
        return controlChars[c + 1];
    else if (c == 127)
        return "DEL";
    else if (c == '"')
        return "\\\"";
    else if (c == '\\')
        return "BSL";
    else
        return std::string(1, static_cast<char>(c));
}

