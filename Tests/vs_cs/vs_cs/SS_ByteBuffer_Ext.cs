namespace xxlib
{

    public static class ByteBuffer_Ext
    {
        public static void WritePackage( this ByteBuffer bb, global::SS.AddressIPv4 v )
        {
            bb.Write( global::SS.AddressIPv4.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Chat2Client.Login_Result v )
        {
            bb.Write( global::SS.Chat2Client.Login_Result.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Chat2Client.Logout_Result v )
        {
            bb.Write( global::SS.Chat2Client.Logout_Result.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Chat2Client.Push_Login v )
        {
            bb.Write( global::SS.Chat2Client.Push_Login.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Chat2Client.Push_Logout v )
        {
            bb.Write( global::SS.Chat2Client.Push_Logout.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Chat2Client.Push_Talk v )
        {
            bb.Write( global::SS.Chat2Client.Push_Talk.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Chat2Client.Push_Batch v )
        {
            bb.Write( global::SS.Chat2Client.Push_Batch.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Client2Chat.Login v )
        {
            bb.Write( global::SS.Client2Chat.Login.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Client2Chat.Talk v )
        {
            bb.Write( global::SS.Client2Chat.Talk.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Client2Chat.LoginPull v )
        {
            bb.Write( global::SS.Client2Chat.LoginPull.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Client2Chat.TalkPull v )
        {
            bb.Write( global::SS.Client2Chat.TalkPull.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Client2Chat.Logout v )
        {
            bb.Write( global::SS.Client2Chat.Logout.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Server2Proxy.SendPackages v )
        {
            bb.Write( global::SS.Server2Proxy.SendPackages.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Proxy2Server.SendPackages v )
        {
            bb.Write( global::SS.Proxy2Server.SendPackages.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Proxy2Client.Connect_Result v )
        {
            bb.Write( global::SS.Proxy2Client.Connect_Result.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Proxy2Client.SendPackages v )
        {
            bb.Write( global::SS.Proxy2Client.SendPackages.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Client2Proxy.Connect v )
        {
            bb.Write( global::SS.Client2Proxy.Connect.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Client2Proxy.SendPackages v )
        {
            bb.Write( global::SS.Client2Proxy.SendPackages.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Switcher2Client.GetProxyAddress_Success v )
        {
            bb.Write( global::SS.Switcher2Client.GetProxyAddress_Success.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Switcher2Client.GetProxyAddress_Fail v )
        {
            bb.Write( global::SS.Switcher2Client.GetProxyAddress_Fail.packageId );
            bb.Write( v );
        } 
        public static void WritePackage( this ByteBuffer bb, global::SS.Client2Switcher.GetProxyAddress v )
        {
            bb.Write( global::SS.Client2Switcher.GetProxyAddress.packageId );
            bb.Write( v );
        } 
    }
}
