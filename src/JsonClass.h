/// \file JsonClass.h
/// \brief Json数据结构定义文件
#ifndef JSONCLASS_H
#define JSONCLASS_H
#include <unordered_map>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>
#include <ostream>
using namespace std;

/// \brief 转换十进制数到四位十六进制字符串
/// \param u 传入十进制数
/// \return 四位十六进制字符串
string to_hex4(unsigned int u);

/// \brief 转换unicode码点值到一到两个四位十六进制带转义的Unicode编码字符串
/// \param u 传入的unicode码点值
/// \return 一到两个四位十六进制带转义的Unicode编码组成的字符串
string decode(unsigned int u);

/// \brief 处理序列化json字符串中需要转义的字符
/// \param str 传入待添加转义符的string字符串
/// \return 转义完成之后的规范json字符串
string encode_str(string str);

/// \brief Json节点类
class Object
{
public:
    union
    {
        double numberdouble_;      ///< real类型数据存放
        long long numberlonglong_; ///< int类型数据存放
    } myvalue_;                    ///< number数据存放
    string mystr_;                 ///< string类型数据存放
    enum Json_Type
    {
        OBJECT,
        ARRAY,
        NUMBER_INT,
        NUMBER_REAL,
        STRING,
        IS_TRUE,
        IS_FALSE,
        IS_NULL
    } mytype_; ///< Json节点类型

    /// \brief 获取当前Json节点类型
    /// \return 当前Json节点类型
    int get_json_type();

    /// \brief 根据索引获取Json节点的value虚函数
    /// \param[in] key string类型的索引
    /// \return 存在该索引返回对应的Json节点，不存在输出异常，返回空Json节点
    virtual Object &get(string key);

    /// \brief 根据下标获取Json节点的element虚函数
    /// \param[in] index int类型的下标
    /// \return 存在该下标返回对应的Json节点，不存在输出异常，返回空Json节点
    virtual Object &get(int index);

    /// \brief 重载[]操作符调用对应的get函数
    /// \param[in] key string类型的索引
    /// \return 存在该索引返回对应的Json节点，不存在输出异常，返回空Json节点
    Object &operator[](string key);

    /// \brief 重载[]操作符调用对应的get函数
    /// \param[in] index int类型的下标
    /// \return 存在该下标返回对应的Json节点，不存在输出异常，返回空Json节点
    Object &operator[](int index);

    /// \brief 序列化当前Json数据结构
    /// \return 返回递归序列化的Json字符串
    virtual string serialize();

    /// \brief 序列化当前Json数据结构(无转义版本)
    /// \return 返回递归序列化的Json字符串(无转义版本)
    virtual string to_str();

    /// \brief 格式化打印Json结构
    /// \param[in] depth 递归打印深度，标志缩进大小
    /// \return 格式化打印Json字符串，带缩进和换行，无转义
    virtual string printer(int depth = 0);

    /// \brief Json三种序列化方法调用的序列化基函数
    /// \param[in] ch 序列化模式
    /// \param[in] depth 递归深度
    /// \return 返回对应模式的序列化字符串
    virtual string base_str(char ch, int depth = 0);

    /// \brief Object的构造函数
    /// \param[in] tp Json节点的Type
    /// \param[in] s Json节点的value
    Object(Json_Type tp = IS_NULL, const char *s = "null");

    /// \brief 文件流输出函数
    /// \param[in] addr 输出文件的路径
    /// \param[in] mode 输出模式，对应三种序列化字符串
    /// \return 返回处理过的ofstream流
    ostream &file_output(const string &addr, char mode = 's');

    /// \brief 输出函数
    /// \param[in] mode 输出模式，对应三种序列化字符串
    /// \return 返回处理过的ostream流
    ostream &output(ostream& os,char mode = 's');

    /// \brief Object的析构函数
    virtual ~Object();

    /// \brief 获取INT类型节点的int类型值
    /// \return 当前节点的int类型值，做对应类型判断
    long long get_int_value();

    /// \brief 获取REAL类型节点的double类型值
    /// \return 当前节点的double类型值，做对应类型判断
    double get_real_value();

    /// \brief 获取STRING类型节点的string类型值
    /// \return 当前节点的string类型值，做对应类型判断
    string get_str_value();

    /// \brief 获取BOOLEAN类型节点的bool类型值
    /// \return 当前节点的bool类型值，做对应类型判断
    bool get_bool_value();

    /// \brief 获取NULL类型节点的null类型值
    /// \return 当前节点的null类型值，做对应类型判断
    string get_null_value();
};

/// \brief 类外调用文件输出函数
/// \param[in] addr 输出文件路径
/// \param[in] obj 调用的Object对象
/// \param[in] mode 输出模式，对应三种序列化字符串
/// \return 处理过输出的ofstream对象
ostream &file_output(const string &addr, Object &obj, char mode = 's');

/// \brief Json Object类
class Json_Object: public Object
{
private:
    unordered_map<string, Object *> mp_; ///< 存储键值对的哈希表

public:
    /// \brief Json Object构造函数
    Json_Object();

    /// \brief 插入新键值对
    /// \param[in] key 键
    /// \param[in] value 值
    void put(string key, Object *value);

    /// \brief 获取当前键对应的Object对象
    /// \param[in] key 键
    /// \return 键对应的Object对象，若无则输出异常
    virtual Object &get(string key);

    /// \brief []访问get方法
    /// \param[in] key 索引
    /// \return 索引对应的Object对象，若无则输出异常
    Object &operator[](string key);

    /// \brief Json Object的序列化方法
    /// \return 序列化字符串
    virtual string serialize();

    /// \brief Json Object的序列化(无转义)方法
    /// \return 序列化字符串(无转义)
    virtual string to_str();

    /// \brief Json Object的格式化打印
    /// \param[in] depth 递归深度
    /// \return 格式化打印的序列化字符串，带换行和缩进，无转义
    virtual string printer(int depth = 0);

    /// \brief Json Object序列化字符串基函数
    /// \param[in] ch 序列化模式
    /// \param[in] depth 递归深度
    /// \return 对应模式的序列化字符串
    virtual string base_str(char ch, int depth = 0);
};

/// \brief Json Array类
class Json_Array: public Object
{
private:
    vector<Object *> arr_; ///< 存储Object*元素的数组

public:
    /// \brief 构造函数
    Json_Array();

    /// \brief 插入新元素
    /// \param[in] obj 待插入的指针
    void add(Object *obj);

    /// \brief 获取当前下标对应的Object对象
    /// \param[in] index 下标
    /// \return 当前下标对应的Object对象，若越界则输出异常
    virtual Object &get(int index);

    /// \brief []访问get方法
    /// \param[in] index 下标
    /// \return 当前下标对应的Object对象，若越界则输出异常
    Object &operator[](int index);

    /// \brief 获取当前节点数组的大小
    /// \return 当前节点数组的大小
    int size();

    /// \brief Json Array的序列化方法
    /// \return 序列化字符串
    virtual string serialize();

    /// \brief Json Array的序列化(无转义)方法
    /// \return 序列化字符串(无转义)
    virtual string to_str();

    /// \brief Json Array的格式化打印
    /// \param[in] depth 递归深度
    /// \return 格式化打印的序列化字符串，带换行和缩进，无转义
    virtual string printer(int depth = 0);

    /// \brief Json Array序列化字符串基函数
    /// \param[in] ch 序列化模式
    /// \param[in] depth 递归深度
    /// \return 对应模式的序列化字符串
    virtual string base_str(char ch, int depth = 0);
};
#endif
