# JsonParser
## 描述
JsonParser 是一个 Json 解析器，用于将 Json 文本解析成对象。

## 使用

### 命名空间
JsonParser所有的方法和类型均在`json_parser`命名空间下。

### 写时复制
```cpp
Json common1 = 1;
Json common2 = common2;

Json special1(JsonType::JSON_ARRAY);
Json special2 = special1;
```
对于普通Json类型的对象，如`common1`和`common2`，它们的关系就如同将一个`int`类型的变量赋值给另一个`int`类型一样，内存并不共享。

对于Json对象或者Json数组，它们会在堆区申请一段内存，如果你将一个Json数组类型的对象拷贝给另一个对象，如`special1`和`special2`，则它们共享同一段内存，即发生浅拷贝，这等价于`special2.Clone(special1)`。

如果你想对其进行深拷贝，则可以调用`void Json::Copy(const Json& other)`方法，如`special2.Copy(special1)`，这样它们的内存就不是共享的。

对于共享内存的对象，如果使用`Append`,`Insert`,`Remove`等方法对其进行修改，则会触发**写时复制**机制，即被修改的对象会先在堆区申请一块内存，然后将原来的值拷贝过来，再进行修改。
对于它们的底层容器，你在使用特定接口修改时无需担心内存泄露问题，它会在底层维护一个引用计数器，当引用为0的时候会自动释放内存。**为了避免循环引用，对于Append等方法传入的值也会进行一次深拷贝。**

但是如果用`[]`运算符对其容器内的对象进行操作，则会**绕过写时复制机制**，甚至对于`special1[0] = (special1)`会发生循环引用导致内存泄露。但由于此操作未发生深拷贝，它们在效率上是大于`Append`等方法的。因此是否使用这种方式对对象进行修改应当取决于你实际需要，如果你确保实际情况下不会发生内存泄露等问题，且对于性能要求较高，则可以考虑使用这种方法。

这里值得注意，共享内存的部分只是`json_parser::Json`类型底层的容器，而`json_parser::Json`对象的引用是否指向同一内存仍取决于你。

### 线程安全
对于内部容器的引用计数器，其线程安全是保证的，但对于`json_parser::Json`对象本身的引用是需要你手动处理。当多个线程共享一个对象时，你应当考虑是否对临界操作加锁这个问题。

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

对于Json数组，你可以用`void Json::Append(const Json& other)`方法去添加元素。
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
如果你想在任意位置进行插入，则可以调用`void Json::Insert(int index, const Json& json)`方法，其索引不应该小于0且不应该大于当前容器长度，否则会抛出异常。

对于`Insert`方法还有两个重载，它们分别是`void Json::Insert(const std::string key, const Json& json)`和`void Json::Insert(const char* key, const Json& json)`，这是对于Json对象类型进行操作的，用于向其内部添加元素。

与`Insert`方法类似的还有`Remove`，用于删除指定索引的元素。

### 判断相等

`json_parser::Json`类型重载了`==`运算符，但是它还有一个判断相等的方法`bool Json::Equal(const Json& other)`，这有些类似于Java，`==`预算符仅比较是否指向同一块内存，而`bool Json::Equal(const Json& other)`会判断Json内容是否相等。**但是对于普通Json类型二者是无区别的，都是判断值是否相等！**

在没有发生深拷贝的情况下，你可以使用`==`来判断值相等以提高效率，毕竟这种情况下指向同一段内存其值相等是一定的。但这对程序员判断是否发生深拷贝有一定的要求，可能会增加程序的维护成本。

### 拷贝与克隆
对于拷贝操作有两种方法，分别是`void Json::Copy(const Json& other)`和`void Json::Clone(const Json& other)`，前者为深拷贝，后者与`=`预算符一致为浅拷贝。

另外你可以用`Json Json::CopySelf()`来返回一个自身的深拷贝对象，由于是深拷贝，其内部容器内存是新申请的，其引用计数器是新的且从0开始，因此不会发生循环引用的问题。

### 输出Json

使用`std::string Json::ToJsonString()`方法可以字符串的方式输出Json，其返回类型为`std::string`。
```cpp
[...]
int main(){
    [...]
    std::cout << obj_obj.ToJsonString() << std::endl;
}

```

### 解析Json格式的字符串

在`parser_json.h`文件中声明了几个用于解析外部Json文本的函数

* `Json ParseJsonString(const std::string &json_string)`
* `Json ParseJsonString(const char *json_string)`
* `void ParseJsonString(const std::string &json_string, Json &json)`
* `void ParseJsonString(const char *json_string, Json &json)`

用法如下所示：
```cpp
const char *s = "{\
					\"name\":\"Can\",\
					\"age\":23,\
					\"sex\":\"hahah\"\
				}";
Json json = ParseJsonString(s);

```

你可能会误以为是这种方式：
```cpp
Json json(s);   //这是错误的用法
```

前者是解析的Json格式的字符串；后者是创建一个对象，该对象对应的Json类型是一个字符串。

## 用语

为了避免歧义，在此声明文本中的“对象”为`json_parser::Json`类型或者其他类型创建的对象（是CPP里面的对象），而“Json对象”是指Json文本中这是一个Json对象类型（是指Json中的Object）。
诸如此类的用语，如果是对Json文本类型进行描述，都在前面加上了“Json”这个词。
