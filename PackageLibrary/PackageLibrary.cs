/********************************************************/
// 关键字
/********************************************************/

// enum                                     // 枚举容器
// class                                    // 包结构容器


/********************************************************/
// 标记
/********************************************************/

// Desc( "注释" )                           // 用于类 及类成员
// Limit( [定长] | [ 最小长, 最大长 ] )     // 用于 string, 数组, 集合  还原时的长度安全限制
// Struct                                   // 用于标记某个 包 属于纯结构体( 不会直接用于收发 )
// NoCompress                               // 用于标记某成员 "不使用变长存储". 仅作用于 16/32/64 位整型成员

/********************************************************/
// BuildIn 数据类型:
/********************************************************/

// byte  
// ushort
// uint  
// ulong 
// sbyte 
// short 
// int   
// long  
// double
// float 
// bool
// string
// DateTime
// ByteBuffer

/********************************************************/
// 数组 数据类型:
/********************************************************/

// BuildIn 或 枚举 或 class[]


/********************************************************/
// 集合 数据类型:
/********************************************************/

// List<BuildIn 或 枚举 或 class>




/********************************************************/
// 跨语言数据类型映射表
/********************************************************/

//   c#/模板            |  c++                |  java               |  lua
//----------------------+---------------------+---------------------+----------------
//   byte               |  byte               |  byte               |  double
//   ushort             |  ushort             |  short              |  double
//   uint               |  uint               |  int                |  double
//   ulong              |  uint64             |  long               |  double
//   sbyte              |  sbyte              |  byte               |  double
//   short              |  short              |  short              |  double
//   int                |  int                |  int                |  double
//   long               |  int64              |  long               |  double
//   double             |  double             |  double             |  double
//   float              |  float              |  float              |  double
//   bool               |  bool               |  boolean            |  bool
//   DateTime           |  xxlib::SimpleDate  |  Util.Date          |  string
//   byte[]/ByteBuffer  |  xxlib::ByteBuffer  |  byte[]/ByteBuffer  |  string
//   string             |  xxlib::String      |  string             |  string
//   List               |  xxlib::List        |  List<引用版>       |  table



namespace PackageLibrary
{
    /// <summary>
    /// 用以描述一个可变长的集合
    /// </summary>
    public class List<V> { }

    /// <summary>
    /// 引入一个外部数据类型
    /// </summary>
    public class ByteBuffer { }


    // more data type here ...


    /// <summary>
    /// 备注。可用于类/枚举 及其 成员
    /// </summary>
    public class Desc : System.Attribute
    {
        public Desc( string v ) { Value = v; }
        public string Value { get; set; }
    }

    /// <summary>
    /// 存储数据用的结构体. 生成时将不作为 "包" 对待
    /// </summary>
    public class Struct : System.Attribute
    {
    }

    /// <summary>
    /// 标记 16-64 位整型变量  传输时不压缩( 默认压缩 )
    /// </summary>
    public class NoCompress : System.Attribute
    {
    }

    /// <summary>
    /// 针对最外层级的 数组, byte[], string 做长度限制。单个长度值为定长
    /// </summary>
    public class Limit : System.Attribute
    {
        public Limit( int fix )
        {
            Min = fix;
            Max = fix;
        }
        public Limit( int min, int max )
        {
            if( max < min ) throw new System.Exception( "error: max < min" );
            Min = min;
            Max = max;
        }
        public int Min { get; set; }
        public int Max { get; set; }
    }


    // more attribute here ...

}



/***********************************************************************************/
// 下面的东西当前未启用, 也不完整. 先放这，待整理
/***********************************************************************************/


///// <summary>
///// 是否启用。默认为启用。可用于类和属性。
///// </summary>
//public class Enabled : System.Attribute
//{
//    public Enabled( bool v ) { Value = v; }
//    public bool Value { get; set; }
//}


///// <summary>
///// 设置类于项目的编码需求
///// </summary>
//public class Encode : System.Attribute
//{
//    public Encode( params object[] ps ) { Value = ps; }
//    public object[] Value { get; set; }
//}


///// <summary>
///// 设置类于项目的解码需求
///// </summary>
//public class Decode : System.Attribute
//{
//    public Decode( params object[] ps ) { Value = ps; }
//    public object[] Value { get; set; }
//}


