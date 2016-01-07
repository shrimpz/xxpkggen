using System;
using System.Collections.Generic;
using System.Text;

namespace xxlib
{
    #region interface IBBReader/Writer
    public interface IBBReader
    {
        void ReadFrom( ByteBuffer tb );
    }


    public interface IBBWriter
    {
        void WriteTo( ByteBuffer tb );
    }
    #endregion

    public partial class ByteBuffer
    {
        public byte[] buf;
        public int offset;
        public int dataLen;

        #region constructors

        public ByteBuffer( int capacity )
        {
            Assign( new byte[ capacity ], 0 );
        }

        public ByteBuffer()
            : this( 1024 )
        {
        }

        public ByteBuffer( byte[] buf_, int dataLen_ )
        {
            Assign( buf_, dataLen_ );
        }

        public ByteBuffer( byte[] buf_ )
        {
            Assign( buf_ );
        }

        #endregion

        #region tool funcs

        public void Assign( byte[] buf_, int dataLen_ )
        {
            offset = 0;
            buf = buf_;
            dataLen = dataLen_;
        }

        public void Assign( byte[] buf_ )
        {
            Assign( buf_, 0 );
        }

        public void Clear()
        {
            dataLen = offset = 0;
        }

        public bool Empty()
        {
            return dataLen == 0;
        }

        public bool Full()
        {
            return dataLen == offset;
        }

        public void Collapse()
        {
            Array.Resize( ref buf, dataLen );
        }

        public void Reserve( int capacity )
        {
            if( capacity <= buf.Length )
            {
                return;
            }
            byte[] newbuf = new byte[ capacity * 2 ];
            Array.Copy( buf, 0, newbuf, 0, dataLen );
            buf = newbuf;
        }

        public byte IndexAt( int idx )
        {

            return buf[ idx ];
        }

        #endregion

        #region dump / tostring func

        public void Dump( ref StringBuilder s )
        {
            BinaryHelper.Dump( ref s, buf, dataLen );
        }
        public void Dump()
        {
            var s = new StringBuilder();
            Dump( ref s );
            Console.Write( s );
        }

        public override string ToString()
        {
            var s = new StringBuilder();
            Dump( ref s );
            return s.ToString();
        }

        #endregion

        #region write funcs

        public void Write( byte v )
        {
            Reserve( dataLen + 1 );
            buf[ dataLen ] = v;
            dataLen++;
        }
        public void Write( sbyte v )
        {
            Write( (byte)v );
        }
        public void Write( ushort v )
        {
            Reserve( dataLen + 2 );
            buf[ dataLen ] = (byte)(v & 0xff);
            buf[ dataLen + 1 ] = (byte)((v & 0xff00) >> 8);
            dataLen += 2;
        }
        public void Write( short v )
        {
            Write( (ushort)v );
        }
        public void Write( uint v )
        {
            Reserve( dataLen + 4 );
            buf[ dataLen ] = (byte)(v & 0xff);
            buf[ dataLen + 1 ] = (byte)((v & 0xff00) >> 8);
            buf[ dataLen + 2 ] = (byte)((v & 0xff0000) >> 16);
            buf[ dataLen + 3 ] = (byte)((v & 0xff000000) >> 24);
            dataLen += 4;
        }
        public void Write( int v )
        {
            Write( (uint)v );
        }
        public void Write( ulong v )
        {
            Reserve( dataLen + 8 );
            buf[ dataLen + 0 ] = (byte)v;
            buf[ dataLen + 1 ] = (byte)(v >> 8);
            buf[ dataLen + 2 ] = (byte)(v >> 16);
            buf[ dataLen + 3 ] = (byte)(v >> 24);
            buf[ dataLen + 4 ] = (byte)(v >> 32);
            buf[ dataLen + 5 ] = (byte)(v >> 40);
            buf[ dataLen + 6 ] = (byte)(v >> 48);
            buf[ dataLen + 7 ] = (byte)(v >> 56);
            dataLen += 8;
        }
        public void Write( long v )
        {
            Write( (ulong)v );
        }

