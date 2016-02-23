namespace xxlib
{
    public static class ByteBuffer_Ext
    {
        public static void WritePackage( this ByteBuffer bb, global::CS.RootPkg1 v )
        {
            bb.Write( global::CS.RootPkg1.packageId );
            bb.Write( v );
        }
        public static void WritePackage( this ByteBuffer bb, global::CS.Ns2.Pkg2 v )
        {
            bb.Write( global::CS.Ns2.Pkg2.packageId );
            bb.Write( v );
        }
        public static void WritePackage( this ByteBuffer bb, global::CS.Ns1.Pkg1 v )
        {
            bb.Write( global::CS.Ns1.Pkg1.packageId );
            bb.Write( v );
        }
        public static void WritePackage( this ByteBuffer bb, global::CS.Ns1.Pkg2 v )
        {
            bb.Write( global::CS.Ns1.Pkg2.packageId );
            bb.Write( v );
        }
    }
}
