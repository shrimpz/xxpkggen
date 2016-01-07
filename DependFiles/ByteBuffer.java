package xxlib.serial;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

public class ByteBuffer
{ 
	public byte[] buf = null;
	public int dataLen = 0;
	public int offset = 0;
    Charset utf8charset = Charset.forName( "UTF-8" );

	public byte[] GetBytes()
	{
		byte[] b = new byte[dataLen];
		System.arraycopy( buf, 0, b, 0, dataLen );
		return b;
	}

    public ByteBuffer( int capacity )
	{
		Assign( new byte[capacity], 0 );
	}
	
	public ByteBuffer() 
	{
		this( 1024 );
	}
	
	public ByteBuffer( byte[] _buf, int _dataLen )
	{
		Assign( _buf, _dataLen );
	}

	public ByteBuffer( byte[] _buf )
	{
		Assign( _buf );
	}
	
	public void Assign( byte[] _buf, int _dataLen )
	{
		offset = 0;
		buf = _buf;
		dataLen = _dataLen;
	}
	
	public void Assign( byte[] _buf )
	{
		Assign( _buf, 0 );
	}

	public void Clear()
	{
		dataLen = offset = 0;
	}

	public boolean Empty()
	{
		return dataLen == 0;
	}

	public boolean Full()
	{
		return dataLen == offset;
	}

	public void Reserve( int capacity )
	{
		if( capacity <= buf.length )
		{
			return;
		}
		byte[] newbuf = new byte[ capacity * 2 ];
		System.arraycopy( buf, 0, newbuf, 0, dataLen );
		buf = newbuf;
	}
	
	public byte IndexAt( int idx )
	{
		return buf[ idx ];
	}

	
	
	
	public void Write( byte v )
	{
		Reserve( dataLen + 1 );
		buf[ dataLen ] = v;
		++dataLen;
	}
	public void Write( short v )
	{
		Reserve( dataLen + 2 );
		buf[ dataLen + 0 ] = (byte)v;
		buf[ dataLen + 1 ] = (byte)( v >>> 8 );
		dataLen += 2;
	}
	public void Write( int v )
	{
		Reserve( dataLen + 4 );
		buf[ dataLen + 0 ] = (byte)v;
		buf[ dataLen + 1 ] = (byte)( v >>> 8 );
		buf[ dataLen + 2 ] = (byte)( v >>> 16 );
		buf[ dataLen + 3 ] = (byte)( v >>> 24 );
		dataLen += 4;
	}
	public void Write( long v )
	{
		Reserve( dataLen + 8 );
		buf[ dataLen + 0 ] = (byte)v;
		buf[ dataLen + 1 ] = (byte)( v >>> 8 );
		buf[ dataLen + 2 ] = (byte)( v >>> 16 );
		buf[ dataLen + 3 ] = (byte)( v >>> 24 );
		buf[ dataLen + 4 ] = (byte)( v >>> 32 );
		buf[ dataLen + 5 ] = (byte)( v >>> 40 );
		buf[ dataLen + 6 ] = (byte)( v >>> 48 );
		buf[ dataLen + 7 ] = (byte)( v >>> 56 );
		dataLen += 8;
	}
	public void Write( char v )
	{
		Reserve( dataLen + 2 );
		buf[ dataLen + 0 ] = (byte)v;
		buf[ dataLen + 1 ] = (byte)( v >>> 8 );
		dataLen += 2;
	}
	public void Write( float v )
	{
        Write( Float.floatToIntBits( v ) );
	}
	public void Write( double v )
	{
		Write( Double.doubleToLongBits( v ) );
	}
	public void Write( boolean v )
	{
		Write( v ? (byte)1 : (byte)0 );
	}
	@SuppressWarnings("deprecation")
	public
	void Write( Date v )
	{
		Reserve( dataLen + 8 );
		short year = (short)( v.getYear() + 1900 );
		buf[ dataLen + 0 ] = (byte)year;
		buf[ dataLen + 1 ] = (byte)( year >>> 8 );
		buf[ dataLen + 2 ] = (byte)( v.getMonth() + 1 );
		buf[ dataLen + 3 ] = (byte)v.getDate();
		buf[ dataLen + 4 ] = (byte)v.getHours();
		buf[ dataLen + 5 ] = (byte)v.getMinutes();
		buf[ dataLen + 6 ] = (byte)v.getSeconds();
		dataLen += 7;
	}
	public void Write( String v )
	{
        byte[] b = v.getBytes( utf8charset );
        Write( b );
	}
	public void Write( byte[] v )
	{
		WriteLength( v.length );
	    Reserve( dataLen + v.length );
		System.arraycopy( v, 0, buf, dataLen, v.length );
		dataLen += v.length;
	}
	public void Write( ByteBuffer v )
	{
		WriteLength( v.dataLen );
	    Reserve( dataLen + v.dataLen );
		System.arraycopy( v.buf, 0, buf, dataLen, v.dataLen );
		dataLen += v.dataLen;
	}
	
