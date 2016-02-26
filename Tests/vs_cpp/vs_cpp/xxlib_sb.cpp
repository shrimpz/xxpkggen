#include "xxlib_sb.h"
#include "xxlib_utils.h"
#include <memory>       // malloc
#include <assert.h>




namespace xxlib
{
    namespace ToString
    {
        // integers

        size_t Calc(signed char const &v)
        {
            return 4;
        }
        size_t Fill(char *dstBuf, signed char const &v)
        {
            return ::xxlib::Utils::i32toa_branchlut((int)v, dstBuf);
        }

        size_t Calc(short const &v)
        {
            return 6;
        }
        size_t Fill(char *dstBuf, short const &v)
        {
            return ::xxlib::Utils::i32toa_branchlut((int)v, dstBuf);
        }

        size_t Calc(int const &v)
        {
            return 11;
        }
        size_t Fill(char *dstBuf, int const &v)
        {
            return ::xxlib::Utils::i32toa_branchlut(v, dstBuf);
        }

        size_t Calc(long long const &v)
        {
            return 20;
        }
        size_t Fill(char *dstBuf, long long const &v)
        {
            return ::xxlib::Utils::i64toa_branchlut(v, dstBuf);
        }

        size_t Calc(long const &v)
        {
#ifdef _MSC_VER
            return 11;
#else
# ifdef XXLIB_64BIT
            return 20;
# else
            return 11;
# endif
#endif
        }
        size_t Fill(char *dstBuf, long const &v)
        {
#ifdef _MSC_VER
            return Fill(dstBuf, (int)v);
#else
# ifdef XXLIB_64BIT
            return Fill(dstBuf, (long long)v);
# else
            return Fill(dstBuf, (int)v);
# endif
#endif
        }



        // unsigned integers

        size_t Calc(unsigned char const &v)
        {
            return 3;
        }
        size_t Fill(char *dstBuf, unsigned char const &v)
        {
            return ::xxlib::Utils::u32toa_branchlut(v, dstBuf);
        }

        size_t Calc(unsigned short const &v)
        {
            return 5;
        }
        size_t Fill(char *dstBuf, unsigned short const &v)
        {
            return ::xxlib::Utils::u32toa_branchlut(v, dstBuf);
        }

        size_t Calc(unsigned int const &v)
        {
            return 10;
        }
        size_t Fill(char *dstBuf, unsigned int const &v)
        {
            return ::xxlib::Utils::u32toa_branchlut(v, dstBuf);
        }

        size_t Calc(unsigned long long const &v)
        {
            return 19;
        }
        size_t Fill(char *dstBuf, unsigned long long const &v)
        {
            return ::xxlib::Utils::u64toa_branchlut(v, dstBuf);
        }


        size_t Calc(unsigned long const &v)
        {
#ifdef _MSC_VER
            return 10;
#else
# ifdef XXLIB_64BIT
            return 19;
# else
            return 10;
# endif
#endif
        }
        size_t Fill(char *dstBuf, unsigned long const &v)
        {
#ifdef _MSC_VER
            return Fill(dstBuf, (unsigned int)v);
#else
# ifdef XXLIB_64BIT
            return Fill(dstBuf, (unsigned long long)v);
# else
            return Fill(dstBuf, (unsigned int)v);
# endif
#endif
        }



        // bool, char, char*

        size_t Calc(bool const &v)
        {
            return v ? 4 : 5;
        }
        size_t Fill(char *dstBuf, bool const &v)
        {
            if (v)
            {
                dstBuf[0] = 't';
                dstBuf[1] = 'r';
                dstBuf[2] = 'u';
                dstBuf[3] = 'e';
                return 4;
            }
            else
            {
                dstBuf[0] = 'f';
                dstBuf[1] = 'a';
                dstBuf[2] = 'l';
                dstBuf[3] = 's';
                dstBuf[4] = 'e';
                return 5;
            }
        }

        size_t Calc(char const &v)
        {
            return 19;
        }
        size_t Fill(char *dstBuf, char const &v)
        {
            dstBuf[0] = v;
            return 1;
        }

        size_t Calc(char const* const &v)
        {
            return strlen(v);
        }
        size_t Fill(char *dstBuf, char const* const &v)
        {
            auto len = strlen(v);
            memcpy(dstBuf, v, len);
            return len;
        }


        // float, double

        size_t Calc(float const &v)
        {
            return 10;
        }
        size_t Fill(char *dstBuf, float const &v)
        {
            return ::xxlib::Utils::DiyFp::dtoa_milo(v, dstBuf);
        }

