using System;
using System.Collections.Generic;
using System.Text;

namespace xxlib
{
    partial class ByteBuffer
    {
        public void Write( global::CS.Enum1 v )
        {
            Write( (int)v );
        }
        public void Write( global::CS.Enum1[] vs )
        {
            for( int i = 0; i < vs.Length; i++ )
            {
                Write( (int)vs[ i ] );
            }
        }
        public void Write( List<global::CS.Enum1> vs )
        {
            WriteLength( vs.Count );
            foreach( var v in vs )
            {
                Write( (int)v );
            }
        }
        public void Read( ref global::CS.Enum1 v )
        {
            int tmp = 0;
            Read( ref tmp );
            v = (global::CS.Enum1)tmp;
        }
        public void Read( ref global::CS.Enum1[] vs )
        {
            int tmp = 0;
            for( int i = 0; i < vs.Length; i++ )
            {
                Read( ref tmp );
                vs[ i ] = (global::CS.Enum1)tmp;
            }
        }
        public void Read( ref List<global::CS.Enum1> vs )
        {
            Read( ref vs, 0, 0 );
        }
        public void Read( ref List<global::CS.Enum1> vs, int minLen = 0, int maxLen = 0 )
        {
            int len = ReadLength( minLen, maxLen );
            vs.Clear();
            for( int i = 0; i < len; i++ )
            {
                int tmp = 0;
                Read( ref tmp );
                vs.Add( (global::CS.Enum1)tmp );
            }
        }

        public void Write( global::CS.Ns2.Enum1 v )
        {
            Write( (byte)v );
        }
        public void Write( global::CS.Ns2.Enum1[] vs )
        {
            for( int i = 0; i < vs.Length; i++ )
            {
                Write( (byte)vs[ i ] );
            }
        }
        public void Write( List<global::CS.Ns2.Enum1> vs )
        {
            WriteLength( vs.Count );
            foreach( var v in vs )
            {
                Write( (byte)v );
            }
        }
        public void Read( ref global::CS.Ns2.Enum1 v )
        {
            byte tmp = 0;
            Read( ref tmp );
            v = (global::CS.Ns2.Enum1)tmp;
        }
        public void Read( ref global::CS.Ns2.Enum1[] vs )
        {
            byte tmp = 0;
            for( int i = 0; i < vs.Length; i++ )
            {
                Read( ref tmp );
                vs[ i ] = (global::CS.Ns2.Enum1)tmp;
            }
        }
        public void Read( ref List<global::CS.Ns2.Enum1> vs )
        {
            Read( ref vs, 0, 0 );
        }
        public void Read( ref List<global::CS.Ns2.Enum1> vs, int minLen = 0, int maxLen = 0 )
        {
            int len = ReadLength( minLen, maxLen );
            vs.Clear();
            for( int i = 0; i < len; i++ )
            {
                byte tmp = 0;
                Read( ref tmp );
                vs.Add( (global::CS.Ns2.Enum1)tmp );
            }
        }

        public void Write( global::CS.Ns1.Enum1 v )
        {
            Write( (ushort)v );
        }
        public void Write( global::CS.Ns1.Enum1[] vs )
        {
            for( int i = 0; i < vs.Length; i++ )
            {
                Write( (ushort)vs[ i ] );
            }
        }
        public void Write( List<global::CS.Ns1.Enum1> vs )
        {
            WriteLength( vs.Count );
            foreach( var v in vs )
            {
                Write( (ushort)v );
            }
        }
        public void Read( ref global::CS.Ns1.Enum1 v )
        {
            ushort tmp = 0;
            Read( ref tmp );
            v = (global::CS.Ns1.Enum1)tmp;
        }
        public void Read( ref global::CS.Ns1.Enum1[] vs )
        {
            ushort tmp = 0;
            for( int i = 0; i < vs.Length; i++ )
            {
                Read( ref tmp );
                vs[ i ] = (global::CS.Ns1.Enum1)tmp;
            }
        }
        public void Read( ref List<global::CS.Ns1.Enum1> vs )
        {
            Read( ref vs, 0, 0 );
        }
        public void Read( ref List<global::CS.Ns1.Enum1> vs, int minLen = 0, int maxLen = 0 )
        {
            int len = ReadLength( minLen, maxLen );
            vs.Clear();
            for( int i = 0; i < len; i++ )
            {
                ushort tmp = 0;
                Read( ref tmp );
                vs.Add( (global::CS.Ns1.Enum1)tmp );
            }
        }

    }
} // ByteBuffer