	public <T extends IBBWriter> void Write( T v )
	{
		v.WriteTo( this );
	}

	public void Write( Byte v ) { Write( v.byteValue() ); } 
	public void Write( Short v ) { Write( v.shortValue() ); }
	public void Write( Integer v ) { Write( v.intValue() ); }
	public void Write( Long v ) { Write( v.longValue() ); }
	public void Write( Character v ) { Write( v.charValue() ); }
	public void Write( Float v ) { Write( v.floatValue() ); }
	public void Write( Double v ) { Write( v.doubleValue() ); }
	public void Write( Boolean v ) { Write( v.booleanValue() ); }
	

	public void WriteListByte( List<Byte> v )
	{
		WriteLength( v.size() );
		for( Byte o : v )
		{
			Write( o );
		}
	}
	public void WriteListShort( List<Short> v )
	{
		WriteLength( v.size() );
		for( Short o : v )
		{
			Write( o );
		}
	}
	public void WriteListInteger( List<Integer> v )
	{
		WriteLength( v.size() );
		for( Integer o : v )
		{
			Write( o );
		}
	}
	public void WriteListLong( List<Long> v )
	{
		WriteLength( v.size() );
		for( Long o : v )
		{
			Write( o );
		}
	}
	public void WriteListFloat( List<Float> v )
	{
		WriteLength( v.size() );
		for( Float o : v )
		{
			Write( o );
		}
	}
	public void WriteListDouble( List<Double> v )
	{
		WriteLength( v.size() );
		for( Double o : v )
		{
			Write( o );
		}
	}
	public void WriteListCharacter( List<Character> v )
	{
		WriteLength( v.size() );
		for( Character o : v )
		{
			Write( o );
		}
	}
	public void WriteListBoolean( List<Boolean> v )
	{
		WriteLength( v.size() );
		for( Boolean o : v )
		{
			Write( o );
		}
	}
	public void WriteListDate( List<Date> v )
	{
		WriteLength( v.size() );
		for( Date o : v )
		{
			Write( o );
		}
	}
	public void WriteListString( List<String> v )
	{
		WriteLength( v.size() );
		for( String o : v )
		{
			Write( o );
		}
	}
	public void WriteListByteBuffer( List<ByteBuffer> v )
	{
		WriteLength( v.size() );
		for( ByteBuffer o : v )
		{
			Write( o );
		}
	}

	
	public <T extends IBBWriter> void WriteListClass( List<T> v )
	{
		WriteLength( v.size() );
		for( T o : v )
		{
			Write( o );
		}
	}


	