        size_t Calc(double const &v)
        {
            return 19;
        }
        size_t Fill(char *dstBuf, double const &v)
        {
            return ::xxlib::Utils::DiyFp::dtoa_milo(v, dstBuf);
        }


    }
}







namespace xxlib
{
    namespace ToString
    {

        size_t Calc(std::string const &v)
        {
            return v.size();
        }
        size_t Fill(char *dstBuf, std::string const &v)
        {
            memcpy(dstBuf, v.data(), v.size());
            return v.size();
        }

    }
}







namespace xxlib
{

    size_t ToString::Calc(StringBuilder const &v)
    {
        return v.dataLen;
    }

    size_t ToString::Fill(char *dstBuf, StringBuilder const &v)
    {
        memcpy(dstBuf, v.buf, v.dataLen);
        return v.dataLen;
    }






    StringBuilder::StringBuilder(size_t capacity)
        : dataLen(0)
        , isRef(false)
    {
        if (capacity < 64)
        {
            bufLen = 64;
        }
        else
        {
            bufLen = Utils::Round2n(capacity);
        }
        buf = (char*)malloc(bufLen);
    }

    StringBuilder::StringBuilder()
        : StringBuilder(64)
    {
    }

    StringBuilder::StringBuilder(char *buf_, size_t bufLen_, size_t dataLen_, bool isRef_)
    {
        dataLen = dataLen_;
        if (isRef_)
        {
            buf = buf_;
            bufLen = bufLen_;
            isRef = true;
        }
        else
        {
            bufLen = Utils::Round2n(dataLen_);
            buf = (char*)malloc(bufLen);
            isRef = false;
            memcpy(buf, buf_, dataLen_);
        }
    }

    StringBuilder::StringBuilder(char *s, bool isRef_)
    {
        dataLen = strlen(s);
        if (isRef)
        {
            buf = (char*)s;
            bufLen = dataLen + 1;
            isRef = true;
        }
        else
        {
            bufLen = Utils::Round2n(dataLen + 1);
            buf = (char*)malloc(bufLen);
            isRef = false;
            memcpy(buf, s, dataLen);
        }
    }

    StringBuilder::StringBuilder(StringBuilder const& o)
        : StringBuilder(o.dataLen)
    {
        memcpy(buf, o.buf, o.dataLen);
        dataLen = o.dataLen;
    }

    StringBuilder::StringBuilder(StringBuilder&& o)
        : buf(o.buf)
        , bufLen(o.bufLen)
        , dataLen(o.dataLen)
        , isRef(o.isRef)
    {
        o.buf = nullptr;
    }

    StringBuilder& StringBuilder::operator=(StringBuilder const& o)
    {
        if (this == &o) return *this;
        dataLen = o.dataLen;
        if (bufLen >= o.dataLen)
        {
            memcpy(buf, o.buf, o.dataLen);
        }
        else
        {
            if (!isRef)
            {
                free(buf);
            }
            else
            {
                isRef = false;
            }
            bufLen = Utils::Round2n(o.dataLen);
            buf = (char*)malloc(bufLen);
            memcpy(buf, o.buf, o.dataLen);
        }
        return *this;
    }

    StringBuilder& StringBuilder::operator=(StringBuilder&& o)
    {
        if (!isRef)
        {
            free(buf);
        }
        buf = o.buf;
        bufLen = o.bufLen;
        dataLen = o.dataLen;
        isRef = o.isRef;
        o.buf = nullptr;
        return *this;
    }

    StringBuilder::~StringBuilder()
    {
        if (buf && !isRef)
        {
            free(buf);
        }
    }

    char* StringBuilder::C_str()
    {
        if (dataLen == bufLen)
        {
            Reserve(dataLen + 1);
        }
        buf[dataLen] = '\0';
        return buf;
    }

    void StringBuilder::Reserve(size_t capacity)
    {
        if (capacity <= bufLen) return;
        bufLen = Utils::Round2n(capacity);
        auto newBuf = (char*)malloc(capacity);
        memcpy(newBuf, buf, dataLen);
        if (!isRef)
        {
            free(buf);
        }
        else
        {
            isRef = false;
        }
        buf = newBuf;
    }


    void StringBuilder::Resize(size_t len)
    {
        if (len == dataLen)
        {
            return;
        }
        else if (len < dataLen)
        {
            dataLen = len;
        }
        else
        {
            Reserve(len);
            dataLen = len;
        }
    }

    void StringBuilder::Pop()
    {
        assert(dataLen > 0);
        --dataLen;
    }

