using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using xxlib;

namespace vs_cs
{
    class Program
    {
        static void Main( string[] args )
        {

        }
    }
}





//using System;
//using System.Collections.Generic;
//using System.Diagnostics;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using xxlib;

//namespace vs_cs
//{
//    class Program
//    {
//        class Fooo { }
//        static void Main( string[] args )
//        {
//            //var bb = new ByteBuffer();
//            //var bs = new List<bool>();
//            //for( int i = 0; i < 32; ++i )
//            //{
//            //    bs.Add( ( i & 1 ) > 0 );
//            //}
//            //// 预热
//            //for( int i = 0; i < 999999; ++i )
//            //{
//            //    bb.Write( bs );
//            //}
//            //bb.Clear();
//            //var sw = Stopwatch.StartNew();
//            //for( int i = 0; i < 999999; ++i )
//            //{
//            //    bb.Write( bs );
//            //}
//            //Console.Write( "C# .NET List<bool>[ 32 ] >> bb 999999 times, elapsed ms = " + sw.ElapsedMilliseconds + '\n' );
//            //sw.Restart();
//            //while( bb.offset < bb.dataLen )
//            //{
//            //    bb.Read( ref bs );
//            //}
//            //Console.Write( "C# .NET bb >> List<bool> 999999 times, elapsed ms = " + sw.ElapsedMilliseconds + '\n' );
//            //return;


//            //var bb = new ByteBuffer();
//            //{
//            //    double d = 0;
//            //    bb.VarWrite( d );
//            //    d = 12;
//            //    bb.VarWrite( d );
//            //    d = 1.234;
//            //    bb.VarWrite( d );
//            //    bb.Dump();
//            //}
//            //do
//            //{
//            //    double d1 = 0, d2 = 0, d3 = 0;
//            //    try
//            //    {
//            //        bb.VarRead( ref d1 );
//            //        bb.VarRead( ref d2 );
//            //        bb.VarRead( ref d3 );
//            //    }
//            //    catch( Exception ex )
//            //    {
//            //        break;
//            //    }
//            //    Console.WriteLine( d1 + " " + d2 + " " + d3 );
//            //}
//            //while( false );


//            var bb = new ByteBuffer();
//            var s = Stopwatch.StartNew();
//            for( int i = 0; i < 9; ++i )
//            {
//                var o = new CS.RootPkg1();
//                o.m_root_e1 = CS.Enum1.Item3;
//                o.m_ns1_pkg2.ns1_pkg1.ns1_enum1 = CS.Ns1.Enum1.item1;
//                o.m_ns1_pkg2.re1 = CS.Enum1.Item3;
//                o.m_byte = 12;
//                o.m_ushort = 12345;
//                o.m_uint = 12345678;
//                o.m_ulong = 1234567890123L;
//                o.m_sbyte = -12;
//                o.m_short = -12345;
//                o.m_int = -12345678;
//                o.m_long = -1234567890123L;
//                o.m_double = 1.234567890123;
//                o.m_float = 1.2345f;
//                o.m_bool = true;
//                o.m_DateTime = new DateTime( 2015, 5, 5, 0, 0, 0 );
//                o.m_string = "123456";
//                o.m_ByteBuffer.Write( (byte)1 );
//                o.m_ByteBuffer.Write( (byte)2 );
//                o.m_ByteBuffer.Write( (byte)3 );
//                o.m_bytes[ 0 ] = 0;
//                o.m_bytes[ 1 ] = 1;
//                o.m_bytes[ 6 ] = 7;
//                o.m_bytes[ 7 ] = 7;
//                o.m_ns2enum1s.Add( CS.Ns2.Enum1.item1 );
//                o.m_ns2enum1s.Add( CS.Ns2.Enum1.item1 );
//                o.m_ns2enum1s.Add( CS.Ns2.Enum1.item1 );
//                for( int j = 0; j < 32; ++j )
//                {
//                    o.m_bools.Add( j % 2 == 0 );
//                }

//                bb.Clear();
//                bb.Write( o );
//            }
//            Console.WriteLine( s.ElapsedMilliseconds );
//            bb.Dump();
//            {
//                var o = new CS.RootPkg1();
//                bb.Read( ref o );
//            }
//        }
//    }
//}
