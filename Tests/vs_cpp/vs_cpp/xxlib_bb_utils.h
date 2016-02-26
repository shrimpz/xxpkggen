#pragma once
#include <stddef.h>     // size_t

namespace xxlib
{
    namespace Utils
    {
        // negative -> ZigZag positive
        // 效果：负数变正，正数 *= 2
        unsigned int ZigZagEncode(int const &in);
        unsigned long long ZigZagEncode64(long long const &in);

        // ZigZag positive -> negative
        int ZigZagDecode(unsigned int const &in);
        long long ZigZagDecode64(unsigned long long const &in);

        // unsigned int 7bit变长读写
        void VarWrite7(char * const &dstBuf, size_t &offset, unsigned int in);
        bool VarRead7(char const * const &srcBuf, size_t dataLen, size_t& offset, unsigned int &out);

        // unsigned long long 变长读写
        void VarWrite764(char * const &dstBuf, size_t &offset, unsigned long long in);
        bool VarRead764(char const * const &srcBuf, size_t dataLen, size_t& offset, unsigned long long &out);

    }

}
