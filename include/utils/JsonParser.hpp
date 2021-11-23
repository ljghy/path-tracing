#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <exception>

#define ISDIGIT(c) ((c) >= '0' && (c) <= '9')
#define ISDIGIT_NON_ZERO(c) ((c) >= '1' && (c) <= '9')

enum JsonParseState
{
    PARSE_SUCCESS,
    PARSE_BUFFER_EMPTY,
    PARSE_INVALID_VALUE,
    PARSE_NO_VALUE_PARSED,
    PARSE_ROOT_NOT_SINGLE_VALUE,
    PARSE_MISS_QUATATION_MARK,
    PARSE_INVALID_STRING_ESCAPE,
    PARSE_INVALID_STRING_CHAR,
    PARSE_INVALID_UNICODE_HEX,
    PARSE_INVALID_UNICODE_SURROGATE,
    PARSE_NUMBER_TOO_BIG,
    PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
    PARSE_MISS_COMMA_OR_CURLY_BRACKET,
    PARSE_KEY_VALUE_PAIR_MISS_KEY,
    PARSE_KEY_VALUE_PAIR_MISS_VALUE,
    PARSE_NULL_PTR_ERROR
};

static const char *JsonErrMsg[]{
    "Success",
    "Buffer is empty",
    "Invalid value",
    "No value parsed",
    "Root is not single value",
    "Miss quatation mark",
    "Invalid string escape",
    "Invalid string char",
    "Invalid unicode hex",
    "Invalid unicode surrogate",
    "Number too big",
    "Miss comma or square bracket",
    "Miss comma or curly bracket",
    "Key-value pair miss key",
    "Key-value pair miss value",
    "Get null pointer when parsing"};

struct JsonParseErr
{
    size_t pos;
    JsonParseState state;
};

enum JsonNodeVisitExceptionType
{
    VST_NODE_TYPE_ERROR,
    VST_KEY_NOT_FOUND
};

class JsonNodeVisitException : public std::exception
{
private:
    JsonNodeVisitExceptionType err;
    std::string additional;

public:
    JsonNodeVisitException(JsonNodeVisitExceptionType t, const std::string &a)
        : err(t), additional(a) {}
    virtual const char *what() const noexcept override
    {
        switch (err)
        {
        case VST_NODE_TYPE_ERROR:
            return "Node type error"; // + additional).c_str();
        case VST_KEY_NOT_FOUND:
            return "Key not found"; //).c_str();
        }
        return "";
    }
};

enum JsonValueType
{
    NULL_TYPE,
    BOOL_TYPE,
    NUM_TYPE,
    STR_TYPE,
    ARR_TYPE,
    OBJ_TYPE
};

struct JsonNode;
typedef std::vector<JsonNode *> JsonArr;
typedef std::unordered_multimap<std::string, JsonNode *> JsonObj;

struct JsonNode
{
    JsonValueType valueType;
    union
    {
        float num;
        bool boo;
        std::string str;
        JsonArr arr;
        JsonObj obj;
    };

    JsonNode() : valueType(NULL_TYPE)
    {
    }

    void setType(JsonValueType t)
    {
        valueType = t;
        switch (t)
        {
        case BOOL_TYPE:
            new (&boo) bool;
            break;
        case NUM_TYPE:
            new (&num) float;
            break;
        case STR_TYPE:
            new (&str) std::string();
            break;
        case ARR_TYPE:
            new (&arr) JsonArr;
            break;
        case OBJ_TYPE:
            new (&obj) JsonObj;
            break;
        default:
            return;
        }
    }

    bool isNull() const { return valueType == NULL_TYPE; }
    bool isBool() const { return valueType == BOOL_TYPE; }
    bool isNum() const { return valueType == NUM_TYPE; }
    bool isStr() const { return valueType == STR_TYPE; }
    bool isArr() const { return valueType == ARR_TYPE; }
    bool isObj() const { return valueType == OBJ_TYPE; }