///// <summary>
///// 消息的返回结果列表。可用于类
///// </summary>
//public class Returns : System.Attribute
//{
//    public Returns( params  System.Type[] v ) { Value = v; }
//    public System.Type[] Value { get; set; }
//}


///// <summary>
///// 经由哪些消息所引发，可能返回该条消息。可用于类
///// </summary>
//public class CallBy : System.Attribute
//{
//    public CallBy( params  System.Type[] v ) { Value = v; }
//    public System.Type[] Value { get; set; }
//}




///// <summary>
///// 简单设置相关性（可用于生成链接文档）
///// </summary>
//public class SeeAlso : System.Attribute
//{
//    public SeeAlso( params  string[] v ) { Value = v; }
//    public string[] Value { get; set; }
//}


///// <summary>
///// 针对 Bytes, String，整个包 做长度 check
///// </summary>
//public class Limit : System.Attribute
//{
//    public Limit( System.UInt32 fix ) {
//        Min = fix;
//        Max = fix;
//    }
//    public Limit( System.UInt32 min, System.UInt32 max ) {
//        if( max < min )
//            throw new System.Exception( "error: max < min" );
//        Min = min;
//        Max = max;
//    }
//    public System.UInt32 Min { get; set; }
//    public System.UInt32 Max { get; set; }
//}


//public enum Operators {
//    Equal,
//    NotEqual,
//    LargerEqual,
//    SmallerEqual,
//    LargerThan,
//    SmallerThan,
//}


///// <summary>
///// 解码条件限制（生成类似 if ( 某属性名key == 值v ) 解码当前属性;
///// </summary>
//public class DecodeCondation : System.Attribute
//{
//    public DecodeCondation( string k, Operators o, System.Object v ) { Key = k; Operate = o; Value = v; }
//    public string Key { get; set; }
//    public Operators Operate { get; set; }
//    public System.Object Value { get; set; }
//}


///// <summary>
///// 编码条件限制（生成类似 if ( 某属性名key == 值v ) 编码当前属性;
///// </summary>
//public class EncodeCondation : System.Attribute
//{
//    public EncodeCondation( string k, Operators o, System.Object v ) { Key = k; Operate = o; Value = v; }
//    public string Key { get; set; }
//    public Operators Operate { get; set; }
//    public System.Object Value { get; set; }
//}


///// <summary>
///// 针对 数值类型 做范围 check
///// </summary>
//public class ChkValue : System.Attribute {
//    public ChkValue(System.Decimal min, System.Decimal max) {
//        if (max < min)
//            throw new System.Exception("error: max < min");
//        Min = min;
//        Max = max;
//    }
//    public System.Decimal Min { get; set; }
//    public System.Decimal Max { get; set; }
//}


// 项目分类枚举示例
//// 模板专用: 项目分类枚举( 当前未启用 )
//enum __projects
//{
//    Server1,
//    Server2,
//    Client,
//    Manage
//}



//public class Limits : System.Attribute
//{
//    public Limits( params int[] v )
//    {
//        Value = v;
//    }
//    public int[] Value { get; set; }
//}

///// <summary>
///// 设置序列化条件: 须满足 某 field  等于 value ( field name, value  成对交替填写 )
///// </summary>
//public class Condation : System.Attribute
//{
//    public Condation( params object[] ps ) { Value = ps; }
//    public object[] Value { get; set; }
//}

///// <summary>
///// 设置 field 初始值
///// </summary>
//public class Default : System.Attribute
//{
//    public Default( object v ) { Value = v; }
//    public object Value { get; set; }
//}

///// <summary>
///// 是否创建 Get 函数
///// </summary>
//public class Get : System.Attribute
//{
//    public Get() { Value = true; }
//    public Get( bool v ) { Value = v; }
//    public bool Value { get; set; }
//}

///// <summary>
///// 是否创建 Set 函数
///// </summary>
//public class Set : System.Attribute
//{
//    public Set() { Value = true; }
//    public Set( bool v ) { Value = v; }
//    public bool Value { get; set; }
//}


//public class Dict<K, V> { }
