#include "jsonparser/json.h"
#include <stdexcept>
#include <sstream>
#include <algorithm>

namespace json_parser{
Json::Json()
    :type_(JsonType::JSON_NULL){
    
}

Json::Json(int value)
    :type_(JsonType::JSON_INT){
    value_.int_value = value;
}

Json::Json(double value)
    :type_(JsonType::JSON_DOUBLE){
    value_.double_value = value;
}

Json::Json(bool value)
    :type_(JsonType::JSON_BOOL){
    value_.bool_value = value;
}

Json::Json(const char* value)
    :type_(JsonType::JSON_STRING){
    value_.string_value = std::make_shared<std::string>(value);
}

Json::Json(const std::string& value)
    :type_(JsonType::JSON_STRING){
    value_.string_value = std::make_shared<std::string>(value);
}

Json::Json(JsonType type)
    :type_(type){
    switch(type_){
    case JsonType::JSON_NULL:
        break;
    case JsonType::JSON_INT:
        value_.int_value = 0;
        break;
    case JsonType::JSON_DOUBLE:
        value_.double_value = 0.0;
        break;
    case JsonType::JSON_BOOL:
        value_.bool_value = false;
        break;
    case JsonType::JSON_STRING:
        value_.string_value = std::make_shared<std::string>("");
        break;
    case JsonType::JSON_ARRAY:
        value_.array_value = std::make_shared<std::vector<Json>>();
        break;
    case JsonType::JSON_OBJECT:
        value_.object_value = std::make_shared<std::map<std::string, Json>>();
        break;
    default:
        break;
    }
}

Json::operator bool(){
    if(this->type_ != JsonType::JSON_BOOL)
        throw std::logic_error("type error: the type is not bool");
    return value_.bool_value;
}

Json::operator int(){
    if(this->type_ != JsonType::JSON_INT)
        throw std::logic_error("type error: the type is not int");
    return value_.int_value;
}

Json::operator double(){
    if(this->type_ != JsonType::JSON_DOUBLE)
        throw std::logic_error("type error: the type is not double");
    return value_.double_value;
}

Json::operator std::string(){
    if(this->type_ != JsonType::JSON_STRING)
        throw std::logic_error("type error: the type is not string");
    return *value_.string_value;
}

//拷贝，写时复制，只要不更改都是浅拷贝
void Json::Clone(const Json& other){
    this->type_ = other.type_;
    switch(type_){
    case JsonType::JSON_NULL:
        break;
    case JsonType::JSON_BOOL:
        value_.bool_value = other.value_.bool_value;
        break;
    case JsonType::JSON_INT:
        value_.int_value = other.value_.int_value;
        break;
    case JsonType::JSON_DOUBLE:
        value_.double_value = other.value_.double_value;
        break;
    case JsonType::JSON_STRING:
        value_.string_value = other.value_.string_value;    
        break;
    case JsonType::JSON_ARRAY:
        value_.array_value = other.value_.array_value;
        break;
    case JsonType::JSON_OBJECT:
        value_.object_value = other.value_.object_value;
        break;
    default:
        break;
    }
}

//直接深拷贝，无论是否发生写操作
void Json::Copy(const Json& other){
    this->type_ = other.type_;
    switch (type_)
    {
    case JsonType::JSON_NULL:
        break;
    case JsonType::JSON_BOOL:
        value_.bool_value = other.value_.bool_value;
        break;
    case JsonType::JSON_INT:
        value_.int_value = other.value_.int_value;
        break;
    case JsonType::JSON_DOUBLE:
        value_.double_value = other.value_.double_value;
        break;
    case JsonType::JSON_STRING:
        value_.string_value = std::make_shared<std::string>(*other.value_.string_value);
        break;
    case JsonType::JSON_ARRAY:
        value_.array_value = std::make_shared<std::vector<Json>>(*other.value_.array_value);
        break;
    case JsonType::JSON_OBJECT:
        value_.object_value = std::make_shared<std::map<std::string, Json>>(*other.value_.object_value);
        break;
    default:
        break;
    }
}


Json::Json(const Json& other){
    this->Clone(other);
}

void Json::operator=(const Json& other){
    this->Clone(other);
}

Json& Json::operator[](int index){
    if(type_ != JsonType::JSON_ARRAY){
        throw std::logic_error("type error: the type is not json array");
    }
    if(index < 0){
        throw std::logic_error("range error: the index cannot less than 0");
    }

    int size = value_.array_value->size();
    if(index >= size){
        throw std::logic_error("range error: the index out of range");
    }

    return (*value_.array_value)[index];
}

Json& Json::operator[](const std::string& key){
    if(type_ != JsonType::JSON_OBJECT){
        throw std::logic_error("type error: the type is not json object");
    }

    return (*value_.object_value)[key];
}

Json& Json::operator[](const char* key){
    if(type_ != JsonType::JSON_OBJECT){
        throw std::logic_error("type error: the type is not json object");
    }

    return (*value_.object_value)[key];
}

void Json::Append(const Json& other){
    if(type_ != JsonType::JSON_ARRAY){
        throw std::logic_error("type error: the type is not json array");
    }
    Json json;
    json.Copy(other);
    value_.array_value = std::make_shared<std::vector<Json>>(*value_.array_value);
    value_.array_value->emplace_back(json);
}

std::string Json::ToJsonString(){
    std::stringstream ss;
    switch (type_)
    {
    case JsonType::JSON_NULL:
        ss << "null";
        break;
    case JsonType::JSON_BOOL:
        ss << (value_.bool_value ? "true" : "false");
        break;
    case JsonType::JSON_INT:
        ss << value_.int_value;
        break;
    case JsonType::JSON_DOUBLE:
        ss << value_.double_value;
        break;
    case JsonType::JSON_STRING:
        ss << '\"' << *(value_.string_value) << '\"';
        break;
    case JsonType::JSON_ARRAY:
        ss << '[';
        for (auto it = value_.array_value->begin();
            it != value_.array_value->end();
            it++)
        {
            if (it != value_.array_value->begin())
            {
                ss << ',';
            }
            ss << it->ToJsonString();
        }
        ss << ']';
        break;
    case JsonType::JSON_OBJECT:
        ss << '{';
        for (auto it = value_.object_value->begin();
            it != value_.object_value->end();
            it++)
        {
            if (it != value_.object_value->begin())
            {
                ss << ',';
            }
            ss << '\"' << it->first << '\"' << ":" << it->second.ToJsonString();
        }
        ss << '}';
        break;
    default:
        break;
    }

    return ss.str();
}

bool Json::operator==(const Json& other)const{
    if(type_ != other.type_)
        return false;
    
    switch(type_){
    case JsonType::JSON_NULL:
        return true;
    case JsonType::JSON_BOOL:
        return value_.bool_value == other.value_.bool_value;
    case JsonType::JSON_INT:
        return value_.int_value == other.value_.int_value;
    case JsonType::JSON_DOUBLE:
        return value_.double_value == other.value_.double_value;
    case JsonType::JSON_STRING:
        return value_.string_value == other.value_.string_value;
    case JsonType::JSON_ARRAY:
        return value_.array_value == other.value_.array_value;
    case JsonType::JSON_OBJECT:
        return value_.object_value == other.value_.object_value;
    default:
        return false;
    }
}

bool Json::operator!=(const Json& other)const{
    return !(*this == other);
}

bool Json::Equal(const Json& other)const{
    if (type_ != other.type_)
        return false;

    switch (type_)
    {
    case JsonType::JSON_NULL:
        return true;
    case JsonType::JSON_BOOL:
        return value_.bool_value == other.value_.bool_value;
    case JsonType::JSON_INT:
        return value_.int_value == other.value_.int_value;
    case JsonType::JSON_DOUBLE:
        return value_.double_value == other.value_.double_value;
    case JsonType::JSON_STRING:
        return *(value_.string_value) == *(other.value_.string_value);
    case JsonType::JSON_ARRAY:
        return *(value_.array_value) == *(other.value_.array_value);
    case JsonType::JSON_OBJECT:
        {
            if (value_.object_value->size() != other.value_.object_value->size())
                return false;
            auto iter1 = value_.object_value->begin();
            auto iter2 = other.value_.object_value->begin();
            for (; iter1 != value_.object_value->end() && iter2 != other.value_.object_value->end(); iter1++, iter2++)
            {
                if (iter1->first != iter2->first)
                    return false;
                if (!(iter2->second.Equal(iter2->second)))
                    return false;
            }
            return true;
        }
    default:
        return false;
    }
}

//制造一份自己的拷贝，深拷贝
Json Json::CopySelf() const{
    Json json;
    json.Copy(*this);
    return json;
}

bool Json::IsNull() const{
    return type_ == JsonType::JSON_NULL;
}

bool Json::IsBool() const{
    return type_ == JsonType::JSON_BOOL;
}

bool Json::IsInt() const{
    return type_ == JsonType::JSON_INT;
}

bool Json::IsDouble() const{
    return type_ == JsonType::JSON_DOUBLE;
}

bool Json::IsString() const{
    return type_ == JsonType::JSON_STRING;
}

bool Json::IsArray() const{
    return type_ == JsonType::JSON_ARRAY;
}

bool Json::IsObject() const{
    return type_ == JsonType::JSON_OBJECT;
}

bool Json::IsNumber() const{
    return type_ == JsonType::JSON_INT || type_ == JsonType::JSON_DOUBLE;
}

JsonType Json::get_type() const{
    return this->type_;
}

unsigned long Json::UseCount(){
    switch(type_){
        case JsonType::JSON_OBJECT:
            return value_.object_value.use_count();
        case JsonType::JSON_ARRAY:
            return value_.array_value.use_count();
        case JsonType::JSON_STRING:
            return value_.string_value.use_count();
        case JsonType::JSON_BOOL:
        case JsonType::JSON_INT:
        case JsonType::JSON_DOUBLE:
        case JsonType::JSON_NULL:
            throw std::logic_error("type error: no reference counters of the type");
        default:
            throw std::runtime_error("unknow error: occurred in method `UseCount()`");
    }
}

void Json::Remove(int index){
    if(type_!=JsonType::JSON_ARRAY){
        throw std::logic_error("type error: the type is not json array");
    }

    if(index < 0){
        throw std::logic_error("range error: the index cannot less than 0");
    }

    if(index >= value_.array_value->size()){
        throw std::logic_error("range error: the index out of the size");
    }
    value_.array_value = std::make_shared<std::vector<Json> >(*value_.array_value);
    value_.array_value->erase(value_.array_value->begin()+index);
}

void Json::Insert(int index, const Json& json){
    if(type_ != JsonType::JSON_ARRAY){
        throw std::logic_error("type error: the type is not json array");
    }

    if(index < 0){
        throw std::logic_error("range error: the index cannot less than 0");
    }

    if(index > value_.array_value->size()){
        throw std::logic_error("range error: the index cannot more than the array size");
    }

    value_.array_value = std::make_shared<std::vector<Json>>(*value_.array_value);
    value_.array_value->insert(value_.array_value->begin()+index,json.CopySelf());
}

void Json::Insert(const std::string& key, const Json& json){
    if(type_!=JsonType::JSON_OBJECT){
        throw std::logic_error("type error: the type is not json object");
    }

    value_.object_value = std::make_shared<std::map<std::string,Json>>(*value_.object_value);
    (*value_.object_value)[key] = json.CopySelf();
}

void Json::Insert(const char* key, const Json& json){
    if(type_!=JsonType::JSON_OBJECT){
        throw std::logic_error("type error: the type is not json object");
    }

    value_.object_value = std::make_shared<std::map<std::string,Json>>(*value_.object_value);
    (*value_.object_value)[key] = json.CopySelf();
}

void Json::Remove(const std::string& key){
    if(type_!=JsonType::JSON_OBJECT){
        throw std::logic_error("type error: the type is not json object");
    }

    if(value_.object_value->find(key) == value_.object_value->end()){
        return; //键不存在直接返回，没必要进行复制
    }

    //否则写时复制
    value_.object_value = std::make_shared<std::map<std::string,Json>>(*value_.object_value);
    value_.object_value->erase(key);
}

void Json::Remove(const char* key){
    if(type_!=JsonType::JSON_OBJECT){
        throw std::logic_error("type error: the type is not json object");
    }

    if(value_.object_value->find(key) == value_.object_value->end()){
        return; //键不存在直接返回，没必要进行复制
    }

    //否则写时复制
    value_.object_value = std::make_shared<std::map<std::string,Json>>(*value_.object_value);
    value_.object_value->erase(key);
}

unsigned long Json::Size()const{
    switch(type_){
        case JsonType::JSON_ARRAY:
            return value_.array_value->size();
        case JsonType::JSON_OBJECT:
            return value_.object_value->size();
        case JsonType::JSON_STRING:
            return value_.string_value->size();
        case JsonType::JSON_INT:
        case JsonType::JSON_BOOL:
        case JsonType::JSON_DOUBLE:
        case JsonType::JSON_NULL:
            throw std::logic_error("type error: unsupport the method for this type");
        default:
            throw std::runtime_error("unknow error: occurred in `Size()`");
    }
}

bool Json::FindKey(const std::string& key) const{
    if(type_ != JsonType::JSON_OBJECT){
        throw std::logic_error("type error: the type is not json object");
    }

    return value_.object_value->find(key) == value_.object_value->end();
}

bool Json::FindKey(const char* key) const{
    if(type_ != JsonType::JSON_OBJECT){
        throw std::logic_error("type error: the type is not json object");
    }

    return value_.object_value->find(key) == value_.object_value->end();
}


Json::~Json() {}

}