        public void Write( float v )
        {
            var _buf = BitConverter.GetBytes(v);
            Reserve( dataLen + 4 );
            Array.Copy( _buf, 0, buf, dataLen, 4 );
            dataLen += 4;
        }
        public void Write( double v )
        {
            var _buf = BitConverter.GetBytes(v);
            Reserve( dataLen + 8 );
            Array.Copy( _buf, 0, buf, dataLen, 8 );
            dataLen += 8;
        }
        public void Write( bool v )
        {
            Write( v ? (byte)1 : (byte)0 );
        }
        public void Write( string v )
        {
            var sbuf = Encoding.UTF8.GetBytes( v );
            int sbufLen = sbuf.Length;
            WriteLength( sbufLen );
            Reserve( dataLen + sbufLen );
            Array.Copy( sbuf, 0, buf, dataLen, sbufLen );
            dataLen += sbufLen;
        }
        public void Write( DateTime v )
        {
            Reserve( dataLen + 8 );
            short year = (short)v.Year;
            buf[ dataLen + 0 ] = (byte)year;
            buf[ dataLen + 1 ] = (byte)(year >> 8);
            buf[ dataLen + 2 ] = (byte)v.Month;
            buf[ dataLen + 3 ] = (byte)v.Day;
            buf[ dataLen + 4 ] = (byte)v.Hour;
            buf[ dataLen + 5 ] = (byte)v.Minute;
            buf[ dataLen + 6 ] = (byte)v.Second;
            buf[ dataLen + 7 ] = (byte)0;
            dataLen += 8;
        }
        public void Write( ByteBuffer v )
        {
            WriteLength( v.dataLen );
            Write( v.buf, 0, v.dataLen );
        }
        public void Write( ByteBuffer v, int offset, int len )
        {
            WriteLength( len );
            Write( v.buf, offset, len );
        }

        public void Write<T>( T v ) where T : IBBWriter
        {
            v.WriteTo( this );
        }


        // 数组都算作定长, 不写长度值
        public void Write<T>( T[] vs ) where T : struct, IBBWriter
        {
            foreach( var v in vs )
            {
                Write( v );
            }
        }
        // string 不是 struct
        public void Write( string[] vs )
        {
            foreach( var v in vs )
            {
                Write( v );
            }
        }
        public void Write( byte[] v )
        {
            if( v == null ) throw new Exception( "v can't be null" );
            Reserve( dataLen + v.Length );
            Array.Copy( v, 0, buf, dataLen, v.Length );
            dataLen += v.Length;
        }
        public void Write( byte[] v, int offset, int len )
        {
            if( v == null ) throw new Exception( "v can't be null" );
            Reserve( dataLen + len );
            Array.Copy( v, offset, buf, dataLen, len );
            dataLen += len;
        }
        public void Write( bool[] v )
        {
            var bits = v.Length;
            var bytes = ( bits + 7 ) >> 3;
            Reserve( dataLen + bytes );
            var mod = bits % 8;
            if( mod > 0 ) --bytes;
            byte n = 0;
            for( var i = 0; i < bytes; ++i )
            {
                var p = i << 3;
                if( v[ p + 0 ] ) n |= 1;
                if( v[ p + 1 ] ) n |= 2;
                if( v[ p + 2 ] ) n |= 4;
                if( v[ p + 3 ] ) n |= 8;
                if( v[ p + 4 ] ) n |= 16;
                if( v[ p + 5 ] ) n |= 32;
                if( v[ p + 6 ] ) n |= 64;
                if( v[ p + 7 ] ) n |= 128;
                buf[ dataLen++ ] = n;
            }
            if( mod > 0 )
            {
                n = 0;
                var offset = bytes << 3;
                for( var i = 0; i < mod; ++i )
                {
                    if( v[ offset + i ] ) n |= (byte)(1 << i);
                }
                buf[ dataLen++ ] = n;
            }
        }

