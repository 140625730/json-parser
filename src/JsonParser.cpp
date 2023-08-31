#include "JsonParser.h"
Object &Json_Parser::file_parser(const string &s)
{
    ifstream is(s);
    if (!is) {
        cout << "file open fail";
        return *new Object();
    }
    return parser(is);
}
Object &Json_Parser::parser(istream &is)
{
    string s;
    getline(is, s);
    return parser(s);
}
Object &Json_Parser::parser(string &s)
{
    Tokenization tk;
    tokens_ = tk.token_list(s);
    try {
        Object *ret_object;
        if (tokens_.size() == 0) {
            ret_object = new Object();
            return *ret_object;
        } else {
            ind_ = 0;
            //根据定界符确定调用Object或者Array解析函数
            if (tokens_[ind_].token_type == Type::OBJECT_BEGIN) {
                return object_parser();
            } else if (tokens_[ind_].token_type == Type::ARRAY_BEGIN) {
                return array_parser();
            } else
                throw "invalid token";
        }
    } catch (const char *err_msg) {
        cout << err_msg << "\n";
    }
    return *new Object();
}
Object &Json_Parser::object_parser()
{
    Object *ret_object = new Json_Object();
    Json_Object &my_json_object = dynamic_cast<Json_Object &>(*ret_object);
    my_json_object.mytype_ = Object::Json_Type::OBJECT;
    Type last_token_type = Type::OBJECT_BEGIN;
    string key = "";
    Object val = Object();
    //根据last token和key进行解析，每次解析Token需往前确认上一节点类型和key值
    while (ind_ + 1 < tokens_.size()) {
        ind_++;
        Type now_token_type = tokens_[ind_].token_type;
        string token_val = tokens_[ind_].value;
        switch (now_token_type) {
        case Type::OBJECT_BEGIN:
        {
            if (last_token_type != Type::IS_COLON)
                throw "invalid token taking OBJECT_BEGIN";
            Object &tmp = object_parser();
            my_json_object.put(key, &tmp);
            key = "";
            last_token_type = Type::OBJECT_END;
            break;
        }
        case Type::OBJECT_END:
        {
            if (last_token_type == Type::ARRAY_BEGIN ||
                last_token_type == Type::IS_COLON ||
                last_token_type == Type::IS_COMMA)
                throw "invalid token taking OBJECT_END";
            if (key != "")
                throw "value is null";
            return my_json_object;
        }
        case Type::ARRAY_BEGIN:
        {
            if (last_token_type != Type::IS_COLON)
                throw "invalid token taking ARRAY_BEGIN";
            Object &tmp = array_parser();
            my_json_object.put(key, &tmp);
            key = "";
            last_token_type = Type::ARRAY_END;
            break;
        }
        case Type::IS_NULL:
        {
            if (last_token_type != Type::IS_COLON)
                throw "invalid token taking NULL";
            Object *tmp = new Object();
            my_json_object.put(key, tmp);
            key = "";
            last_token_type = Type::IS_NULL;
            break;
        }
        case Type::NUMBER:
        {
            if (last_token_type != Type::IS_COLON)
                throw "invalid token taking NUMBER";
            Object *tmp;
            //根据字符串内是否有real的特殊符号确定number Token的具体类型
            if (token_val.find(".") != token_val.npos ||
                token_val.find("e") != token_val.npos ||
                token_val.find("E") != token_val.npos) {
                tmp = new Object(Object::Json_Type::NUMBER_REAL);
                tmp->myvalue_.numberdouble_ = stod(token_val);
            } else {
                tmp = new Object(Object::Json_Type::NUMBER_INT);
                long long num = stoll(token_val);
                tmp->myvalue_.numberlonglong_ = num;
            }
            my_json_object.put(key, tmp);
            key = "";
            last_token_type = Type::NUMBER;
            break;
        }
        case Type::BOOLEAN:
        {
            if (last_token_type != Type::IS_COLON)
                throw "invalid token taking BOOLEAN";
            Object *tmp;
            if (token_val == "true") {
                tmp = new Object(Object::Json_Type::IS_TRUE);
                tmp->mystr_ = "true";
            } else {
                tmp = new Object(Object::Json_Type::IS_FALSE);
                tmp->mystr_ = "false";
            }
            my_json_object.put(key, tmp);
            key = "";
            last_token_type = Type::BOOLEAN;
            break;
        }
        case Type::STRING:
        {
            if (last_token_type != Type::OBJECT_BEGIN &&
                last_token_type != Type::IS_COLON &&
                last_token_type != Type::IS_COMMA)
                throw "invalid token taking STRING";
            if (last_token_type == Type::IS_COLON) {
                if (key == "")
                    throw "key is null";
                Object *tmp = new Object(Object::Json_Type::STRING);
                tmp->mystr_ = token_val;
                my_json_object.put(key, tmp);
                key = "";
            } else {
                key = token_val;
            }
            last_token_type = Type::STRING;
            break;
        }
        case Type::IS_COLON:
        {
            if (last_token_type != Type::STRING)
                throw "invalid token taking COLON";
            last_token_type = Type::IS_COLON;
            break;
        }
        case Type::IS_COMMA:
        {
            if (last_token_type != Type::OBJECT_END &&
                last_token_type != Type::ARRAY_END &&
                last_token_type != Type::IS_NULL &&
                last_token_type != Type::NUMBER &&
                last_token_type != Type::STRING &&
                last_token_type != Type::BOOLEAN)
                throw "invalid token taking COMMA";
            if (key != "")
                throw "value is null";
            last_token_type = Type::IS_COMMA;
            break;
        }
        default:
        {
            throw "invalid token";
        }
        }
    }
    throw "parse error";
}

