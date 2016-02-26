#pragma once
#include "assert.h"
#include <string>
#include <algorithm>    // upper_bound

namespace xxlib
{
    namespace Utils
    {
        inline unsigned int GetHashCode(bool in) { return in ? 1 : 0; }
        inline unsigned int GetHashCode(char in) { return (unsigned int)in; }
        inline unsigned int GetHashCode(unsigned char in) { return (unsigned int)in; }
        inline unsigned int GetHashCode(signed char in) { return (unsigned char)in; }
        inline unsigned int GetHashCode(unsigned short in) { return (unsigned int)in; }
        inline unsigned int GetHashCode(short in) { return (unsigned short)in; }
        inline unsigned int GetHashCode(int in) { return (unsigned int)in; }
        inline unsigned int GetHashCode(unsigned int in) { return in; }
        inline unsigned int GetHashCode(unsigned long long in) { return (in & 0xFFFFFFFFu) ^ (in >> 32); }
        inline unsigned int GetHashCode(long long in) { return GetHashCode((unsigned long long)in); }
        inline unsigned int GetHashCode(float in) { return *(unsigned int*)(float*)&in; }
        inline unsigned int GetHashCode(double in) { return GetHashCode(*(unsigned long long*)(double*)&in); }
        // 经验数据. 经测试发现 x64 下 vc new 至少是 16 的倍数, x86 至少是 8 的倍数
        inline unsigned int GetHashCode(void* in) { return (unsigned int)((size_t)(void*)in / (sizeof(size_t) * 2)); }

        // !!! IMMPORTENT: the buf need align by 4
        inline unsigned int GetHashCode(unsigned char const *buf, size_t len)
        {
#ifndef XX_IA
            assert((size_t)buf % 4 == 0);
#endif
            if (!len)
            {
                return 0;
            }
            int n2 = 0x15051505, n1 = 0x15051505;
            unsigned int mod = len % 8, i = 0;
            for (; i < len - mod; i += 8)
            {
                n1 = (((n1 << 5) + n1) + (n1 >> 0x1b)) ^ *(int*)(buf + i);
                n2 = (((n2 << 5) + n2) + (n2 >> 0x1b)) ^ *(int*)(buf + i + 4);
            }
            if (mod > 4)
            {
                n1 = (((n1 << 5) + n1) + (n1 >> 0x1b)) ^ *(int*)(buf + i);
                n2 = (((n2 << 5) + n2) + (n2 >> 0x1b)) ^ (*(int*)(buf + i + 4) & (0xFFFFFFFF >> ((8 - mod) * 8)));
            }
            else if (mod)
            {
                n1 = (((n1 << 5) + n1) + (n1 >> 0x1b)) ^ (*(int*)(buf + i) & (0xFFFFFFFF >> ((4 - mod) * 8)));
            }
            return n2 + n1 * 0x5d588b65;
        }

        template<typename T>
        unsigned int GetHashCode(T *in) { return GetHashCode(*in); };

        template<typename T>
        unsigned int GetHashCode(T const &in) { return GetHashCode((unsigned char*)&in, sizeof(in)); };

        inline unsigned int GetHashCode(std::string const &in)
        {
            return in.size() == 0 ? 0 : GetHashCode((unsigned char*)in.data(), in.size());
        }



    }
}
