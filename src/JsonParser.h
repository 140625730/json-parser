/// \file JsonParser.h
/// \brief 语法分析器类定义文件
#ifndef JsonParser_H
#define JsonParser_H
#include "JsonClass.h"
#include "Tokenization.h"
#include "token.h"
#include <cstring>
#include <fstream>
#include <istream>

/// \brief Json Parser类型定义，语法分析器
class Json_Parser
{
public:
    int ind_;              ///< 当前Token下标
    vector<Token> tokens_; ///< 待解析的Token序列

    /// \brief 解析对应文件的Json字符串
    /// \param[in] s 文件路径
    /// \return 解析返回的Object对象
    Object& file_parser(const string& s);

    /// \brief 解析对应输入流中的序列化Json对象
    /// \param[in] is 输入流
    /// \return 解析返回的Object对象
    Object& parser(istream& is);

    /// \brief 解析对应字符串中的Json对象
    /// \param[in] s Json字符串
    /// \return 解析返回的Object对象
    Object& parser(string& s);

    /// \brief 解析Json Object对象
    /// \return 解析返回的Json Object对象
    Object& object_parser();

    /// \brief 解析Json Array对象
    /// \return 解析返回的Json Array对象
    Object& array_parser();
};

#endif
