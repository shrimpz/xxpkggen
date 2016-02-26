#include "xxlib_bb_utils.h"

namespace xxlib
{
    namespace Utils
    {
        // negative -> ZigZag positive
        // 效果：负数变正，正数 *= 2
        unsigned int ZigZagEncode(int const &in)
        {
            return (in << 1) ^ (in >> 31);
        }
        unsigned long long ZigZagEncode64(long long const &in)
        {
            return (in << 1) ^ (in >> 63);
        }

        // ZigZag positive -> negative
        int ZigZagDecode(unsigned int const &in)
        {
            return (int)(in >> 1) ^ (-(int)(in & 1));
        }
        long long ZigZagDecode64(unsigned long long const &in)
        {
            return (long long)(in >> 1) ^ (-(long long)(in & 1));
        }


        // unsigned int
        void VarWrite7(char * const &dstBuf, size_t &offset, unsigned int in)
        {
        Lab1:
            auto b7 = (unsigned char)in;
            if ((in >>= 7))
            {
                dstBuf[offset++] = b7 | 0x80u;
                goto Lab1;
            }
            dstBuf[offset++] = b7;
        }
        bool VarRead7(char const * const &srcBuf, size_t dataLen, size_t& offset, unsigned int &out)
        {
            if (offset >= dataLen)
            {
                return false;// ReadStatus::NotEnoughData;
            }
            auto p = srcBuf + offset;
            unsigned int i = 0, b7;
            int lshift = 0;
            size_t idx = 0;
            dataLen -= offset;
        Lab1:
            b7 = p[idx++];
            if (b7 > 0x7F)
            {
                if (idx == 5)
                {
                    return false;// ReadStatus::Overflow;
                }
                if (idx >= dataLen)
                {
                    return false;// ReadStatus::NotEnoughData;
                }
                i |= (b7 & 0x7F) << lshift;
                lshift += 7;
                goto Lab1;
            }
            else
            {
                if (idx == 5 && b7 > 15)
                {
                    return false;// ReadStatus::Overflow;
                }
                else
                {
                    i |= b7 << lshift;
                }
            }
            out = i;
            offset += idx;
            return true;// ReadStatus::Success;
        }


        // unsigned long long
        void VarWrite764(char * const &dstBuf, size_t &offset, unsigned long long in)
        {
            auto idx8 = offset + 8;
        Lab1:
            auto b7 = (unsigned char)in;
            if ((in >>= 7))
            {
                dstBuf[offset++] = b7 | 0x80u;
                if (offset == idx8)
                {
                    dstBuf[offset++] = (unsigned char)in;
                }
                else
                {
                    goto Lab1;
                }
            }
            else
            {
                dstBuf[offset++] = b7;
            }
        }
        bool VarRead764(char const * const &srcBuf, size_t dataLen, size_t& offset, unsigned long long &out)
        {
            if (offset >= dataLen)
            {
                return false;// ReadStatus::NotEnoughData;
            }
            auto p = srcBuf + offset;
            unsigned long long i = 0, b7;
            int lshift = 0;
            size_t idx = 0;
            dataLen -= offset;
        Lab1:
            b7 = p[idx++];
            if (b7 > 0x7F)
            {
                if (idx >= dataLen)
                {
                    return false;// ReadStatus::NotEnoughData;
                }
                if (idx < 8)
                {
                    i |= (b7 & 0x7F) << lshift;
                    lshift += 7;
                    goto Lab1;
                }
                else
                {
                    i |= (b7 << lshift) | ((unsigned long long)p[idx++] << 28 << 28);
                }
            }
            else
            {
                i |= b7 << lshift;
            }
            out = i;
            offset += idx;
            return true;// ReadStatus::Success;
        }

    }
}
