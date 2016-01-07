//#include "precompile.h"
# include "CS.h"
# include "xxlib.h"
using namespace xxlib;

const short ::CS::Ns2::Pkg2::packageId = 1;
const short ::CS::Ns1::Pkg1::packageId = 2;
const short ::CS::Ns1::Pkg2::packageId = 3;
const short ::CS::RootPkg1::packageId = 0;
void ::CS::Ns2::Pkg2::WriteTo( ByteBuffer& bb ) const
{
    bb.Write( this->re2 ); 
}

bool ::CS::Ns2::Pkg2::ReadFrom( ByteBuffer& bb )
{
    if( !bb.Read( this->re2 ) ) return false;
    return true;
}
void ::CS::Ns1::Pkg1::WriteTo( ByteBuffer& bb ) const
{
    bb.Write( this->ns1_enum1 ); 
}

bool ::CS::Ns1::Pkg1::ReadFrom( ByteBuffer& bb )
{
    if( !bb.Read( this->ns1_enum1 ) ) return false;
    return true;
}
void ::CS::Ns1::Pkg2::WriteTo( ByteBuffer& bb ) const
{
    bb.Write( this->re1 );
    bb.Write( this->ns1_pkg1 );
    bb.Write( this->root_ns2_pkg1 ); 
}

bool ::CS::Ns1::Pkg2::ReadFrom( ByteBuffer& bb )
{
    if( !bb.Read( this->re1 ) ) return false;
    if( !bb.Read( this->ns1_pkg1 ) ) return false;
    if( !bb.Read( this->root_ns2_pkg1 ) ) return false;
    return true;
}
void ::CS::RootPkg1::WriteTo( ByteBuffer& bb ) const
{
    bb.Write( this->m_root_e1 );
    bb.Write( this->m_ns1_pkg2 );
    bb.Write( this->m_byte );
    bb.Write( this->m_ushort );
    bb.VarWrite( this->m_uint );
    bb.VarWrite( this->m_ulong );
    bb.Write( this->m_sbyte );
    bb.Write( this->m_short );
    bb.VarWrite( this->m_int );
    bb.VarWrite( this->m_long );
    bb.Write( this->m_double );
    bb.Write( this->m_float );
    bb.Write( this->m_bool );
    bb.Write( this->m_DateTime );
    bb.Write( this->m_string );
    bb.Write( this->m_ByteBuffer );
    bb.Write( this->m_bytes );
    bb.Write( this->m_ns2enum1s );
    bb.Write( this->m_bools ); 
}

bool ::CS::RootPkg1::ReadFrom( ByteBuffer& bb )
{
    if( !bb.Read( this->m_root_e1 ) ) return false;
    if( !bb.Read( this->m_ns1_pkg2 ) ) return false;
    if( !bb.Read( this->m_byte ) ) return false;
    if( !bb.Read( this->m_ushort ) ) return false;
    if( !bb.VarRead( this->m_uint ) ) return false;
    if( !bb.VarRead( this->m_ulong ) ) return false;
    if( !bb.Read( this->m_sbyte ) ) return false;
    if( !bb.Read( this->m_short ) ) return false;
    if( !bb.VarRead( this->m_int ) ) return false;
    if( !bb.VarRead( this->m_long ) ) return false;
    if( !bb.Read( this->m_double ) ) return false;
    if( !bb.Read( this->m_float ) ) return false;
    if( !bb.Read( this->m_bool ) ) return false;
    if( !bb.Read( this->m_DateTime ) ) return false;
    if( !bb.Read( this->m_string, 4, 16 ) ) return false;
    if( !bb.Read( this->m_ByteBuffer, 0, 128 ) ) return false;
    if( !bb.Read( this->m_bytes ) ) return false;
    if( !bb.Read( this->m_ns2enum1s, 0, 0 ) ) return false;
    if( !bb.Read( this->m_bools, 0, 0 ) ) return false;
    return true;
}