	public byte ReadByte()
	{
		byte rtv = buf[ offset ];
		offset += 1;
		return rtv;
	}
	public short ReadShort()
	{
		short rtv = (short)( 
				(0xff   &  buf[ offset + 0 ]      ) | 
				(0xff00 & (buf[ offset + 1 ] << 8))
				);
		offset += 2;
		return rtv;
	}
	public int ReadInteger()
	{
		int rtv = (
				(0xff 		&  buf[ offset + 0 ]       ) | 
				(0xff00 	& (buf[ offset + 1 ] << 8) ) | 
				(0xff0000 	& (buf[ offset + 2 ] << 16)) | 
				(0xff000000 & (buf[ offset + 3 ] << 24))
				);
		offset += 4;
		return rtv;
	}
	public long ReadLong()
	{
		long rtv = (long)(
				(0xffL					&  (long)buf[ offset + 0 ]       ) | 
				(0xff00L				& ((long)buf[ offset + 1 ] << 8) ) | 
				(0xff0000L				& ((long)buf[ offset + 2 ] << 16)) | 
				(0xff000000L			& ((long)buf[ offset + 3 ] << 24)) |
				(0xff00000000L 			& ((long)buf[ offset + 4 ] << 32)) |
				(0xff0000000000L 		& ((long)buf[ offset + 5 ] << 40)) |
				(0xff000000000000L 		& ((long)buf[ offset + 6 ] << 48)) |
				(0xff00000000000000L 	& ((long)buf[ offset + 7 ] << 56))
				);
		offset += 8;
		return rtv;
	}
	public char ReadCharacter()
	{
		char rtv = (char)( 
				(0xff 	&  buf[ offset + 0 ]      ) | 
				(0xff00 & (buf[ offset + 1 ] << 8))
				);
		offset += 2;
		return rtv;
	}
	public float ReadFloat()
	{
		return Float.intBitsToFloat( ReadInteger() );
	}
	public double ReadDouble()
	{
		return Double.longBitsToDouble( ReadLong() );
	}
	public boolean ReadBoolean()
	{
		return ReadByte() != 0;
	}
	@SuppressWarnings("deprecation")
	public
	Date ReadDate()
	{
		Date rtv = new Date();
		rtv.setYear( ReadShort() - 1900 );
		rtv.setMonth( ReadByte() - 1 );
		rtv.setDate( ReadByte() );
		rtv.setHours( ReadByte() );
		rtv.setMinutes( ReadByte() );
		rtv.setSeconds( ReadByte() );
		return rtv;
	}
	public String ReadString()
	{
		return ReadString(0,0);
	}
	
	public String ReadString( int len )
	{
		return ReadString( len, len );
	}
	public String ReadString( int minLen, int maxLen )
	{
		int len = ReadLength();
		if( len < minLen || (maxLen > 0 && len > maxLen ) )
		{
		    throw new NullPointerException( "length out of range" );
		}
		if( offset + len > dataLen ) throw new RuntimeException();
		byte[] newbuf = new byte[ len ];
		System.arraycopy( buf, offset, newbuf, 0, len );
		String rtv = new String( newbuf, utf8charset );
		offset += len;
		return rtv;
	}
	
	
	public byte[] ReadBytes()
	{
		return ReadBytes(0,0);
	}
	
	public byte[] ReadBytes(int len)
	{
		return ReadBytes(len,len);
	}
	public byte[] ReadBytes(int minLen,int maxLen)
	{
		int len = ReadLength();
		if( len < minLen || (maxLen > 0 && len > maxLen ) )
		{
		    throw new NullPointerException( "length out of range" );
		}
		if( offset + len > dataLen ) throw new RuntimeException();
		byte[] rtv = new byte[ len ];
		System.arraycopy( buf, offset, rtv, 0, len );
		offset += len;
		return rtv;
	}
	public ByteBuffer ReadByteBuffer()
	{
		byte[] newbuf = ReadBytes();
		return new ByteBuffer( newbuf, newbuf.length );
	}
	
	
	
	
	
	
	public List<Byte> ReadListByte()
	{
		return ReadListByte(0,0);
	}
	
	public List<Byte> ReadListByte( int len )
	{
		return ReadListByte(len,len);
	}
	public List<Byte> ReadListByte( int minLen, int maxLen )
	{
		List<Byte> rtv = new ArrayList<Byte>();
		int len = ReadLength();
		if( len < minLen || (maxLen > 0 && len > maxLen ) )
		{
		    throw new NullPointerException( "length out of range" );
		}
		for( int i = 0; i < len; ++i )
		{
			rtv.add( ReadByte() );
		}
		return rtv;
	}
	
	public List<Short> ReadListShort()
	{
		return ReadListShort(0,0);
	}
	
	public List<Short> ReadListShort( int len )
	{
		return ReadListShort(len,len);
	}	
	public List<Short> ReadListShort( int minLen, int maxLen )
	{
		List<Short> rtv = new ArrayList<Short>();
		int len = ReadLength();
		if( len < minLen || (maxLen > 0 && len > maxLen ) )
		{
		    throw new NullPointerException( "length out of range" );
		}
		for( int i = 0; i < len; ++i )
		{
			rtv.add( ReadShort() );
		}
		return rtv;
	}
	
