//#include "precompile.h"
# include "CS.h"
# include "xxlib.h"
using namespace xxlib;

bool PackageHandle( ByteBuffer& bb )
{
    short pkgId = -1; 
    if( !bb.Read( pkgId ) )
    {
        return false;
    }

    // custom pkgId handler code here

    switch( pkgId )
    {
    case 0:
    {
        ::CS::RootPkg1 o;
        if( !bb.Read( o ) )
        {
            return false;
        }
            
        // code here

        return true;
    }
    case 1:
    {
        ::CS::Ns2::Pkg2 o;
        if( !bb.Read( o ) )
        {
            return false;
        }
            
        // code here

        return true;
    }
    case 2:
    {
        ::CS::Ns1::Pkg1 o;
        if( !bb.Read( o ) )
        {
            return false;
        }
            
        // code here

        return true;
    }
    case 3:
    {
        ::CS::Ns1::Pkg2 o;
        if( !bb.Read( o ) )
        {
            return false;
        }
            
        // code here

        return true;
    }
    default:
        return false;
    }
}

bool PackageHandle( char* buf, int len )
{
    ByteBuffer bb( buf, len, len, true );
    return PackageHandle( bb );
}
