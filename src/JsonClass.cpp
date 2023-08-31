#include "JsonClass.h"
string to_hex4(unsigned int u)
{
    string ret;
    int tp = 0;
    for (int i = 0; i < 4; i++) {
        tp = u % 16;
        string cc;
        if (tp < 10)
            cc += '0' + tp;
        else
            cc += 'A' + tp - 10;
        ret = cc + ret;
        u /= 16;
    }
    return ret;
}
string decode(unsigned int u)
{
    if (u <= 0xFFFF) {
        return "\\u" + to_hex4(u);
    } else {
        u -= 0x10000;
        unsigned u1 = ((u & 0xFFC00) >> 10) + 0xD800;
        unsigned u2 = (u & 0x003FF) + 0xDC00;
        return "\\u" + to_hex4(u1) + "\\u" + to_hex4(u2);
    }
}
string encode_str(string str)
{
    string ret;
    char ch;
    for (int i = 0; i < str.size(); i++) {
        ch = str[i];
        unsigned int u = ch;
        if (ch == '\"')
            ret += "\\\"";
        else if (ch == '\\')
            ret += "\\\\";
        else if (ch == '\b')
            ret += "\\b";
        else if (ch == '\f')
            ret += "\\f";
        else if (ch == '\n')
            ret += "\\n";
        else if (ch == '\r')
            ret += "\\r";
        else if (ch == '\t')
            ret += "\\t";
        else if (ch == '/')
            ret += "\\/";
        else if (u < 0x20) {
            ret += "\\u" + to_hex4(ch);
        } else if ((u & 0xF0) == 0xF0) {
            unsigned int uu = 0;
            uu += (u & 0x07);
            for (int j = 0; j < 3; j++) {
                uu <<= 6;
                i++;
                u = ch = str[i];
                uu += (u & 0x3F);
            }
            ret += decode(uu);
        } else if ((u & 0xE0) == 0xE0) {
            unsigned int uu = 0;
            uu += (u & 0x0F);
            for (int j = 0; j < 2; j++) {
                uu <<= 6;
                i++;
                u = ch = str[i];
                uu += (u & 0x3F);
            }
            ret += decode(uu);
        } else if ((u & 0xC0) == 0xC0) {
            unsigned int uu = 0;
            uu += (u & 0x1F);
            uu <<= 6;
            i++;
            u = ch = str[i];
            uu += (u & 0x3F);
            ret += decode(uu);
        } else {
            ret += ch;
        }
    }
    return string("\"") + ret + "\"";
}

// Object-class function

ostream &Object::file_output(const string &addr, char mode)
{
    ofstream &os=*new ofstream(addr);
    if (!os) {
        cout << "file open fail";
        throw "file open fail";
    }
    string ss;
    if (mode == 's')
        ss = this->serialize();
    else if (mode == 't')
        ss = this->to_str();
    else
        ss = this->printer();
    os << ss;
    os.close();
    return os;
}
ostream &Object::output(ostream &os,char mode)
{
    os<<this->base_str(mode);
    return os;
}
ostream &file_output(const string &addr, Object &obj, char mode)
{
    return obj.file_output(addr, mode);
}
Object::Object(Json_Type tp, const char *s)
{
    mytype_ = tp;
    mystr_ = s;
}
Object::~Object() {}
string Object::serialize()
{
    return base_str('s');
}
string Object::to_str()
{
    return base_str('t');
}
string Object::printer(int depth)
{
    return base_str('p', depth);
}

string Object::base_str(char ch, int depth)
{
    if (mytype_ == IS_NULL) {
        if (ch == 'p')
            return string(depth * 4, ' ') + "null";
        else
            return "null";
    } else if (mytype_ == IS_TRUE) {
        if (ch == 'p')
            return string(depth * 4, ' ') + "true";
        else
            return "true";
    } else if (mytype_ == IS_FALSE) {
        if (ch == 'p')
            return string(depth * 4, ' ') + "false";
        else
            return "false";
    } else if (mytype_ == STRING) {
        if (ch == 'p')
            return string(depth * 4, ' ') + "\"" + mystr_ + "\"";
        else if (ch == 's')
            return encode_str(mystr_);
        else
            return "\"" + mystr_ + "\"";
    } else if (mytype_ == NUMBER_INT) {
        if (ch == 'p')
            return string(depth * 4, ' ') +
                   to_string(myvalue_.numberlonglong_);
        else
            return to_string(myvalue_.numberlonglong_);
    } else if (mytype_ == NUMBER_REAL) {
        if (ch == 'p')
            return string(depth * 4, ' ') + to_string(myvalue_.numberdouble_);
        else
            return to_string(myvalue_.numberdouble_);
    }
    if (ch == 'p')
        return string(depth * 4, ' ') + "Object";
    else
        return "Object";
}