        public void Write<T>( List<T> vs ) where T : struct, IBBWriter
        {
            WriteLength( vs.Count );
            foreach( var v in vs )
            {
                Write( v );
            }
        }
        public void Write( List<string> vs )
        {
            WriteLength( vs.Count );
            foreach( var v in vs )
            {
                Write( v );
            }
        }
        public void Write( List<bool> vs )
        {
            WriteLength( vs.Count );
            var bits = vs.Count;
            var bytes = ( bits + 7 ) >> 3;
            Reserve( dataLen + bytes );
            var mod = bits % 8;
            if( mod > 0 ) --bytes;
            byte n = 0;
            for( var i = 0; i < bytes; ++i )
            {
                var p = i << 3;
                if( vs[ p + 0 ] ) n |= 1;
                if( vs[ p + 1 ] ) n |= 2;
                if( vs[ p + 2 ] ) n |= 4;
                if( vs[ p + 3 ] ) n |= 8;
                if( vs[ p + 4 ] ) n |= 16;
                if( vs[ p + 5 ] ) n |= 32;
                if( vs[ p + 6 ] ) n |= 64;
                if( vs[ p + 7 ] ) n |= 128;
                buf[ dataLen++ ] = n;
            }
            if( mod > 0 )
            {
                n = 0;
                var offset = bytes << 3;
                for( var i = 0; i < mod; ++i )
                {
                    if( vs[ offset + i ] ) n |= (byte)(1 << i);
                }
                buf[ dataLen++ ] = n;
            }
        }


        public void Write<T>( List<List<T>> vs ) where T : struct, IBBWriter
        {
            WriteLength( vs.Count );
            foreach( var v in vs )
            {
                Write( v );
            }
        }
        public void Write( List<List<string>> vs )
        {
            WriteLength( vs.Count );
            foreach( var v in vs )
            {
                Write( v );
            }
        }
        public void Write( List<List<bool>> vs )
        {
            WriteLength( vs.Count );
            foreach( var v in vs )
            {
                Write( v );
            }
        }

        #endregion

        #region read funcs

        public void Read( ref byte v )
        {
            v = buf[ offset++ ];
        }
        public void Read( ref sbyte v )
        {
            v = (sbyte)buf[ offset++ ];
        }
        public void Read( ref ushort v )
        {
            v = (ushort)((0xff & buf[ offset ])
                | (0xff00 & (buf[ offset + 1 ] << 8)));
            offset += 2;
        }
        public void Read( ref short v )
        {
            v = (short)((0xff & buf[ offset ])
                | (0xff00 & (buf[ offset + 1 ] << 8)));
            offset += 2;
        }
        public void Read( ref uint v )
        {
            v = (uint)(buf[ offset ]
                | (buf[ offset + 1 ] << 8)
                | (buf[ offset + 2 ] << 16)
                | (buf[ offset + 3 ] << 24));
            offset += 4;
        }
        public void Read( ref int v )
        {
            v = buf[ offset ]
                | (buf[ offset + 1 ] << 8)
                | (buf[ offset + 2 ] << 16)
                | (buf[ offset + 3 ] << 24);
            offset += 4;
        }
        public void Read( ref ulong v )
        {
            v = ((ulong)buf[ offset ])
                | (((ulong)buf[ offset + 1 ] << 8))
                | (((ulong)buf[ offset + 2 ] << 16))
                | (((ulong)buf[ offset + 3 ] << 24))
                | (((ulong)buf[ offset + 4 ] << 32))
                | (((ulong)buf[ offset + 5 ] << 40))
                | (((ulong)buf[ offset + 6 ] << 48))
                | (((ulong)buf[ offset + 7 ] << 56));
            offset += 8;
        }
        public void Read( ref long v )
        {
            ulong tmp = 0;
            Read( ref tmp );
            v = (long)tmp;
        }