Object &Json_Parser::array_parser()
{
    Object *ret_object = new Json_Array();
    Json_Array &my_json_array = dynamic_cast<Json_Array &>(*ret_object);
    my_json_array.mytype_ = Object::Json_Type::ARRAY;
    Type last_token_type = Type::ARRAY_BEGIN;
    //根据last token进行解析，每次解析Token需往前确认上一节点类型
    while (ind_ + 1 < tokens_.size()) {
        ind_++;
        Type now_token_type = tokens_[ind_].token_type;
        string token_val = tokens_[ind_].value;
        switch (now_token_type) {
        case Type::OBJECT_BEGIN:
        {
            if (last_token_type != Type::IS_COMMA &&
                last_token_type != Type::ARRAY_BEGIN)
                throw "invalid token taking OBJECT_BEGIN";
            Object &tmp = object_parser();
            my_json_array.add(&tmp);
            last_token_type = Type::OBJECT_END;
            break;
        }
        case Type::ARRAY_BEGIN:
        {
            if (last_token_type != Type::IS_COMMA &&
                last_token_type != Type::ARRAY_BEGIN)
                throw "invalid token taking ARRAY_BEGIN";
            Object &tmp = array_parser();
            my_json_array.add(&tmp);
            last_token_type = Type::ARRAY_END;
            break;
        }
        case Type::ARRAY_END:
        {
            if (last_token_type == Type::OBJECT_BEGIN ||
                last_token_type == Type::IS_COLON ||
                last_token_type == Type::IS_COMMA)
                throw "invalid token taking ARRAY_END";
            return my_json_array;
        }
        case Type::IS_NULL:
        {
            if (last_token_type != Type::IS_COMMA &&
                last_token_type != Type::ARRAY_BEGIN)
                throw "invalid token taking NULL";
            Object *tmp = new Object();
            my_json_array.add(tmp);
            last_token_type = Type::IS_NULL;
            break;
        }
        case Type::NUMBER:
        {
            if (last_token_type != Type::IS_COMMA &&
                last_token_type != Type::ARRAY_BEGIN)
                throw "invalid token taking NUMBER";
            Object *tmp;
            //根据字符串内是否有real的特殊符号确定number Token的具体类型
            if (token_val.find(".") != token_val.npos ||
                token_val.find("e") != token_val.npos ||
                token_val.find("E") != token_val.npos) {
                tmp = new Object(Object::Json_Type::NUMBER_REAL);
                tmp->myvalue_.numberdouble_ = stod(token_val);
            } else {
                tmp = new Object(Object::Json_Type::NUMBER_INT);
                long long num = stoll(token_val);
                tmp->myvalue_.numberlonglong_ = num;
            }
            my_json_array.add(tmp);
            last_token_type = Type::NUMBER;
            break;
        }
        case Type::BOOLEAN:
        {
            if (last_token_type != Type::IS_COMMA &&
                last_token_type != Type::ARRAY_BEGIN)
                throw "invalid token taking BOOLEAN";
            Object *tmp;
            if (token_val == "true") {
                tmp = new Object(Object::Json_Type::IS_TRUE);
                tmp->mystr_ = "true";
            } else {
                tmp = new Object(Object::Json_Type::IS_FALSE);
                tmp->mystr_ = "false";
            }
            my_json_array.add(tmp);
            last_token_type = Type::BOOLEAN;
            break;
        }
        case Type::STRING:
        {
            if (last_token_type != Type::IS_COMMA &&
                last_token_type != Type::ARRAY_BEGIN)
                throw "invalid token taking STRING";
            Object *tmp = new Object(Object::Json_Type::STRING);
            tmp->mystr_ = token_val;
            my_json_array.add(tmp);
            last_token_type = Type::STRING;
            break;
        }
        case Type::IS_COMMA:
        {
            if (last_token_type != Type::OBJECT_END &&
                last_token_type != Type::ARRAY_END &&
                last_token_type != Type::IS_NULL &&
                last_token_type != Type::NUMBER &&
                last_token_type != Type::STRING &&
                last_token_type != Type::BOOLEAN)
                throw "invalid token taking COMMA";
            last_token_type = Type::IS_COMMA;
            break;
        }
        default:
        {
            throw "invalid token";
        }
        }
    }
    throw "parse error";
}
