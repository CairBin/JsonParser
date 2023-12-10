# JsonParser
## 描述
JsonParser 是一个 Json 解析器，用于将 Json 字符串解析成对象。

## 使用

### 命名空间
JsonParser所有的方法和类型均在`json_parser`命名空间下。

### Json对象
你可以直接对Json对象赋值，如果不用等号进行赋值或者构造器构造，默认情况下为json下的`null`。
```cpp
#include "json_parser.h"
using json_parser;

[...]
int main(){
    Json int_obj = 1;               //json int
    Json string_obj = "string";     //json string
    Json bool_obj = true;           //json bool
    Json double_obj = 1.23;         //json double

    //或者你可以这样做
    /*
    Json int_obj(1);
    Json string_obj("string");
    Json bool_obj(true);
    Json double_obj(1.23);
    */
}

```
当然你也可以将Json对象的值取出，然后赋值给基础类型。
```cpp
[...]
int main(){
    [...]
    int a = int_obj;
    std::string b = string_obj;
    bool c = bool_obj;
    double d = double_obj;

}

```
在Json中还有两类特殊的类型，Json对象和Json数组，你需要用构造函数去创建这种类型的对象。
```cpp
[...]
int main(){
    [...]
    Json arr_obj(JsonType::JSON_ARRAY);
    Json obj_obj(JsonType::JSON_OBJECT);
}
```

对于Json数组，你可以用`Append(Json& other)`方法去添加元素。
```cpp
[...]
int main(){
    [...]
    Json elem1 = 1;
    arr_obj.Append(elem1);

    //你可以更加方便地使用这个方法
    arr_obj.Append(1);
}

```

```cpp

```