#ifndef JSON_H
#define JSON_H

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace json_parser{
enum class JsonType
{
    JSON_NULL,
    JSON_INT,
    JSON_DOUBLE,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT,
};


class Json{
public:
    Json();
    Json(JsonType type);
    Json(int value);
    Json(double value);
    Json(bool value);
    Json(const char *value);
    Json(const std::string &value);

    Json(const Json &other);    //拷贝构造

    ~Json();

    operator bool();
    operator int();
    operator double();
    operator std::string();
    void operator=(const Json & other);

    void Clone(const Json &other);
    void Copy(const Json &other);
    void Append(const Json &other);

    void Remove(int index);
    void Remove(const std::string& key);
    void Remove(const char* key);

    void Insert(int index,const Json& json);
    void Insert(const std::string& key, const Json& json);
    void Insert(const char* key, const Json& json);

    bool FindKey(const std::string& key) const;
    bool FindKey(const char* key) const;

    unsigned long Size() const;

    Json &operator[](int index);
    Json &operator[](const std::string &key);
    Json &operator[](const char* key);

    bool operator==(const Json &other) const;
    bool operator!=(const Json &other) const;
    bool Equal(const Json &other) const;
    Json CopySelf() const;
    unsigned long UseCount();
    std::string ToJsonString();
    
    //类型判断
    bool IsNull() const;
    bool IsInt() const;
    bool IsDouble() const;
    bool IsBool() const;
    bool IsNumber() const;
    bool IsString() const;
    bool IsArray() const;
    bool IsObject() const;

    JsonType get_type() const;

private:
    struct Value{
        int int_value;
        double double_value;
        bool bool_value;
        std::shared_ptr<std::string> string_value;
        std::shared_ptr<std::vector<Json>> array_value;
        std::shared_ptr<std::map<std::string,Json>> object_value;
    };

    struct Value value_;
    JsonType type_;
};
}

#endif