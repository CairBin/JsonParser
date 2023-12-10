#include "jsonparser/parser.h"
#include <stdexcept>
#include <cmath>
// #include <iostream>

namespace json_parser{

bool Scanner::IsEnd(){
    return current_ >= json_string_.end();
}

char Scanner::Advance(){
    char c = *current_;
    ++current_;
    return c;
}

void Scanner::ScanTrue(){
    if(json_string_.compare(
            std::distance(json_string_.begin(),current_),   //获取当前索引
            3,
            "rue"
        )==0){
        current_ += 3;
    }else{
        throw std::runtime_error("format error: invalid json string, the `true` error");
    }
}

void Scanner::ScanFalse(){
    if (json_string_.compare(
            std::distance(json_string_.begin(), current_), // 获取当前索引
            4,
            "alse") == 0){
        current_ += 4;
    }
    else{
        throw std::runtime_error("format error: invalid json string, the `false` error");
    }
}

void Scanner::ScanNull(){
    if (json_string_.compare(
            std::distance(json_string_.begin(), current_), // 获取当前索引
            3,
            "ull") == 0){
        current_ += 3;
    }
    else{
        throw std::runtime_error("format error: invalid json string, the `null` error");
    }
}

void Scanner::ScanString(){
    std::string::iterator temp = current_;

    while(Peek()!= '\"' && !IsEnd()){
        Advance();
        if(Peek()!='\\')
            continue;
        Advance();
        switch(Peek()){
        case 'b':
        case 't':
        case 'n':
        case 'f':
        case 'r':
        case '\"':
        case '\\':
            Advance();
            break;
        case 'u':
            for (int i = 0; i < 4; i++){    //Unicode长度4字节
                Advance();
            }
            break;
        default:
            break;
        }
    }

    if(IsEnd()){
        throw std::runtime_error("format error: invalid json string, missing closing quote");
    }
    Advance();

    this->value_string_ = std::string(temp, current_-1);
}

void Scanner::ScanNumber(){
    std::string::iterator temp = current_-1;

    while(IsDigit(Peek())){
        Advance();
    }

    if(Peek()=='.' && IsDigit(PeekNext())){
        Advance();
        while(IsDigit(Peek())){
            Advance();
        }
    }

    value_number_ = std::atof(
        std::string(temp, current_)
            .c_str()
        );
}

bool Scanner::IsDigit(char c){
    return c>='0' && c<='9';
}

char Scanner::Peek(){
    if(IsEnd())
        return '\0';
    return *current_;
}

char Scanner::PeekNext(){
    if(current_+1 >= json_string_.end())
        return '\0';
    
    return *(current_+1);
}

JsonTokenType Scanner::Scan()
{
    if(IsEnd())
        return JsonTokenType::END_OF_FILE;
    last_ = current_;   //记录当前指针，以保证下次Scan可以回滚至当前状态
    char c = Advance();
    // std::cout << c << std::endl;
    switch(c){
    case '{':
        return JsonTokenType::BEGIN_OBJECT;
    case '}':
        return JsonTokenType::END_OBJECT;
    case '[':
        return JsonTokenType::BEGIN_ARRAY;
    case ']':
        return JsonTokenType::END_ARRAY;
    case ':':
        return JsonTokenType::NAME_SEPARATOR;
    case ',':
        return JsonTokenType::VALUE_SEPARATOR;
    case 't':
        ScanTrue();
        return JsonTokenType::LITERAL_TRUE;
    case 'f':
        ScanFalse();
        return JsonTokenType::LITERAL_FALSE;
    case 'n':
        ScanNull();
        return JsonTokenType::LITERAL_NULL;
    case ' ':
    case '\t':
    case '\n':
    case '\r':
        return Scan();
    case '\"':
        ScanString();
        return JsonTokenType::VALUE_STRING;
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        ScanNumber();
        return JsonTokenType::VALUE_NUMBER;
    default:
        throw std::runtime_error("format error: invalid json string");
    }
}

Scanner::Scanner(const std::string& json_string)
    : json_string_(json_string){
    current_ = json_string_.begin();
}

Scanner::Scanner(const char* json_string)
    : json_string_(json_string){
    current_ = json_string_.begin();
}

bool Scanner::get_bool_value() const{
    return this->value_bool_;
}

double Scanner::get_number_value() const{
    return this->value_number_;
}

std::string Scanner::get_string_value() const{
    return this->value_string_;
}

const std::string& Scanner::get_string_value_quick() const{
    return this->value_string_;
}

//状态回滚
void Scanner::Rollback(){
    current_ = last_;
}

Parser::Parser(const std::string& json_string)
    :scanner_(json_string){
    
}

Parser::Parser(const char *json_string)
    :scanner_(json_string){

}

Json Parser::Parse(){
    JsonTokenType token_type = scanner_.Scan();
    switch (token_type)
    {
    case JsonTokenType::END_OF_FILE:
    case JsonTokenType::LITERAL_NULL:
        return Json(JsonType::JSON_NULL);
    case JsonTokenType::VALUE_STRING:
        return Json(scanner_.get_string_value());
    case JsonTokenType::VALUE_NUMBER:
        {
            double temp = scanner_.get_number_value();
            if (std::ceil(temp) == floor(temp))
                return Json((int)temp);
            else
                return Json(temp);
        }
    case JsonTokenType::LITERAL_TRUE:
        return Json(true);
    case JsonTokenType::LITERAL_FALSE:
        return Json(false);
    case JsonTokenType::BEGIN_ARRAY:
        return ParseArray();
    case JsonTokenType::BEGIN_OBJECT:
        return ParseObject();
    default:
        return Json();
    }
}

Json Parser::ParseObject(){
    Json json(JsonType::JSON_OBJECT);
    JsonTokenType token_type = scanner_.Scan();
    if(token_type == JsonTokenType::END_OBJECT){
        return json;
    }
    scanner_.Rollback();
    while(true){
        token_type = scanner_.Scan();
        if(token_type!=JsonTokenType::VALUE_STRING){
            throw std::runtime_error("format error: invalid json string, the key must be a string");
        }
        std::string key = scanner_.get_string_value();
        token_type = scanner_.Scan();
        if(token_type != JsonTokenType::NAME_SEPARATOR){
            throw std::runtime_error("format error: invalid json string, expected `:`");
        }
        json[key] = Parse();    //递归解析

        token_type = scanner_.Scan();
        if(token_type == JsonTokenType::END_OBJECT){
            break;
        }

        if(token_type != JsonTokenType::VALUE_SEPARATOR){
            throw std::runtime_error("format error: invalid json string, expected `,`");
        }
    }
    return json;
}

Json Parser::ParseArray(){
    Json json(JsonType::JSON_ARRAY);
    JsonTokenType token_type = scanner_.Scan();
    if(token_type == JsonTokenType::END_ARRAY){
        return json;
    }
    scanner_.Rollback();

    while(true){
        json.Append(Parse());
        token_type = scanner_.Scan();

        if(token_type == JsonTokenType::END_ARRAY){
            break;
        }

        if(token_type != JsonTokenType::VALUE_SEPARATOR){
            throw std::runtime_error("format error: invalid json string, expected `,`");
        }
    }

    return json;
}

}