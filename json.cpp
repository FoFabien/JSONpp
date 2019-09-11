#include "json.hpp"

#include <cmath>
#include <stdexcept>

JSON::JSON()
{
    m_t = NONE;
    mem = std::make_shared<JSON_none>(0);
}

JSON::JSON(const JSON& cpy)
{
    m_t = cpy.m_t;
    mem = cpy.mem;
}

JSON::JSON(const JSON_str_char *str)
{
    m_t = STR;
    mem = std::make_shared<JSON_str>(str);
}

JSON::JSON(const JSON_str &str)
{
    m_t = STR;
    mem = std::make_shared<JSON_str>(str);
}

JSON::JSON(const int &i)
{
    m_t = INT;
    mem = std::make_shared<JSON_int>(i);
}

JSON::JSON(const JSON_int &i)
{
    m_t = INT;
    mem = std::make_shared<JSON_int>(i);
}

JSON::JSON(const float &f)
{
    m_t = FLOAT;
    mem = std::make_shared<JSON_float>(f);
}

JSON::JSON(const JSON_float &f)
{
    m_t = FLOAT;
    mem = std::make_shared<JSON_float>(f);
}

JSON::JSON(const JSON_bool &b)
{
    m_t = BOOL;
    mem = std::make_shared<JSON_bool>(b);
}

JSON::JSON(const JSON_obj &o)
{
    m_t = OBJ;
    mem = std::make_shared<JSON_obj>(o);
}

JSON::JSON(const JSON_list &l)
{
    m_t = LIST;
    mem = std::make_shared<JSON_list>(l);
}

JSON::~JSON()
{

}

JSON::Type JSON::type() const
{
    return m_t;
}

bool JSON::parse(const std::string& file)
{
    std::ifstream f(file, std::ios::in | std::ios::binary);
    if(!f) return false;

    m_t = UNDEF;
    return _parse(f);
}

bool JSON::write(const std::string& file)
{
    std::ofstream f(file, std::ios::out | std::ios::trunc | std::ios::binary);
    if(!f) return false;

    f << *this;
    return true;
}

bool JSON::_parse(std::istream& f)
{
    char c;
    while(true)
    {
        f.read(&c, 1);
        if(!f.good()) break;
        switch (c)
        {
            case '\t':
            case '\r':
            case '\n':
            case ' ':
                break;
            case '{':
            {
                if(!_parse_obj(f))
                    return false;
                break;
            }
            case '[':
            {
                if(!_parse_list(f))
                    return false;
                break;
            }
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                if(!_parse_value(f, c))
                    return false;
                break;
            }
            case 't':
            {
                if(!_parse_true(f))
                    return false;
                break;
            }
            case 'f':
            {
                if(!_parse_false(f))
                    return false;
                break;
            }
            case 'n':
            {
                if(!_parse_null(f))
                    return false;
                break;
            }
            case '\"':
            {
                if(!_parse_str(f))
                    return false;
                break;
            }
            default:
                return false;
        }
    }
    return true;
}