        public void Read( ref float v )
        {
            v = BitConverter.ToSingle( buf, offset );
            offset += 4;
        }
        public void Read( ref double v )
        {
            v = BitConverter.ToDouble( buf, offset );
            offset += 8;
        }
        public void Read( ref bool v )
        {
            v = buf[ offset++ ] == 1;
        }
        public void Read( ref string v )
        {
            Read( ref v, 0, 0 );
        }
        public void Read( ref string v, int minLen, int maxLen )
        {
            int len = ReadLength( minLen, maxLen );
            v = Encoding.UTF8.GetString( buf, offset, len );
            offset += len;
        }
        public void Read( ref ByteBuffer v )
        {
            v.dataLen = ReadLength();
            Array.Resize( ref v.buf, v.dataLen );
            Read( ref v.buf );
        }
        public void Read( ref ByteBuffer v, int minLen, int maxLen )
        {
            v.dataLen = ReadLength( minLen, maxLen );
            Array.Resize( ref v.buf, v.dataLen );
            Read( ref v.buf );
        }
        public void Read( ref DateTime v )
        {
            if( offset + 8 > dataLen )
                throw new Exception( "out of range" );
            short years = 0;
            Read( ref years );
            v = new DateTime( years,
                (int)buf[ offset ],
                (int)buf[ offset + 1 ],
                (int)buf[ offset + 2 ],
                (int)buf[ offset + 3 ],
                (int)buf[ offset + 4 ] );
            offset += 6;    // Read short already += 2
        }

        public void Read<T>( ref T v ) where T : IBBReader
        {
            v.ReadFrom( this );
        }

        // 数组都算作定长, 不读长度值
        public void Read<T>( ref T[] vs ) where T : struct, IBBReader
        {
            for( int i = 0; i < vs.Length; ++i )
            {
                Read( ref vs[ i ] );
            }
        }
        public void Read( ref byte[] v )
        {
            Array.Copy( buf, offset, v, 0, v.Length );
            offset += v.Length;
        }
        public void Read( ref string[] vs )
        {
            for( int i = 0; i < vs.Length; ++i )
            {
                Read( ref vs[ i ] );
            }
        }
        public void Read( ref bool[] vs )
        {
            var bytes = ( vs.Length + 7 ) >> 3;
            if( dataLen < offset + bytes ) throw new Exception( "not enough data" );
            var p = offset;
            offset += bytes;
            var mod = vs.Length % 8;
            if( mod > 0 ) --bytes;
            int idx = 0;
            for( var i = 0; i < bytes; ++i )
            {
                var n = buf[ p + i ];
                vs[ idx + 0 ] = ((n & 1) > 0);
                vs[ idx + 1 ] = ((n & 2) > 0);
                vs[ idx + 2 ] = ((n & 4) > 0);
                vs[ idx + 3 ] = ((n & 8) > 0);
                vs[ idx + 4 ] = ((n & 16) > 0);
                vs[ idx + 5 ] = ((n & 32) > 0);
                vs[ idx + 6 ] = ((n & 64) > 0);
                vs[ idx + 7 ] = ((n & 128) > 0);
                idx += 8;
            }
            if( mod > 0 )
            {
                var n = buf[ p + bytes ];
                for( var i = 0; i < mod; ++i )
                {
                    vs[ idx++ ] = ((n & (1 << i)) > 0);
                }
            }
        }


        public void Read<T>( ref List<T> vs ) where T : struct, IBBReader
        {
            Read( ref vs, 0, 0 );
        }
        public void Read<T>( ref List<T> vs, int minLen, int maxLen ) where T : struct, IBBReader
        {
            int len = ReadLength( minLen, maxLen );
            vs.Clear();
            for( int i = 0; i < len; i++ )
            {
                vs.Add( Read<T>() );
            }
        }
        public T Read<T>() where T : IBBReader, new()
        {
            var rtv = new T();
            rtv.ReadFrom( this );
            return rtv;
        }

