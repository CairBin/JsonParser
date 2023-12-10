#include "json_parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <codecvt>
#include <locale>

const char *s = "{\
					\"name\":\"Can\",\
					\"age\":23,\
					\"sex\":\"hahah\"\
					}";

using namespace std;
using namespace json_parser;
int main(){
    ifstream st;
    st.open("test.json");
    if(st.is_open()){
        cout << "File opend successfully" << endl;
    }else{
        cout << "Failed to open the file" << endl;
        return -1;
    }
    string str;
    string line;
    while(getline(st,line)){
        str += line + "\n";
    }
    cout << str << endl;
    st.close();
    cout << "-----------" << endl;

    Json json(JsonType::JSON_ARRAY);
    json.Append("HelloWorld");
    json.Append(0);
    json.Append(1.2);
    json.Append(true);
    json.Append("Test");
    string temp = json.ToJsonString();
    cout << temp << endl;
    Json json2 = ParseJsonString(temp);
    cout << json2.ToJsonString() << endl;

    Json json3(JsonType::JSON_OBJECT);
    json3["Hello"] = 3;
    json3["nihao"] = true;
    json3["hihi"] = 1.21;
    json3["你好"] = "世界";

    wstring_convert<codecvt_utf8<wchar_t>> wcv;
    wstring ret = wcv.from_bytes(json3.ToJsonString());
    wcout << ret << endl;
    Json json4 = ParseJsonString(json3.ToJsonString());
    cout << json4.ToJsonString() << endl;
    cout << "---------------" << endl;

    Json test = ParseJsonString(str);
    cout << test.ToJsonString() << endl;

    // cout << "---------------" << endl;
    // Json e = true;
    // cout << e.IsBool() << endl;
    // cout << e.ToJsonString() << endl;
    // cout << "***********" <<endl;
    // printf("%s", s);

    Json haha = ParseJsonString(s);
    cout << haha.ToJsonString() << endl;
    cout << "你好世界" << endl;

    cout << "*************" << endl;

    Json t1(JsonType::JSON_ARRAY);
    Json tt1(JsonType::JSON_ARRAY);
    Json t2(JsonType::JSON_ARRAY);
    Json t3(2);
    t2.Append(t3);
    t1.Append(t2);
    t2[0] = 1;
    
    cout << t3.ToJsonString() << endl;
    cout << t1.ToJsonString() << endl;
    cout << t2.ToJsonString() << endl;

    Json u1(JsonType::JSON_ARRAY);
    u1.Append(3);
    {
        Json u2 = u1;
        cout << u2.ToJsonString() << endl << u2.UseCount() << endl;
    }
    
    cout << u1.ToJsonString() << endl << u1.UseCount() << endl;
    
    Json u3(JsonType::JSON_ARRAY);
    u3.Append(2);
    Json u4 = u3;

    cout << u3.ToJsonString() << endl;
    cout << u4.ToJsonString() << endl;


    int c;
    cin >> c;
    return 0;
}