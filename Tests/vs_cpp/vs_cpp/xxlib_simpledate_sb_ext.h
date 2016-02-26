#pragma once
#include "xxlib_sb.h"
#include "xxlib_simpledate.h"

namespace xxlib
{
    namespace ToString
    {
        inline size_t Calc( SimpleDate const &v )
        {
            return 19;
        }
        inline size_t Fill( char *dstBuf, SimpleDate const &v )
        {
            int n = v.year;
            dstBuf[ 3 ] = '0' + ( n - n / 10 * 10 );    n /= 10;
            dstBuf[ 2 ] = '0' + ( n - n / 10 * 10 );    n /= 10;
            dstBuf[ 1 ] = '0' + ( n - n / 10 * 10 );    n /= 10;
            dstBuf[ 0 ] = '0' + n;
            dstBuf[ 4 ] = '-';
            dstBuf[ 5 ] = '0' + ( v.month / 10 );
            dstBuf[ 6 ] = '0' + ( v.month - v.month / 10 * 10 );
            dstBuf[ 7 ] = '-';
            dstBuf[ 8 ] = '0' + ( v.day / 10 );
            dstBuf[ 9 ] = '0' + ( v.day - v.day / 10 * 10 );
            dstBuf[ 10 ] = ' ';
            dstBuf[ 11 ] = '0' + ( v.hour / 10 );
            dstBuf[ 12 ] = '0' + ( v.hour - v.hour / 10 * 10 );
            dstBuf[ 13 ] = ':';
            dstBuf[ 14 ] = '0' + ( v.minute / 10 );
            dstBuf[ 15 ] = '0' + ( v.minute - v.minute / 10 * 10 );
            dstBuf[ 16 ] = ':';
            dstBuf[ 17 ] = '0' + ( v.second / 10 );
            dstBuf[ 18 ] = '0' + ( v.second - v.second / 10 * 10 );
            return 19;
        }
        template<>
        struct CalcIsAccually<SimpleDate const>
        {
            static const bool value = true;
        };
    }
}
