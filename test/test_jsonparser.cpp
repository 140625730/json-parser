#include "gtest/gtest.h"
#include "JsonParser.h"
TEST(TestJsonParser, test_jsonparser_parser)
{
    //解析器测试
    string s;
    s = "{\"\\u0075\\u006e\\u0069\\u0063\\u006f\\u0064\\u0065\\u0020\\u7f16\\u7801\\u0020\\u548c\\u0020\\u89e3\\u7801\" : -123e6}";
    Json_Parser jp;
    Object &obj = jp.parser(s);
    EXPECT_EQ(obj.get_json_type(), Object::Json_Type::OBJECT);
    string ss = obj.serialize();
    EXPECT_EQ(
            ss,
            "{\"unicode \\u7F16\\u7801 \\u548C \\u89E3\\u7801\":-123000000.000000}");
    // to_str测试
    ss = obj.to_str();
    EXPECT_EQ(ss, "{\"unicode 编码 和 解码\":-123000000.000000}");
    // printer测试
    ss = obj.printer();
    EXPECT_EQ(
            ss,
            "{\n    \"unicode 编码 和 解码\":\n        -123000000.000000\n}");
    //检测Object内属性,[]访问测试
    string ind =
            "\"\\u0075\\u006e\\u0069\\u0063\\u006f\\u0064\\u0065\\u0020\\u7f16\\u7801\\u0020\\u548c\\u0020\\u89e3\\u7801\"";
    Tokenization tk(ind);
    ind = tk.read_str().value;
    double val = stod("-123E6");
    Json_Object *jo = dynamic_cast<Json_Object *>(&obj);
    EXPECT_EQ(jo->get(ind).get_json_type(), Object::Json_Type::NUMBER_REAL);
    EXPECT_EQ(
            jo->operator[](ind).get_json_type(),
            Object::Json_Type::NUMBER_REAL);
    val = -123000000.000000;
    EXPECT_DOUBLE_EQ(jo->get(ind).myvalue_.numberdouble_, val);
    EXPECT_DOUBLE_EQ((*jo)[ind].myvalue_.numberdouble_, val);
    //测试Array获取属性,[]索引访问测试
    s = "[1,2.6,5,\"abc\",{\"db\":true}]";
    Object &ao = jp.parser(s);
    int fi = 1, tird = 5;
    double se = 2.6;
    ss = "abc";
    EXPECT_EQ(ao.get(0).get_json_type(), Object::Json_Type::NUMBER_INT);
    EXPECT_EQ(ao[0].myvalue_.numberlonglong_, fi);
    EXPECT_EQ(ao[0].get_int_value(), fi);
    EXPECT_EQ(ao.get(1).get_json_type(), Object::Json_Type::NUMBER_REAL);
    EXPECT_DOUBLE_EQ(ao[1].myvalue_.numberdouble_, se);
    EXPECT_EQ(ao[1].get_real_value(), se);
    EXPECT_EQ(ao.get(2).get_json_type(), Object::Json_Type::NUMBER_INT);
    EXPECT_EQ(ao[2].myvalue_.numberlonglong_, tird);
    EXPECT_EQ(ao[2].get_int_value(), tird);
    EXPECT_EQ(ao.get(3).get_json_type(), Object::Json_Type::STRING);
    EXPECT_STREQ(ao[3].mystr_.c_str(), ss.c_str());
    EXPECT_STREQ(ao[3].get_str_value().c_str(), ss.c_str());
    EXPECT_EQ(ao.get(4).get_json_type(), Object::Json_Type::OBJECT);
    EXPECT_EQ(ao.get(4).get("db").get_json_type(), Object::Json_Type::IS_TRUE);
    EXPECT_EQ(ao[4]["db"].get_bool_value(), true);
    // istream测试
    Object &obj1 = jp.file_parser("../test/test_1");
    ss = obj1.to_str();
    EXPECT_EQ(obj1.get_json_type(), Object::Json_Type::OBJECT);
    EXPECT_EQ(ss, "{\"json\":404}");
    ifstream is("../test/test_1");
    Object &obj2 = jp.parser(is);
    ss = obj2.to_str();
    EXPECT_EQ(obj2.get_json_type(), Object::Json_Type::OBJECT);
    EXPECT_EQ(ss, "{\"json\":404}");
    //ostream测试
    ofstream os2("../test/output_1");
    obj2.output(os2);
    os2.close();
    ifstream ifs("../test/output_1");
    getline(ifs, ss);
    EXPECT_EQ(ss, "{\"json\":404}");
    ostream &os = file_output("../test/output_1", obj2, 's');
    ifstream ifs2("../test/output_1");
    getline(ifs2, ss);
    EXPECT_EQ(ss, "{\"json\":404}");
}

TEST(TestJsonParser, test_jsonparser_large_file)
{
    //测试长串json的文件解析和读写
    Json_Parser jp;
    Object &obj=jp.file_parser("../test/test_2");
    EXPECT_NO_THROW(obj.file_output("../test/output_2"));
}
