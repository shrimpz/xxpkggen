using System;
using System.Collections.Generic;
using xxlib;
namespace CS
{
/// <summary>
/// RootEnum1's desc
/// </summary>
public enum Enum1 : byte
{
    /// <summary>
    /// RootEnum1 Item1's desc
    /// </summary>
    Item1 = 0, 
    /// <summary>
    /// RootEnum1 Ite2's desc
    /// </summary>
    Item2 = 1, 
    Item3 = 100, 
    Item4 = 101, 
    Item5 = 102, 
};
namespace Ns2
{
public enum Enum1 : byte
{
    item1 = 0, 
};
} // Ns2
namespace Ns1
{
public enum Enum1 : ushort
{
    item1 = 0, 
};
} // Ns1
partial class RootPkg1
{
    public const short packageId = 0;

    public global::CS.Enum1 m_root_e1 = global::CS.Enum1.Item1;
    public global::CS.Ns1.Pkg2 m_ns1_pkg2 = new global::CS.Ns1.Pkg2();
    public byte m_byte = 0;
    public ushort m_ushort = 0;
    public uint m_uint = 0;
    public ulong m_ulong = 0;
    public sbyte m_sbyte = 0;
    public short m_short = 0;
    public int m_int = 0;
    public long m_long = 0;
    public double m_double = 0;
    public float m_float = 0;
    public bool m_bool = false;
    public DateTime m_DateTime = new DateTime( 1991, 1, 1, 0, 0, 0 );
    public string m_string = "";
    public ByteBuffer m_ByteBuffer = new ByteBuffer();
    public byte[] m_bytes = new byte[ 8 ];
    public List<global::CS.Ns2.Enum1> m_ns2enum1s = new List<global::CS.Ns2.Enum1>();
    public List<bool> m_bools = new List<bool>();
};
namespace Ns2
{
partial class Pkg2
{
    public const short packageId = 1;

    public global::CS.Enum1 re2 = global::CS.Enum1.Item1;
};
} // Ns2
namespace Ns1
{
partial class Pkg1
{
    public const short packageId = 2;

    public global::CS.Ns1.Enum1 ns1_enum1 = global::CS.Ns1.Enum1.item1;
};
} // Ns1
namespace Ns1
{
partial class Pkg2
{
    public const short packageId = 3;

    public global::CS.Enum1 re1 = global::CS.Enum1.Item1;
    public global::CS.Ns1.Pkg1 ns1_pkg1 = new global::CS.Ns1.Pkg1();
    public global::CS.Ns2.Pkg2 root_ns2_pkg1 = new global::CS.Ns2.Pkg2();
};
} // Ns1
} // CS
