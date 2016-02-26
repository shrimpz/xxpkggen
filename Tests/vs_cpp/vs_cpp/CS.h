#pragma once

#include "xxlib_list_bb_ext.h"
#include "xxlib_simpledate_bb_ext.h"
#include "xxlib_sb_bb_ext.h"


// bytebuffer serial struct's base code
#define XX_SERIALABLE_STRUCT_BASE( CN )         \
CN() {}                                         \
CN( CN const& o ) = default;                    \
CN( CN && o ) = default;                        \
CN& operator=( CN const& o ) = default;         \
CN& operator=( CN && o ) = default;             \
void WriteTo( xxlib::ByteBuffer& bb ) const;    \
bool ReadFrom( xxlib::ByteBuffer& bb );


namespace CS
{
// RootEnum1's desc
enum class Enum1 : uint8
{
    // RootEnum1 Item1's desc
    Item1 = 0,
    // RootEnum1 Ite2's desc
    Item2 = 1,
    Item3 = 100,
    Item4 = 101,
    Item5 = 102,
};
namespace Ns2
{
enum class Enum1 : uint8
{
    item1 = 0,
};
} // Ns2
namespace Ns1
{
enum class Enum1 : uint16
{
    item1 = 0,
};
} // Ns1
namespace Ns2
{
struct Pkg2 : xxlib::Memmoveable
{
    XX_SERIALABLE_STRUCT_BASE( Pkg2 );
    const static short packageId;

    ::CS::Enum1 re2 = ::CS::Enum1::Item1;
};
} // Ns2
namespace Ns1
{
struct Pkg1 : xxlib::Memmoveable
{
    XX_SERIALABLE_STRUCT_BASE( Pkg1 );
    const static short packageId;

    ::CS::Ns1::Enum1 ns1_enum1 = ::CS::Ns1::Enum1::item1;
};
} // Ns1
namespace Ns1
{
struct Pkg2 : xxlib::Memmoveable
{
    XX_SERIALABLE_STRUCT_BASE( Pkg2 );
    const static short packageId;

    ::CS::Enum1 re1 = ::CS::Enum1::Item1;
    ::CS::Ns1::Pkg1 ns1_pkg1;
    ::CS::Ns2::Pkg2 root_ns2_pkg1;
};
} // Ns1
struct RootPkg1 : xxlib::Memmoveable
{
    XX_SERIALABLE_STRUCT_BASE( RootPkg1 );
    const static short packageId;

    ::CS::Enum1 m_root_e1 = ::CS::Enum1::Item1;
    ::CS::Ns1::Pkg2 m_ns1_pkg2;
    byte m_byte = 0;
    ushort m_ushort = 0;
    uint m_uint = 0;
    uint64 m_ulong = 0;
    sbyte m_sbyte = 0;
    short m_short = 0;
    int m_int = 0;
    int64 m_long = 0;
    double m_double = 0;
    float m_float = 0;
    bool m_bool = false;
    xxlib::SimpleDate m_DateTime;
    xxlib::StringBuilder m_string;
    xxlib::ByteBuffer m_ByteBuffer;
    byte m_bytes[8] = { 0 };
    xxlib::List<::CS::Ns2::Enum1> m_ns2enum1s;
    xxlib::List<bool> m_bools;
};
} // CS
