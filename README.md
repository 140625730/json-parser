# JSON解析器



## 项目说明
### 引言
JSON是一种轻量级数据交换格式,被广泛应用于 RESTful 和各种 RPC 应用中。<br>
JSON解析器是一种用于解析和处理JSON数据的工具或库。它可以将JSON字符串转换为相应的数据结构，如对象、数组、字符串和数字等，以便在编程语言中进行进一步的操作和处理。<br>
JSON解析器提供了一些常用的方法，如将JSON字符串转换为对象、获取对象中的属性值、将对象转换为JSON字符串等。通过这些方法，开发人员可以根据需要对JSON数据进行操作和处理。

### 项目描述
该项目目标为使用C++实现一个JSON解析器，通过词法分析和语法分析，该解析器能识别JSON中的 object, array, int, real 和 string 基本类型并存储在相应的数据结构中，能通过输入流解析JSON类型、获取JSON对象的属性值和将JSON类型序列化输出。

## 使用说明

#### 解析器使用
1. 引入源代码目录`src`中的.h和.cpp文件
2. 在代码文件中引入头文件` “JsonParser.h” `
    ```c++
    #include "JsonParser.h" 
    ```
3. 创建` Json_Parser `对象
    ```c++
    Json_Parser jp;
    ```
4. 调用解析函数，创建`Object&`对象接收解析返回的Json对象值<br>
   解析函数有几种可选接收参数：<br>
   1. string或c风格json字符串输入
   ```c++
    string s="{\"myjson\":true}";
    // or const char * s= "{\\"myjson\\":true}";
    Object &obj=jp.parser(s);
   ```
   2. json文件路径输入
   ```c++
    string s="your file path";
    Object& obj=jp.file_parser(s);
   ```
   3. 输入流`ostream`输入
    ```c++
    istream &is=cin or other istream;
    Object& obj=jp.parser(is);
    ```
5. 对接受到的`Object&`对象，可以调用序列化方法获取string输出
    1. 调用`printer()`方法获取格式化打印输出,查看json解析结果，对原输入json格式字符串处理了其转义字符和unicode编码成utf8格式编码，并有缩进和换行
    ```c++
    string s=obj.printer();
    cout<<s;
    ```
    2. 调用`to_str()`方法获取，相对`printer()`无缩进无换行
    ```c++
    string s=obj.to_str();
    cout<<s;
    ```
    3. 调用`serialize()`方法获取序列化输出，该方法相对to_str()的差别是重新处理了需转义和需转unicode编码的字符，是能被再次解析的json格式字符串
    ```c++
    string s=obj.serialize();
    cout<<s;
    //此时s是能被obj.parser(s)再次解析的json格式字符串
    ```
    4. 也可调用`base_str()`并指定序列化模式来输出上面三种序列化的结果，三种模式分别为`p`、`t`、`s`，对应上面三个函数的首字符
    ```c++
    string s=obj.base_str('p');//or 't' 's'
    cout<<s;
    ```
6. 对接受到的`Object&`对象，可以调用属性查询相关方法
    1. `obj.get_json_type()`,返回一个`Object::Json_Type`枚举类型<br>
    其定义如下:<br>
        ```c++
        enum Json_Type
        {
            OBJECT,     //对象类型
            ARRAY,      //数组类型
            NUMBER_INT, //int number
            NUMBER_REAL,//real number
            STRING,     //string类型
            IS_TRUE,    //true
            IS_FALSE,   //false
            IS_NULL     //null
        };
        ```
        在判断类型时可使用`obj.get_json_type()==Object::Json_Type::OBJECT`等代码
    2. 对于`OBJECT`，`ARRAY`等可递归查找子Json节点的类型，可以调用相关方法获取子Object对象
        1. `get()`函数,`OBJECT`使用索引参数，`ARRAY`使用下标参数，注意`OBJECT`内的`key`值是**解析处理过后**的不带转义和utf8编码的unicode字符串，要查看`key`值可用`printer()`或`to_str()`方法获取解析结果并用`cout`输出查看打印结果等方法
        ```c++
        string key="your key";
        int index=0;
        Object &son=obj.get(key);//or get(index) 
        ```
        2. []访问,参数如上
        ```c++
        string key="your key";
        int index=0;
        Object &son=obj[key];//or obj[index]、obj["your key"]、obj[0]
        ```
    3. 对于其他类型，可以调用相应get value方法获取该Json节点的值
        ```c++
        long long val=obj.get_int_value();
        double    val=obj.get_real_value();
        string    val=obj.get_str_value();
        string    val=obj.get_null_value();
        bool      val=obj.get_bool_value();
        ```
7. `Object`还支持将序列化结果输出到文件中,指定输出路径和输出模式(三种序列化模式，默认为`s`)，返回结果为处理的`ofstream`，函数声明如下
    ```c++
    ostream &file_output(const string &addr, char mode = 's');//成员函数
    ostream &file_output(const string &addr, Object &obj, char mode = 's');//非成员函数
    ```
8. 要获取ostream类型的输出，也有对应函数调用
    ```c++
    ostream &output(ostream& os,char mode = 's');
    ```
    使用时传入要返回的ostream,指定输出模式
    ```c++
    ofstream os("path")//or other ostream
    obj.output(os);//or obj.output(cout)输出到标准输出流
    ```
## 测试说明
在`json`目录下运行以下命令
```
mkdir build
cmake -B build
cd build
make
ctest
```
即可看到测试结果，如果**文件读写**相关测试失败，可以尝试`sudo su`切换到root用户或者用`ll`查看`json/test/`目录中需要写文件的`output`文件权限是否有`w`权限，使用`sudo chmod 777 file`可以修改对应文件权限，再在`build`目录下启动测试`ctest`
