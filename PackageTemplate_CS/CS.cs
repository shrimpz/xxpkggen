#pragma warning disable 0169
using PackageLibrary;
using System;

[Desc( "RootEnum1's desc" )]
enum Enum1
{
    [Desc( "RootEnum1 Item1's desc" )]
    Item1 = 0,
    [Desc( "RootEnum1 Ite2's desc" )]
    Item2,
    Item3 = 100,
    Item4,
    Item5,
}
class RootPkg1
{
    Enum1          m_root_e1   ;
    Ns1.Pkg2       m_ns1_pkg2  ;
    byte           m_byte      ;
    ushort         m_ushort    ;
    uint           m_uint      ;
    ulong          m_ulong     ;
    sbyte          m_sbyte     ;
    short          m_short     ;
    int            m_int       ;
    long           m_long      ;
    double         m_double    ;
    float          m_float     ;
    bool           m_bool      ;
    DateTime       m_DateTime  ;

    [Limit( 4, 16 )]
    string         m_string    ;

    [Limit( 0, 128 )]
    ByteBuffer     m_ByteBuffer;

    [Limit( 8 )]
    byte[]         m_bytes     ;

    List<Ns2.Enum1> m_ns2enum1s;

    List<bool>     m_bools;
}
namespace Ns1
{
    enum Enum1 : ushort
    {
        item1,
    }

    class Pkg1
    {
        Enum1 ns1_enum1;
    }
    class Pkg2
    {
        global::Enum1 re1;
        Pkg1 ns1_pkg1;
        Ns2.Pkg2 root_ns2_pkg1;
    }
}
namespace Ns2
{
    enum Enum1 : byte
    {
        item1,
    }
    class Pkg2
    {
        global::Enum1 re2;
    }
}
