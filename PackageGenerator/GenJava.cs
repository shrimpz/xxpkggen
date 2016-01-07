using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace PackageGenerator
{
    class GenJava
    {
        // 项目名上下文，省掉下面的传参了
        public static string _pn;

        public static string Gen( Template t, string outDir, string pn )
        {
            var enums = t.Structs.Where( a => a.IsEnum ).ToList();
            var packages = t.Structs.Where( a => a.IsPackage ).ToList();
            var packages_and_structs = t.Structs.Where( a => a.IsPackage || a.IsStruct ).ToList();

            _pn = pn;
            var ns = pn;

            #region 生成 所有枚举

            var sb = new StringBuilder();


            // 生成所有枚举
            foreach( var e in enums )
            {
                sb.Clear();
                sb.Append( @"
package " + pn + @";
import xxlib.serial.*;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

" );
                if( !string.IsNullOrWhiteSpace( e.Desc ) )
                {
                    sb.Append( @"
// " + e.Desc );
                }
                sb.Append( @"
public enum " + e.Name + @"
{" );
                for( int i = 0; i < e.Members.Count; i++ )
                {
                    var ef = e.Members[ i ];
                    sb.Append( @"
    " + ef.Name + "( (" + GetEnumTypeKeyword( e ) + ")" + ef.EnumValue + " )" );
                    if( i != e.Members.Count - 1 )
                    {
                        sb.Append( @"," );
                    }
                    else
                    {
                        sb.Append( @";" );
                    }
                    if( !string.IsNullOrWhiteSpace( ef.Desc ) )
                    {
                        sb.Append( "// " + ef.Desc );
                    }
                }
                sb.Append( @"
    " + GetEnumTypeKeyword( e ) + @" value;" );
                sb.Append( @"
    " + e.Name + @"( " + GetEnumTypeKeyword( e ) + @" v )
    {
        this.value = v;
    }" );
                sb.Append( @"
    public static " + e.Name + @" valueOf( int v )
    {   
        switch( v )
        {" );
                for( int i = 0; i < e.Members.Count; i++ )
                {
                    var ef = e.Members[i];
                    sb.Append( @"
            case " + ef.EnumValue + @":
                return " + ef.Name + ";" );
                }
                sb.Append( @"
            default:
                return null;
        }
    }" );
                sb.Append( @"
};" );
                try
                {
                    sb.WriteToFile( Path.Combine( outDir, e.Name + ".java" ) );
                }
                catch( Exception ex )
                {
                    return ex.Message;
                }
            }

            // 生成类声明
            foreach( var c in packages_and_structs )
            {
                sb.Clear();
                sb.Append( @"
package " + pn + @";
import xxlib.serial.*;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

" );
                sb.Append( @"" );
                if( !string.IsNullOrWhiteSpace( c.Desc ) )
                {
                    sb.Append( @"
// " + c.Desc );
                }

                sb.Append( @"
public class " + c.Name + @" implements IBBWriter, IBBReader
{" );
                // 生成类成员
                foreach( var f in c.Members )
                {
                    var tn = GetTypeKeyword( f.Type );
                    sb.Append( (f.Desc == "" ? "" : GetComment( f.Desc, 8 )) + @"
    public " + tn + " " + f.Name.ToFirstLower() + " = " + GetDefaultValueByType( f.Type ) + ";" );
                }

                // 生成静态直写函数
                if( c.StructType == StructTypes.Package )
                {
                    sb.Append( @"
    public static final short packageId = " + c.PackageId + @";
" );
                }

                // 生成 ByteBuffer 接口函数的定义
                sb.Append( @"
    public void WriteTo( ByteBuffer bb )
    {" );
                foreach( var f in c.Members )
                {
                    sb.Append( @"
        bb." + GetWriteTypeBuffer( f.Type ) + "( " + f.Name );
                    if( f.Type.IsEnum )
                    {
                        sb.Append( @".value" );
                    }
                    sb.Append( @" );" );
                }
                sb.Append( @" 
    }

    public void ReadFrom( ByteBuffer bb )
    {" );
                for( int i = 0; i < c.Members.Count; ++i )
                {
                    var f = c.Members[ i ];
                    var ft = f.Type;

                    sb.Append( @"
        " + f.Name + " = " );
                    if( ft.IsEnum )
                    {
                        sb.Append( ft.Custom.Name + ".valueOf( " );
                    }
                    sb.Append( "bb." + GetReadTypeBuffer( ft ) + @"( " );
                    if( ft.IsGeneric && ft.ChildType.IsCustom )
                    {
                        sb.Append( GetBufferString( ft.ChildType ) + ".class" );
                    }
                    else if( ft.IsCustom && !ft.IsEnum )
                    {
                        sb.Append( GetBufferString( ft ) + ".class" );
                    }
                    if( ft.IsGeneric )
                    {
                        if( ft.ChildType.IsCustom )
                        {
                            sb.Append( @", " );
                        }
                        sb.Append( f.MinLen + @", " + f.MaxLen );
                    }
                    else if( ft.IsBuiltIn && ft.Name == "String" )
                    {
                        sb.Append( f.MinLen + @", " + f.MaxLen );
                    }
                    if( ft.IsEnum )
                    {
                        sb.Append( @" )" );
                    }
                    sb.Append( @" );" );

                }
                sb.Append( @"
    }
" );
                sb.Append( @"
};" );
                try
                {
                    sb.WriteToFile( Path.Combine( outDir, c.Name + ".java" ) );
                }
                catch( Exception ex )
                {
                    return ex.Message;
                }
            }



            #endregion

            #region 生成 ns_java_switch.txt
            sb.Clear();
            sb.Append( @"
public static boolean PackageHandle( byte[] buf )
{
	ByteBuffer bb = new ByteBuffer();
	bb.buf = buf;
	bb.dataLen = buf.length;

    short pkgId = -1;
    try
    {
        pkgId = bb.ReadShort();
    }
    catch
    {
        // error handler code here
        return false;
    }

    switch( pkgId )
    {" );
            foreach( var c in packages )
            {
                sb.Append( @"
        // else if( pkgId == " + c.Name + @".packageId )
        case " + c.PackageId + @" :
        {
            " + c.Name + @" o = null;
            try
            {
                o = bb.ReadClass( " + c.Name + @".class );
            }
            catch
            {
                // error handler code here
                return false;
            }

            // handler code here

            return true;
        }" );
            }

            sb.Append( @"
        default:
            return true;
    }
}" );
            try
            {
                sb.WriteToFile( Path.Combine( outDir, pn + "_java_switch.txt" ) );
            }
            catch( Exception ex )
            {
                return ex.Message;
            }
            #endregion

            #region 生成 EnumToString.java
            sb.Clear();
            sb.Append( @"
public class EnumToString
{" );
            foreach( var e in enums )
            {
                sb.Append( @"
    public static string Get(" + e.Name + @" e )
    {
        switch( e )
        {" );
                foreach( var ee in e.Members )
                {
                    sb.Append( @"
            case " + e.Name + "." + ee.Name + @": 
                return """ + e.Name + "." + ee.Name + " " + ee.Desc + @""";" );

                }
                sb.Append( @"
        }
    }" );

                sb.Append( @"
    public static string GetName(" + e.Name + @" e )
    {
        switch( e )
        {" );
                foreach( var ee in e.Members )
                {
                    sb.Append( @"
            case " + e.Name + "." + ee.Name + @": 
                return """ + e.Name + "." + ee.Name + @""";" );

                }
                sb.Append( @"
        }
    }" );

                sb.Append( @"
    public static string GetDesc(" + e.Name + @" e )
    {
        switch( e )
        {" );
                foreach( var ee in e.Members )
                {
                    sb.Append( @"
            case " + e.Name + "." + ee.Name + @": 
                return """ + e.Desc + @""";" );

                }
                sb.Append( @"
        }
    }" );

            }
            sb.Append( @"
}" );
            try
            {
                sb.WriteToFile( Path.Combine( outDir, "EnumToString.java" ) );
            }
            catch( Exception ex )
            {
                return ex.Message;
            }
            #endregion

            return "";
        }

        public static string GetComment( string s, int space )
        {
            if( s.Trim() == "" ) return "";
            var sps = new string( ' ', space );
            return "\r\n" +
    sps + @"/*
" + sps + @" * " + s + @"
" + sps + @"*/";
        }

        public static string GetTypeKeyword( DataType d )
        {
            string rtv = " ";
            if( d.Category == DataTypeCategories.Array )
            {
                rtv = GetTypeKeyword( d.ChildType ) + "[]";
            }
            else if( d.Category == DataTypeCategories.BuiltIn )
            {
                switch( d.Name )
                {
                    case "Byte": rtv = "byte"; break;
                    case "SByte": rtv = "byte"; break;
                    case "UInt16": rtv = "short"; break;
                    case "Int16": rtv = "short"; break;
                    case "UInt32": rtv = "int"; break;
                    case "Int32": rtv = "int"; break;
                    case "UInt64": rtv = "long"; break;
                    case "Int64": rtv = "long"; break;
                    case "Double": rtv = "double"; break;
                    case "Single": rtv = "float"; break;
                    case "Boolean": rtv = "boolean"; break;
                    case "DateTime": rtv = "Date"; break;
                    default:
                        rtv = d.Name;
                        break;
                }
            }

            else if( d.Category == DataTypeCategories.Custom )
            {
                rtv = d.Name;
            }
            else
            {
                rtv = d.Name + "<";
                for( int i = 0; i < d.ChildTypes.Count; ++i )
                {
                    if( i > 0 ) rtv += ",";
                    rtv += GetTypeKeyword( d.ChildTypes[ i ] );
                }
                rtv += ">";

            }
            return rtv;
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

        public static string GetWriteTypeBuffer( DataType d )
        {
            string rtv = " ";
            if( d.Category == DataTypeCategories.BuiltIn )
            {
                rtv = "Write";
                return rtv;
            }
            else if( d.Category == DataTypeCategories.Custom )
            {
                if( d.IsEnum )
                {
                    rtv = "Write";
                    return rtv;
                }
                else
                {
                    rtv = "Write";
                    return rtv;
                }
            }
            else if( d.Category == DataTypeCategories.Generic )
            {
                var xxx = d.ChildTypes[0];
                if( xxx.Category == DataTypeCategories.BuiltIn )
                {
                    switch( xxx.Name )
                    {
                        case "Byte": rtv = "WriteListByte"; break;
                        case "Int16": rtv = "WriteListShort"; break;
                        case "Int32": rtv = "WriteListInteger"; break;
                        case "Int64": rtv = "WriteListLong"; break;
                        case "Char": rtv = "WriteListCharacter"; break;
                        case "Double": rtv = "WriteListDouble"; break;
                        case "Single": rtv = "WriteListFloat"; break;
                        case "Boolean": rtv = "WriteListBoolean"; break;
                        case "DateTime": rtv = "WriteListDate"; break;
                        case "String": rtv = "WriteListString"; break;

                    }
                }
                else if( xxx.Category == DataTypeCategories.Custom )
                {
                    rtv = "WriteListClass";
                }
            }
            return rtv;
        }

        public static string GetReadTypeBuffer( DataType d )
        {
            string rtv = " ";
            if( d.Category == DataTypeCategories.BuiltIn )
            {
                switch( d.Name )
                {
                    case "Byte": rtv = "ReadByte"; break;
                    case "Int16": rtv = "ReadShort"; break;
                    case "Int32": rtv = "ReadInteger"; break;
                    case "Int64": rtv = "ReadLong"; break;
                    case "Char": rtv = "ReadCharacter"; break;
                    case "Double": rtv = "ReadDouble"; break;
                    case "Single": rtv = "ReadFloat"; break;
                    case "Boolean": rtv = "ReadBoolean"; break;
                    case "DateTime": rtv = "ReadDate"; break;
                    case "String": rtv = "ReadString"; break;
                    default:
                        rtv = d.Name;
                        break;
                }
            }
            else if( d.Category == DataTypeCategories.Custom )
            {
                if( d.IsEnum )
                {
                    switch( GetEnumTypeKeyword( d.Custom ) )
                    {
                        case "byte": rtv = "ReadByte"; break;
                        case "sbyte": rtv = "ReadByte"; break;
                        case "ushort": rtv = "ReadShort"; break;
                        case "short": rtv = "ReadShort"; break;
                        case "uint": rtv = "ReadInteger"; break;
                        case "int": rtv = "ReadInteger"; break;
                        case "ulong": rtv = "ReadLong"; break;
                        case "long": rtv = "ReadLong"; break;
                    }

                    return rtv;
                }
                else
                {
                    rtv = "ReadClass";
                    return rtv;
                }
            }
            else if( d.Category == DataTypeCategories.Generic )
            {
                switch( GetReadTypeBuffer( d.ChildType ) )
                {
                    case "ReadByte": rtv = "ReadListByte"; break;
                    case "ReadShort": rtv = "ReadListShort"; break;
                    case "ReadInteger": rtv = "ReadListInteger"; break;
                    case "ReadLong": rtv = "ReadListLong"; break;
                    case "ReadCharacter": rtv = "ReadListCharacter"; break;
                    case "ReadDouble": rtv = "ReadListDouble"; break;
                    case "ReadFloat": rtv = "ReadListFloat"; break;
                    case "ReadBoolean": rtv = "ReadListBoolean"; break;
                    case "ReadDate": rtv = "ReadListDate"; break;
                    case "ReadString": rtv = "ReadListString"; break;
                    case "ReadClass": rtv = "ReadListClass"; break;
                }
            }
            return rtv;
        }

        public static string GetBufferString( DataType d )
        {
            string rtv = " ";
            if( d.Category == DataTypeCategories.BuiltIn )
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
                    case "String": rtv = "string"; break;
                    // case "List<int>": rtv = "xxx::List<int>"; break;
                    default:
                        rtv = d.Name;
                        break;
                }
            }

            else if( d.Category == DataTypeCategories.Custom )
            {
                rtv = d.Name;
            }

            return rtv;
        }

        public static string GetEnumFristString( Struct e )
        {
            return e.Name + "." + e.Members[ 0 ].Name;
        }


        #region GetDefaultValueByType

        public static string GetDefaultValueByType( Member f )
        {
            return GetDefaultValueByType( f.Type );
        }
        public static string GetDefaultValueByType( DataType d )
        {
            if( d.Category == DataTypeCategories.Array )                // 当前特指 byte[]
            {
                return "new byte[ 0 ]";   // "null"
            }
            else if( d.Category == DataTypeCategories.Generic )
            {
                return "new ArrayList<" + GetTypeKeyword( d ) + ">()";   // "null"
            }
            else if( d.Category == DataTypeCategories.BuiltIn )
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
                    case "DateTime": return "new Date( 1, 0, 1, 0, 0, 0 )";
                    case "ByteBuffer": return "new ByteBuffer()";
                    default:
                        throw new Exception( "unhandled data type" );
                }
            }
            else    // custom / enum
            {
                if( d.IsEnum )
                {
                    return (d.Namespace != "" ? d.Namespace : _pn) + "." + d.Name + "." + d.Custom.Members.First().Name;
                }
                else
                {
                    return "new " + (d.Namespace != "" ? d.Namespace : _pn) + "." + d.Name + "()";    // "null"; 
                }
            }
        }

        #endregion


    }
}