        public void Read( ref List<string> vs )
        {
            Read( ref vs, 0, 0 );
        }
        public void Read( ref List<string> vs, int minLen = 0, int maxLen = 0 )
        {
            int len = ReadLength( minLen, maxLen );
            vs.Clear();
            for( int i = 0; i < len; i++ )
            {
                var s = "";
                Read( ref s );
                vs.Add( s );
            }
        }
        public void Read( ref List<bool> vs )
        {
            Read( ref vs, 0, 0 );
        }
        public void Read( ref List<bool> vs, int minLen = 0, int maxLen = 0 )
        {
            int len = ReadLength( minLen, maxLen );
            vs.Clear();
            if( len == 0 ) return;

            var bytes = ( len + 7 ) >> 3;
            if( dataLen < offset + bytes ) throw new Exception( "not enough data" );
            var p = offset;
            offset += bytes;
            var mod = len % 8;
            if( mod > 0 ) --bytes;
            for( var i = 0; i < bytes; ++i )
            {
                var n = buf[ p + i ];
                vs.Add( (n & 1) > 0 );
                vs.Add( (n & 2) > 0 );
                vs.Add( (n & 4) > 0 );
                vs.Add( (n & 8) > 0 );
                vs.Add( (n & 16) > 0 );
                vs.Add( (n & 32) > 0 );
                vs.Add( (n & 64) > 0 );
                vs.Add( (n & 128) > 0 );
            }
            if( mod > 0 )
            {
                var n = buf[ p + bytes ];
                for( var i = 0; i < mod; ++i )
                {
                    vs.Add( (n & (1 << i)) > 0 );
                }
            }
        }


        public void Read<T>( ref List<List<T>> vs ) where T : struct, IBBReader
        {
            Read( ref vs, 0, 0 );
        }

        public void Read<T>( ref List<List<T>> vs, int minLen = 0, int maxLen = 0 ) where T : struct, IBBReader
        {
            int len = ReadLength( minLen, maxLen );
            vs.Clear();
            for( int i = 0; i < len; i++ )
            {
                var o = new List<T>();
                Read( ref o );
                vs.Add( o );
            }
        }
        public void Read( ref List<List<string>> vs )
        {
            Read( ref vs, 0, 0 );
        }
        public void Read( ref List<List<string>> vs, int minLen = 0, int maxLen = 0 )
        {
            int len = ReadLength( minLen, maxLen );
            vs.Clear();
            for( int i = 0; i < len; i++ )
            {
                var o = new List<string>();
                Read( ref o, 0, 0 );
                vs.Add( o );
            }
        }
        public void Read( ref List<List<bool>> vs )
        {
            Read( ref vs, 0, 0 );
        }
        public void Read( ref List<List<bool>> vs, int minLen = 0, int maxLen = 0 )
        {
            int len = ReadLength( minLen, maxLen );
            vs.Clear();
            for( int i = 0; i < len; i++ )
            {
                var o = new List<bool>();
                Read( ref o, 0, 0 );
                vs.Add( o );
            }
        }


        #endregion

        #region var write funcs

        public void VarWrite( uint v )
        {
            Reserve( dataLen + 5 );
            BinaryHelper.WriteDirect732( buf, ref dataLen, v );
        }
        public void VarWrite( int v )
        {
            Reserve( dataLen + 5 );
            BinaryHelper.WriteDirect732( buf, ref dataLen, BinaryHelper.ZigZagEncode32( v ) );
        }
        public void VarWrite( ulong v )
        {
            Reserve( dataLen + 9 );
            BinaryHelper.WriteDirect764( buf, ref dataLen, v );
        }
        public void VarWrite( long v )
        {
            Reserve( dataLen + 5 );
            BinaryHelper.WriteDirect764( buf, ref dataLen, BinaryHelper.ZigZagEncode64( v ) );
        }

