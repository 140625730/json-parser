#include "gtest/gtest.h"
#include "Tokenization.h"
TEST(TestTokenizaion, test_reader)
{
    // 默认构造函数
    Reader rd1;
    EXPECT_EQ(rd1.get(), 0);
    EXPECT_FALSE(rd1.has_char());
    EXPECT_FALSE(rd1.has_more());
    EXPECT_EQ(rd1.get_next(), 0);
    EXPECT_FALSE(rd1.next());
    // string构造和设值
    string s2 = "ab";
    Reader rd2(s2);
    EXPECT_TRUE(rd2.has_char());
    EXPECT_EQ(rd2.get(), 'a');
    EXPECT_TRUE(rd2.has_more());
    EXPECT_EQ(rd2.get_next(), 'b');
    EXPECT_TRUE(rd2.next());
    EXPECT_FALSE(rd2.has_more());
    // const char*构造和设值
    const char *s3 = "bca";
    Reader rd3(s3);
    EXPECT_TRUE(rd3.has_char());
    EXPECT_EQ(rd3.get(), 'b');
    EXPECT_TRUE(rd3.has_more());
    EXPECT_EQ(rd3.get_next(), 'c');
    EXPECT_TRUE(rd3.next());
    EXPECT_TRUE(rd3.has_more());
    // setstr方法和默认赋值
    rd3.set_str(s2);
    rd2.set_str(s3);
    EXPECT_NE(rd2.get(), rd3.get());
    rd3 = rd2;
    EXPECT_EQ(rd2.get(), rd3.get());
    EXPECT_EQ(rd2.get_next(), rd3.get_next());
}

TEST(TestTokenizaion, test_tokenization_readhex)
{
    //正常
    Tokenization tk(" 011A");
    EXPECT_EQ(tk.read_hex(), 0x011A);
    //越界检测
    EXPECT_THROW(tk.read_hex(), const char *);
    //无效字符
    tk = Tokenization(" DEFG");
    EXPECT_THROW(tk.read_hex(), const char *);
}

TEST(TestTokenizaion, test_tokenization_encode_utf)
{
    Tokenization tk;
    string s1, s2;
    unsigned int u = 0x00;
    //边界0x00
    tk.encode_utf(s1, u);
    s2 = "0";
    s2[0] = '\u0000';
    EXPECT_EQ(s1, s2);
    // 0x00-0x7F
    u = 0x6F;
    s1.clear();
    tk.encode_utf(s1, u);
    s2[0] = '\u006F';
    EXPECT_EQ(s1, s2);
    u = 0x7F;
    s1.clear();
    tk.encode_utf(s1, u);
    s2[0] = '\u007F';
    EXPECT_EQ(s1, s2);
    // 0x7F-0x7FF
    u = 0xFF;
    s1.clear();
    tk.encode_utf(s1, u);
    s2 = "\303\277";
    EXPECT_EQ(s1, s2);
    u = 0x7FF;
    s1.clear();
    tk.encode_utf(s1, u);
    s2 = "\337\277";
    EXPECT_EQ(s1, s2);
    // 0x7FF-0xFFFF
    u = 0x6FFF;
    s1.clear();
    tk.encode_utf(s1, u);
    s2 = "\346\277\277";
    EXPECT_EQ(s1, s2);
    u = 0xFFFF;
    s1.clear();
    tk.encode_utf(s1, u);
    s2 = "\357\277\277";
    EXPECT_EQ(s1, s2);
    // 0xFFFF-0x10FFFF
    u = 0x6FFFF;
    s1.clear();
    tk.encode_utf(s1, u);
    s2 = "\361\257\277\277";
    EXPECT_EQ(s1, s2);
    u = 0x10FFFF;
    s1.clear();
    tk.encode_utf(s1, u);
    s2 = "\364\217\277\277";
    EXPECT_EQ(s1, s2);
}

TEST(TestTokenizaion, test_tokenization_readconstchar)
{
    // null
    Tokenization tk("null");
    EXPECT_TRUE(tk.read_null());
    EXPECT_FALSE(tk.read_null());
    tk = Tokenization("nule");
    EXPECT_FALSE(tk.read_null());
    // true
    tk = Tokenization("true");
    EXPECT_TRUE(tk.read_true());
    EXPECT_FALSE(tk.read_true());
    tk = Tokenization("truu");
    EXPECT_FALSE(tk.read_true());
    // false
    tk = Tokenization("false");
    EXPECT_TRUE(tk.read_false());
    EXPECT_FALSE(tk.read_false());
    tk = Tokenization("falss");
    EXPECT_FALSE(tk.read_false());
}

