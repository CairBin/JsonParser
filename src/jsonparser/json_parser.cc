#include "json_parser.h"
#include <cmath>

namespace json_parser{
Json ParseJsonString(const std::string& json_string){
    Parser parser(json_string);
    return parser.Parse();
}

Json ParseJsonString(const char* json_string){
    Parser parser(json_string);
    return parser.Parse();
}

void ParseJsonString(const std::string &json_string,Json& json){
    Parser parser(json_string);
    json = parser.Parse();
}

void ParseJsonString(const char* json_string,Json& json){
    Parser parser(json_string);
    json = parser.Parse();
}

}