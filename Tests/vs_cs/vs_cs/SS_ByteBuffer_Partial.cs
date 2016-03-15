using System;
using System.Collections.Generic;
using System.Text;

namespace xxlib
{
    partial class ByteBuffer
    {
        public void Write( global::SS.ServerTypes v )
        {
            Write( (int)v );
        }
        public void Write( global::SS.ServerTypes[] vs )
        {
            for( int i = 0; i < vs.Length; i++ )
            {
                Write( (int)vs[ i ] );
            }
        }
        public void Write( List<global::SS.ServerTypes> vs )
        {
            WriteLength( vs.Count );
            foreach( var v in vs )
            {
                Write( (int)v );
            }
        }
        public void Read( ref global::SS.ServerTypes v )
        {
            int tmp = 0;
            Read( ref tmp );
            v = (global::SS.ServerTypes)tmp;
        }
        public void Read( ref global::SS.ServerTypes[] vs )
        {
            int tmp = 0;
            for( int i = 0; i < vs.Length; i++ )
            {
                Read( ref tmp );
                vs[ i ] = (global::SS.ServerTypes)tmp;
            }
        }
        public void Read( ref List<global::SS.ServerTypes> vs )
        {
            Read( ref vs, 0, 0 );
        }
        public void Read( ref List<global::SS.ServerTypes> vs, int minLen, int maxLen )
        {
            int len = ReadLength( minLen, maxLen );
            vs.Clear();
            for( int i = 0; i < len; i++ )
            {
                int tmp = 0;
                Read( ref tmp );
                vs.Add( (global::SS.ServerTypes)tmp );
            }
        }

        public void Write( global::SS.Proxy2Client.ClientConnectResultTypes v )
        {
            Write( (int)v );
        }
        public void Write( global::SS.Proxy2Client.ClientConnectResultTypes[] vs )
        {
            for( int i = 0; i < vs.Length; i++ )
            {
                Write( (int)vs[ i ] );
            }
        }
        public void Write( List<global::SS.Proxy2Client.ClientConnectResultTypes> vs )
        {
            WriteLength( vs.Count );
            foreach( var v in vs )
            {
                Write( (int)v );
            }
        }
        public void Read( ref global::SS.Proxy2Client.ClientConnectResultTypes v )
        {
            int tmp = 0;
            Read( ref tmp );
            v = (global::SS.Proxy2Client.ClientConnectResultTypes)tmp;
        }
        public void Read( ref global::SS.Proxy2Client.ClientConnectResultTypes[] vs )
        {
            int tmp = 0;
            for( int i = 0; i < vs.Length; i++ )
            {
                Read( ref tmp );
                vs[ i ] = (global::SS.Proxy2Client.ClientConnectResultTypes)tmp;
            }
        }
        public void Read( ref List<global::SS.Proxy2Client.ClientConnectResultTypes> vs )
        {
            Read( ref vs, 0, 0 );
        }
        public void Read( ref List<global::SS.Proxy2Client.ClientConnectResultTypes> vs, int minLen, int maxLen )
        {
            int len = ReadLength( minLen, maxLen );
            vs.Clear();
            for( int i = 0; i < len; i++ )
            {
                int tmp = 0;
                Read( ref tmp );
                vs.Add( (global::SS.Proxy2Client.ClientConnectResultTypes)tmp );
            }
        }

    }
}