	public List<Integer> ReadListInteger()
	{
		return ReadListInteger(0,0);
	}
	public List<Integer> ReadListInteger( int len )
	{
		return ReadListInteger(len,len);
	}	
	public List<Integer> ReadListInteger( int minLen, int maxLen )
	{
		List<Integer> rtv = new ArrayList<Integer>();
		int len = ReadLength();
		if( len < minLen || (maxLen > 0 && len > maxLen ) )
		{
		    throw new NullPointerException( "length out of range" );
		}
		for( int i = 0; i < len; ++i )
		{
			rtv.add( ReadInteger() );
		}
		return rtv;
	}
	
	
	public List<Long> ReadListLong()
	{
		return ReadListLong(0,0);
	}
	public List<Long> ReadListLong( int len )
	{
		return ReadListLong(len,len);
	}	
	public List<Long> ReadListLong( int minLen, int maxLen )
	{
		List<Long> rtv = new ArrayList<Long>();
		int len = ReadLength();
		if( len < minLen || (maxLen > 0 && len > maxLen ) )
		{
		    throw new NullPointerException( "length out of range" );
		}
		for( int i = 0; i < len; ++i )
		{
			rtv.add( ReadLong() );
		}
		return rtv;
	}
	
	
	public List<Character> ReadListCharacter()
	{
		return ReadListCharacter(0,0);
	}
	public List<Character> ReadListCharacter( int len )
	{
		return ReadListCharacter(len,len);
	}	
	public List<Character> ReadListCharacter( int minLen, int maxLen )
	{
		List<Character> rtv = new ArrayList<Character>();
		int len = ReadLength();
		if( len < minLen || (maxLen > 0 && len > maxLen ) )
		{
		    throw new NullPointerException( "length out of range" );
		}
		for( int i = 0; i < len; ++i )
		{
			rtv.add( ReadCharacter() );
		}
		return rtv;
	}
	
	
	public List<Float> ReadListFloat()
	{
		return ReadListFloat(0,0);
	}
	public List<Float> ReadListFloat( int len )
	{
		return ReadListFloat(len,len);
	}	
	public List<Float> ReadListFloat( int minLen, int maxLen )
	{
		List<Float> rtv = new ArrayList<Float>();
		int len = ReadLength();
		if( len < minLen || (maxLen > 0 && len > maxLen ) )
		{
		    throw new NullPointerException( "length out of range" );
		}
		for( int i = 0; i < len; ++i )
		{
			rtv.add( ReadFloat() );
		}
		return rtv;
	}
	
	public List<Double> ReadListDouble()
	{
		return ReadListDouble(0,0);
	}
	public List<Double> ReadListDouble( int len )
	{
		return ReadListDouble(len,len);
	}	
	public List<Double> ReadListDouble( int minLen, int maxLen )
	{
		List<Double> rtv = new ArrayList<Double>();
		int len = ReadLength();
		if( len < minLen || (maxLen > 0 && len > maxLen ) )
		{
		    throw new NullPointerException( "length out of range" );
		}
		for( int i = 0; i < len; ++i )
		{
			rtv.add( ReadDouble() );
		}
		return rtv;
	}
	
	public List<Boolean> ReadListBoolean()
	{
		return ReadListBoolean(0,0);
	}
	public List<Boolean> ReadListBoolean( int len )
	{
		return ReadListBoolean(len,len);
	}	
	public List<Boolean> ReadListBoolean( int minLen, int maxLen )
	{
		List<Boolean> rtv = new ArrayList<Boolean>();
		int len = ReadLength();
		if( len < minLen || (maxLen > 0 && len > maxLen ) )
		{
		    throw new NullPointerException( "length out of range" );
		}
		for( int i = 0; i < len; ++i )
		{
			rtv.add( ReadBoolean() );
		}
		return rtv;
	}
	
