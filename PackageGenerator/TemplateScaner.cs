using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using LIB = PackageLibrary;

namespace PackageGenerator
{
    public static class TemplateScaner
    {
        public static string libNS = "PackageLibrary";          // 重要：这个字串需要保持与模板 Lib 名一致
        public static string libNSdot = libNS + ".";

        public static Template GetTemplate( Assembly asm )
        {
            var template = new Template();

            // 扫枚举
            var r_enums = ( from t in asm.GetTypes() where ( t.IsEnum ) select t ).ToList();
            foreach( var r_enum in r_enums )
            {
                var e = new Struct();
                e.StructType = StructTypes.Enum;
                e.Namespace = r_enum.Namespace ?? "";
                e.Name = r_enum.Name.ToString();
                e.Desc = r_enum.GetAttrDesc();
                var ut = r_enum.GetEnumUnderlyingType();
                e.EnumUnderlyingType = ut.Name;

                var r_fields = r_enum.GetFields( BindingFlags.Static | BindingFlags.Public );
                if( r_fields.Count() == 0 )
                {
                    throw new Exception( "enum " + r_enum.FullName + " can't be empty" );
                }
                foreach( var r_field in r_fields )
                {
                    var ef = new Member();
                    ef.Parent = e;
                    ef.Name = r_field.Name;
                    ef.Desc = r_field.GetAttrDesc();
                    ef.EnumValue = r_field.GetValue( null ).ToIntegerString( ut );
                    e.Members.Add( ef );
                }
                template.Structs.Add( e );
            }

            // 浅扫包和结构体
            // Members 在下面填, 需要先确保容器有所有类才方便引用
            var r_classes = from t in asm.GetTypes() where ( t.IsClass || t.IsValueType && !t.IsEnum ) && t.Namespace != libNS select t;
            short pkgid = 0;
            foreach( var r_class in r_classes )
            {
                var c = new Struct();
                c.StructType = r_class.GetAttrStruct() ? StructTypes.Struct : StructTypes.Package;
                c.Namespace = r_class.Namespace ?? "";
                c.Name = r_class.Name;
                c.Desc = r_class.GetAttrDesc();
                // c.Member ...
                if( c.IsPackage ) c.PackageId = pkgid++;      // 填包自增
                template.Structs.Add( c );
            }

            // 继续扫包和结构体
            foreach( var r_class in r_classes )
            {
                // 定位到已填充的 struct
                var c = template.Structs.Find( a => a.Name == r_class.Name && a.Namespace == ( r_class.Namespace ?? "" ) );

                // 扫继承字段( 展开到当前类 )
                var r_fields = r_class.GetFields( BindingFlags.NonPublic | BindingFlags.Public | BindingFlags.Instance ).ToList();
                if( r_class.BaseType != typeof( object ) )
                {
                    r_fields.InsertRange( 0, r_class.BaseType.GetFields( BindingFlags.NonPublic | BindingFlags.Public | BindingFlags.Instance ) );
                }

                // 扫字段
                foreach( var r_field in r_fields )
                {
                    var f = new Member();
                    f.Parent = c;
                    f.Name = r_field.Name;
                    f.Desc = r_field.GetAttrDesc();
                    FillDataType( template, f.Type, r_field.FieldType );
                    if( f.Type.IsContainer )
                    {
                        var limit = r_field.GetAttrLimit();
                        f.MinLen = limit.Key;
                        f.MaxLen = limit.Value;
                    }
                    if( f.Type.IsArray
                        && (f.MinLen != f.MaxLen || f.MinLen == 0) )
                    {
                        throw new Exception( r_class.FullName + "." + r_field.Name + " need Limit( n )" );
                    }
                    f.NoCompress = r_field.GetAttrNoCompress();
                    c.Members.Add( f );
                }
            }

            // 算 RefCounter
            var structs = template.Structs.Where( a => !a.IsEnum ).ToList();
            foreach( var s in structs )
            {
                IncRefCounter( s );
            }


            // 整理命名空间
            template.Namespaces = template.Structs.Select( a => a.Namespace ).Distinct().ToList();

            return template;
        }


        public static void IncRefCounter( Struct s )
        {
            // todo: 递归引用检测
            foreach( var m in s.Members )
            {
                if( m.Type.IsStruct || m.Type.IsPackage )
                {
                    m.Type.Custom.RefCounter++;
                    IncRefCounter( m.Type.Custom );
                }
            }
        }

