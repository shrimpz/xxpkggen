using System;
using System.Collections.Generic;
using xxlib;
namespace CS
{
public partial class RootPkg1 : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.Write( m_root_e1 );
        bb.Write( m_ns1_pkg2 );
        bb.Write( m_byte );
        bb.Write( m_ushort );
        bb.VarWrite( m_uint );
        bb.VarWrite( m_ulong );
        bb.Write( m_sbyte );
        bb.Write( m_short );
        bb.VarWrite( m_int );
        bb.VarWrite( m_long );
        bb.Write( m_double );
        bb.Write( m_float );
        bb.Write( m_bool );
        bb.Write( m_DateTime );
        bb.Write( m_string );
        bb.Write( m_ByteBuffer );
        bb.Write( m_bytes );
        bb.Write( m_ns2enum1s );
        bb.Write( m_bools ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.Read( ref m_root_e1 );
        bb.Read( ref m_ns1_pkg2 );
        bb.Read( ref m_byte );
        bb.Read( ref m_ushort );
        bb.VarRead( ref m_uint );
        bb.VarRead( ref m_ulong );
        bb.Read( ref m_sbyte );
        bb.Read( ref m_short );
        bb.VarRead( ref m_int );
        bb.VarRead( ref m_long );
        bb.Read( ref m_double );
        bb.Read( ref m_float );
        bb.Read( ref m_bool );
        bb.Read( ref m_DateTime );
        bb.Read( ref m_string, 4, 16 );
        bb.Read( ref m_ByteBuffer, 0, 128 );
        bb.Read( ref m_bytes );
        bb.Read( ref m_ns2enum1s, 0, 0 );
        bb.Read( ref m_bools, 0, 0 );
    }
};
namespace Ns2
{
public partial class Pkg2 : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.Write( re2 ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.Read( ref re2 );
    }
};
} // Ns2
namespace Ns1
{
public partial class Pkg1 : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.Write( ns1_enum1 ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.Read( ref ns1_enum1 );
    }
};
} // Ns1
namespace Ns1
{
public partial class Pkg2 : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.Write( re1 );
        bb.Write( ns1_pkg1 );
        bb.Write( root_ns2_pkg1 ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.Read( ref re1 );
        bb.Read( ref ns1_pkg1 );
        bb.Read( ref root_ns2_pkg1 );
    }
};
} // Ns1
} // CS
