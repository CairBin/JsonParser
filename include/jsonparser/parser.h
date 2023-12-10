#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "jsonparser/json.h"

namespace json_parser{

enum class JsonTokenType
{
    BEGIN_OBJECT, //{
    END_OBJECT,   //}

    BEGIN_ARRAY, //[
    END_ARRAY,   //]

    VALUE_SEPARATOR, //,
    NAME_SEPARATOR,  //:

    VALUE_STRING,
    VALUE_NUMBER,

    LITERAL_TRUE,
    LITERAL_FALSE,
    LITERAL_NULL,

    END_OF_FILE    //EOF
};

class Scanner{
public:
    Scanner(const char* json_string);
    Scanner(const std::string &json_string);

    JsonTokenType Scan();

    bool get_bool_value() const;
    double get_number_value() const;
    std::string get_string_value() const;
    const std::string &get_string_value_quick() const;

    void Rollback();    //状态回滚

private:
    bool IsEnd();
    char Advance();
    void ScanTrue();
    void ScanFalse();
    void ScanNull();
    void ScanString();
    void ScanNumber();

    bool IsDigit(char c);
    char Peek();
    char PeekNext();


private:
    std::string json_string_;
    std::string::iterator current_;
    std::string::iterator last_;

    std::string value_string_;
    double value_number_;
    bool value_bool_;
};

class Parser{
public:
    Parser(const std::string &json_string);
    Parser(const char *json_string);
    Json Parse();

private:
    Json ParseObject();
    Json ParseArray();
    

private:
    Scanner scanner_;
};
}

#endif