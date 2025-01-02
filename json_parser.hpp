#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

namespace sp
{

    enum JsonType
    {
        JSON_ERROR = -1,
        JSON_NULL,
        JSON_BOOL,
        JSON_NUMBER,
        JSON_STRING,
        JSON_ARRAY,
        JSON_OBJECT
    };

    struct json_t
    {
        JsonType type = JSON_ERROR;
        std::vector<std::string> keys = {};
        std::vector<json_t> values = {};

        bool to_bool() const
        {
            if (type == JSON_BOOL)
                return keys[0] == "true";
            return false;
        }

        int to_int() const
        {
            if (type == JSON_NUMBER)
                return std::stoi(keys[0]);
            return 0;
        }

        double to_double() const
        {
            if (type == JSON_NUMBER)
                return std::stod(keys[0]);
            return 0.0;
        }

        std::string to_string() const
        {
            if (type == JSON_STRING)
                return keys[0];
            return "";
        }

        json_t &operator[](const std::string &key)
        {
            if (type == JSON_OBJECT)
            {
                for (size_t i = 0; i < keys.size(); ++i)
                {
                    if (keys[i] == key)
                        return values[i];
                }
            }
            keys.push_back(key);
            values.push_back(json_t());
            return values.back();
        }


        json_t &operator[](uint32_t index)
        {
            if (type == JSON_ARRAY && index < values.size())
                return values[index];
            keys.push_back(std::to_string(index));
            values.push_back(json_t());
            return values.back();
        }

        size_t size() const { return values.size(); }


        json_t & operator=(const json_t &other)
        {
            type = other.type;
            keys = other.keys;
            values = other.values;
            return *this;
        }
        
        json_t& operator=(const std::string & str)
        {
            type = JSON_STRING;
            keys.push_back(str);
            return *this;
        }

        json_t& operator=(const char * str)
        {
            type = JSON_STRING;
            keys.push_back(str);
            return *this;
        }

        json_t& operator=(const int & num)
        {
            type = JSON_NUMBER;
            keys.push_back(std::to_string(num));
            return *this;
        }

        json_t& operator=(const double & num)
        {
            type = JSON_NUMBER;
            keys.push_back(std::to_string(num));
            return *this;
        }

        json_t& operator=(const bool & b)
        {
            type = JSON_BOOL;
            keys.push_back(b ? "true" : "false");
            return *this;
        }

        void push_back(const json_t &json)
        {
            if (type == JSON_ARRAY)
                values.push_back(json);
        }

        void push_back(const std::string &str)
        {
            if (type == JSON_ARRAY)
            {
                json_t json;
                json.type = JSON_STRING;
                json.keys.push_back(str);
                values.push_back(json);
            }
        }

        void push_back(const char *str)
        {
            if (type == JSON_ARRAY)
            {
                json_t json;
                json.type = JSON_STRING;
                json.keys.push_back(str);
                values.push_back(json);
            }
        }


    };

    static json_t _parse_json(char *&cptr);

    static void skip_ws(char **cptr)
    {
        auto c = *cptr;
        while (*c == ' ' || *c == '\t' || *c == '\n' || *c == '\r')
        {
            c++;
        }
        *cptr = c;
    }

    static void _read_string(char **cptr, std::string &str)
    {
        char *c = *cptr + 1;
        std::string result;
        while (*c != '"' && *c != '\0')
        {
            if (*c == '\\')
            {
                c++;
                switch (*c)
                {
                case '"':
                case '\\':
                case '/':
                    result.push_back(*c);
                    break;
                case 'b':
                    result.push_back('\b');
                    break;
                case 'f':
                    result.push_back('\f');
                    break;
                case 'n':
                    result.push_back('\n');
                    break;
                case 'r':
                    result.push_back('\r');
                    break;
                case 't':
                    result.push_back('\t');
                    break;
                default:
                    result.push_back(*c);
                    break;
                }
            }
            else
            {
                result.push_back(*c);
            }
            c++;
        }
        if (*c == '"')
            c++;
        *cptr = c;
        str = result;
    }

    static void _read_number(char **cptr, std::string &str)
    {
        char *c = *cptr;
        char *start = c;
        if (*c == '-')
            c++;
        while (*c >= '0' && *c <= '9')
            c++;
        if (*c == '.')
        {
            c++;
            while (*c >= '0' && *c <= '9')
                c++;
        }
        if (*c == 'e' || *c == 'E')
        {
            c++;
            if (*c == '+' || *c == '-')
                c++;
            while (*c >= '0' && *c <= '9')
                c++;
        }
        *cptr = c;
        str.assign(start, c);
    }

