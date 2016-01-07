#pragma warning disable 0169
using PackageLibrary;
using System;

[Desc( "注册成功" )]
class Register_Success
{
}

[Desc( "注册失败--错误类型" )]
enum Register_Fail_Types
{
    [Desc( "登录名已重复" )]
    LoginName_Duplicated,
    [Desc( "登录名含有非法字符" )]
    LoginName_Illegal,
    [Desc( "登录名太短" )]
    LoginName_TooShort,
    [Desc( "登录名太长" )]
    LoginName_TooLong,
    [Desc( "密码太简单" )]
    Password_TooSimple,
}

[Desc( "注册失败" )]
class Register_Fail
{
    Register_Fail_Types type;
}


[Desc( "登录成功" )]
class Login_Success
{
	ulong token;
}

[Desc( "登录失败" )]
class Login_Fail
{
}


[Desc("用户网络状态")]
enum UserNetStates
{
	[Desc("未知/所有")]
	Unknown,
	[Desc("在线")]
	Online,
	[Desc("离线")]
	Offline
}

[Struct, Desc( "用户网络相关信息数据结构" )]
class UserNetInfo
{
	[Desc( "数据库生成的用户自增编号" )]
	int 				userId;
    [Desc("登录名")]
	string 				loginName;
	[Desc("用户网络状态")]
	UserNetStates 	netState;
}

class GetUsers
{
	List<UserNetInfo> users;
}

