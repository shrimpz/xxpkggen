using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;


namespace PackageGenerator
{
    public static class GenCPP
    {
        // 上下文, root namespace
        public static string _pn;
        public static string Gen( Template t, string outDir, string pn )
        {
            _pn = pn;    // root namespace
            ConvertNamespace( t, pn );

            var enums = t.Structs.Where( a => a.IsEnum ).ToList();
            var packages = t.Structs.Where( a => a.IsPackage )
                .OrderByDescending( a=>a.RefCounter ).ToList();     // 按依赖顺序排序
            var packages_and_structs = t.Structs.Where( a => a.IsPackage || a.IsStruct )
                .OrderByDescending( a => a.RefCounter ).ToList();   // 按依赖顺序排序

            var sb = new StringBuilder();


            #region 生成 pn.h

            // 文件头部
            sb.Append( @"#pragma once

#include ""xxlib.h""
" );

            /*******************************************************/
            // 所有枚举
            /*******************************************************/

            // namespace root {
            sb.Append( @"
namespace " + pn + @"
{" );

            foreach( var e in enums )
            {
                // namespace xxx {
                if( e.Namespace != "" )
                {
                    sb.Append( @"
namespace " + e.Namespace + @"
{" );
                }

                // // desc
                sb.Append( GetComment( e.Desc, 0 ) );

                // enum class xxx : valType {
                sb.Append( @"
enum class " + e.Name + @" : " + GetEnumTypeKeyword( e ) + @"
{" );
                foreach( var ef in e.Members )
                {
                    // // desc
                    sb.Append( GetComment( ef.Desc, 4 ) );

                    // enum item = val
                    sb.Append( @"
    " + ef.Name + " = " + ef.EnumValue + "," );
                }

                // enum class }; // xxx
                sb.Append( @"
};" );
                // namespace } // xxx
                if( e.Namespace != "" )
                {
                    sb.Append( @"
} // " + e.Namespace );
                }
            }



#if false
            /*******************************************************/
            // 预声明部分
            /*******************************************************/

            // namespace root {
            sb.Append( @"
namespace " + pn + @"
{" );

            foreach( var c in packages_and_structs )
            {
                // namespace xxx {
                if( c.Namespace != "" )
                {
                    sb.Append( @"
namespace " + c.Namespace + @"
{" );
                }

                // // desc
                sb.Append( GetComment( c.Desc, 0 ) );

                // struct xxx
                sb.Append( @"
struct " + c.Name + @";" );

                // namespace } // xxx
                if( c.Namespace != "" )
                {
                    sb.Append( @"
} // " + c.Namespace );
                }
            }

            // namespace } // root
            sb.Append( @"
} // " + pn + @"
" );
#endif



            /*******************************************************/
            // 结构体部分
            /*******************************************************/


            foreach( var c in packages_and_structs )
            {
                // namespace xxx {
                if( c.Namespace != "" )
                {
                    sb.Append( @"
namespace " + c.Namespace + @"
{" );
                }

                // // desc
                sb.Append( GetComment( c.Desc, 0 ) );

                // struct xxx {
                sb.Append( @"
struct " + c.Name + @" : xxlib::Memmoveable
{" );
#if false
                // packageId;
                if( c.StructType == StructTypes.Package )
                {
                    sb.Append( @"
    const static short packageId;" );
                }

                // constructors
                sb.Append( @"
    " + c.Name + @"() {}
    " + c.Name + @"( " + c.Name + @" const& o ) = default;
    " + c.Name + @"( " + c.Name + @" && o ) = default;
    " + c.Name + @"& operator=( " + c.Name + @" const& o ) = default;
    " + c.Name + @"& operator=( " + c.Name + @" && o ) = default;
" );
#else
                // XX_SERIALABLE_STRUCT_BASE( cn )
                sb.Append( @"
    XX_SERIALABLE_STRUCT_BASE( " + c.Name + " );" );
                if( c.StructType == StructTypes.Package )
                {
                    sb.Append( @"
    const static short packageId;
" );
                }
#endif

                // members
                foreach( var f in c.Members )
                {
                    var tn = f.Type.IsArray ? GetTypeKeyword( f.Type.ChildType ) : GetTypeKeyword( f );
                    var dv = GetDefaultValueByType( f );

                    // // desc
                    sb.Append( GetComment( f.Desc, 4 ) );

                    // type val = default;
                    sb.Append( @"
    " + tn + " " + f.Name.ToFirstLower()
    + (f.Type.IsArray ? ("[" + f.MinLen + "]") : "")
    + (dv != "" ? (" = " + dv) : "") + ";" );
                }

#if false
                // interface for ByteBuffer
                sb.Append( @"

    // interface for ByteBuffer
    void WriteTo( xxlib::ByteBuffer& bb ) const;
    bool ReadFrom( xxlib::ByteBuffer& bb );");
#endif

                sb.Append( @"
};" );

                // namespace } // xxx
                if( c.Namespace != "" )
                {
                    sb.Append( @"
} // " + c.Namespace );
                }

            }

            // namespace } // root
            sb.Append( @"
} // " + pn + @"
" );


            try
            {
                sb.WriteToFile( Path.Combine( outDir, pn + ".h" ) );
            }
            catch( Exception ex )
            {
                return ex.Message;
            }

            #endregion


            #region 生成 pn.cpp
            sb.Clear();

            // includes, usings
            sb.Append( @"//#include ""precompile.h""
# include """ + pn + @".h""
# include ""xxlib.h""
using namespace xxlib;
" );
            foreach( var c in packages )
            {
                // ns::packageId = val;
                sb.Append( @"
const short " + GetNamespace( c ) + "::" + c.Name + @"::packageId = " + c.PackageId + ";" );
            }

            foreach( var c in packages_and_structs )
            {
                // ns::WriteTo
                sb.Append( @"
void " + GetNamespace( c ) + "::" + c.Name + @"::WriteTo( ByteBuffer& bb ) const
{" );
                foreach( var f in c.Members )
                {
                    // bb.[Var]Write
                    sb.Append( @"
    bb." + (f.NoCompress ? "" : (f.Type.Compressable() ? "Var" : "")) + "Write( this->" + f.Name + " );" );
                }

                // } // ns::WriteTo
                sb.Append( @" 
}
" );
                // ns::ReadFrom
                sb.Append( @"
bool " + GetNamespace( c ) + "::" + c.Name + @"::ReadFrom( ByteBuffer& bb )
{" );
                foreach( var f in c.Members )
                {
                    // bb.[Var]Read
                    sb.Append( @"
    if( !bb." + (f.NoCompress ? "" : (f.Type.Compressable() ? "Var" : "")) + "Read( this->" + f.Name );
                    if( f.Type.IsContainer && !f.Type.IsArray )
                    {
                        sb.Append( @", " + f.MinLen + ", " + f.MaxLen );
                    }
                    sb.Append( @" ) ) return false;" );
                }
                sb.Append( @"
    return true;" );

                // } // ns::ReadFrom
                sb.Append( @"
}" );

            }

            try
            {
                sb.WriteToFile( Path.Combine( outDir, pn + ".cpp" ) );
            }
            catch( Exception ex )
            {
                return ex.Message;
            }
            #endregion


            #region 生成 pn_Handler.cpp
            sb.Clear();
            sb.Append( @"//#include ""precompile.h""
# include """ + pn + @".h""
# include ""xxlib.h""
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
    {" );
            foreach( var c in packages.OrderBy( a => a.PackageId ).ToList() )
            {
                if( c.PackageId == -1 ) continue;
                sb.Append( @"
    case " + c.PackageId + @":
    {
        " + GetNamespace( c ) + "::" + c.Name + @" o;
        if( !bb.Read( o ) )
        {
            return false;
        }
            
        // code here

        return true;
    }" );
            }

            sb.Append( @"
    default:
        return false;
    }
}

bool PackageHandle( char* buf, int len )
{
    ByteBuffer bb( buf, len, len, true );
    return PackageHandle( bb );
}
" );
            try
            {
                sb.WriteToFile( Path.Combine( outDir, pn + "_Handler.cpp" ) );
            }
            catch( Exception ex )
            {
                return ex.Message;
            }
            #endregion


            #region 生成 pn_EnumToString.h
            sb.Clear();

            string ns = pn + "_EnumToString";

            sb.Append( @"#pragma once

# include """ + pn + @".h""
" );
            sb.Append( @"
namespace " + ns + @"
{" );
            foreach( var e in enums )
            {
                var tn = "::" + pn + @"::" + e.Namespace + (e.Namespace == "" ? "" : "::") + e.Name;
                sb.Append( @"
static char const* const Get( " + tn + @" e );
static char const* const GetName( " + tn + @" e );
static char const* const GetDesc( " + tn + @" e );" );
            }

            sb.Append( @"
} // " + ns + @"
" );

            try
            {
                sb.WriteToFile( Path.Combine( outDir, ns + ".h" ) );
            }
            catch( Exception ex )
            {
                return ex.Message;
            }
            #endregion


            #region 生成 pn_EnumToString.cpp
            sb.Clear();
            sb.Append( @"//#include ""precompile.h""
# include """ + ns + @".h""" );

            // namespace ns {
            sb.Append( @"
namespace " + ns + @"
{" );

            foreach( var e in enums )
            {
                var tn = "::" + pn + @"::" + e.Namespace + (e.Namespace == "" ? "" : "::") + e.Name;
                sb.Append( @"
char const* const Get( " + tn + @" e )
{
    switch(e)
    {" );
                foreach( var ee in e.Members )
                {
                    sb.Append( @"
    case " + tn + @"::" + ee.Name + @": 
        return u8""" + tn + "::" + ee.Name + " " + ee.Desc + @""";" );
                }
                sb.Append( @"
    }
    assert( false );
    return u8"""";
}" );

                sb.Append( @"
char const* const GetName( " + tn + @" e )
{
    switch(e)
    {" );
                foreach( var ee in e.Members )
                {
                    sb.Append( @"
    case " + tn + "::" + ee.Name + @": 
        return u8""" + ee.Name + @""";" );
                }
                sb.Append( @"
    }
    assert( false );
    return u8"""";
}" );

                sb.Append( @"
char const* const GetDesc( " + tn + @" e )
{
    switch(e)
    {" );
                foreach( var ee in e.Members )
                {
                    sb.Append( @"
    case " + tn + @"::" + ee.Name + @": 
        return u8""" + ee.Desc + @""";" );
                }
                sb.Append( @"
    }
    assert( false );
    return u8"""";
}" );
            }

            // namespace } // ns
            sb.Append( @"
} // " + ns + @"
" );

            try
            {
                sb.WriteToFile( Path.Combine( outDir, ns + ".cpp" ) );
            }
            catch( Exception ex )
            {
                return ex.Message;
            }
            #endregion

            return "";
        }


        public static string ToFieldName( string s )
        {
            if( s.Length <= 1 ) return "_" + s.ToLower();
            return "_" + s.Substring( 0, 1 ).ToLower() + s.Substring( 1 );
        }



        // 把 Namespace 啥中间的 . 转为 ::
        public static void ConvertNamespace( Template t, string baseNS = "" )
        {
            foreach( var c in t.Structs )
            {
                c.Namespace = c.Namespace.Replace( ".", "::" );
            }
        }





        public static string GetDefaultValueByType( Member f )
        {
            return GetDefaultValueByType( f.Type );
        }
        public static string GetDefaultValueByType( DataType d )
        {
            if( d.IsGeneric
                || d.IsCustom && !d.IsEnum )
            {
                return "";
            }
            if( d.IsArray )
            {
                return "{ " + GetDefaultValueByType( d.ChildType ) + " }";
            }
            if( d.IsEnum )
            {
                return GetTypeKeyword( d ) + "::" + d.Custom.Members.First().Name;
            }
            // BuiltIn
            switch( d.Name )
            {
                case "Byte":
                case "UInt16":
                case "UInt32":
                case "UInt64":
                case "SByte":
                case "Int16":
                case "Int32":
                case "Int64":
                case "Double":
                case "Single": return "0";
                case "Boolean": return "false";
                case "String":
                case "DateTime":
                case "ByteBuffer": return "";
                default:
                    throw new Exception( "unhandled data type" );
            }
        }

        public static string GetComment( string s, int space )
        {
            if( s.Trim() == "" ) return "";
            var sps = new string( ' ', space );
            return "\r\n" + sps + "// " + s;
        }

        public static string GetTypeKeyword( Member m )
        {
            return GetTypeKeyword( m, m.Type );
        }
        public static string GetTypeKeyword( DataType d )
        {
            return GetTypeKeyword( null, d );
        }
        public static string GetTypeKeyword( Member m, DataType d )
        {
            string rtv = "";
            if( d.IsArray )
            {
                rtv = GetTypeKeyword( m, d.ChildType ) + "[" + m.MinLen + "]";
            }
            else if( d.IsBuiltIn )
            {
                switch( d.Name )
                {
                    case "Byte": rtv = "byte"; break;
                    case "SByte": rtv = "sbyte"; break;
                    case "UInt16": rtv = "ushort"; break;
                    case "Int16": rtv = "short"; break;
                    case "UInt32": rtv = "uint"; break;
                    case "Int32": rtv = "int"; break;
                    case "UInt64": rtv = "uint64"; break;
                    case "Int64": rtv = "int64"; break;
                    case "Double": rtv = "double"; break;
                    case "Single": rtv = "float"; break;
                    case "Boolean": rtv = "bool"; break;
                    case "String": rtv = "xxlib::String"; break;
                    case "DateTime": rtv = "xxlib::SimpleDate"; break;
                    case "ByteBuffer": rtv = "xxlib::ByteBuffer"; break;
                    default:
                        throw new Exception( "unknown BuiltIn data type" );
                }

            }
            else if( d.IsCustom )
            {
                rtv = "::" + _pn + "::" + (d.Namespace != "" ? (d.Namespace + "::") : "") + d.Name;
            }
            else // Generic
            {
                rtv = "xxlib::" + d.Name + "<";
                for( int i = 0; i < d.ChildTypes.Count; ++i )
                {
                    if( i > 0 ) rtv += ",";
                    rtv += GetTypeKeyword( m, d.ChildTypes[ i ] );
                }
                rtv += ">";
            }
            return rtv;
        }

        public static string GetEnumTypeKeyword( this Struct e )
        {
            switch( e.EnumUnderlyingType )
            {
                case "Byte": return "uint8";
                case "SByte": return "int8";
                case "UInt16": return "uint16";
                case "Int16": return "int16";
                case "UInt32": return "uint";
                case "Int32": return "int";
                case "UInt64": return "uint64";
                case "Int64": return "int64";
            }
            throw new Exception( "unsupported data type" );
        }

        public static string GetNamespace( Struct e )
        {
            if( e.Namespace == "" )
            {
                return "::" + _pn;
            }
            else
            {
                return "::" + _pn + "::" + e.Namespace;
            }
        }

    }
}




//// todo: 将类型声明尾部的 1组或多组 [???] 分离并返回
//public static KeyValuePair<string, string> SplitTypeKeyword( string k, Member m )
//{
//    int n = 0;
//    var d = m.Type;
//    while( d.IsArray )
//    {
//        n += 2 + m.MinLen.ToString().Length;
//        d = m.Type.ChildType;
//    }
//    return new KeyValuePair<string, string>( k.Substring( 0, k.Length - n ), k.Substring( k.Length - n, n ) );
//}