TEST(TestTokenizaion, test_tokenization_readstr)
{
    //正常字符
    Tokenization tk("\"str\"");
    Token tst = tk.read_str();
    EXPECT_EQ(tst.token_type, Type::STRING);
    EXPECT_EQ(tst.value, "str");
    //越界检测
    EXPECT_ANY_THROW(tk.read_str());
    //字符串内有未转义\r和\n
    tk = Tokenization("\"\r\"");
    EXPECT_ANY_THROW(tk.read_str());
    tk = Tokenization("\"\nabs\"");
    EXPECT_ANY_THROW(tk.read_str());
    //转义字符识别
    tk = Tokenization("\"\\\"\\\\\\r\\n\\b\\t\\f\\/\"");
    tst = tk.read_str();
    EXPECT_EQ(tst.value, "\"\\\r\n\b\t\f/");
    // unicode转义识别
    tk = Tokenization(
            "\"\\u0075\\u006e\\u0069\\u0063\\u006f\\u0064\\u0065\\u0020\\u7f16\\u7801\\u0020\\u548c\\u0020\\u89e3\\u7801\"");
    tst = tk.read_str();
    EXPECT_EQ(tst.value, "unicode 编码 和 解码");
    //双字节检测第二个字节的范围
    tk = Tokenization("\"\\uD811\\uFFFF\"");
    EXPECT_ANY_THROW(tk.read_str());
    tk = Tokenization("\"\\uD811\\FFFF\"");
    EXPECT_ANY_THROW(tk.read_str());
    tk = Tokenization("\"\\uD811uFFFF\"");
    EXPECT_ANY_THROW(tk.read_str());
}

TEST(TestTokenizaion, test_tokenization_readnumber)
{
    //正常数字
    Tokenization tk("123");
    Token tst = tk.read_number();
    EXPECT_EQ(tst.token_type, Type::NUMBER);
    EXPECT_EQ(tst.value, "123");
    tk = Tokenization("-123e6");
    tst = tk.read_number();
    EXPECT_EQ(tst.token_type, Type::NUMBER);
    EXPECT_EQ(tst.value, "-123e6");
    tk = Tokenization("-123.55E-88");
    tst = tk.read_number();
    EXPECT_EQ(tst.token_type, Type::NUMBER);
    EXPECT_EQ(tst.value, "-123.55E-88");
    //错误检测
    tk = Tokenization("ab");
    EXPECT_ANY_THROW(tk.read_number());
    tk = Tokenization("-123.");
    EXPECT_ANY_THROW(tk.read_number());
    tk = Tokenization("-123.55E-");
    EXPECT_ANY_THROW(tk.read_number());
    tk = Tokenization("-0015");
    EXPECT_ANY_THROW(tk.read_number());
}

TEST(TestTokenizaion, test_tokenization_readtoken)
{
    //为空返回结束符，跳过空白字符
    Tokenization tk(" \n\r\t\f");
    Token tst = tk.read_token();
    EXPECT_EQ(tst.token_type, Type::DOCUMENT_END);
    //分界符和常量字符测试
    tk = Tokenization("{}[],:null false true");
    tst = tk.read_token();
    EXPECT_EQ(tst.token_type, Type::OBJECT_BEGIN);
    tk.rd_next();
    tst = tk.read_token();
    EXPECT_EQ(tst.token_type, Type::OBJECT_END);
    tk.rd_next();
    tst = tk.read_token();
    EXPECT_EQ(tst.token_type, Type::ARRAY_BEGIN);
    tk.rd_next();
    tst = tk.read_token();
    EXPECT_EQ(tst.token_type, Type::ARRAY_END);
    tk.rd_next();
    tst = tk.read_token();
    EXPECT_EQ(tst.token_type, Type::IS_COMMA);
    tk.rd_next();
    tst = tk.read_token();
    EXPECT_EQ(tst.token_type, Type::IS_COLON);
    tk.rd_next();
    tst = tk.read_token();
    EXPECT_EQ(tst.token_type, Type::IS_NULL);
    EXPECT_EQ(tst.value, "null");
    tk.rd_next();
    tst = tk.read_token();
    EXPECT_EQ(tst.token_type, Type::BOOLEAN);
    EXPECT_EQ(tst.value, "false");
    tk.rd_next();
    tst = tk.read_token();
    EXPECT_EQ(tst.token_type, Type::BOOLEAN);
    EXPECT_EQ(tst.value, "true");
}

TEST(TestTokenizaion, test_tokenization_tokenlist)
{
    Tokenization tk;
    vector<Token> tklist;
    tklist = tk.token_list(
            "{\"\\u0075\\u006e\\u0069\\u0063\\u006f\\u0064\\u0065\\u0020\\u7f16\\u7801\\u0020\\u548c\\u0020\\u89e3\\u7801\" : -123e6}");
    EXPECT_EQ(tklist[0].token_type, Type::OBJECT_BEGIN);
    EXPECT_EQ(tklist[1].token_type, Type::STRING);
    EXPECT_EQ(tklist[1].value, "unicode 编码 和 解码");
    EXPECT_EQ(tklist[2].token_type, Type::IS_COLON);
    EXPECT_EQ(tklist[3].token_type, Type::NUMBER);
    EXPECT_EQ(tklist[4].token_type, Type::OBJECT_END);
}