    int toInt() const
    {
        if (valueType != NUM_TYPE)
            throw JsonNodeVisitException(VST_NODE_TYPE_ERROR, ": Expect number type");
        return int(num + 0.5);
    }

    JsonNode(const JsonNode &) = delete;
    JsonNode &operator=(const JsonNode &) = delete;

    std::string toString() const
    {
        switch (valueType)
        {
        case NULL_TYPE:
            return "null";
        case BOOL_TYPE:
            return boo ? "true" : "false";
        case NUM_TYPE:
            return std::to_string(num);
        case STR_TYPE:
            return '\"' + str + '\"';
        case ARR_TYPE:
        {
            std::string result("[");
            for (auto it = arr.begin(); it != arr.end(); ++it)
            {
                result += (*it)->toString();
                if (it + 1 != arr.end())
                    result += ", ";
            }
            result += ']';
            return result;
        }
        default:
        {
            std::string result("{");
            size_t count = 0;
            for (auto each : obj)
            {
                result += '"' + each.first + "\": " + each.second->toString();
                if (++count < obj.size())
                    result += ", ";
            }
            return result += '}';
        }
        }
    }

    friend std::ostream &operator<<(std::ostream &os, JsonNode *node)
    {
        if (node == nullptr)
            return os;
        switch (node->valueType)
        {
        case NULL_TYPE:
            return os << "null";
        case BOOL_TYPE:
            if (node->boo)
                os << "true";
            else
                os << "false";
            return os;
        case NUM_TYPE:
            return os << node->num;
        case STR_TYPE:
            return os << '"' << node->str << '"';
        case ARR_TYPE:
            os << "[";
            for (auto it = node->arr.begin(); it != node->arr.end(); ++it)
            {
                os << *it;
                if (it + 1 != node->arr.end())
                    os << ", ";
            }
            return os << "]";
        default:
            os << "{";
            size_t count = 0;
            for (auto each : node->obj)
            {
                os << '"' << each.first << "\": " << each.second;
                if (++count < node->obj.size())
                    os << ", ";
            }
            return os << "}";
        }
    }

    JsonNode *deepCopy() const
    {
        JsonNode *result = new JsonNode;
        result->setType(valueType);
        switch (valueType)
        {
        case NULL_TYPE:
            return result;
        case BOOL_TYPE:
            result->boo = boo;
            return result;
        case NUM_TYPE:
            result->num = num;
            return result;
        case STR_TYPE:
            result->str = str;
            return result;
        case ARR_TYPE:
            for (auto &p : arr)
                result->arr.push_back(p->deepCopy());
            return result;
        default:
            for (auto &p : obj)
                result->obj.insert(std::make_pair(p.first, p.second->deepCopy()));
            return result;
        }
    }

    bool findKey(const std::string &key) const
    {
        if (valueType != OBJ_TYPE)
            return false;
        return obj.find(key) != obj.end();
    }

    JsonNode &operator[](size_t i) const
    {
        if (valueType != ARR_TYPE)
            throw JsonNodeVisitException(VST_NODE_TYPE_ERROR, ": Expect array type");
        return *arr[i];
    }

    JsonNode &operator[](const std::string &s) const
    {
        if (valueType != OBJ_TYPE)
            throw JsonNodeVisitException(VST_NODE_TYPE_ERROR, ": Expect object type");
        auto it = obj.find(s);
        if (it == obj.end())
            throw JsonNodeVisitException(VST_KEY_NOT_FOUND, s);
        return *(it->second);
    }

    ~JsonNode()
    {
        switch (valueType)
        {
        case STR_TYPE:
            str.~basic_string();
            break;
        case ARR_TYPE:
            arr.~vector();
            break;
        case OBJ_TYPE:
            obj.~unordered_multimap();
            break;
        default:;
        }
    }
};