int Object::get_json_type()
{
    return mytype_;
}
Object &Object::get(string key)
{
    try {
        throw "this Object is not Json Object";
    } catch (const char *err_msg) {
        cout << err_msg << "\n";
    }
    return *new Object();
}
Object &Object::operator[](string key)
{
    return this->get(key);
}
Object &Object::get(int index)
{
    try {
        throw "this Object is not Json Array";
    } catch (const char *err_msg) {
        cout << err_msg << "\n";
    }
    return *new Object();
}
Object &Object::operator[](int index)
{
    return this->get(index);
}
long long Object::get_int_value()
{
    if (this->get_json_type() == NUMBER_INT)
        return this->myvalue_.numberlonglong_;
    else
        try {
            throw "this Object is not int number";
        } catch (const char *err_msg) {
            cout << err_msg << "\n";
        }
    return 0;
}
double Object::get_real_value()
{
    if (this->get_json_type() == NUMBER_REAL)
        return this->myvalue_.numberdouble_;
    else
        try {
            throw "this Object is not real number";
        } catch (const char *err_msg) {
            cout << err_msg << "\n";
        }
    return 0;
}
string Object::get_str_value()
{
    if (this->get_json_type() == STRING)
        return this->mystr_;
    else
        try {
            throw "this Object is not string";
        } catch (const char *err_msg) {
            cout << err_msg << "\n";
        }
    return "";
}
bool Object::get_bool_value()
{
    if (this->get_json_type() == IS_TRUE)
        return true;
    else if (this->get_json_type() == IS_FALSE)
        return false;
    else
        try {
            throw "this Object is not boolean value";
        } catch (const char *err_msg) {
            cout << err_msg << "\n";
        }
    return false;
}
string Object::get_null_value()
{
    if (this->get_json_type() == IS_NULL)
        return "null";
    else
        try {
            throw "this Object is not null value";
        } catch (const char *err_msg) {
            cout << err_msg << "\n";
        }
    return "null";
}

// JsonObject-class function

Json_Object::Json_Object()
{
    mytype_ = OBJECT;
}
void Json_Object::put(string key, Object *value)
{
    mp_[key] = value;
}

Object &Json_Object::get(string key)
{
    try {
        if (mp_.count(key))
            return *mp_[key];
        else
            throw "invalid key in the object map";
    } catch (const char *err_msg) {
        cout << err_msg << "\n";
    }
    return *new Object();
}

Object &Json_Object::operator[](string key)
{
    return this->get(key);
}

string Json_Object::serialize()
{
    return base_str('s');
}

string Json_Object::to_str()
{
    return base_str('t');
}

string Json_Object::printer(int depth)
{
    return base_str('p', depth);
}

string Json_Object::base_str(char ch, int depth)
{
    string ret;
    if (ch == 'p')
        ret = string(depth * 4, ' ') + "{\n";
    else
        ret = "{";
    depth++;
    for (auto it = mp_.begin(); it != mp_.end();) {
        if (ch == 'p')
            ret += string(depth * 4, ' ');
        if (ch == 's')
            ret += encode_str(it->first);
        else
            ret += "\"" + it->first + "\"";
        ret += ":";
        if (ch == 'p')
            ret += "\n";
        ret += it->second->base_str(ch, depth + 1);
        if (++it != mp_.end())
            ret += ",";
        if (ch == 'p')
            ret += "\n";
    }
    depth--;
    if (ch == 'p')
        ret += string(depth * 4, ' ');
    ret += "}";
    return ret;
}

// JsonArray-class function

Json_Array::Json_Array()
{
    mytype_ = ARRAY;
}
void Json_Array::add(Object *obj)
{
    arr_.push_back(obj);
}

Object &Json_Array::get(int index)
{
    try {
        if (index < arr_.size()) {
            return *arr_[index];
        }
        throw "out of bound in array";
    } catch (const char *err_msg) {
        cout << err_msg << "\n";
    }
    Object *tmp = new Object();
    return *tmp;
}

Object &Json_Array::operator[](int index)
{
    return this->get(index);
}

int Json_Array::size()
{
    return arr_.size();
}

string Json_Array::to_str()
{
    return base_str('t');
}

string Json_Array::serialize()
{
    return base_str('s');
}

string Json_Array::printer(int depth)
{
    return base_str('p', depth);
}

string Json_Array::base_str(char ch, int depth)
{
    if (this->arr_.size() == 0) {
        if (ch == 'p') {
            return string(depth * 4, ' ') + "[\n" + string(depth * 4, ' ') +
                   "]";
        } else
            return "[]";
    }
    string ret;
    if (ch == 'p')
        ret = string(depth * 4, ' ') + "[\n";
    else
        ret = "[";
    depth++;
    for (int i = 0; i < arr_.size() - 1; i++) {
        ret += arr_[i]->base_str(ch, depth);
        if (ch == 'p')
            ret += ",\n";
        else
            ret += ",";
    }
    ret += arr_[arr_.size() - 1]->base_str(ch, depth);
    if (ch == 'p')
        ret += "\n";
    depth--;
    if (ch == 'p')
        ret += string(depth * 4, ' ');
    ret += "]";
    return ret;
}
