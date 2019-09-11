#ifndef JSON_HPP
#define JSON_HPP

#include <memory>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <ostream>

class JSON;
#ifdef JSON_STR_UNICODE
typedef std::wstring JSON_str;
typedef wchar_t JSON_str_char;
#else
typedef std::string JSON_str;
typedef char JSON_str_char;
#endif
typedef std::map<JSON_str, JSON> JSON_obj;
typedef std::vector<JSON> JSON_list;
typedef int64_t JSON_int;
typedef double JSON_float;
typedef bool JSON_bool;
typedef int JSON_none;

class JSON
{
    public:
        enum Type {UNDEF=-1, OBJ=0, LIST=1, STR=2, BOOL=3, INT=4, FLOAT=5, NONE=6};

        JSON();
        JSON(const JSON& cpy);
        JSON(const JSON_str_char* str);
        JSON(const JSON_str &str);
        JSON(const int &i);
        JSON(const JSON_int &i);
        JSON(const float &f);
        JSON(const JSON_float &f);
        JSON(const JSON_bool &b);
        JSON(const JSON_obj &o);
        JSON(const JSON_list &l);
        ~JSON();
        Type type() const;
        bool parse(const std::string& file);
        bool write(const std::string& file);
        JSON& operator[](size_t index) const;
        JSON& operator[](const JSON_str &key) const;
        size_t size() const;
        void* get() const;

        void set(); // set to null
        void set(const JSON_str_char* str);
        void set(const JSON_str &str);
        void set(const int &i);
        void set(const JSON_int &i);
        void set(const float &f);
        void set(const JSON_float &f);
        void set(const JSON_bool &b);
        void set(const JSON_obj &o);
        void set(const JSON_list &l);

        std::string to_string(const JSON_str& str) const; // convert unicode string to ansi
    protected:
        bool _parse(std::istream& f);
        bool _parse_obj(std::istream& f);
        bool _parse_list(std::istream& f);
        bool _parse_value(std::istream& f, char& c);
        bool _read_value(std::istream& f, char& c, JSON_float& d);
        bool _parse_true(std::istream& f);
        bool _parse_false(std::istream& f);
        bool _parse_null(std::istream& f);
        bool _parse_str(std::istream& f);
        bool _parse_key(std::istream& f, JSON_str& key);
        bool _parse_hexa(std::istream& f, JSON_str& key);
        Type m_t;
        std::shared_ptr<void> mem;
};


std::ostream& operator<<(std::ostream& os, const JSON& j);

#endif // JSON_HPP