class JsonParser
{
public:
    JsonNode *root;

private:
    size_t pos;
    std::string buffer;

public:
    static void PrintErrMsg(JsonParseErr &err)
    {
        if (err.state != PARSE_SUCCESS)
            printf("Error %d: %s at position %d\n", (int)err.state,
                   JsonErrMsg[err.state], (int)err.pos);
    }

public:
    JsonParser() : root(nullptr), pos(0), buffer("") {}

    JsonParser(const JsonParser &) = delete;
    JsonParser &operator=(const JsonParser &) = delete;

    bool loadFromFile(const char *filename)
    {
        FILE *fp;
        if (!(fp = fopen(filename, "rt")))
        {
            printf("Failed to load from file: %s\n", filename);
            return false;
        }
        char ch;
        buffer.clear();
        while (true)
        {
            fread(&ch, 1, 1, fp);
            if (!feof(fp))
                buffer.push_back(ch);
            else
                break;
        }
        fclose(fp);
        return true;
    }

    JsonParseErr parse(const std::string &buf = "")
    {
        if (buf.size())
            buffer = buf;
        pos = 0;
        parseSpace();
        if (reachEnd())
            return {0, PARSE_BUFFER_EMPTY};
        release();
        root = new JsonNode;
        JsonParseErr err = parseValue(root);
        if (err.state != PARSE_SUCCESS)
            return err;
        parseSpace();
        if (!reachEnd())
            return {pos, PARSE_ROOT_NOT_SINGLE_VALUE};
        return {0, PARSE_SUCCESS};
    }

    void release()
    {
        delete root;
        root = nullptr;
    }

    ~JsonParser() { release(); }

private:
    bool reachEnd() { return (pos == buffer.size()); }

    JsonParseErr parseValue(JsonNode *node)
    {
        if (node == nullptr)
            return {0, PARSE_NULL_PTR_ERROR};
        parseSpace();
        if (reachEnd())
            return {0, PARSE_NO_VALUE_PARSED};

        JsonParseErr err;
        switch (buffer[pos])
        {
        case 't':
        case 'f':
            node->setType(BOOL_TYPE);
            err = parseBool(node);
            break;
        case 'n':
            node->setType(NULL_TYPE);
            err = parseNull(node);
            break;
        case '"':
            node->setType(STR_TYPE);
            err = parseString(node->str);
            break;
        default:
            node->setType(NUM_TYPE);
            err = parseNumber(node);
            break;
        case '[':
            node->setType(ARR_TYPE);
            err = parseArray(node);
            break;
        case '{':
            node->setType(OBJ_TYPE);
            err = parseObject(node);
            break;
        }
        if (err.state != PARSE_SUCCESS)
            return err;
        return {0, PARSE_SUCCESS};
    }

    JsonParseErr parseObject(JsonNode *node)
    {
        if (node == nullptr)
            return {0, PARSE_NULL_PTR_ERROR};
        pos++;
        parseSpace();
        if (buffer[pos] == '}')
        {
            pos++;
            return {0, PARSE_SUCCESS};
        }
        JsonParseErr err;
        while (true)
        {
            if (buffer[pos] != '"')
                return {pos, PARSE_KEY_VALUE_PAIR_MISS_KEY};
            std::string key;
            err = parseString(key);
            if (err.state != PARSE_SUCCESS)
                return err;
            parseSpace();
            if (buffer[pos] == ':')
            {
                pos++;
                parseSpace();
            }
            else
                return {pos, PARSE_KEY_VALUE_PAIR_MISS_VALUE};

            JsonNode *val = new JsonNode;
            err = parseValue(val);
            if (err.state != PARSE_SUCCESS)
            {
                delete val;
                return err;
            }
            node->obj.insert(std::pair<std::string, JsonNode *>(key, val));
            parseSpace();
            if (buffer[pos] == ',')
            {
                pos++;
                parseSpace();
            }
            else if (buffer[pos] == '}')
            {
                pos++;
                return {0, PARSE_SUCCESS};
            }
            else
                return {pos, PARSE_MISS_COMMA_OR_CURLY_BRACKET};
        }
    }

