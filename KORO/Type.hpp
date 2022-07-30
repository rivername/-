#include "const.hpp"
#include <string>
#include <algorithm>

using std::string;

#define OPERATOR(_Return, _Operator) _Return operator ## _Operator(T _Right) { return this->raw _Operator _Right; }
#define TYPE_CONST(_Type)            public: _Type() : Type({}) {} _Type(const RawType value) : Type(value) {} private:

/*==================================
            タイプ親クラス
==================================*/
template <class T>
class Type
{
public:
    using RawType  = T;

public:
    Type(const T value) : raw(value)
    {
    }

public:
    operator T()
    {
        return this->raw;
    }

public:
    OPERATOR(T, +)
    OPERATOR(T, -)
    OPERATOR(T, *)
    OPERATOR(T, /)

    OPERATOR(T, +=)
    OPERATOR(T, -=)
    OPERATOR(T, *=)
    OPERATOR(T, /=)

    OPERATOR(bool, >)
    OPERATOR(bool, >=)
    OPERATOR(bool, <)
    OPERATOR(bool, <=)

    OPERATOR(bool, ==)
    OPERATOR(bool, !=)

public:
    virtual string ToString() = 0;

    void Clamp(T min, T max)
    {
        this->raw = std::clamp(this->raw, min, max);
    }

public:
    // 生データ
    T raw;
};

/*==================================
               string
==================================*/
class String : public Type<string>
{
    TYPE_CONST(String)

public:
    String(const char* string) : Type(string)
    {
    }

public:
    virtual string ToString() override
    {
        return this->raw;
    }

public:
    std::vector<RawType> Split(char ch)
    {
        std::vector<string> elems;

        string elem;

        for (char c : this->raw)
        {
            if (c == ch)
            {
                if (!elem.empty())
                {
                    elems.push_back(elem);
                }

                elem = "";
            }
            else
            {
                elem += c;
            }
        }

        if (!elem.empty())
        {
            elems.push_back(elem);
        }

        return elems;
    }

    size_t IndexOf(char ch)
    {
        std::string_view view = this->raw;
        return view.find(ch);
    }

    size_t LastIndexOf(char ch)
    {
        std::string_view view = this->raw;
        return view.find_last_of(ch);
    }

    RawType SubString(size_t offset)
    {
        std::string_view view = this->raw;
        return view.substr(offset).data();
    }
};

/*==================================
               float
==================================*/
class Float : public Type<float>
{
    TYPE_CONST(Float)

public:
    virtual string ToString() override
    {
        return std::format("{}", this->raw);
    }
};

/*==================================
             D3DXVECTOR3
==================================*/
class Vec3 : public Type<D3DXVECTOR3>
{
    TYPE_CONST(Vec3)

public:
    Vec3(Float x, Float y, Float z) : Type(RawType(x, y, z))
    {
    }

public:
    Vec3 Normalized()
    {
        D3DXVECTOR3 n;
        D3DXVec3Normalize(&n, &this->raw);
        return n;
    }

public:
    Float Length()
    {
        return D3DXVec3Length(&this->raw);
    }

public:
    virtual string ToString() override
    {
        return std::format("( X : {}, Y : {}, Z : {} )", this->raw.x, this->raw.y, this->raw.z);
    }
};