#include <iostream>
bool JSON::_parse_obj(std::istream& f)
{
    m_t = OBJ;
    mem = std::make_shared<JSON_obj>();
    JSON_obj* ptr = (JSON_obj*)mem.get();
    JSON_str key;
    char c;
    int state = 0;
    while(true)
    {
        f.read(&c, 1);
        if(!f.good()) break;
        switch (c)
        {
            case '\t':
            case '\r':
            case '\n':
            case ' ':
                break;
            case '}':
                return (state == 3);
            case ',':
                if(state != 3) return false;
                state = 0;
                break;
            case ':':
                if(state != 1) return false;
                state = 2;
                break;
            case '{':
            {
                if(state != 2) return false;
                (*ptr)[key] = JSON();
                if(!(*ptr)[key]._parse_obj(f))
                    return false;
                state = 3;
                break;
            }
            case '[':
            {
                if(state != 2) return false;
                (*ptr)[key] = JSON();
                if(!(*ptr)[key]._parse_list(f))
                    return false;
                state = 3;
                break;
            }
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if(state != 2) return false;
                (*ptr)[key] = JSON();
                if(!(*ptr)[key]._parse_value(f, c))
                    return false;
                state = 3;
                break;
            case 't':
            {
                if(state != 2) return false;
                (*ptr)[key] = JSON();
                if(!(*ptr)[key]._parse_true(f))
                    return false;
                state = 3;
                break;
            }
            case 'f':
            {
                if(state != 2) return false;
                (*ptr)[key] = JSON();
                if(!(*ptr)[key]._parse_false(f))
                    return false;
                state = 3;
                break;
            }
            case 'n':
            {
                if(state != 2) return false;
                (*ptr)[key] = JSON();
                if(!(*ptr)[key]._parse_null(f))
                    return false;
                state = 3;
                break;
            }
            case '\"':
            {
                if(state == 0)
                {
                    if(!_parse_key(f, key))
                        return false;
                    state = 1;
                }
                else if(state == 2)
                {
                    (*ptr)[key] = JSON();
                    if(!(*ptr)[key]._parse_str(f))
                        return false;
                    state = 3;
                }
                else return false;
                break;
            }
            default:
                return false;
        }
    }
    return false;
}

bool JSON::_parse_list(std::istream& f)
{
    m_t = LIST;
    mem = std::make_shared<JSON_list>();
    JSON_list* ptr = (JSON_list*)mem.get();
    char c;
    bool comma = false;
    while(true)
    {
        f.read(&c, 1);
        if(!f.good()) break;
        switch (c)
        {
            case '\t':
            case '\r':
            case '\n':
            case ' ':
                break;
            case ']':
                if(ptr->size() > 0 && !comma)
                    return false;
                return true;
            case ',':
                if(!comma) return false;
                comma = false;
                break;
            case '{':
            {
                if(comma) return false;
                ptr->push_back(JSON());
                if(!ptr->back()._parse_obj(f))
                    return false;
                comma = true;
                break;
            }
            case '[':
            {
                if(comma) return false;
                ptr->push_back(JSON());
                if(!ptr->back()._parse_list(f))
                    return false;
                comma = true;
                break;
            }
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if(comma) return false;
                ptr->push_back(JSON());
                if(!ptr->back()._parse_value(f, c))
                    return false;
                comma = true;
                break;
            case 't':
            {
                if(comma) return false;
                ptr->push_back(JSON());
                if(!ptr->back()._parse_true(f))
                    return false;
                comma = true;
                break;
            }
            case 'f':
            {
                if(comma) return false;
                ptr->push_back(JSON());
                if(!ptr->back()._parse_false(f))
                    return false;
                comma = true;
                break;
            }
            case 'n':
            {
                if(comma) return false;
                ptr->push_back(JSON());
                if(!ptr->back()._parse_null(f))
                    return false;
                comma = true;
                break;
            }
            case '\"':
            {
                if(comma) return false;
                ptr->push_back(JSON());
                if(!ptr->back()._parse_str(f))
                    return false;
                comma = true;
                break;
            }
            default:
                return false;
        }
    }
    return false;
}

bool JSON::_parse_value(std::istream& f, char& c)
{
    JSON_float d;
    if(!_read_value(f, c, d)) return false;
    if(ceil(d) != d)
    {
        m_t = FLOAT;
        mem = std::make_shared<JSON_float>(d);
    }
    else
    {
        m_t = INT;
        mem = std::make_shared<JSON_int>(ceil(d));
    }
    return true;
}