    JsonParseErr parseArray(JsonNode *node)
    {
        if (node == nullptr)
            return {0, PARSE_NULL_PTR_ERROR};
        pos++;
        parseSpace();
        if (buffer[pos] == ']')
        {
            pos++;
            return {0, PARSE_SUCCESS};
        }
        while (true)
        {
            JsonNode *val = new JsonNode;
            JsonParseErr err = parseValue(val);
            if (err.state != PARSE_SUCCESS)
            {
                delete val;
                return err;
            }

            node->arr.push_back(val);
            parseSpace();
            if (buffer[pos] == ',')
            {
                pos++;
                parseSpace();
            }
            else if (buffer[pos] == ']')
            {
                pos++;
                return {0, PARSE_SUCCESS};
            }
            else
                return {pos, PARSE_MISS_COMMA_OR_SQUARE_BRACKET};
        }
    }

    JsonParseErr parseNumber(JsonNode *node)
    {
        if (node == nullptr)
            return {0, PARSE_NULL_PTR_ERROR};
        size_t p = pos;
        if (buffer[p] == '-' && !ISDIGIT(buffer[p + 1]))
            return {pos, PARSE_INVALID_VALUE};
        p += (buffer[p] == '-');
        if (buffer[p] == '0')
            p++;
        else
        {
            if (!ISDIGIT_NON_ZERO(buffer[p]))
                return {pos, PARSE_INVALID_VALUE};
            while (ISDIGIT(buffer[p]))
                p++;
        }
        if (buffer[p] == '.')
        {
            p++;
            if (!ISDIGIT(buffer[p]))
                return {pos, PARSE_INVALID_VALUE};
            while (ISDIGIT(buffer[p]))
                p++;
        }
        if (buffer[p] == 'e' || buffer[p] == 'E')
        {
            p++;
            if (buffer[p] == '+' || buffer[p] == '-')
                p++;
            if (!ISDIGIT(buffer[p]))
                return {pos, PARSE_INVALID_VALUE};
            while (ISDIGIT(buffer[p]))
                p++;
        }
        errno = 0;
        node->num = strtod((const char *)&buffer[pos], NULL);
        if (errno == ERANGE &&
            (node->num == HUGE_VAL || node->num == -HUGE_VAL))
            return {pos, PARSE_NUMBER_TOO_BIG};
        node->valueType = NUM_TYPE;
        pos = p;
        return {0, PARSE_SUCCESS};
    }

    JsonParseErr parseHex4(uint64_t &u)
    {
        u = 0;
        size_t pos0 = pos;
        for (; pos < pos0 + 4; pos++)
        {
            u <<= 4;
            if (buffer[pos] >= '0' && buffer[pos] <= '9')
                u |= buffer[pos] - '0';
            else if (buffer[pos] >= 'A' && buffer[pos] <= 'F')
                u |= buffer[pos] - 'A' + 10;
            else if (buffer[pos] >= 'a' && buffer[pos] <= 'f')
                u |= buffer[pos] - 'a' + 10;
            else
                return {pos, PARSE_INVALID_UNICODE_HEX};
        }
        pos--;
        return {0, PARSE_SUCCESS};
    }

    JsonParseErr encodeUTF8(uint64_t u, std::string &s)
    {
        if (u > 0x10FFFF)
            return {pos, PARSE_INVALID_UNICODE_HEX};
        if (u <= 0x7F)
            s.push_back(u & 0xFF);
        else if (u <= 0x7FF)
        {
            s.push_back(0xC0 | ((u >> 6) & 0xFF));
            s.push_back(0x80 | (u & 0x3F));
        }
        else if (u <= 0xFFFF)
        {
            s.push_back(0xE0 | ((u >> 12) & 0xFF));
            s.push_back(0x80 | ((u >> 6) & 0x3F));
            s.push_back(0x80 | (u & 0x3F));
        }
        else
        {
            s.push_back(0xF0 | ((u >> 18) & 0xFF));
            s.push_back(0x80 | ((u >> 12) & 0x3F));
            s.push_back(0x80 | ((u >> 6) & 0x3F));
            s.push_back(0x80 | (u & 0x3F));
        }
        return {0, PARSE_SUCCESS};
    }

