#include "gtest/gtest.h"
#include "JsonClass.h"
TEST(TestJsonClass, test_json_class_tohex4)
{
    //十六进制数转字符串
    unsigned int u = 0x11FF;
    string s = to_hex4(u);
    EXPECT_EQ(s, "11FF");
}

TEST(TestJsonClass, test_json_class_decode)
{
    //散列unsigned int 到一到两个unicode编码单元
    unsigned int u = 0x12FF;
    string s = decode(u);
    EXPECT_EQ(s, "\\u12FF");
    u = 0x1FFFF;
    s = decode(u);
    EXPECT_EQ(s, "\\uD83F\\uDFFF");
}

TEST(TestJsonClass, test_json_class_encodestr)
{
    //处理字符串中需转义的字符
    string s = "\"\\\b\f\n\r\t/\u0001";
    s = encode_str(s);
    EXPECT_EQ(s, "\"\\\"\\\\\\b\\f\\n\\r\\t\\/\\u0001\"");
    // unicode转义
    s = encode_str("编码");
    EXPECT_EQ(s, "\"\\u7F16\\u7801\"");
}

TEST(TestJsonClass, test_json_class_jsonclass)
{
    //几种Json类的构建和成员函数使用测试
    Json_Object obj;
    Json_Array arr;
    EXPECT_EQ(obj.get_json_type(), Object::Json_Type::OBJECT);
    EXPECT_EQ(arr.get_json_type(), Object::Json_Type::ARRAY);
    Json_Array iner = Json_Array();
    Json_Object oj;
    arr.add(&iner);
    EXPECT_EQ(arr.serialize(), "[[]]");
    EXPECT_EQ(obj.serialize(), "{}");
    obj.put("1", &iner);
    EXPECT_EQ(obj.serialize(), "{\"1\":[]}");
    obj.put("2", &oj);
    EXPECT_EQ(obj.serialize(), "{\"2\":{},\"1\":[]}");
}