bool JSON::_read_value(std::istream& f, char& c, JSON_float& d)
{
    d = 0.f;
    JSON_float e = 0.f;
    bool dot = false;
    int sign = 0;
    bool num = false;
    JSON_float decimal = 10.f;
    std::streampos beg = f.tellg();
    int state = 0;
    while(state == 0)
    {
        switch(c)
        {
            case '-':
                if(f.tellg() != beg || sign != 0)
                    return false;
                sign = 1;
                break;
            case '.':
                if(f.tellg() != beg || dot || !num)
                    return false;
                dot = true;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                num = true;
                if(!dot) d = d * 10 + (c - '0');
                else
                {
                    d = d + (c - '0') / decimal;
                    decimal *= 10.f;
                }
                break;
            case 'e':
            case 'E':
                if(f.tellg() == beg || !num)
                    return false;
                state = 2;
                continue;
            case '\t':
            case '\r':
            case '\n':
            case ' ':
            case ',':
            case '}':
            case ']':
                state = 1;
                f.seekg(-1, f.cur);
                if(!f.good()) return false;
                continue;
            default:
                return false;
        }
        f.read(&c, 1);
        if(!f.good()) return false;
    }
    if(sign == 1) d = -d;
    sign = 0;
    num = false;
    beg = f.tellg();
    while(state == 2)
    {
        f.read(&c, 1);
        if(!f.good()) return false;
        switch(c)
        {
            case '+':
                if(f.tellg() != beg || sign != 0) return false;
                sign = 2;
                break;
            case '-':
                if(f.tellg() != beg || sign != 0) return false;
                sign = 1;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                num = true;
                e = e * 10 + (c - '0');
                break;
            case '\t':
            case '\r':
            case '\n':
            case ' ':
            case ',':
            case '}':
            case ']':
                state = 1;
                if(sign == 1) e = - e;
                d = pow(d, e);
                break;
            default:
                return false;
        }
    }
    if(state == 0) return false;
    return true;
}

bool JSON::_parse_true(std::istream& f)
{
    char c[3];
    f.read(c, 3);
    if(!f.good() || JSON_str(c) != "rue") return false;
    m_t = BOOL;
    mem = std::make_shared<JSON_bool>(true);
    return true;
}

bool JSON::_parse_false(std::istream& f)
{
    char c[4];
    f.read(c, 4);
    if(!f.good() || JSON_str(c) != "alse") return false;
    m_t = BOOL;
    mem = std::make_shared<JSON_bool>(false);
    return true;
}

bool JSON::_parse_null(std::istream& f)
{
    char c[3];
    f.read(c, 3);
    if(!f.good() || JSON_str(c) != "ull") return false;
    m_t = NONE;
    mem = std::make_shared<JSON_none>(0);
    return true;
}

bool JSON::_parse_str(std::istream& f)
{
    mem = std::make_shared<JSON_str>("");
    if(!_parse_key(f, (*(JSON_str*)mem.get()))) return false;
    m_t = STR;
    return true;
}

bool JSON::_parse_key(std::istream& f, JSON_str& key)
{
    char c;
    key = "";
    while(true)
    {
        f.read(&c, 1);
        if(!f.good()) return false;
        switch(c)
        {
            case '\\':
                f.read(&c, 1);
                if(!f.good()) return false;
                switch(c)
                {
                    case '"':
                    case '\\':
                    case '/':
                        key += c;
                        break;
                    case 'b':
                        key += '\b';
                        break;
                    case 'f':
                        key += '\f';
                        break;
                    case 'n':
                        key += '\n';
                        break;
                    case 'r':
                        key += '\r';
                        break;
                    case 't':
                        key += '\t';
                        break;
                    case 'u':
                        if(!_parse_hexa(f, key)) return false;
                        break;
                    default:
                        return false;
                }
                break;
            case '"':
                return true;
            default:
                key += c;
                break;
        }
    }
    return true;
}

bool JSON::_parse_hexa(std::istream& f, JSON_str& key)
{
    char hex_code[4];
    f.read(hex_code, 4);
    if(!f.good()) return false;
    #ifndef JSON_STR_UNICODE
        for(int i = 0; i < 4; ++i)
            if((hex_code[i] < '0' && hex_code[i] > '9') && (hex_code[i] < 'a' && hex_code[i] > 'f'))
                return false;
        key += "\\u" + JSON_str(hex_code);
    #elif
        JSON_str_char uchar = 0; // assuming UTF-8 char
        for(int i = 0; i < 4; ++i)
        {
            switch(hex_code[i])
            {
                case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':
                    uchar = hex_code[i] - 'a' + 10 + uchar << 4;
                    break;
                case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
                    uchar = hex_code[i] - '0' + uchar << 4;
                    break;
                default: return false;
            }
        }
        key += uchar;
    #endif
    return true;
}