        public static string GetAttrDesc<T>( this T t ) where T : _MemberInfo
        {
            foreach( var r_attribute in t.GetCustomAttributes( false ) )
            {
                if( r_attribute is LIB.Desc ) return ((LIB.Desc)r_attribute).Value;
            }
            return "";
        }
        public static bool GetAttrStruct<T>( this T t ) where T : _MemberInfo
        {
            foreach( var r_attribute in t.GetCustomAttributes( false ) )
            {
                if( r_attribute is LIB.Struct ) return true;
            }
            return false;
        }
        public static bool GetAttrNoCompress<T>( this T t ) where T : _MemberInfo
        {
            foreach( var r_attribute in t.GetCustomAttributes( false ) )
            {
                if( r_attribute is LIB.NoCompress ) return true;
            }
            return false;
        }

        public static KeyValuePair<int, int> GetAttrLimit<T>( this T t ) where T : _MemberInfo
        {
            foreach( var r_attribute in t.GetCustomAttributes( false ) )
            {
                if( r_attribute is LIB.Limit )
                {
                    return new KeyValuePair<int, int>( ((LIB.Limit)r_attribute).Min, ((LIB.Limit)r_attribute).Max );
                }
            }
            return new KeyValuePair<int, int>( 0, 0 );
        }




        // 转枚举值专用
        public static string ToIntegerString( this object enumValue, System.Type ut )
        {
            switch( ut.Name )
            {
                case "Byte": return Convert.ToByte( enumValue ).ToString();
                case "SByte": return Convert.ToSByte( enumValue ).ToString();
                case "UInt16": return Convert.ToUInt16( enumValue ).ToString();
                case "Int16": return Convert.ToInt16( enumValue ).ToString();
                case "UInt32": return Convert.ToUInt32( enumValue ).ToString();
                case "Int32": return Convert.ToInt32( enumValue ).ToString();
                case "UInt64": return Convert.ToUInt64( enumValue ).ToString();
                case "Int64": return Convert.ToInt64( enumValue ).ToString();
            }
            throw new Exception( "unknown Underlying Type" );
        }



        public static void FillDataType( Template template, DataType d, System.Type t )
        {
            var tn = t.Name;
            if( t.IsArray )
            {
                d.Category = DataTypeCategories.Array;
                var cd = new DataType();
                d.ChildTypes.Add( cd );
                FillDataType( template, cd, t.GetElementType() );
            }
            else if( t.IsGenericType )
            {
                if( t.Namespace != libNS )
                    throw new Exception( "unknown data type." );
                d.Category = DataTypeCategories.Generic;
                d.Name = tn.Substring( 0, tn.LastIndexOf( '`' ) );
                d.Namespace = "";
                foreach( var ct in t.GenericTypeArguments )
                {
                    var cd = new DataType();
                    d.ChildTypes.Add( cd );
                    FillDataType( template, cd, ct );
                }
            }
            else if( t.Namespace == "System" || t.Namespace == libNS )
            {
                switch( t.Name )
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
                    case "Single":
                    case "Boolean":
                    case "String":
                    case "DateTime":
                    case "ByteBuffer":
                        d.Category = DataTypeCategories.BuiltIn;
                        d.Name = t.Name;
                        d.Namespace = "";
                        break;

                    default:
                        throw new Exception( "unknown data type." );
                }
            }
            else
            {
                d.Category = DataTypeCategories.Custom;
                d.Name = t.Name;
                d.Namespace = t.Namespace ?? "";
                d.Custom = template.Structs.Find( a => a.Name == t.Name && a.Namespace == d.Namespace );
            }
        }



    }
}

//else if( a is Enabled ) fps.Enabled = ( (Enabled)a ).Value;
//else if( a is DecodeCondation ) fps.DecodeCondation = (DecodeCondation)a;
//else if( a is EncodeCondation ) fps.EncodeCondation = (EncodeCondation)a;
// "ChkValue": fps.MinValue = ((ChkValue)a).Min; fps.MinValue = ((ChkValue)a).Max; break;

