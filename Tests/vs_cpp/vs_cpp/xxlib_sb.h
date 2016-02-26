#pragma once
#include "xxlib_utils.h"
#include "xxlib_sb_utils.h"
#include <stddef.h>                 // ptrdiff_t, size_t
#include <string.h>                 // memcpy
#include <utility>                  // std::pair
#include <type_traits>              // std::enable_if
#include <string>                   // std::string
#include <stdexcept>                // std::invalid_argument
#include <iostream>                 // std::cout


/********************************************************/
// 兼顾性能和安全的 Format 最佳实践:
/********************************************************/
/*
...
char buf[ 4096 ];
StringBuilder sb( buf, 4096, 0, true );
...
sb.AppendFormat( ".......{0}.......{0}.......{1}......", ......... );
...
std::cout << sb.C_str() << endl;

*/




/********************************************************/
// StringBuilder 接口需求:
/********************************************************/
//namespace xxlib
//{
//namespace ToString
//{
//// 返回 v ToString 后的最大长估计值( 一定不能小于实际长 ), 
//// 如果是精确长度 还要写  template<> struct CalcIsAccually<T const> { static const bool value = true; };
//size_t Calc( T const &v );

//// 向 dstBuf 中填充 v 的 ToString 的结果, 并返回结果实际长度
//size_t Fill( char *dstBuf, T const &v );
//}
//}
/********************************************************/



namespace xxlib
{
    namespace ToString
    {
        /*
        signed char
        short
        int                     // size_t               in MSVC  32 bit
        long long               // ptrdiff_t            in MSVC  64 bit
        unsigned char
        unsigned short
        unsigned int            // size_t               in MSVC  32 bit
        unsigned long long      // size_t               in MSVC  64 bit

        long                    // ptrdiff_t / ssize_t  in GCC 32/64 bit
        unsigned long           // size_t               in GCC 32/64 bit

        bool
        char
        char*
        float
        double

        template<size_t len>
        char const( &s )[ len ]

        */

        // todo: 其他数组版本?

        // todo: wchar_t  wchar_t*  utf8 ??



        // 对于 Calc 返回值是精确的类型, 用这个模板来特化. ( 主要是 insert 操作受此影响 )
        template<typename T>
        struct CalcIsAccually
        {
            static const bool value = false;
        };

        // integers

        size_t Calc(signed char const &v);
        size_t Fill(char *dstBuf, signed char const &v);

        size_t Calc(short const &v);
        size_t Fill(char *dstBuf, short const &v);

        size_t Calc(int const &v);
        size_t Fill(char *dstBuf, int const &v);

        size_t Calc(long long const &v);
        size_t Fill(char *dstBuf, long long const &v);

        size_t Calc(long const &v);
        size_t Fill(char *dstBuf, long const &v);



        // unsigned integers

        size_t Calc(unsigned char const &v);
        size_t Fill(char *dstBuf, unsigned char const &v);

        size_t Calc(unsigned short const &v);
        size_t Fill(char *dstBuf, unsigned short const &v);

        size_t Calc(unsigned int const &v);
        size_t Fill(char *dstBuf, unsigned int const &v);

        size_t Calc(unsigned long long const &v);
        size_t Fill(char *dstBuf, unsigned long long const &v);


        size_t Calc(unsigned long const &v);
        size_t Fill(char *dstBuf, unsigned long const &v);



        // bool, char, char*

        size_t Calc(bool const &v);
        size_t Fill(char *dstBuf, bool const &v);

        size_t Calc(char const &v);
        size_t Fill(char *dstBuf, char const &v);

        size_t Calc(char const* const &v);
        size_t Fill(char *dstBuf, char const* const &v);



        // float, double

        size_t Calc(float const &v);
        size_t Fill(char *dstBuf, float const &v);

        size_t Calc(double const &v);
        size_t Fill(char *dstBuf, double const &v);



        // enum
        template<typename ET>
        size_t Calc(ET const &v)
        {
            return Calc((typename std::underlying_type<ET>::type)v);
        }
        template<typename ET>
        size_t Fill(char *dstBuf, ET const &v)
        {
            return Fill(dstBuf, (typename std::underlying_type<ET>::type)v);
        }



        // 定长数组

        template<size_t len>
        size_t Calc(char const(&v)[len])
        {
            static_assert(len > 0, "must be a literal string");
            return len - 1;
        }
        template<size_t len>
        size_t Fill(char *dstBuf, char const(&v)[len])
        {
            static_assert(len > 0, "must be a literal string");
            memcpy(dstBuf, v, len - 1);
            return len - 1;
        }



        // 定长数组长度是精确的
        template<size_t len>
        struct CalcIsAccually<char const(&)[len]>
        {
            static const bool value = true;
        };

        // 单个字符长度是精确的
        template<>
        struct CalcIsAccually<char const>
        {
            static const bool value = true;
        };