JSON& JSON::operator[](size_t index) const
{
    if(m_t != LIST) throw std::runtime_error("invalid JSON type: can't use the json[int] on a non-list type");
    JSON_list* ptr = (JSON_list*)mem.get();
    return (*ptr)[index];
}

JSON& JSON::operator[](const JSON_str& key) const
{
    if(m_t != OBJ) throw std::runtime_error("invalid JSON type: can't use the json[string] on a non-ojbect m_t");
    JSON_obj* ptr = (JSON_obj*)mem.get();
    return (*ptr)[key];
}

size_t JSON::size() const
{
    switch(m_t)
    {
        case OBJ:
            return ((JSON_obj*)mem.get())->size();
        case LIST:
            return ((JSON_list*)mem.get())->size();
        case STR:
            return ((JSON_str*)mem.get())->size();
        default:
            throw std::runtime_error("invalid JSON type: can't use size() on a non-list type");
    }
}

void* JSON::get() const
{
    if(m_t == UNDEF) throw std::runtime_error("invalid JSON type: can't use ptr() on an undefined JSON");
    return mem.get();
}

void JSON::set()
{
    m_t = NONE;
    mem = std::make_shared<JSON_none>(0);
}

void JSON::set(const JSON_str_char *str)
{
    m_t = STR;
    mem = std::make_shared<JSON_str>(str);
}

void JSON::set(const JSON_str &str)
{
    m_t = STR;
    mem = std::make_shared<JSON_str>(str);
}

void JSON::set(const int &i)
{
    m_t = INT;
    mem = std::make_shared<JSON_int>(i);
}

void JSON::set(const JSON_int &i)
{
    m_t = INT;
    mem = std::make_shared<JSON_int>(i);
}

void JSON::set(const float &f)
{
    m_t = FLOAT;
    mem = std::make_shared<JSON_float>(f);
}

void JSON::set(const JSON_float &f)
{
    m_t = FLOAT;
    mem = std::make_shared<JSON_float>(f);
}

void JSON::set(const JSON_bool &b)
{
    m_t = BOOL;
    mem = std::make_shared<JSON_bool>(b);
}

void JSON::set(const JSON_obj &o)
{
    m_t = OBJ;
    mem = std::make_shared<JSON_obj>(o);
}

void JSON::set(const JSON_list &l)
{
    m_t = LIST;
    mem = std::make_shared<JSON_list>(l);
}

std::ostream& operator<<(std::ostream& os, const JSON& j)
{
    switch(j.type())
    {
        case JSON::UNDEF:
            throw std::runtime_error("invalid JSON type: can't use << on an undefined JSON");
        case JSON::OBJ:
            os << "{ ";
            for(JSON_obj::iterator it = ((JSON_obj*)j.get())->begin(); it != ((JSON_obj*)j.get())->end(); ++it)
            {
                if(it != ((JSON_obj*)j.get())->begin()) os << ", ";
                os << it->first << " : " << it->second;
            }
            os << " }";
            break;
        case JSON::LIST:
            os << "[ ";
            for(size_t i = 0; i < j.size(); ++i)
            {
                if(i != 0) os << ", ";
                os << j[i];
            }
            os << " ]";
            break;
        case JSON::STR:
            os << "\"" << (*(JSON_str*)j.get()) << "\"";
            break;
        case JSON::INT:
            os << (*(JSON_int*)j.get());
            break;
        case JSON::BOOL:
            os << ((*(JSON_bool*)j.get()) ? "true" : "false");
            break;
        case JSON::FLOAT:
            os << (*(JSON_float*)j.get());
            break;
        case JSON::NONE:
            os << "null";
            break;
    }
    return os;
}