    JsonParseErr parseString(std::string &s)
    {
        s = std::string();
        pos++;
        while (buffer[pos] != '\"')
        {
            uint64_t u, l;
            JsonParseErr err;
            switch (buffer[pos])
            {
            case '\\':
                switch (buffer[++pos])
                {
                case '\"':
                case '\\':
                case '/':
                    s.push_back(buffer[pos]);
                    break;
                case 'b':
                    s.push_back('\b');
                    break;
                case 'f':
                    s.push_back('\f');
                    break;
                case 'n':
                    s.push_back('\n');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'u':
                    pos++;
                    err = parseHex4(u);
                    if (err.state != PARSE_SUCCESS)
                        return err;
                    if (u >= 0xD800 && u <= 0xDBFF)
                    {
                        if (buffer[pos + 1] != '\\' ||
                            buffer[pos + 2] != 'u')
                            return {pos, PARSE_INVALID_UNICODE_SURROGATE};
                        pos += 2;
                        err = parseHex4(l);
                        if (err.state != PARSE_SUCCESS)
                            return err;
                        if (l < 0xDC00 || l > 0xDFFF)
                            return {pos, PARSE_INVALID_UNICODE_SURROGATE};
                        u = (((u - 0xD800) << 10) | (l - 0xDC00)) +
                            0x10000;
                    }
                    err = encodeUTF8(u, s);
                    if (err.state != PARSE_SUCCESS)
                        return err;
                    break;
                default:
                    return {pos, PARSE_INVALID_STRING_ESCAPE};
                }
                break;

            case '\0':
                return {pos, PARSE_MISS_QUATATION_MARK};
            default:
                if ((unsigned char)buffer[pos] < 0x20)
                    return {pos, PARSE_INVALID_STRING_CHAR};
                s.push_back(buffer[pos]);
            }
            pos++;
        }
        pos++;
        return {0, PARSE_SUCCESS};
    }

    JsonParseErr parseSpace()
    {
        while (buffer[pos] == ' ' || buffer[pos] == '\t' ||
               buffer[pos] == '\n' || buffer[pos] == '\r')
            pos++;
        return {0, PARSE_SUCCESS};
    }

    JsonParseErr parseBool(JsonNode *node)
    {
        if (node == nullptr)
            return {0, PARSE_NULL_PTR_ERROR};
        if (buffer[pos] == 't' && buffer[pos + 1] == 'r' &&
            buffer[pos + 2] == 'u' && buffer[pos + 3] == 'e')
        {
            pos += 4;
            node->valueType = BOOL_TYPE;
            node->boo = true;
            return {0, PARSE_SUCCESS};
        }
        if (buffer[pos] == 'f' && buffer[pos + 1] == 'a' &&
            buffer[pos + 2] == 'l' && buffer[pos + 3] == 's' &&
            buffer[pos + 4] == 'e')
        {
            pos += 5;
            node->valueType = BOOL_TYPE;
            node->boo = false;
            return {0, PARSE_SUCCESS};
        }
        return {pos, PARSE_INVALID_VALUE};
    }

    JsonParseErr parseNull(JsonNode *node)
    {
        if (node == nullptr)
            return {0, PARSE_NULL_PTR_ERROR};
        if (buffer[pos] == 'n' && buffer[pos + 1] == 'u' &&
            buffer[pos + 2] == 'l' && buffer[pos + 3] == 'l')
        {
            pos += 4;
            node->valueType = NULL_TYPE;
            node->boo = true;
            return {0, PARSE_SUCCESS};
        }
        return {pos, PARSE_INVALID_VALUE};
    }
};

#endif // JSON_PARSER_HPP
