#pragma once
#include "xxlib_sb.h"
#include "xxlib_bb.h"

namespace xxlib
{
    namespace ToString
    {
        inline size_t Calc(ByteBuffer const &v)
        {
            return 100 + (v.dataLen / 16 + ((v.dataLen % 16) == 0 ? 0 : 1)) * 90;
        }


        inline void FillCore(char* dstBuf, size_t& offset, char const* buf, size_t len)
        {
            for (size_t i = 0; i < len; ++i)
            {
                auto c = (unsigned char)buf[i];
                if (c < 32 || c > 126)
                {
                    offset += Fill(dstBuf + offset, '.');
                }
                else offset += Fill(dstBuf + offset, (char)c);
            }
        }

        static char const Int2HexTable[] =
        {
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            'A', 'B', 'C', 'D', 'E', 'F'
        };

        inline size_t FillHex(char* dstBuf, unsigned char v)
        {
            dstBuf[0] = Int2HexTable[v >> 4];
            dstBuf[1] = Int2HexTable[v & 15];
            return 2;
        }

        inline size_t FillHex(char* dstBuf, unsigned short v)
        {
            FillHex(dstBuf, (unsigned char)(v >> 8));
            FillHex(dstBuf + 2, (unsigned char)v);
            return 4;
        }

        inline size_t FillHex(char* dstBuf, unsigned int v)
        {
            FillHex(dstBuf, (unsigned short)(v >> 16));
            FillHex(dstBuf + 4, (unsigned short)v);
            return 8;
        }

        inline size_t FillHex(char* dstBuf, unsigned long long v)
        {
            FillHex(dstBuf, (unsigned int)(v >> 32));
            FillHex(dstBuf + 8, (unsigned int)v);
            return 16;
        }


        inline size_t FillHex(char* dstBuf, signed char v)
        {
            return FillHex(dstBuf, (unsigned char)v);
        }

        inline size_t FillHex(char* dstBuf, char v)
        {
            return FillHex(dstBuf, (unsigned char)v);
        }

        inline size_t FillHex(char* dstBuf, short v)
        {
            return FillHex(dstBuf, (unsigned short)v);
        }

        inline size_t FillHex(char* dstBuf, int v)
        {
            return FillHex(dstBuf, (unsigned int)v);
        }

        inline size_t FillHex(char* dstBuf, long long v)
        {
            return FillHex(dstBuf, (unsigned long long)v);
        }

        inline size_t FillHex(char* dstBuf, long v)
        {
#ifdef _MSC_VER
            return FillHex(dstBuf, (int)v);
#else
#   if defined(__LP64__) || __WORDSIZE == 64 || defined(_WIN64) || defined(_M_X64)
            return FillHex(dstBuf, (int)v);
#   else
            return FillHex(dstBuf, (long long)v);
#   endif
#endif
        }

        inline size_t FillHex(char* dstBuf, unsigned long v)
        {
#ifdef _MSC_VER
            return FillHex(dstBuf, (unsigned int)v);
#else
#   if defined(__LP64__) || __WORDSIZE == 64 || defined(_WIN64) || defined(_M_X64)
            return FillHex(dstBuf, (unsigned int)v);
#   else
            return FillHex(dstBuf, (unsigned long long)v);
#   endif
#endif
        }









        inline size_t Fill(char* dstBuf, ByteBuffer const& v)
        {
            if (v.dataLen == 0)
            {
                return 0;
            }
            size_t len = v.dataLen;
            size_t offset = 0;
            offset += Fill(dstBuf + offset, "--------  0  1  2  3 | 4  5  6  7 | 8  9  A  B | C  D  E  F   dataLen = ");
            offset += Fill(dstBuf + offset, v.dataLen);
            size_t i = 0;
            for (; i < len; ++i)
            {
                if ((i % 16) == 0)
                {
                    if (i)
                    {           // output ascii to the end of the line
                        offset += Fill(dstBuf + offset, "  ");
                        FillCore(dstBuf, offset, v.buf + i - 16, 16);
                    }
                    offset += Fill(dstBuf + offset, '\n');
                    offset += FillHex(dstBuf + offset, (unsigned int)i);
                    offset += Fill(dstBuf + offset, "  ");
                }
                else if (i && (i % 4 == 0))
                {
                    offset += Fill(dstBuf + offset, "  ");
                }
                else
                {
                    offset += Fill(dstBuf + offset, ' ');
                }
                offset += FillHex(dstBuf + offset, (unsigned char)v.buf[i]);
            }
            size_t left = i % 16;
            if (left == 0)
            {
                left = 16;
            }
            if (left)
            {
                len = len + 16 - left;
                for (; i < len; ++i)
                {
                    if (i && (i % 4 == 0))
                    {
                        offset += Fill(dstBuf + offset, "  ");
                    }
                    else
                    {
                        offset += Fill(dstBuf + offset, ' ');
                    }
                    offset += Fill(dstBuf + offset, "  ");
                }
                offset += Fill(dstBuf + offset, "  ");
                FillCore(dstBuf, offset, v.buf + i - 16, left);
            }
            return offset;
        }

    }
}
