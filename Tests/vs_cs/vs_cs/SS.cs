using System;
using System.Collections.Generic;
using xxlib;
namespace SS
{
public enum ServerTypes : int
{
    Agent = 0, 
    Switcher = 1, 
    Proxy = 2, 
    Chat = 3, 
};
namespace Proxy2Client
{
public enum ClientConnectResultTypes : int
{
    /// <summary>
    /// 客户端还存在，只是先前断过线，而且并未超时，而且 proxy 上的 “已发包缓存队列” 任然包含 客户端提交的最后包id 
    /// </summary>
    ReconnectSuccess = 0, 
    /// <summary>
    /// 客户端可能重启过，或是 “已发包缓存队列” 已爆炸，没办法补发数据包了，客户端应该走重入逻辑
    /// </summary>
    ReconnectFail = 1, 
    /// <summary>
    /// proxy 不知道当前客户端是谁（未经预约或已超时被移除）
    /// </summary>
    AccessDeny = 2, 
};
} // Proxy2Client
partial class AddressIPv4
{
    public const short packageId = 0;

    public string ip = "";
    public ushort port = 0;
};
namespace Chat2Client
{
partial class Login_Result
{
    public const short packageId = 1;

    public byte result = 0;
};
} // Chat2Client
namespace Chat2Client
{
partial class Logout_Result
{
    public const short packageId = 2;

};
} // Chat2Client
namespace Chat2Client
{
partial class Push_Login
{
    public const short packageId = 3;

    public int pushId = 0;
    public string username = "";
};
} // Chat2Client
namespace Chat2Client
{
partial class Push_Logout
{
    public const short packageId = 4;

    public int pushId = 0;
    public string username = "";
};
} // Chat2Client
namespace Chat2Client
{
partial class Push_Talk
{
    public const short packageId = 5;

    public int pushId = 0;
    public string username = "";
    public string txt = "";
};
} // Chat2Client
namespace Chat2Client
{
partial class Push_Batch
{
    public const short packageId = 6;

    public ByteBuffer data = new ByteBuffer();
};
} // Chat2Client
namespace Client2Chat
{
partial class Login
{
    public const short packageId = 7;

    public string username = "";
};
} // Client2Chat
namespace Client2Chat
{
partial class Talk
{
    public const short packageId = 8;

    public string txt = "";
};
} // Client2Chat
namespace Client2Chat
{
partial class LoginPull
{
    public const short packageId = 9;

    public int lastLoginPushId = 0;
};
} // Client2Chat
namespace Client2Chat
{
partial class TalkPull
{
    public const short packageId = 10;

    public int lastTalkPushId = 0;
};
} // Client2Chat
namespace Client2Chat
{
partial class Logout
{
    public const short packageId = 11;

};
} // Client2Chat
namespace Server2Proxy
{
partial class SendPackages
{
    public const short packageId = 12;

    public string receiver = "";
        /// <summary>
        /// 来自 内部服务器 的1到多个包的序列化集合，全是包体( PackageId + data )，没有包头
        /// </summary>
    public ByteBuffer data = new ByteBuffer();
};
} // Server2Proxy
namespace Proxy2Server
{
partial class SendPackages
{
    public const short packageId = 13;

    public string sender = "";
        /// <summary>
        /// 来自 客户端 的1到多个包的序列化集合，全是包体( PackageId + data )，没有包头
        /// </summary>
    public ByteBuffer data = new ByteBuffer();
};
} // Proxy2Server
namespace Proxy2Client
{
/// <summary>
/// 客户端收到这个包时，包计数器的值不改变
/// </summary>
partial class Connect_Result
{
    public const short packageId = 14;

    public global::SS.Proxy2Client.ClientConnectResultTypes type = global::SS.Proxy2Client.ClientConnectResultTypes.ReconnectSuccess;
};
} // Proxy2Client
namespace Proxy2Client
{
partial class SendPackages
{
    public const short packageId = 15;

        /// <summary>
        /// 来自 内部服务器 的1到多个包的序列化集合（可能是补发的），全是包体( PackageId + data )，没有包头
        /// </summary>
    public ByteBuffer data = new ByteBuffer();
};
} // Proxy2Client
namespace Client2Proxy
{
/// <summary>
/// 断线重连 或首次与 proxy 建立连接
/// </summary>
partial class Connect
{
    public const short packageId = 16;

        /// <summary>
        /// 包计数器的值，即 最后一次收到的包的编号，以便走自动断线重连流程, proxy 补发数据。0 代表初次登陆
        /// </summary>
    public uint lastReceivePackageId = 0;
};
} // Client2Proxy
namespace Client2Proxy
{
/// <summary>
/// 建立连接之后，向内部服务器发包
/// </summary>
partial class SendPackages
{
    public const short packageId = 17;

        /// <summary>
        /// 一到多个数据封装包的序列化结果，包结构为：包头(包体长度2字节，不含包头本身) + 服务器角色类型 + 包体(含包编号的序列化后的数据)
        /// </summary>
    public ByteBuffer data = new ByteBuffer();
};
} // Client2Proxy
namespace Switcher2Client
{
partial class GetProxyAddress_Success
{
    public const short packageId = 18;

    public global::SS.AddressIPv4 address = new global::SS.AddressIPv4();
};
} // Switcher2Client
namespace Switcher2Client
{
/// <summary>
/// version 对不上, 没有可用的 proxy 需要排队之类
/// </summary>
partial class GetProxyAddress_Fail
{
    public const short packageId = 19;

};
} // Switcher2Client
namespace Client2Switcher
{
/// <summary>
/// 用于客户端通过 switcher 定位到要连接的 proxy
/// </summary>
partial class GetProxyAddress
{
    public const short packageId = 20;

        /// <summary>
        /// 通信协议的版本号
        /// </summary>
    public uint version = 0;
    public string sender = "";
};
} // Client2Switcher
} // SS