// 扫空 interface（用于 Decode, Encode 的参数列表）
//var interfaces = from t in asm.GetTypes() where t.IsInterface && t.Namespace != libNS select t;
//foreach( var i in interfaces )
//{
//    var p = new Project { Name = i.Name };
//    foreach( var a in i.GetCustomAttributes( false ) )
//    {
//        if( a is Desc ) p.Desc = ( (Desc)a ).Value;
//        // more proj attributes
//    }
//    template.Projects.Add( p );
//}
//else if( a is Enabled ) cps.Enabled = ( (Enabled)a ).Value;
//else if( a is Opcode ) cps.Opcode = ( (Opcode)a ).Value;
//else if( a is Returns ) cps.Results = ( (Returns)a ).Value.ToList();
//else if( a is CallBy ) cps.CallBy = ( (CallBy)a ).Value.ToList();


// 扫项目列表
//var r_projenums = from t in asm.GetTypes() where ( t.IsEnum ) && t.Namespace != libNS && t.Name == projEnum select t;
//if( r_projenums.Count() > 0 )
//{
//    var e = r_projenums.First();
//    var r_fields = e.GetFields( BindingFlags.Static | BindingFlags.Public );
//    foreach( var r_field in r_fields )
//    {
//        var p = new Project { Name = r_field.Name };
//        foreach( var a in r_field.GetCustomAttributes( false ) )
//        {
//            if( a is LIB.Desc ) p.Desc = ( (LIB.Desc)a ).Value;
//        }
//        template.Projects.Add( p );
//    }
//}
//else if( r_attribute is LIB.Default ) f.Default = ((LIB.Default)r_attribute).Value;
//else if( r_attribute is LIB.Get ) f.Get = ((LIB.Get)r_attribute).Value;
//else if( r_attribute is LIB.Get ) f.Get = ((LIB.Get)r_attribute).Value;
//else if( r_attribute is LIB.Limits ) { fillDeclareLimits( f.Declare, (LIB.Limits)r_attribute ); }
//else if( r_attribute is LIB.Condation )
//{
//    var ps = ( (LIB.Condation)r_attribute ).Value;
//    for( int i = 0; i < ps.Length; i += 2 )
//    {
//        // todo: 检查如果被引用的 fields 位于当前 field 的后方，条件非法
//        f.Condations.Add( c.Fields.Find( a => a.Name == (string)ps[ i ] ), ps[ i + 1 ] );
//    }
//}
//else if( r_attribute is LIB.Decode ) c.Decode.AddRange( ( (LIB.Decode)r_attribute ).Value.Select( o => template.Projects.FirstOrDefault( oo => oo.Name == o.ToString() ) ) );
//else if( r_attribute is LIB.Encode ) c.Encode.AddRange( ( (LIB.Encode)r_attribute ).Value.Select( o => template.Projects.FirstOrDefault( oo => oo.Name == o.ToString() ) ) );
//public static void fillDeclareLimits( Declare d, LIB.Limits ls, int i = 0 )
//{
//    if( d.Category == DataCategories.Array )
//    {
//        if( i >= ls.Value.Length )
//        {
//            throw new Exception( "the Limits is not enough length." );
//        }
//        d.MinLen = ls.Value[ i++ ];
//        fillDeclareLimits( d.Childs[ 0 ], ls, i );
//    }
//    else if( d.Category == DataCategories.Generic )
//    {
//        if( i >= ls.Value.Length )
//        {
//            throw new Exception( "the Limits is not enough length." );
//        }
//        d.MinLen = ls.Value[ i++ ];
//        if( i >= ls.Value.Length )
//        {
//            throw new Exception( "the Limits is not enough length." );
//        }
//        d.MaxLen = ls.Value[ i++ ];
//        foreach( var cd in d.Childs )
//        {
//            fillDeclareLimits( cd, ls, i );
//        }
//    }
//}           

//// 整理引用关系, 调整 Encode, Decode
//foreach( var c in template.Classes )
//{
//    // todo
//}

//// 整理 enable 属性( 合并 encode & decode )
//foreach( var c in template.Classes )
//{
//    if( c.Encode.Count + c.Decode.Count == 0 )
//        c.Enable = c.Encode = c.Decode = template.Projects;
//    else
//        c.Enable = c.Encode.Concat( c.Decode ).Distinct().ToList();
//}
//public static string projEnum = "__projects";           // 重要：生成过程中通过这个枚举来识别项目分类