	public List<Date> ReadListDate()
	{
		return ReadListDate(0,0);
	}
	public List<Date> ReadListDate( int len )
	{
		return ReadListDate(len,len);
	}	
	public List<Date> ReadListDate( int minLen, int maxLen )
	{
		List<Date> rtv = new ArrayList<Date>();
		int len = ReadLength();
		if( len < minLen || (maxLen > 0 && len > maxLen ) )
		{
		    throw new NullPointerException( "length out of range" );
		}
		for( int i = 0; i < len; ++i )
		{
			rtv.add( ReadDate() );
		}
		return rtv;
	}
	
	
	public List<String> ReadListString()
	{
		return ReadListString(0,0);
	}
	public List<String> ReadListString( int len )
	{
		return ReadListString(len,len);
	}
	public List<String> ReadListString( int minLen, int maxLen )
	{
		List<String> rtv = new ArrayList<String>();
		int len = ReadLength();
		if( len < minLen || (maxLen > 0 && len > maxLen ) )
		{
		    throw new NullPointerException( "length out of range" );
		}
		for( int i = 0; i < len; ++i )
		{
			rtv.add( ReadString() );
		}
		return rtv;
	}
	
	
	public List<ByteBuffer> ReadListByteBuffer()
	{
		return ReadListByteBuffer(0,0);
	}
	public List<ByteBuffer> ReadListByteBuffer( int len )
	{
		return ReadListByteBuffer(len,len);
	}
	public List<ByteBuffer> ReadListByteBuffer( int minLen, int maxLen )
	{
		List<ByteBuffer> rtv = new ArrayList<ByteBuffer>();
		int len = ReadLength();
		if( len < minLen || (maxLen > 0 && len > maxLen ) )
		{
		    throw new NullPointerException( "length out of range" );
		}
		for( int i = 0; i < len; ++i )
		{
			rtv.add( ReadByteBuffer() );
		}
		return rtv;
	}

	
	
	
	
	
	
	
	
	void WriteLength( int v )
	{
		do
		{
			byte b7 = (byte)v;
			v >>>= 7;
			if( v > 0 )
			{
				buf[ dataLen++ ] = (byte)(b7 | 0x80);
				continue;
			}
			buf[ dataLen++ ] = b7;
			break;
		} while( true );
	}
	
	int ReadLength()
	{
		if( offset >= dataLen ) throw new RuntimeException();
        int idx5 = offset + 5;
        int lshift = 0;
        int v = 0;
	    do
	    {
	        int b7 = buf[ offset++ ]& 0xff;
	        if( b7 > 0x7F )
	        {
	            if( offset == idx5 ) throw new RuntimeException();
	            if( offset >= dataLen ) throw new RuntimeException();
	            v |= ( b7 & 0x7F ) << lshift;
	            lshift += 7;
	            continue;
	        }
	        else
	        {
	            if( offset == idx5 && b7 > 15 ) throw new RuntimeException();
	            else v |= ( b7 << lshift );
	        }
	        break;
	    } while( true );
	    return v;
	}
	
	
	
	public static <T> T NewInstance( Class<T> clazz )
	{
	    try
		{
	        //T must have a no arg constructor for this to work 
	        return clazz.newInstance(); 
	    }
		catch (InstantiationException e)
		{
	        throw new IllegalStateException(e);
	    }
		catch (IllegalAccessException e)
		{
	        throw new IllegalStateException(e);
	    }
	}

	<T extends IBBReader> T ReadClass( Class<T> c )
	{
		T t = NewInstance( c );
		t.ReadFrom( this );
		return t;
	}
	
	
	<T extends IBBReader> List<T> ReadListClass( Class<T> c )
	{
		return ReadListClass(c,0,0);
	}
	<T extends IBBReader> List<T> ReadListClass( Class<T> c,int len )
	{
		return ReadListClass(c,len,len);
	}
	<T extends IBBReader> List<T> ReadListClass( Class<T> c,int minLen,int maxLen )
	{
		int len = ReadLength();
		if( len < minLen || (maxLen > 0 && len > maxLen ) )
		{
		    throw new NullPointerException( "length out of range" );
		}
		List<T> rtv = new ArrayList<T>( len );
		for( int i = 0; i < len; ++i )
		{
			rtv.add( ReadClass( c ) );
		}
		return rtv;
	}
	
}
