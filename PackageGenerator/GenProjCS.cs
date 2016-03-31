using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace PackageGenerator
{

    /// <summary>
    /// 本生成物只支持带项目标记的模板( 换句话说，如果统统没有，是否可以加一个默认的 )
    /// </summary>
    public static class GenProjCS
    {
        public static void Gen_tn_pn_ByteBuffer_Partial(StringBuilder sb, List<Struct> proj_enums)
        {

            sb.Clear();
            // 文件头部
            sb.Append(@"using System;
using System.Collections.Generic;
using System.Text;

namespace xxlib
{
    partial class ByteBuffer
    {");
            foreach (var e in proj_enums)
            {
                sb.Append(@"
        public void Write( " + GetNamespace(e) + "." + e.Name + @" v )
        {
            Write( (" + GetEnumTypeKeyword(e) + @")v );
        }
        public void Write( " + GetNamespace(e) + "." + e.Name + @"[] vs )
        {
            for( int i = 0; i < vs.Length; i++ )
            {
                Write( (" + GetEnumTypeKeyword(e) + @")vs[ i ] );
            }
        }
        public void Write( List<" + GetNamespace(e) + "." + e.Name + @"> vs )
        {
            WriteLength( vs.Count );
            foreach( var v in vs )
            {
                Write( (" + GetEnumTypeKeyword(e) + @")v );
            }
        }
        public void Read( ref " + GetNamespace(e) + "." + e.Name + @" v )
        {
            " + GetEnumTypeKeyword(e) + @" tmp = 0;
            Read( ref tmp );
            v = (" + GetNamespace(e) + "." + e.Name + @")tmp;
        }
        public void Read( ref " + GetNamespace(e) + "." + e.Name + @"[] vs )
        {
            " + GetEnumTypeKeyword(e) + @" tmp = 0;
            for( int i = 0; i < vs.Length; i++ )
            {
                Read( ref tmp );
                vs[ i ] = (" + GetNamespace(e) + "." + e.Name + @")tmp;
            }
        }
        public void Read( ref List<" + GetNamespace(e) + "." + e.Name + @"> vs )
        {
            Read( ref vs, 0, 0 );
        }
        public void Read( ref List<" + GetNamespace(e) + "." + e.Name + @"> vs, int minLen, int maxLen )
        {
            int len = ReadLength( minLen, maxLen );
            vs.Clear();
            for( int i = 0; i < len; i++ )
            {
                " + GetEnumTypeKeyword(e) + @" tmp = 0;
                Read( ref tmp );
                vs.Add( (" + GetNamespace(e) + "." + e.Name + @")tmp );
            }
        }
");
            }
            sb.Append(@"
    }
}
");
        }

        public static void Gen_tn_pn(StringBuilder sb, Template t, List<Struct> proj_enums, List<Struct> proj_packages_and_structs)
        {
            // 文件头部
            sb.Clear();
            sb.Append(@"using System;
using System.Collections.Generic;
using xxlib;");


            // namespace root {
            sb.Append(@"
namespace " + t.Name + @"
{");

            /*******************************************************/
            // 所有项目枚举
            /*******************************************************/

            foreach (var e in proj_enums)
            {
                // namespace xxx {
                if (e.Namespace != "")
                {
                    sb.Append(@"
namespace " + e.Namespace + @"
{");
                }

                // ///  /// desc  ///
                sb.Append(GetComment(e.Desc, 0) + @"
public enum " + e.Name + @" : " + GetEnumTypeKeyword(e) + @"
{");
                // enum item = val
                foreach (var ef in e.Members)
                {
                    sb.Append(GetComment(ef.Desc, 4) + @"
    " + ef.Name + " = " + ef.EnumValue + ", ");
                }
                // enum class }; // xxx
                sb.Append(@"
};");

                // namespace } // xxx
                if (e.Namespace != "")
                {
                    sb.Append(@"
} // " + e.Namespace);
                }
            }



            /*******************************************************/
            // 结构体部分
            /*******************************************************/

            foreach (var c in proj_packages_and_structs)
            {
                // namespace xxx {
                if (c.Namespace != "")
                {
                    sb.Append(@"
namespace " + c.Namespace + @"
{");
                }

                // ///  /// desc  ///
                sb.Append(GetComment(c.Desc, 0) + @"
partial class " + c.Name + @"
{");
                if (c.StructType == StructTypes.Package)
                {
                    sb.Append(@"
    public const short packageId = " + c.PackageId + @";
");
                }

                // members = default vals;
                foreach (var f in c.Members)
                {
                    sb.Append(GetComment(f.Desc, 8) + @"
    " + "public " + GetTypeKeyword(f) + " " + f.Name.ToFirstLower() + " = " + GetDefaultValueByType(f) + ";");
                }

                // class }
                sb.Append(@"
};");

                // namespace } // xxx
                if (c.Namespace != "")
                {
                    sb.Append(@"
} // " + c.Namespace);
                }
            }



            // namespace } // root
            sb.Append(@"
} // " + t.Name + @"
");
        }

        public static void Gen_tn_pn_Partial(StringBuilder sb, Template t, List<Struct> proj_enums, List<Struct> proj_packages_and_structs)
        {
            // 文件头部
            sb.Clear();
            sb.Append(@"using System;
using System.Collections.Generic;
using xxlib;");

            // namespace root {
            sb.Append(@"
namespace " + t.Name + @"
{");

            /*******************************************************/
            // 结构体部分
            /*******************************************************/

            foreach (var c in proj_packages_and_structs)
            {
                // namespace xxx {
                if (c.Namespace != "")
                {
                    sb.Append(@"
namespace " + c.Namespace + @"
{");
                }

                // partial class xxx
                sb.Append(@"
public partial class " + c.Name + @" : IBBReader, IBBWriter 
{
    public void WriteTo( ByteBuffer bb )
    {");
                foreach (var f in c.Members)
                {
                    sb.Append(@"
        bb." + (f.Compress ? "" : (f.Type.Compressable() ? "Var" : "")) + "Write( " + f.Name + " );");
                }
                sb.Append(@" 
    }

    public void ReadFrom( ByteBuffer bb )
    {");
                foreach (var f in c.Members)
                {
                    sb.Append(@"
        bb." + (f.Compress ? "" : (f.Type.Compressable() ? "Var" : "")) + "Read( ref " + f.Name);
                    if (f.Type.IsContainer && !f.Type.IsArray)
                    {
                        sb.Append(@", " + f.MinLen + ", " + f.MaxLen);
                    }
                    sb.Append(@" );");
                }
                sb.Append(@"
    }
};");
                // namespace } // xxx
                if (c.Namespace != "")
                {
                    sb.Append(@"
} // " + c.Namespace);
                }
            }

            // namespace } // root
            sb.Append(@"
} // " + t.Name + @"
");
        }

        public static void Gen_tn_pn_Handlers(StringBuilder sb, Template t, List<Struct> proj_packages)
        {
            sb.Clear();
            sb.Append(@"
using System;
using System.Collections.Generic;
using xxlib;
using " + t.Name + @";

public static partial class PackageHandler
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
        {");
            foreach (var c in proj_packages)
            {
                sb.Append(@"
            case " + GetNamespace(c) + "." + c.Name + @".packageId :
            {
                var o = new " + GetNamespace(c) + "." + c.Name + @"();
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
            }");
            }

            sb.Append(@"
            default:
                return false;
        }
    }
}
");
        }

        public static void Gen_tn_pn_ByteBuffer_Ext(StringBuilder sb, List<Struct> proj_packages)
        {
            sb.Clear();
            sb.Append(@"namespace xxlib
{

    public static partial class ByteBuffer_Ext
    {");
            foreach (var c in proj_packages)
            {
                var tn = GetNamespace(c) + "." + c.Name;
                sb.Append(@"
        public static void WritePackage( this ByteBuffer bb, " + tn + @" v )
        {
            bb.Write( " + tn + @".packageId );
            bb.Write( v );
        } ");

            }
            sb.Append(@"
    }
}
");
        }



        public static string Gen(Template t, string outDir)
        {
            _tn = t.Name;

            // 分类预处理( 跨所有项目 )
            var enums = t.Structs.Where(a => a.IsEnum).ToList();
            var packages = t.Structs.Where(a => a.IsPackage).ToList();
            var packages_and_structs = t.Structs.Where(a => a.IsPackage || a.IsStruct).ToList();
            var sb = new StringBuilder();

            // 生成项目的
            foreach (var proj in t.Projects)
            {
                var proj_enums = enums.Where(a => a.Projects.Contains(proj)).ToList();
                var proj_packages_and_structs = packages_and_structs.Where(a => a.Projects.Contains(proj)).ToList();
                var proj_packages = packages.Where(a => a.Projects.Contains(proj)).ToList();

                Gen_tn_pn_ByteBuffer_Partial(sb, proj_enums);
                sb.WriteToFile(Path.Combine(outDir, t.Name + "_" + proj.Name + "_ByteBuffer_Partial.cs"));

                Gen_tn_pn(sb, t, proj_enums, proj_packages_and_structs);
                sb.WriteToFile(Path.Combine(outDir, t.Name + "_" + proj.Name + ".cs"));

                Gen_tn_pn_Partial(sb, t, proj_enums, proj_packages_and_structs);
                sb.WriteToFile(Path.Combine(outDir, t.Name + "_" + proj.Name + "_Partial.cs"));

                Gen_tn_pn_Handlers(sb, t, proj_packages);
                sb.WriteToFile(Path.Combine(outDir, t.Name + "_" + proj.Name + "_Handlers.cs"));

                Gen_tn_pn_ByteBuffer_Ext(sb, proj_enums);
                sb.WriteToFile(Path.Combine(outDir, t.Name + "_" + proj.Name + "_ByteBuffer_Ext.cs"));
            }

            // 生成全局的
            {
                var global_enums = enums.Where(a => a.Projects.Count == 0).ToList();
                var global_packages_and_structs = packages_and_structs.Where(a => a.Projects.Count == 0).ToList();
                var global_packages = packages.Where(a => a.Projects.Count == 0).ToList();

                Gen_tn_pn_ByteBuffer_Partial(sb, global_enums);
                sb.WriteToFile(Path.Combine(outDir, "_" + t.Name + "_ByteBuffer_Partial.cs"));

                Gen_tn_pn(sb, t, global_enums, global_packages_and_structs);
                sb.WriteToFile(Path.Combine(outDir, "_" + t.Name + ".cs"));

                Gen_tn_pn_Partial(sb, t, global_enums, global_packages_and_structs);
                sb.WriteToFile(Path.Combine(outDir, "_" + t.Name + "_Partial.cs"));

                Gen_tn_pn_Handlers(sb, t, global_packages);
                sb.WriteToFile(Path.Combine(outDir, "_" + t.Name + "_Handlers.cs"));

                Gen_tn_pn_ByteBuffer_Ext(sb, global_enums);
                sb.WriteToFile(Path.Combine(outDir, "_" + t.Name + "_ByteBuffer_Ext.cs"));
            }

            return "";
        }

        // 项目名上下文，省掉下面的传参了
        public static string _tn;


        #region GetComment

        public static string GetComment(string s, int space)
        {
            if (s.Trim() == "")
                return "";
            var sps = new string(' ', space);
            return "\r\n" +
sps + @"/// <summary>
" + sps + @"/// " + s + @"
" + sps + @"/// </summary>";
        }

        #endregion


        #region GetDefaultValueByType

        public static string GetDefaultValueByType(Member f)
        {
            return GetDefaultValueByType(f.Type, f);
        }
        public static string GetDefaultValueByType(DataType d, Member f = null)
        {
            if (d.IsArray)
            {
                return "new " + GetTypeKeyword(d.ChildType) + "[ " + f.MinLen + " ]";   // "null"
            }
            else if (d.IsGeneric)
            {
                return "new " + d.Name + "<" + GetTypeKeyword(d.ChildType) + ">()";   // "null"
            }
            else if (d.IsBuiltIn)
            {
                switch (d.Name)
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
                case "Single":
                    return "0";
                case "Boolean":
                case "Bool":
                    return "false";
                case "String":
                    return "\"\"";
                case "DateTime":
                    return "new DateTime( 1991, 1, 1, 0, 0, 0 )"; //"DateTime.MinValue";
                case "ByteBuffer":
                    return "new ByteBuffer()";
                default:
                    throw new Exception("unhandled data type");
                }
            }
            else    // custom / enum
            {
                if (d.IsEnum)
                {
                    return GetNamespace(d) + "." + d.Name + "." + d.Custom.Members.First().Name;
                }
                else
                {
                    return "new " + GetNamespace(d) + "." + d.Name + "()";    // "null"; 
                }
            }
        }

        #endregion


        #region GetTypeKeyword

        public static string GetTypeKeyword(Member d)
        {
            return GetTypeKeyword(d.Type);
        }
        public static string GetTypeKeyword(DataType d)
        {
            if (d.IsArray)                // 当前特指 byte[]
            {
                return GetTypeKeyword(d.ChildType) + "[]";
            }
            else if (d.IsGeneric)
            {
                string rtv = d.Name + "<";
                for (int i = 0; i < d.ChildTypes.Count; ++i)
                {
                    if (i > 0)
                        rtv += ", ";
                    rtv += GetTypeKeyword(d.ChildTypes[i]);
                }
                rtv += ">";
                return rtv;
            }
            else if (d.IsBuiltIn)
            {
                switch (d.Name)
                {
                case "Byte":
                    return "byte";
                case "UInt8":
                    return "byte";
                case "UInt16":
                    return "ushort";
                case "UInt32":
                    return "uint";
                case "UInt64":
                    return "ulong";
                case "SByte":
                    return "sbyte";
                case "Int8":
                    return "sbyte";
                case "Int16":
                    return "short";
                case "Int32":
                    return "int";
                case "Int64":
                    return "long";
                case "Double":
                    return "double";
                case "Float":
                    return "float";
                case "Single":
                    return "float";
                case "Boolean":
                    return "bool";
                case "Bool":
                    return "bool";
                case "String":
                    return "string";
                case "DateTime":
                    return "DateTime";
                case "ByteBuffer":
                    return "ByteBuffer";
                default:
                    throw new Exception("unhandled data type");
                }

            }
            else // ( d.IsCustom )
            {
                return GetNamespace(d) + "." + d.Name;
            }
        }
        #endregion


        #region GetEnumTypeKeyword

        public static string GetEnumTypeKeyword(Member f)
        {
            return GetEnumTypeKeyword(f.Type.Custom);
        }
        public static string GetEnumTypeKeyword(Struct e)
        {
            switch (e.EnumUnderlyingType)
            {
            case "Byte":
                return "byte";
            case "SByte":
                return "sbyte";
            case "UInt16":
                return "ushort";
            case "Int16":
                return "short";
            case "UInt32":
                return "uint";
            case "Int32":
                return "int";
            case "UInt64":
                return "ulong";
            case "Int64":
                return "long";
            }
            throw new Exception("unsupported data type");
        }

        #endregion


        #region GetByteBufferReadFuncName
        public static string GetByteBufferReadFuncName(DataType d)
        {
            string rtv = " ";
            if (d.IsBuiltIn)
            {
                switch (d.Name)
                {

                case "Byte":
                    rtv = "ReadByte";
                    break;
                case "Int16":
                    rtv = "ReadShort";
                    break;
                case "Int32":
                    rtv = "ReadInt";
                    break;
                case "Int64":
                    rtv = "ReadLong";
                    break;
                case "Char":
                    rtv = "ReadChar";
                    break;
                case "Double":
                    rtv = "ReadDouble";
                    break;
                case "Single":
                    rtv = "ReadFloat";
                    break;
                case "Boolean":
                    rtv = "ReadBoolean";
                    break;
                case "DateTime":
                    rtv = "ReadDate";
                    break;
                case "String":
                    rtv = "ReadString";
                    break;
                default:
                    rtv = (d.Namespace != "" ? (d.Namespace + ".") : "") + d.Name;
                    break;
                }

            }
            else if (d.IsCustom)
            {
                if (d.IsEnum)
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
            else if (d.IsGeneric)
            {
                if (d.IsArray)
                {
                    rtv = "ReadBytes";      // 当前只支持 byte[]
                }
                else
                {
                    switch (GetByteBufferReadFuncName(d.ChildType))
                    {
                    case "ReadByte":
                        rtv = "ReadListByte";
                        break;
                    case "ReadShort":
                        rtv = "ReadListShort";
                        break;
                    case "ReadInt":
                        rtv = "ReadListInt";
                        break;
                    case "ReadLong":
                        rtv = "ReadListLong";
                        break;
                    case "ReadChar":
                        rtv = "ReadListChar";
                        break;
                    case "ReadDouble":
                        rtv = "ReadListDouble";
                        break;
                    case "ReadFloat":
                        rtv = "ReadListFloat";
                        break;
                    case "ReadBoolean":
                        rtv = "ReadListBool";
                        break;
                    case "ReadDate":
                        rtv = "ReadListDate";
                        break;
                    case "ReadString":
                        rtv = "ReadListString";
                        break;
                    case "Read":
                        rtv = "ReadListObject";
                        break;
                    }
                }
            }
            return rtv;
        }

        #endregion


        #region GetNamespace

        public static string GetNamespace(DataType e)
        {
            if (e.Namespace == "")
            {
                return "global::" + _tn;
            }
            else
            {
                return "global::" + _tn + "." + e.Namespace;
            }
        }
        public static string GetNamespace(Struct e)
        {
            if (e.Namespace == "")
            {
                return "global::" + _tn;
            }
            else
            {
                return "global::" + _tn + "." + e.Namespace;
            }
        }

        #endregion

    }
}
