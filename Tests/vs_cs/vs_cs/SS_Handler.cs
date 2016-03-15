
using System;
using System.Collections.Generic;
using xxlib;
using SS;

public static class PackageHandler
{
    public static bool Handle( ByteBuffer bb )
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
            case global::SS.AddressIPv4.packageId :
            {
                var o = new global::SS.AddressIPv4();
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
            case global::SS.Chat2Client.Login_Result.packageId :
            {
                var o = new global::SS.Chat2Client.Login_Result();
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
            case global::SS.Chat2Client.Logout_Result.packageId :
            {
                var o = new global::SS.Chat2Client.Logout_Result();
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
            case global::SS.Chat2Client.Push_Login.packageId :
            {
                var o = new global::SS.Chat2Client.Push_Login();
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
            case global::SS.Chat2Client.Push_Logout.packageId :
            {
                var o = new global::SS.Chat2Client.Push_Logout();
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
            case global::SS.Chat2Client.Push_Talk.packageId :
            {
                var o = new global::SS.Chat2Client.Push_Talk();
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
            case global::SS.Chat2Client.Push_Batch.packageId :
            {
                var o = new global::SS.Chat2Client.Push_Batch();
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
            case global::SS.Client2Chat.Login.packageId :
            {
                var o = new global::SS.Client2Chat.Login();
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
            case global::SS.Client2Chat.Talk.packageId :
            {
                var o = new global::SS.Client2Chat.Talk();
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
            case global::SS.Client2Chat.LoginPull.packageId :
            {
                var o = new global::SS.Client2Chat.LoginPull();
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
            case global::SS.Client2Chat.TalkPull.packageId :
            {
                var o = new global::SS.Client2Chat.TalkPull();
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
            case global::SS.Client2Chat.Logout.packageId :
            {
                var o = new global::SS.Client2Chat.Logout();
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
            case global::SS.Server2Proxy.SendPackages.packageId :
            {
                var o = new global::SS.Server2Proxy.SendPackages();
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
            case global::SS.Proxy2Server.SendPackages.packageId :
            {
                var o = new global::SS.Proxy2Server.SendPackages();
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
            case global::SS.Proxy2Client.Connect_Result.packageId :
            {
                var o = new global::SS.Proxy2Client.Connect_Result();
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
            case global::SS.Proxy2Client.SendPackages.packageId :
            {
                var o = new global::SS.Proxy2Client.SendPackages();
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
            case global::SS.Client2Proxy.Connect.packageId :
            {
                var o = new global::SS.Client2Proxy.Connect();
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
            case global::SS.Client2Proxy.SendPackages.packageId :
            {
                var o = new global::SS.Client2Proxy.SendPackages();
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
            case global::SS.Switcher2Client.GetProxyAddress_Success.packageId :
            {
                var o = new global::SS.Switcher2Client.GetProxyAddress_Success();
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
            case global::SS.Switcher2Client.GetProxyAddress_Fail.packageId :
            {
                var o = new global::SS.Switcher2Client.GetProxyAddress_Fail();
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
            case global::SS.Client2Switcher.GetProxyAddress.packageId :
            {
                var o = new global::SS.Client2Switcher.GetProxyAddress();
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
