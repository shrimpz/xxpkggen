using System;
using System.Collections.Generic;
using xxlib;
namespace SS
{
public partial class AddressIPv4 : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.Write( ip );
        bb.Write( port ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.Read( ref ip, 0, 0 );
        bb.Read( ref port );
    }
};
namespace Chat2Client
{
public partial class Login_Result : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.Write( result ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.Read( ref result );
    }
};
} // Chat2Client
namespace Chat2Client
{
public partial class Logout_Result : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    { 
    }

    public void ReadFrom( ByteBuffer bb )
    {
    }
};
} // Chat2Client
namespace Chat2Client
{
public partial class Push_Login : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.VarWrite( pushId );
        bb.Write( username ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.VarRead( ref pushId );
        bb.Read( ref username, 0, 0 );
    }
};
} // Chat2Client
namespace Chat2Client
{
public partial class Push_Logout : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.VarWrite( pushId );
        bb.Write( username ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.VarRead( ref pushId );
        bb.Read( ref username, 0, 0 );
    }
};
} // Chat2Client
namespace Chat2Client
{
public partial class Push_Talk : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.VarWrite( pushId );
        bb.Write( username );
        bb.Write( txt ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.VarRead( ref pushId );
        bb.Read( ref username, 0, 0 );
        bb.Read( ref txt, 0, 0 );
    }
};
} // Chat2Client
namespace Chat2Client
{
public partial class Push_Batch : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.Write( data ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.Read( ref data, 0, 0 );
    }
};
} // Chat2Client
namespace Client2Chat
{
public partial class Login : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.Write( username ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.Read( ref username, 0, 0 );
    }
};
} // Client2Chat
namespace Client2Chat
{
public partial class Talk : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.Write( txt ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.Read( ref txt, 0, 0 );
    }
};
} // Client2Chat
namespace Client2Chat
{
public partial class LoginPull : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.VarWrite( lastLoginPushId ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.VarRead( ref lastLoginPushId );
    }
};
} // Client2Chat
namespace Client2Chat
{
public partial class TalkPull : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.VarWrite( lastTalkPushId ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.VarRead( ref lastTalkPushId );
    }
};
} // Client2Chat
namespace Client2Chat
{
public partial class Logout : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    { 
    }

    public void ReadFrom( ByteBuffer bb )
    {
    }
};
} // Client2Chat
namespace Server2Proxy
{
public partial class SendPackages : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.Write( receiver );
        bb.Write( data ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.Read( ref receiver, 0, 0 );
        bb.Read( ref data, 0, 0 );
    }
};
} // Server2Proxy
namespace Proxy2Server
{
public partial class SendPackages : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.Write( sender );
        bb.Write( data ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.Read( ref sender, 0, 0 );
        bb.Read( ref data, 0, 0 );
    }
};
} // Proxy2Server
namespace Proxy2Client
{
public partial class Connect_Result : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.Write( type ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.Read( ref type );
    }
};
} // Proxy2Client
namespace Proxy2Client
{
public partial class SendPackages : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.Write( data ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.Read( ref data, 0, 0 );
    }
};
} // Proxy2Client
namespace Client2Proxy
{
public partial class Connect : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.VarWrite( lastReceivePackageId ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.VarRead( ref lastReceivePackageId );
    }
};
} // Client2Proxy
namespace Client2Proxy
{
public partial class SendPackages : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.Write( data ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.Read( ref data, 0, 0 );
    }
};
} // Client2Proxy
namespace Switcher2Client
{
public partial class GetProxyAddress_Success : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.Write( address ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.Read( ref address );
    }
};
} // Switcher2Client
namespace Switcher2Client
{
public partial class GetProxyAddress_Fail : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    { 
    }

    public void ReadFrom( ByteBuffer bb )
    {
    }
};
} // Switcher2Client
namespace Client2Switcher
{
public partial class GetProxyAddress : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {
        bb.VarWrite( version );
        bb.Write( sender ); 
    }

    public void ReadFrom( ByteBuffer bb )
    {
        bb.VarRead( ref version );
        bb.Read( ref sender, 0, 0 );
    }
};
} // Client2Switcher
} // SS
