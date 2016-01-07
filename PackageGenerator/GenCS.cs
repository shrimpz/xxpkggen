using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace PackageGenerator
{
    public static class GenCS
    {
        // 项目名上下文，省掉下面的传参了
        public static string _pn;

        public static string Gen( Template t, string outDir, string pn )
        {
            _pn = pn;

            var enums = t.Structs.Where( a => a.IsEnum ).ToList();
            var packages = t.Structs.Where( a => a.IsPackage ).ToList();
            var packages_and_structs = t.Structs.Where( a => a.IsPackage || a.IsStruct ).ToList();



            #region 生成 pn_ByteBufferExt.cs

            // 文件头部
            var sb = new StringBuilder();
            sb.Append( @"using System;
using System.Collections.Generic;
using System.Text;

namespace xxlib
{
    partial class ByteBuffer
    {" );
            /*******************************************************/
            // 所有枚举
            /*******************************************************/

            foreach( var e in enums )
            {
                sb.Append( @"
        public void Write( " + GetNamespace( e ) + "." + e.Name + @" v )
        {
            Write( (" + GetEnumTypeKeyword( e ) + @")v );
        }
        public void Write( " + GetNamespace( e ) + "." + e.Name + @"[] vs )
        {
            for( int i = 0; i < vs.Length; i++ )
            {
                Write( (" + GetEnumTypeKeyword( e ) + @")vs[ i ] );
            }
        }
        public void Write( List<" + GetNamespace( e ) + "." + e.Name + @"> vs )
        {
            WriteLength( vs.Count );
            foreach( var v in vs )
            {
                Write( (" + GetEnumTypeKeyword( e ) + @")v );
            }
        }
        public void Read( ref " + GetNamespace( e ) + "." + e.Name + @" v )
        {
            " + GetEnumTypeKeyword( e ) + @" tmp = 0;
            Read( ref tmp );
            v = (" + GetNamespace( e ) + "." + e.Name + @")tmp;
        }
        public void Read( ref " + GetNamespace( e ) + "." + e.Name + @"[] vs )
        {
            " + GetEnumTypeKeyword( e ) + @" tmp = 0;
            for( int i = 0; i < vs.Length; i++ )
            {
                Read( ref tmp );
                vs[ i ] = (" + GetNamespace( e ) + "." + e.Name + @")tmp;
            }
        }
        public void Read( ref List<" + GetNamespace( e ) + "." + e.Name + @"> vs )
        {
            Read( ref vs, 0, 0 );
        }
        public void Read( ref List<" + GetNamespace( e ) + "." + e.Name + @"> vs, int minLen = 0, int maxLen = 0 )
        {
            int len = ReadLength( minLen, maxLen );
            vs.Clear();
            for( int i = 0; i < len; i++ )
            {
                " + GetEnumTypeKeyword( e ) + @" tmp = 0;
                Read( ref tmp );
                vs.Add( (" + GetNamespace( e ) + "." + e.Name + @")tmp );
            }
        }
" );
            }
            sb.Append( @"
    }
} // ByteBuffer
" );

            try
            {
                sb.WriteToFile( Path.Combine( outDir, pn + "_ByteBufferExt.cs" ) );
            }
            catch( Exception ex )
            {
                return ex.Message;
            }

            #endregion


            #region 生成 pn.cs
            // 文件头部
            sb.Clear();
            sb.Append( @"using System;
using System.Collections.Generic;
using xxlib;" );


            // namespace root {
            sb.Append( @"
namespace " + pn + @"
{" );

            /*******************************************************/
            // 所有枚举
            /*******************************************************/

            foreach( var e in enums )
            {
                // namespace xxx {
                if( e.Namespace != "" )
                {
                    sb.Append( @"
namespace " + e.Namespace + @"
{" );
                }

                // ///  /// desc  ///
                sb.Append( GetComment( e.Desc, 0 ) + @"
public enum " + e.Name + @" : " + GetEnumTypeKeyword( e ) + @"
{" );
                // enum item = val
                foreach( var ef in e.Members )
                {
                    sb.Append( GetComment( ef.Desc, 4 ) + @"
    " + ef.Name + " = " + ef.EnumValue + ", " );
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

                // ///  /// desc  ///
                sb.Append( GetComment( c.Desc, 0 ) + @"
partial class " + c.Name + @"
{" );
                if( c.StructType == StructTypes.Package )
                {
                    sb.Append( @"
    public const short packageId = " + c.PackageId + @";
" );
                }

                // members = default vals;
                foreach( var f in c.Members )
                {
                    sb.Append( GetComment( f.Desc, 8 ) + @"
    " + "public " + GetTypeKeyword( f ) + " " + f.Name.ToFirstLower() + " = " + GetDefaultValueByType( f ) + ";" );
                }

                // class }
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
                sb.WriteToFile( Path.Combine( outDir, pn + ".cs" ) );
            }
            catch( Exception ex )
            {
                return ex.Message;
            }
            #endregion



            #region 生成 pn_IBBReaderWriter.cs

            // 文件头部
            sb.Clear();
            sb.Append( @"using System;
using System.Collections.Generic;
using xxlib;" );

            // namespace root {
            sb.Append( @"
namespace " + pn + @"
{" );

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

                // partial class xxx
                sb.Append( @"
public partial class " + c.Name + @" : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {" );
                foreach( var f in c.Members )
                {
                    sb.Append( @"
        bb." + (f.NoCompress ? "" : (f.Type.Compressable() ? "Var" : "")) + "Write( " + f.Name + " );" );
                }
                sb.Append( @" 
    }

    public void ReadFrom( ByteBuffer bb )
    {" );
                foreach( var f in c.Members )
                {
                    sb.Append( @"
        bb." + (f.NoCompress ? "" : (f.Type.Compressable() ? "Var" : "")) + "Read( ref " + f.Name );
                    if( f.Type.IsContainer && !f.Type.IsArray )
                    {
                        sb.Append( @", " + f.MinLen + ", " + f.MaxLen );
                    }
                    sb.Append( @" );" );
                }
                sb.Append( @"
    }
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
                sb.WriteToFile( Path.Combine( outDir, pn + "_IBBReaderWriter.cs" ) );
            }
            catch( Exception ex )
            {
                return ex.Message;
            }
            #endregion


            #region 生成 pn_Handler.cs
            sb.Clear();
            sb.Append( @"
using System;
using System.Collections.Generic;
using xxlib;
using " + pn + @"

bool PackageHandle( ByteBuffer bb )
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
    {" );
            foreach( var c in packages )
            {
                sb.Append( @"
        case " + GetNamespace( c ) + "." + c.Name + @".packageId :
        {
            var o = new " + GetNamespace( c ) + "." + c.Name + @"();
            try
            {
                o =  bb.Read( ref " + GetNamespace( c ) + "." + c.Name + @" );
            }
            catch
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
" );
            try
            {
                sb.WriteToFile( Path.Combine( outDir, pn + "_Handler.cs" ) );
            }
            catch( Exception ex )
            {
                return ex.Message;
            }

            #endregion

            #region 生成 _cs_log.txt
            sb.Clear();
            sb.Append( @"
var sb = new StringBuilder();
short pkgId = bb.ReadShort();
switch( pkgId )
{" );
            foreach( var c in packages )
            {
                sb.Append( @"
    case " + pn + "." + c.Name + @".packageId:
        {
            " + pn + "." + c.Name + @" o = null;
            try
            {
                o =  bb.Read<" + pn + "." + c.Name + @">();
            }
            catch
            {
                Console.Write( ""ByteBuff Error"" );
            }
            // code here
            string endString = JsonConvert.SerializeObject( o );" );
                if( pn == "CS" )
                {
                    sb.Append( @"
            sb.Append( ""客服端发送类型为:"" );" );
                }
                else if( pn == "SC" )
                {
                    sb.Append( @"
            sb.Append( ""服务器发送类型为:"" );" );
                }

                sb.Append( @"
            sb.Append( """ + c.Name + @"\n"" );" );
                if( pn == "CS" )
                {
                    sb.Append( @"
            sb.Append( ""客服端返回数据为:"" );" );
                }
                else if( pn == "SC" )
                {
                    sb.Append( @"
            sb.Append( ""服务器返回数据为:"" );" );
                }
                sb.Append( @"
            sb.Append( endString );" );
                sb.Append( @"
            sb.Append( ""(  " + c.Desc + @"  )"" );" );
                sb.Append( @"
            sb.Append( ""\n"" );
            break;
         
        }" );
            }

            sb.Append( @"
    default:
            sb.Append( ""no packageId ""+ pkgId );
        break;
}
 bb.offset = 0;
 DateTime dt = DateTime.Now;" );
            sb.Append( @"
 sb.Append(""时间: """ + @"+dt.ToString()+" + @"""\n"");
" );

            sb.Append( @" File.AppendAllText( filePath, sb.ToString(), Encoding.UTF8 );
" );
            try
            {
                sb.WriteToFile( Path.Combine( outDir, pn + "_cs_log.txt" ) );
            }
            catch( Exception ex )
            {
                return ex.Message;
            }

            #endregion

            #region 生成 pn_WritePackage.cs

            sb.Clear();
            sb.Append( @"
public static class ByteBufferExt
{" );
            foreach( var c in packages )
            {
                var tn = GetNamespace( c ) + "." + c.Name;
                sb.Append( @"
    public static void WritePackage( this ByteBuffer bb, " + tn + @" v )
    {
        bb.Write( " + tn + @".packageId );
        bb.Write( v );
    } " );

            }
            sb.Append( @"
}" );
            try
            {
                sb.WriteToFile( Path.Combine( outDir, pn + "_WritePackage.cs" ) );
            }
            catch( Exception ex )
            {
                return ex.Message;
            }

            #endregion

            return "";
        }


        #region GetComment

        public static string GetComment( string s, int space )
        {
            if( s.Trim() == "" ) return "";
            var sps = new string( ' ', space );
            return "\r\n" +
sps + @"/// <summary>
" + sps + @"/// " + s + @"
" + sps + @"/// </summary>";
        }

        #endregion


        #region GetDefaultValueByType

        public static string GetDefaultValueByType( Member f )
        {
            return GetDefaultValueByType( f.Type, f );
        }
        public static string GetDefaultValueByType( DataType d, Member f = null )
        {
            if( d.IsArray )
            {
                return "new " + GetTypeKeyword( d.ChildType ) + "[ " + f.MinLen + " ]";   // "null"
            }
            else if( d.IsGeneric )
            {
                return "new " + d.Name + "<" + GetTypeKeyword( d.ChildType ) + ">()";   // "null"
            }
            else if( d.IsBuiltIn )
            {
                switch( d.Name )
                {
                    case "Byte":
                    case "UInt8":
                    case "UInt16":
                    case "UInt32":
                    case "UInt64":
                    case "SByte":
                    case "Int8":
                    case "Int16":
                    case "Int32":
                    case "Int64":
                    case "Double":
                    case "Float":
                    case "Single": return "0";
                    case "Boolean":
                    case "Bool": return "false";
                    case "String": return "\"\"";
                    case "DateTime": return "new DateTime( 1991, 1, 1, 0, 0, 0 )"; //"DateTime.MinValue";
                    case "ByteBuffer": return "new ByteBuffer()";
                    default:
                        throw new Exception( "unhandled data type" );
                }
            }
            else    // custom / enum
            {
                if( d.IsEnum )
                {
                    return GetNamespace( d ) + "." + d.Name + "." + d.Custom.Members.First().Name;
                }
                else
                {
                    return "new " + GetNamespace( d ) + "." + d.Name + "()";    // "null"; 
                }
            }
        }

        #endregion


        #region GetTypeKeyword

        public static string GetTypeKeyword( Member d )
        {
            return GetTypeKeyword( d.Type );
        }
        public static string GetTypeKeyword( DataType d )
        {
            if( d.IsArray )                // 当前特指 byte[]
            {
                return GetTypeKeyword( d.ChildType ) + "[]";
            }
            else if( d.IsGeneric )
            {
                string rtv = d.Name + "<";
                for( int i = 0; i < d.ChildTypes.Count; ++i )
                {
                    if( i > 0 ) rtv += ", ";
                    rtv += GetTypeKeyword( d.ChildTypes[ i ] );
                }
                rtv += ">";
                return rtv;
            }
            else if( d.IsBuiltIn )
            {
                switch( d.Name )
                {
                    case "Byte": return "byte";
                    case "UInt8": return "byte";
                    case "UInt16": return "ushort";
                    case "UInt32": return "uint";
                    case "UInt64": return "ulong";
                    case "SByte": return "sbyte";
                    case "Int8": return "sbyte";
                    case "Int16": return "short";
                    case "Int32": return "int";
                    case "Int64": return "long";
                    case "Double": return "double";
                    case "Float": return "float";
                    case "Single": return "float";
                    case "Boolean": return "bool";
                    case "Bool": return "bool";
                    case "String": return "string";
                    case "DateTime": return "DateTime";
                    case "ByteBuffer": return "ByteBuffer";
                    default:
                        throw new Exception( "unhandled data type" );
                }

            }
            else // ( d.IsCustom )
            {
                return GetNamespace( d ) + "." + d.Name;
            }
        }
        #endregion


        #region GetEnumTypeKeyword

        public static string GetEnumTypeKeyword( Member f )
        {
            return GetEnumTypeKeyword( f.Type.Custom );
        }
        public static string GetEnumTypeKeyword( Struct e )
        {
            switch( e.EnumUnderlyingType )
            {
                case "Byte": return "byte";
                case "SByte": return "sbyte";
                case "UInt16": return "ushort";
                case "Int16": return "short";
                case "UInt32": return "uint";
                case "Int32": return "int";
                case "UInt64": return "ulong";
                case "Int64": return "long";
            }
            throw new Exception( "unsupported data type" );
        }

        #endregion


        #region GetByteBufferReadFuncName
        public static string GetByteBufferReadFuncName( DataType d )
        {
            string rtv = " ";
            if( d.IsBuiltIn )
            {
                switch( d.Name )
                {

                    case "Byte": rtv = "ReadByte"; break;
                    case "Int16": rtv = "ReadShort"; break;
                    case "Int32": rtv = "ReadInt"; break;
                    case "Int64": rtv = "ReadLong"; break;
                    case "Char": rtv = "ReadChar"; break;
                    case "Double": rtv = "ReadDouble"; break;
                    case "Single": rtv = "ReadFloat"; break;
                    case "Boolean": rtv = "ReadBoolean"; break;
                    case "DateTime": rtv = "ReadDate"; break;
                    case "String": rtv = "ReadString"; break;
                    default:
                        rtv = (d.Namespace != "" ? (d.Namespace + ".") : "") + d.Name;
                        break;
                }

            }
            else if( d.IsCustom )
            {
                if( d.IsEnum )
                {
                    rtv = "ReadInt";
                    return rtv;
                }
                else
                {
                    rtv = "Read";
                    return rtv;
                }

            }
            else if( d.IsGeneric )
            {
                if( d.IsArray )
                {
                    rtv = "ReadBytes";      // 当前只支持 byte[]
                }
                else
                {
                    switch( GetByteBufferReadFuncName( d.ChildType ) )
                    {
                        case "ReadByte": rtv = "ReadListByte"; break;
                        case "ReadShort": rtv = "ReadListShort"; break;
                        case "ReadInt": rtv = "ReadListInt"; break;
                        case "ReadLong": rtv = "ReadListLong"; break;
                        case "ReadChar": rtv = "ReadListChar"; break;
                        case "ReadDouble": rtv = "ReadListDouble"; break;
                        case "ReadFloat": rtv = "ReadListFloat"; break;
                        case "ReadBoolean": rtv = "ReadListBool"; break;
                        case "ReadDate": rtv = "ReadListDate"; break;
                        case "ReadString": rtv = "ReadListString"; break;
                        case "Read": rtv = "ReadListObject"; break;
                    }
                }
            }
            return rtv;
        }

        #endregion


        public static string GetNamespace( DataType e )
        {
            if( e.Namespace == "" )
            {
                return "global::" + _pn;
            }
            else
            {
                return "global::" + _pn + "." + e.Namespace;
            }
        }
        public static string GetNamespace( Struct e )
        {
            if( e.Namespace == "" )
            {
                return "global::" + _pn;
            }
            else
            {
                return "global::" + _pn + "." + e.Namespace;
            }
        }


    }
}
