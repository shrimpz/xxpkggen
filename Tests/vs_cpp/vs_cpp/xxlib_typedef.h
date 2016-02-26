#pragma once
#include <stddef.h>     // size_t, ptrdiff_t

// byte 同 c#, 为 无符号( java 中全是有符号数 )
// long 数据类型是不推荐使用的. gcc 下它的长度为 32 或 64bit,  vc 下它的长度为 32
// 关于 size_t:
// vc 下的 size_t 等同于 unsigned long long, 没有 ssize_t
// gcc 下的 size_t 等同于 gcc 的 unsigned long, ssize_t = long

typedef signed char         int8, sbyte, Int8, SByte, Sbyte;
typedef short               int16, Int16;
typedef int                 int32, Int32;
typedef long long           int64, Int64;
typedef unsigned char       uint8, byte, Byte, UInt8, Uint8;
typedef unsigned short      uint16, ushort, UInt16, Uint16, UShort, Ushort;
typedef unsigned int        uint32, uint, UInt32, Uint32, Uint, UInt;
typedef unsigned long long  uint64, UInt64, Uint64;
#ifdef _MSC_VER
typedef ptrdiff_t           ssize_t;
#endif
typedef long                Long;
typedef unsigned long       ulong, ULong, Ulong;
