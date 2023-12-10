#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "jsonparser/json.h"
#include "jsonparser/parser.h"

namespace json_parser{

Json ParseJsonString(const std::string &json_string);
Json ParseJsonString(const char *json_string);
void ParseJsonString(const std::string &json_string, Json &json);
void ParseJsonString(const char *json_string, Json &json);
}

#endif