    char& StringBuilder::Top()
    {
        assert(dataLen > 0);
        return buf[dataLen - 1];
    }

    char const& StringBuilder::Top() const
    {
        assert(dataLen > 0);
        return buf[dataLen - 1];
    }

    void StringBuilder::Clear()
    {
        dataLen = 0;
    }

    size_t StringBuilder::Size() const
    {
        return dataLen;
    }

    bool StringBuilder::Empty() const
    {
        return dataLen == 0;
    }

    void StringBuilder::Insert(size_t idx, char const* s, size_t sLen)
    {
        if (!sLen)
        {
            sLen = strlen(s);
            if (!sLen)
            {
                return;
            }
        }
        if (idx > dataLen)
        {
            idx = dataLen;
        }
        if (dataLen + sLen > bufLen)
        {
            Reserve(dataLen + sLen);
        }
        memmove(buf + idx + sLen, buf + idx, dataLen - idx);
        memcpy(buf + idx, s, sLen);
        dataLen += sLen;
    }

    char StringBuilder::operator[](size_t idx) const
    {
        assert(idx < dataLen);
        return buf[idx];
    }

    char& StringBuilder::operator[](size_t idx)
    {
        assert(idx < dataLen);
        return buf[idx];
    }

    char StringBuilder::At(size_t idx) const
    {
        assert(idx < dataLen);
        return buf[idx];
    }

    char& StringBuilder::At(size_t idx)
    {
        assert(idx < dataLen);
        return buf[idx];
    }

    void StringBuilder::Assign(char *buf_, size_t bufLen_, size_t dataLen_, bool isRef_)
    {
        assert(buf != buf_);
        if (isRef_)
        {
            if (!isRef)
            {
                free(this->buf);
                isRef = true;
            }
            buf = buf_;
            bufLen = bufLen_;
            dataLen = dataLen_;
            return;
        }
        else if (bufLen <= dataLen_)
        {
            if (!this->isRef)
            {
                free(this->buf);
            }
            else
            {
                this->isRef = false;
            }
            bufLen = Utils::Round2n(dataLen_);
            buf = (char*)malloc(bufLen);
        }
        dataLen = dataLen_;
        memcpy(buf, buf_, dataLen_);
    }




    static unsigned char const lowerchars[] =
    {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
        27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106,
        107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 91, 92, 93, 94, 95,
        96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116,
        117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136,
        137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156,
        157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
        177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196,
        197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216,
        217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236,
        237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
    };
    static unsigned char const upperchars[] =
    {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
        27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
        77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 65, 66, 67, 68, 69,
        70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 123, 124, 125,
        126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145,
        146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165,
        166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185,
        186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205,
        206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225,
        226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245,
        246, 247, 248, 249, 250, 251, 252, 253, 254, 255
    };

    void StringBuilder::ToLower()
    {
        for (size_t i = 0; i < dataLen; ++i)
        {
            buf[i] = lowerchars[buf[i]];
        }
    }

    void StringBuilder::ToUpper()
    {
        for (size_t i = 0; i < dataLen; ++i)
        {
            buf[i] = upperchars[buf[i]];
        }
    }

    void StringBuilder::ToUpperFirstChar()
    {
        if (dataLen)
        {
            buf[0] = upperchars[buf[0]];
        }
    }



    bool StringBuilder::operator==(char const* o) const
    {
        if (!o) return false;
        return memcmp(buf, o, dataLen) == 0;
    }

    bool StringBuilder::operator==(StringBuilder const& o) const
    {
        if (this == &o) return true;
        if (dataLen != o.dataLen) return false;
        return memcmp(buf, o.buf, dataLen) == 0;
    }

    bool StringBuilder::operator!=(StringBuilder const& o) const
    {
        return !operator==(o);
    }

    bool StringBuilder::operator<(StringBuilder const& o) const
    {
        if (this == &o) return false;
        auto r = memcmp(buf, o.buf, dataLen < o.dataLen ? dataLen : o.dataLen);
        if (r == 0) return dataLen < o.dataLen;
        return r < 0;
    }

    bool StringBuilder::operator>(StringBuilder const& o) const
    {
        if (this == &o) return false;
        auto r = memcmp(buf, o.buf, dataLen < o.dataLen ? dataLen : o.dataLen);
        if (r == 0) return dataLen > o.dataLen;
        return r > 0;
    }

    bool StringBuilder::operator<=(StringBuilder const& o) const
    {
        return !operator>(o);
    }

    bool StringBuilder::operator>=(StringBuilder const& o) const
    {
        return !operator<(o);
    }

}