    static void _read_object(char **cptr, json_t &json)
    {
        json.type = JSON_OBJECT;
        (*cptr)++;
        skip_ws(cptr);
        while (**cptr != '}' && **cptr != '\0')
        {
            std::string key;
            _read_string(cptr, key);
            json.keys.push_back(key);

            skip_ws(cptr);
            if (**cptr != ':')
            {
                json.type = JSON_ERROR;
                return;
            }
            (*cptr)++;
            skip_ws(cptr);

            json.values.push_back(_parse_json(*cptr));
            skip_ws(cptr);

            if (**cptr == ',')
                (*cptr)++;
            skip_ws(cptr);
        }
        if (**cptr == '}')
            (*cptr)++;
    }

    static void _read_array(char **cptr, json_t &json)
    {
        json.type = JSON_ARRAY;
        (*cptr)++;
        skip_ws(cptr);
        while (**cptr != ']' && **cptr != '\0')
        {
            json.values.push_back(_parse_json(*cptr));
            skip_ws(cptr);
            if (**cptr == ',')
                (*cptr)++;
            skip_ws(cptr);
        }
        if (**cptr == ']')
            (*cptr)++;
    }

    static json_t _parse_json(char *&cptr)
    {
        skip_ws(&cptr);
        json_t root;

        if (*cptr == '"')
        {
            root.type = JSON_STRING;
            std::string value;
            _read_string(&cptr, value);
            root.keys.push_back(value);
        }
        else if (*cptr == '-' || (*cptr >= '0' && *cptr <= '9'))
        {
            root.type = JSON_NUMBER;
            std::string value;
            _read_number(&cptr, value);
            root.keys.push_back(value);
        }
        else if (*cptr == 't' && strncmp(cptr, "true", 4) == 0)
        {
            root.type = JSON_BOOL;
            root.keys.emplace_back("true");
            cptr += 4;
        }
        else if (*cptr == 'f' && strncmp(cptr, "false", 5) == 0)
        {
            root.type = JSON_BOOL;
            root.keys.emplace_back("false");
            cptr += 5;
        }
        else if (*cptr == 'n' && strncmp(cptr, "null", 4) == 0)
        {
            root.type = JSON_NULL;
            root.keys.emplace_back("null");
            cptr += 4;
        }
        else if (*cptr == '{')
        {
            _read_object(&cptr, root);
        }
        else if (*cptr == '[')
        {
            _read_array(&cptr, root);
        }
        else
        {
            root.type = JSON_ERROR;
        }
        return root;
    }

    inline json_t parse_json(const std::string &str)
    {
        char *cptr = const_cast<char *>(str.c_str());
        return _parse_json(cptr);
    }

    inline json_t parse_json(const char *str)
    {
        char *cptr = const_cast<char *>(str);
        return _parse_json(cptr);
    }

    static std::string to_string(const json_t &json, bool compact = true, int indent = 2, int current_indent = 0)
    {
        std::ostringstream oss;

        auto append_indent = [&](int level)
        {
            if (!compact)
                oss << std::string(level, ' ');
        };

        switch (json.type)
        {
        case JSON_NULL:
            oss << "null";
            break;

        case JSON_BOOL:
        case JSON_NUMBER:
        case JSON_STRING:
            if (json.type == JSON_STRING)
                oss << "\"" << json.keys[0] << "\"";
            else
                oss << json.keys[0];
            break;

        case JSON_ARRAY:
            if (json.values.empty())
            {
                oss << "[]";
                break;
            }
            oss << "[";
            if (!compact)
                oss << "\n";
            for (size_t i = 0; i < json.values.size(); ++i)
            {
                append_indent(current_indent + indent);
                oss << to_string(json.values[i], compact, indent, current_indent + indent);
                if (i < json.values.size() - 1)
                    oss << ",";
                if (!compact)
                    oss << "\n";
            }
            if (!compact)
                append_indent(current_indent);
            oss << "]";
            break;

        case JSON_OBJECT:
            if (json.keys.empty())
            {
                oss << "{}";
                break;
            }
            oss << "{";
            if (!compact)
                oss << "\n";
            for (size_t i = 0; i < json.keys.size(); ++i)
            {
                append_indent(current_indent + indent);
                oss << "\"" << json.keys[i] << "\": ";
                oss << to_string(json.values[i], compact, indent, current_indent + indent);
                if (i < json.keys.size() - 1)
                    oss << ",";
                if (!compact)
                    oss << "\n";
            }
            if (!compact)
                append_indent(current_indent);
            oss << "}";
            break;

        default:
            oss << "null";
            break;
        }

        return oss.str();
    }

    json_t cteate_json_object()
    {
        json_t json;
        json.type = JSON_OBJECT;
        return json;
    }

    json_t create_json_array()
    {
        json_t json;
        json.type = JSON_ARRAY;
        return json;
    }
};
