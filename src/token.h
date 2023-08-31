/// \file token.h
/// \brief token类定义文件
#ifndef TOKEN_H
#define TOKEN_H
#include <string>

/// \brief token类型枚举
enum class Type
{
    OBJECT_BEGIN,
    OBJECT_END,
    ARRAY_BEGIN,
    ARRAY_END,
    NUMBER,
    STRING,
    BOOLEAN,
    IS_NULL,
    IS_COLON,
    IS_COMMA,
    DOCUMENT_BEGIN,
    DOCUMENT_END
};

/// \brief Token类型定义
struct Token
{
    Type token_type;   ///< Token的类型
    std::string value; ///< Token的值
};
#endif