        #endregion

        #region var read funcs

        public void VarRead( ref uint v )
        {
            BinaryHelper.Read732( ref v, buf, ref offset, dataLen );
        }
        public void VarRead( ref int v )
        {
            uint tmp = 0;
            BinaryHelper.Read732( ref tmp, buf, ref offset, dataLen );
            v = BinaryHelper.ZigZagDecode32( tmp );
        }
        public void VarRead( ref ulong v )
        {
            BinaryHelper.Read764( ref v, buf, ref offset, dataLen );
        }
        public void VarRead( ref long v )
        {
            ulong tmp = 0;
            BinaryHelper.Read764( ref tmp, buf, ref offset, dataLen );
            v = BinaryHelper.ZigZagDecode64( tmp );
        }

        #endregion

        #region var read write length funcs

        public void WriteLength( int v )
        {
            Reserve( dataLen + 5 );
            BinaryHelper.WriteDirect732( buf, ref dataLen, (uint)v );
        }

        public int ReadLength()
        {
            uint len = 0;
            BinaryHelper.Read732( ref len, buf, ref offset, dataLen );
            return (int)len;
        }
        public int ReadLength( int minLen, int maxLen )
        {
            uint len = 0;
            BinaryHelper.Read732( ref len, buf, ref offset, dataLen );
            if( len < minLen || (maxLen > 0 && len > maxLen) ) throw new Exception( "len out of range" );
            return (int)len;
        }

        #endregion
    }

    public static class BinaryHelper
    {
        /// <summary>
        /// 统计有多少个 1
        /// </summary>
        public static int Popcnt( uint x )
        {
            x -= ((x >> 1) & 0x55555555);
            x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
            x = (((x >> 4) + x) & 0x0f0f0f0f);
            x += (x >> 8);
            x += (x >> 16);
            return (int)(x & 0x0000003f);
        }
        /// <summary>
        /// 统计高位有多少个 0
        /// </summary>
        public static int Clz( uint x )
        {
            x |= (x >> 1);
            x |= (x >> 2);
            x |= (x >> 4);
            x |= (x >> 8);
            x |= (x >> 16);
            return (int)(32 - Popcnt( x ));
        }
        /// <summary>
        /// 求大于 v 的 2^n 值
        /// </summary>
        public static uint Round2n( uint v )
        {
            int bits = 31 - Clz( v );
            var rtv = (uint)( 1u << bits );
            if( rtv == v ) return v;
            return rtv << 1;
        }

        // 负转正：利用单数来存负数，双数来存正数
        // 等效代码： if( v < 0 ) return -v * 2 - 1; else return v * 2;
        public static ushort ZigZagEncode16( short v ) { return (ushort)((v << 1) ^ (v >> 15)); }
        public static uint ZigZagEncode32( int v ) { return (uint)((v << 1) ^ (v >> 31)); }
        public static ulong ZigZagEncode64( long v ) { return (ulong)((v << 1) ^ (v >> 63)); }
        // 等效代码： if( (v & 1) > 0 ) return -(v + 1) / 2; else return v / 2;
        public static short ZigZagDecode16( ushort v ) { return (short)((short)(v >> 1) ^ (-(short)(v & 1))); }
        public static int ZigZagDecode32( uint v ) { return (int)(v >> 1) ^ (-(int)(v & 1)); }
        public static long ZigZagDecode64( ulong v ) { return (long)(v >> 1) ^ (-(long)(v & 1)); }