        // bool 长度是精确的
        template<>
        struct CalcIsAccually<bool const>
        {
            static const bool value = true;
        };

        // c 字串 长度是精确的( 因为使用的 strlen 返回值 )
        template<>
        struct CalcIsAccually<char const*>
        {
            static const bool value = true;
        };

    }
}









namespace xxlib
{
    namespace ToString
    {
        size_t Calc(std::string const &v);
        size_t Fill(char *dstBuf, std::string const &v);

        template<>
        struct CalcIsAccually<std::string>
        {
            static const bool value = true;
        };
    }
}










namespace xxlib
{
    struct StringBuilder;

    namespace ToString
    {

        template<>
        struct CalcIsAccually<StringBuilder>
        {
            static const bool value = true;
        };
        
        size_t Calc(StringBuilder const &v);
        size_t Fill(char *dstBuf, StringBuilder const &v);
    }
}









namespace xxlib
{
    struct StringBuilder
    {
        char   *buf;
        size_t  bufLen;
        size_t  dataLen;
        bool    isRef;      // 是否为引用模式( 并非 SB 自己管理当前 buf 内存 )



        // 只是为了能 for( auto c : 
        struct Iter
        {
            char *ptr;
            bool operator!=(Iter const& other) { return ptr != other.ptr; }
            Iter& operator++()
            {
                ++ptr;
                return *this;
            }
            char& operator*() { return *ptr; }
        };
        Iter begin()
        {
            return Iter{ buf };
        }
        Iter end()
        {
            return Iter{ &buf[dataLen] };
        }



        StringBuilder();                                                        // Reserve( 64 )
        explicit StringBuilder(size_t capacity);                                // Reserve( capacity )
        StringBuilder(char *buf, size_t bufLen, size_t dataLen, bool isRef);    // ref or copy
        StringBuilder(char *s, bool isRef = false);                             // ref or copy
        //StringBuilder(std::string s);                                         // copy

        StringBuilder(StringBuilder const& o);                                  // copy
        StringBuilder(StringBuilder&& o);                                       // move
        StringBuilder& operator=(StringBuilder const& o);                       // copy
        StringBuilder& operator=(StringBuilder&& o);                            // move
        ~StringBuilder();                                                       // free buf if needed

        void Assign(char *buf, size_t bufLen, size_t dataLen, bool isRef);      // ref or copy
        void Reserve(size_t capacity);                                          // prepare memory( capacity )
        void Resize(size_t len);									            // cut or grow( do not fill 0 )
        void Clear();                                                           // dataLen = 0
        char* C_str();                                                          // buf[ dataLen ] = 0; return buf

        char operator[] (size_t idx) const;                                     // return buf[ idx ]
        char& operator[] (size_t idx);                                          // return buf[ idx ]

        char At(size_t idx) const;                                         // return buf[ idx ]
        char& At(size_t idx);                                              // return buf[ idx ]

        void Pop();
        char& Top();
        char const& Top() const;
        size_t Size() const;
        bool Empty() const;


        void ToLower();
        void ToUpper();
        void ToUpperFirstChar();

        void Insert(size_t idx, char const *s, size_t sLen = 0);

        template<typename T>
        void Insert(size_t idx, T const &v);


        template<typename ...TS>
        void Append(TS const & ...vs);

        template<typename...TS>
        void AppendFormat( char const *format, TS const &...vs );

        template<typename T>
        StringBuilder& operator<<( T const& v );

        bool operator==(StringBuilder const &other) const;
        bool operator!=(StringBuilder const &other) const;
        bool operator<(StringBuilder const &other) const;
        bool operator>(StringBuilder const &other) const;
        bool operator<=(StringBuilder const &other) const;
        bool operator>=(StringBuilder const &other) const;

        bool operator==(char const* s) const;

        template<size_t sLen>
        bool operator==(char const(&s)[sLen]) const;

    private:
        template<typename T>
        void AppendFormatCore(StringBuilder &s, size_t &n, T const &v);

        template<typename T, typename...TS>
        void AppendFormatCore(StringBuilder &s, size_t &n, T const &v, TS const &...vs);

        template<typename T>
        typename std::enable_if<ToString::CalcIsAccually<T>::value, void>::type InsertCore(size_t idx, T const &v);

        template<typename T>
        typename std::enable_if<!ToString::CalcIsAccually<T>::value, void>::type InsertCore(size_t idx, T const &v);

    };
}





















namespace xxlib
{
    namespace ToString
    {
        template<typename T, typename ...TS>
        size_t Calc(T const &v, TS const &...vs)
        {
            return Calc(v) + Calc(vs...);
        }

        template<typename T, typename ...TS>
        size_t Fill(char *dstBuf, T const &v, TS const &...vs)
        {
            auto offset = Fill(dstBuf, v);
            return offset + Fill(dstBuf + offset, vs...);
        }
    }


