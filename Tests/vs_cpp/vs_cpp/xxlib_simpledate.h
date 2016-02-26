#pragma once
#include "xxlib_sb.h"	// todo: 未来 c++ 支持  是否 include 过的检测再做 define 控制代码开放. 当前先硬引用
#include "xxlib_bb.h"

namespace xxlib
{

    union SimpleDate
    {
        struct
        {
            unsigned short year;
            unsigned char month;
            unsigned char day;
            unsigned char hour;
            unsigned char minute;
            unsigned char second;
            unsigned char __dummy;    // 占位凑8
        };
        unsigned long long data;


        // 这些默认值是为了兼容 java 序列化
        SimpleDate()
            : year( 1991 )
            , month( 1 )
            , day( 1 )
            , hour( 0 )
            , minute( 0 )
            , second( 0 )
            , __dummy( 0 )
        {
        }

        SimpleDate( unsigned short year, unsigned char month, unsigned char day, unsigned char hour, unsigned char minute, unsigned char second )
            : year( year )
            , month( month )
            , day( day )
            , hour( hour )
            , minute( minute )
            , second( second )
            , __dummy( 0 )
        {
        }

        SimpleDate( SimpleDate const &other )
        {
            data = other.data;
        }

        inline SimpleDate& operator=( SimpleDate const &other )
        {
            data = other.data;
            return *this;
        }

    };
}

