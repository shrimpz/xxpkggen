
using System;
using System.Collections.Generic;
using xxlib;
using CS;

public static class PackageHandler
{
    static bool Handle( ByteBuffer bb )
    {
        short pkgId = 0;
        try
        {
            bb.Read( ref pkgId );
        }
        catch
        {
            return false;
        }

        // custom pkgId handler code here

        switch( pkgId )
        {
        case global::CS.RootPkg1.packageId:
            {
                var o = new global::CS.RootPkg1();
                try
                {
                    bb.Read( ref o );
                }
                catch
                {
                    return false;
                }

                // code here

                return true;
            }
        case global::CS.Ns2.Pkg2.packageId:
            {
                var o = new global::CS.Ns2.Pkg2();
                try
                {
                    bb.Read( ref o );
                }
                catch
                {
                    return false;
                }

                // code here

                return true;
            }
        case global::CS.Ns1.Pkg1.packageId:
            {
                var o = new global::CS.Ns1.Pkg1();
                try
                {
                    bb.Read( ref o );
                }
                catch
                {
                    return false;
                }

                // code here

                return true;
            }
        case global::CS.Ns1.Pkg2.packageId:
            {
                var o = new global::CS.Ns1.Pkg2();
                try
                {
                    bb.Read( ref o );
                }
                catch
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
}