    template<typename T>
    typename std::enable_if<ToString::CalcIsAccually<T>::value, void>::type StringBuilder::InsertCore(size_t idx, T const &v)
    {
        auto len = ToString::Calc(v);   // 这个就是精确的长度
        if (!len)
        {
            return;
        }
        if (dataLen + len > bufLen)
        {
            Reserve(dataLen + len);
        }
        memmove(buf + idx + len, buf + idx, dataLen - idx);
        ToString::Fill(buf + idx, v);
        dataLen += len;
    }

    template<typename T>
    typename std::enable_if<!ToString::CalcIsAccually<T>::value, void>::type StringBuilder::InsertCore(size_t idx, T const &v)
    {
        auto len = ToString::Calc(v);   // 这只是估计值
        if (dataLen + len > bufLen)
        {
            Reserve(dataLen + len);
        }
        char *tmpBuf = (char*)malloc(len);
        len = ToString::Fill(tmpBuf, v);        // 填充并得到实际的 ToString 后的长度
        memmove(buf + idx + len, buf + idx, dataLen - idx);
        memcpy(buf + idx, tmpBuf, len);
        free(tmpBuf);

        dataLen += len;
    }

    template<typename T>
    void StringBuilder::Insert(size_t idx, T const &v)
    {
        if (idx > dataLen)
        {
            idx = dataLen;
        }
        InsertCore(idx, v);
    }


    template<typename ...TS>
    void StringBuilder::Append(TS const & ...vs)
    {
        size_t maxLen = ToString::Calc(vs...);
        if (dataLen + maxLen > bufLen)
        {
            Reserve(dataLen + maxLen);
        }
        dataLen += ToString::Fill(buf + dataLen, vs...);
    }

    template<typename T>
    StringBuilder& StringBuilder::operator<<(T const& v)
    {
        Append(v);
        return *this;
    }


    template<typename T>
    void StringBuilder::AppendFormatCore(StringBuilder &s, size_t &n, T const &v)
    {
        if (!n) s.Append(v);
    }

    template<typename T, typename...TS>
    void StringBuilder::AppendFormatCore(StringBuilder &s, size_t &n, T const &v, TS const &...vs)
    {
        if (n-- == 0)
        {
            s.Append(v);
            return;
        }
        AppendFormatCore(s, n, vs...);
    }

    template<typename...TS>
    void StringBuilder::AppendFormat(char const* format, TS const&...vs)
    {
        struct flag
        {
            size_t idx, len;
        };
        flag flags[sizeof...(vs)];
        memset(flags, 0, sizeof(flag)*sizeof...(vs));

#ifdef _MSC_VER
        __declspec(align(8)) char numBuf[32];
#else
        char numBuf[32] __attribute__((aligned(8)));
#endif
        StringBuilder numStr(numBuf, 32, 0, true);

        size_t offset = 0, i = 0, n = 0;
        while (auto c = format[offset])
        {
            if (c == '{')
            {
                c = format[++offset];
                if (c == '{')
                {
                    Append('{');
                }
                else
                {
                    while ((c = format[offset]))
                    {
                        if (c == '}')
                        {
                            numStr.C_str();				// for terminate that FromString is need it
                            Utils::FromString(i, numBuf);
                            numStr.Clear();
                            if (i < 0 || i >= sizeof...(vs))
                            {
                                throw std::invalid_argument("argument out of range.");
                            }
                            if (flags[i].len)
                            {
                                if (dataLen + flags[i].len > bufLen)
                                {
                                    Reserve(dataLen + flags[i].len);
                                }
                                memcpy(buf + dataLen, buf + flags[i].idx, flags[i].len);
                                dataLen += flags[i].len;
                            }
                            else
                            {
                                flags[i].idx = dataLen;
                                n = i;
                                AppendFormatCore(*this, n, vs...);
                                flags[i].len = dataLen - flags[i].idx;
                            }

                            break;
                        }
                        else
                        {
                            numStr.Append(c);
                        }
                        ++offset;
                    }
                }
            }
            else
            {
                Append(c);
            }
            ++offset;
        }
    }



    template<size_t sLen>
    bool StringBuilder::operator==(char const(&s)[sLen]) const
    {
        if (sLen != dataLen) return false;
        return memcmp(buf, s, dataLen) == 0;
    }







    /************************************************************************/
    /* Cout / CoutPos                                                       */
    /************************************************************************/

    // 一组简单的输出函数
    template<typename...TS>
    void Cout(TS const&...ps)
    {
        char buf[16384];
        StringBuilder sb(buf, _countof(buf), 0, true);
        sb.Append(ps...);
        Utils::ConsoleWrite(sb.buf, sb.dataLen);
    }

    template<typename...TS>
    void CoutPos(int x, int y, TS const&...ps)
    {
        xxlib::Utils::SetConsoleCursorPosition(x, y);
        Cout(ps...);
    }

}
