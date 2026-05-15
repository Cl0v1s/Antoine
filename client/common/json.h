#ifndef JSON_H
#define JSON_H

#include <stdint.h>
#include <string>

static inline std::string jsonStringValue(const std::string &json, const std::string &key) {
    std::string search = "\"" + key + "\":\"";
    size_t start = json.find(search);
    if (start == std::string::npos) return "";
    start += search.length();
    std::string result;
    bool escape = false;
    for (size_t i = start; i < json.length(); i++) {
        char c = json[i];
        if (escape) {
            if      (c == 'n')  result += '\n';
            else if (c == '"')  result += '"';
            else if (c == '\\') result += '\\';
            else                result += c;
            escape = false;
        } else if (c == '\\') {
            escape = true;
        } else if (c == '"') {
            break;
        } else {
            result += c;
        }
    }
    return result;
}

static inline uint16_t jsonUint16Value(const std::string &json, const std::string &key) {
    std::string search = "\"" + key + "\":";
    size_t start = json.find(search);
    if (start == std::string::npos) return 0;
    start += search.length();
    return (uint16_t)std::stoul(json.substr(start));
}

static inline std::string jsonEscape(const std::string &input) {
    std::string output;
    for (char c : input) {
        switch (c) {
            case '\"': output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\b': output += "\\b"; break;
            case '\f': output += "\\f"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default:
                if (c >= 0x20 && c <= 0x7E) {
                    output += c;
                }
        }
    }
    return output;
}


#endif 