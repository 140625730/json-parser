#include "Tokenization.h"
// reader function
Reader::Reader()
{
    str_ = "";
}
Reader::Reader(const string& str)
{
    set_str(str);
}
Reader::Reader(const char* str)
{
    set_str(str);
}
Reader::~Reader() {}
void Reader::set_str(const string& str)
{
    ind_ = 0;
    str_ = str;
    maxsize_ = str_.size();
}
void Reader::set_str(const char* str)
{
    ind_ = 0;
    str_ = str;
    maxsize_ = str_.size();
}
char Reader::get()
{
    if (has_char())
        return str_[ind_];
    else
        return 0;
}
bool Reader::has_char()
{
    if (ind_ < maxsize_)
        return true;
    else
        return false;
}
bool Reader::next()
{
    ind_++;
    if (has_char())
        return true;
    else
        return false;
}
bool Reader::has_more()
{
    if (ind_ + 1 < maxsize_)
        return true;
    else
        return false;
}
char Reader::get_next()
{
    if (has_more())
        return str_[ind_ + 1];
    else
        return 0;
}

// tokenization function
Tokenization::Tokenization() {}
Tokenization::Tokenization(const string& str)
{
    rd_.set_str(str);
}
Tokenization::Tokenization(const char* str)
{
    rd_.set_str(str);
}
Tokenization::~Tokenization() {}
void Tokenization::rd_next()
{
    rd_.next();
}
unsigned int Tokenization::read_hex()
{
    unsigned ret = 0;
    for (int i = 0; i < 4; i++) {
        if (rd_.next()) {
            ret <<= 4;
            char ch = rd_.get();
            if (ch >= '0' && ch <= '9')
                ret |= (ch - '0');
            else if (ch >= 'A' && ch <= 'F')
                ret |= (ch - 'A' + 10);
            else if (ch >= 'a' && ch <= 'f')
                ret |= (ch - 'a' + 10);
            else
                throw "invaild char in token string while taking hex";
        } else
            throw "end with no enough hex number";
    }
    return ret;
}
void Tokenization::encode_utf(string& str, unsigned int u)
{
    //根据码点值大小散列到1-4字节的utf8编码
    if (u <= 0x7F) {
        str += char(u & 0xFF);
    } else if (u <= 0x7FF) {
        str += char(0xC0 | ((u >> 6) & 0xFF));
        str += char(0x80 | (u & 0x3F));
    } else if (u <= 0xFFFF) {
        str += char(0xE0 | ((u >> 12) & 0xFF));
        str += char(0x80 | ((u >> 6) & 0x3F));
        str += char(0x80 | (u & 0x3F));
    } else if (u <= 0x10FFFF) {
        str += char(0xF0 | ((u >> 18) & 0xFF));
        str += char(0x80 | ((u >> 12) & 0x3F));
        str += char(0x80 | ((u >> 6) & 0x3F));
        str += char(0x80 | (u & 0x3F));
    } else
        throw "out of bound in encoding utf8";
}
bool Tokenization::read_null()
{
    if (rd_.next() && rd_.get() == 'u' && rd_.next() && rd_.get() == 'l' &&
        rd_.next() && rd_.get() == 'l') {
        return true;
    } else
        return false;
}
bool Tokenization::read_false()
{
    if (rd_.next() && rd_.get() == 'a' && rd_.next() && rd_.get() == 'l' &&
        rd_.next() && rd_.get() == 's' && rd_.next() && rd_.get() == 'e') {
        return true;
    } else
        return false;
}
bool Tokenization::read_true()
{
    if (rd_.next() && rd_.get() == 'r' && rd_.next() && rd_.get() == 'u' &&
        rd_.next() && rd_.get() == 'e') {
        return true;
    } else
        return false;
}
Token Tokenization::read_str()
{
    rd_.next();
    string tmp;
    while (rd_.has_char() && rd_.get() != '\"') {
        if (rd_.get() == '\\') {
            rd_.next();
            //转义字符处理
            if (!(rd_.has_char() &&
                  (rd_.get() == '\"' || rd_.get() == '\\' ||
                   rd_.get() == 'u' || rd_.get() == 'r' || rd_.get() == 'n' ||
                   rd_.get() == 'b' || rd_.get() == 't' || rd_.get() == 'f' ||
                   rd_.get() == '/'))) {
                throw "invalid character in token string while taking \\";
            }
            //unicode转义
            if (rd_.get() == 'u') {
                unsigned int u = 0;
                u = read_hex();
                if (u >= 0xD800 && u <= 0xD8FF) {
                    if (rd_.next() && rd_.get() != '\\')
                        throw "invalid character in token string while taking utf prefix \\";
                    if (rd_.next() && rd_.get() != 'u')
                        throw "invalid character in token string while taking utf prefix \\";
                    unsigned int uu = read_hex();
                    if (uu < 0xDC00 || uu > 0xDFFF)
                        throw "out of bound in utf char 2";
                    u = (((u - 0xD800) << 10) | (uu - 0xDC00)) + 0x10000;
                }
                encode_utf(tmp, u);
            } else if (rd_.get() == '\"') {
                tmp += '\"';
            } else if (rd_.get() == '\\') {
                tmp += '\\';
            } else if (rd_.get() == '/') {
                tmp += '/';
            } else if (rd_.get() == 'b') {
                tmp += '\b';
            } else if (rd_.get() == 'f') {
                tmp += '\f';
            } else if (rd_.get() == 'n') {
                tmp += '\n';
            } else if (rd_.get() == 'r') {
                tmp += '\r';
            } else if (rd_.get() == 't') {
                tmp += '\t';
            }
        } else if (rd_.get() == '\r' || rd_.get() == '\n') {
            throw "invalid character in token string";
        } else
            tmp += rd_.get();
        rd_.next();
    }
    if (rd_.get() == '\"') {
        return Token{Type::STRING, tmp};
    } else
        throw "invalid character in token string,end with no \"";
}
Token Tokenization::read_number()
{
    string tmp;
    if (rd_.get() == '-') {
        tmp += '-';
        rd_.next();
    }
    if (rd_.has_char() && isdigit(rd_.get())) {
        if (rd_.get() == '0') {
            tmp += rd_.get();
            //判断是否有多余前导0
            if (rd_.has_more() && rd_.get_next() == '0')
                throw "more than one zero";
        } else {
            tmp += rd_.get();
            while (rd_.has_more() && isdigit(rd_.get_next())) {
                rd_.next();
                tmp += rd_.get();
            }
        }
    } else
        throw "invalid character";
    if (rd_.has_more() && rd_.get_next() == '.') {
        rd_.next();
        tmp += rd_.get();
        rd_.next();
        if (rd_.has_char() && isdigit(rd_.get())) {
            tmp += rd_.get();
            while (rd_.has_more() && isdigit(rd_.get_next())) {
                rd_.next();
                tmp += rd_.get();
            }
        } else
            throw "invalid character in token number while taking fraction";
    }
    if (rd_.has_more() && (rd_.get_next() == 'e' || rd_.get_next() == 'E')) {
        rd_.next();
        tmp += rd_.get();
        rd_.next();
        if (rd_.has_char() && (rd_.get() == '+' || rd_.get() == '-')) {
            tmp += rd_.get();
            rd_.next();
        }
        if (rd_.has_char() && isdigit(rd_.get())) {
            tmp += rd_.get();
            while (rd_.has_more() && isdigit(rd_.get_next())) {
                rd_.next();
                tmp += rd_.get();
            }
        } else
            throw "invalid character in token number while taking exponent";
    }
    return Token{Type::NUMBER, tmp};
}
Token Tokenization::read_token()
{
    try {
        //根据字符开头确定Token类型,捕获错误字符异常
        while (rd_.has_char()) {
            switch (rd_.get()) {
            case ' ':
            case '\n':
            case '\r':
            case '\t':
            case '\f':
                break;
            case '{':
                return (Token{Type::OBJECT_BEGIN, "{"});
            case '}':
                return (Token{Type::OBJECT_END, "}"});
            case '[':
                return (Token{Type::ARRAY_BEGIN, "["});
            case ']':
                return (Token{Type::ARRAY_END, "]"});
            case ',':
                return (Token{Type::IS_COMMA, ","});
            case ':':
                return (Token{Type::IS_COLON, ":"});
            case 'n':
            {
                if (read_null()) {
                    return (Token{Type::IS_NULL, "null"});
                } else
                    throw "invalid character in token null";
            }
            case 'f':
            {
                if (read_false()) {
                    return (Token{Type::BOOLEAN, "false"});
                } else
                    throw "invalid character in token false";
            }
            case 't':
            {
                if (read_true()) {
                    return (Token{Type::BOOLEAN, "true"});
                } else
                    throw "invalid character in token true";
            }
            case '"':
            {
                return read_str();
            }
            default:
            {
                return read_number();
            }
            }
            rd_.next();
        }
    } catch (const char* err_msg) {
        cout << err_msg << "\n";
    }
    return (Token{Type::DOCUMENT_END, ""});
}
vector<Token> Tokenization::token_list(const string& s)
{
    rd_ = Reader(s);
    vector<Token> tokens;
    // tokens.push_back({type::DOCUMENT_BEGIN,""});
    while (rd_.has_char()) {
        Token next_token = read_token();
        if (next_token.token_type == Type::DOCUMENT_END)
            break;
        tokens.push_back(next_token);
        rd_.next();
    }
    // tokens.push_back(Token{type::DOCUMENT_END,""});
    return tokens;
}