        // need ensure 5
        public static void WriteDirect732( byte[] buf, ref int offset, uint v )
        {
            Lab1:
            byte b7 = (byte)v;
            v >>= 7;
            if( v > 0 )
            {
                buf[ offset++ ] = (byte)(b7 | (byte)0x80);
                goto Lab1;
            }
            buf[ offset++ ] = b7;
        }
        // need ensure 9
        public static void WriteDirect764( byte[] buf, ref int offset, ulong v )
        {
            var idx8 = offset + 8;
            Lab1:
            byte b7 = (byte)v;
            v >>= 7;
            if( v > 0 )
            {
                buf[ offset++ ] = (byte)(b7 | (byte)0x80);
                if( offset == idx8 )
                    buf[ offset++ ] = (byte)v;
                else
                    goto Lab1;
            }
            else
            {
                buf[ offset++ ] = b7;
            }
        }


        public static void Read764( ref ulong v, byte[] buf, ref int offset, int dataLen )
        {
            if( offset >= dataLen ) throw new Exception( "not enough data" );
            var idx8 = offset + 8;
            v = 0;
            int lshift = 0;
            Lab1:
            ulong b7 = buf[ offset++ ];
            if( b7 > 0x7F )
            {
                if( offset >= dataLen ) throw new Exception( "not enough data" );
                if( offset < idx8 )
                {
                    v |= (b7 & 0x7F) << lshift;
                    lshift += 7;
                    goto Lab1;
                }
                else v |= (b7 << lshift) | ((ulong)buf[ offset++ ] << 28 << 28);
            }
            else v |= b7 << lshift;
        }
        public static void Read732( ref uint v, byte[] buf, ref int offset, int dataLen )
        {
            if( offset >= dataLen ) throw new Exception( "not enough data" );
            var idx5 = offset + 5;
            int lshift = 0;
            v = 0;
            Lab1:
            uint b7 = buf[ offset++ ];
            if( b7 > 0x7F )
            {
                if( offset == idx5 ) throw new Exception( "overflow" );
                if( offset >= dataLen ) throw new Exception( "not enough data" );
                v |= (b7 & 0x7F) << lshift;
                lshift += 7;
                goto Lab1;
            }
            else
            {
                if( offset == idx5 && b7 > 15 ) throw new Exception( "overflow" );
                else v |= b7 << lshift;
            }
        }


        private static void DumpAscII( ref StringBuilder s, byte[] buf, int offset, int len )
        {
            for( int i = offset; i < offset + len; ++i )
            {
                byte c = buf[ i ];
                if( c < 32 || c > 126 ) s.Append( '.' );
                else s.Append( (char)c );
            }
        }

        // write buf's binary dump text to s
        public static void Dump( ref StringBuilder s, byte[] buf, int len = 0 )
        {
            if( buf == null || buf.Length == 0 ) return;
            if( len == 0 ) len = buf.Length;
            s.Append( "--------  0  1  2  3 | 4  5  6  7 | 8  9  A  B | C  D  E  F" );
            s.Append( "   dataLen = " + len );
            int i = 0;
            for( ; i < len; ++i )
            {
                if( (i % 16) == 0 )
                {
                    if( i > 0 )
                    {           // output ascii to the end of the line
                        s.Append( "  " );
                        DumpAscII( ref s, buf, i - 16, 16 );
                    }
                    s.Append( '\n' );
                    s.Append( i.ToString( "x8" ) );
                    s.Append( "  " );
                }
                else if( (i > 0 && (i % 4 == 0)) )
                {
                    s.Append( "  " );
                }
                else s.Append( ' ' );
                s.Append( BitConverter.ToString( buf, i, 1 ) );
            }
            int left = i % 16;
            if( left == 0 )
            {
                left = 16;
            }
            if( left > 0 )
            {
                len = len + 16 - left;
                for( ; i < len; ++i )
                {
                    if( i > 0 && (i % 4 == 0) )
                        s.Append( "  " );
                    else s.Append( ' ' );
                    s.Append( "  " );
                }
                s.Append( "  " );
                DumpAscII( ref s, buf, i - 16, left );
            }
            s.Append( '\n' );
        }

        public static string Dump( byte[] buf, int len = 0 )
        {
            var sb = new StringBuilder();
            Dump( ref sb, buf, len );
            return sb.ToString();
        }
    }
}
