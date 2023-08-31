/// \file Tokenization.h
/// \brief 词法分析器类定义文件
#ifndef TOKENIZATION_H
#define TOKENIZATION_H
#include "token.h"
#include <iostream>
#include <vector>
using namespace std;

/// \brief Reader类型定义，用于管理读取到的string串
class Reader
{
private:
    int ind_ = 0;     ///< 当前下标
    int maxsize_ = 0; ///< 字符串大小
    string str_;      ///< 存储的字符串

public:
    /// \brief Reader无参构造函数
    Reader();

    /// \brief Reader string构造函数
    /// \param[in] str 传入字符串
    Reader(const string& str);

    /// \brief Reader const char* 构造函数
    /// \param[in] str 传入字符串
    Reader(const char* str);

    /// \brief Reader析构函数
    ~Reader();

    /// \brief 传入string设置成员str_的set方法
    /// \param[in] str 传入字符串
    /// \return void
    void set_str(const string& str);

    /// \brief 传入string设置成员str_的set方法
    /// \param[in] str 传入字符串
    /// \return void
    void set_str(const char* str);

    /// \brief 获取当前下标字符
    /// \return 当前下标字符
    char get();

    /// \brief 判断下标是否有字符，越界检测
    /// \return 有字符返回true,无返回false
    bool has_char();

    /// \brief 下标后移
    /// \return 移动后下标存在字符返回true,无返回false
    bool next();

    /// \brief 是否还有下一字符
    /// \return 存在下一字符返回true,无返回false
    bool has_more();

    /// \brief 获取当前下标的下一字符
    /// \return 返回当前下标字符的下一个字符
    char get_next();
};

/// \brief Tokenization类型定义，用于读取输入字符串解析成Token流
class Tokenization
{
private:
    Reader rd_; ///< Reader成员类

public:
    /// \brief Tokenization无参构造函数
    Tokenization();

    /// \brief Tokenization string构造函数
    /// \param[in] str 传入字符串
    Tokenization(const string& str);

    /// \brief Tokenization const char*构造函数
    /// \param[in] str 传入字符串
    Tokenization(const char* str);

    /// \brief Tokenization析构函数
    ~Tokenization();

    /// \brief 调用rd.next()
    void rd_next();

    /// \brief 读取4位十六进制字符转换成十进制
    /// \return 转换到的十进制unsigned int类型变量
    unsigned int read_hex();

    /// \brief 将unicode码点转换成1-4位的utf8编码字符串
    /// \param[out] str 返回的utf8编码字符串
    /// \param[in] u  unicode码点值
    void encode_utf(string& str, unsigned int u);

    /// \brief 读取null类型的Token
    /// \return 成功返回true,失败返回false
    bool read_null();

    /// \brief 读取false类型的Token
    /// \return 成功返回true,失败返回false
    bool read_false();

    /// \brief 读取true类型的Token
    /// \return 成功返回true,失败返回false
    bool read_true();

    /// \brief 读取STRING类型的Token
    /// \return 读取到的STRING类型Token节点
    Token read_str();

    /// \brief 读取NUMBER类型的Token
    /// \return 读取到的NUMBER_INT或者NUMBER_REAL类型Token节点
    Token read_number();

    /// \brief 读取下一Token
    /// \return 根据字符串流词法分析返回对应类型的Token节点
    Token read_token();

    /// \brief 对字符串进行词法分析生成Token序列
    /// \param s 传入字符串
    /// \return 词法分析到的Token序列
    vector<Token> token_list(const string& s);
};
#endif
