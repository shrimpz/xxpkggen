#pragma once
namespace xxlib
{

template<typename T>
void ByteBuffer::WriteCore( char* dest, T const& src )
{
#ifdef XX_IA
    *(T*)dest = src;
#else
    auto p = (char*)&src;
    if( sizeof( T ) >= 1 )
    {
        dest[ 0 ] = p[ 0 ];
    }
    if( sizeof( T ) >= 2 )
    {
        dest[ 1 ] = p[ 1 ];
    }
    if( sizeof( T ) >= 4 )
    {
        dest[ 2 ] = p[ 2 ];
        dest[ 3 ] = p[ 3 ];
    }
    if( sizeof( T ) == 8 )
    {
        dest[ 4 ] = p[ 4 ];
        dest[ 5 ] = p[ 5 ];
        dest[ 6 ] = p[ 6 ];
        dest[ 7 ] = p[ 7 ];
    }
#endif
}



template<typename T>
void ByteBuffer::ReadCore( T& dest, char const* src )
{
#ifdef XX_IA
    dest = *(T*)src;
#else
    auto p = (char*)&dest;
    if( sizeof( T ) >= 1 )
    {
        p[ 0 ] = src[ 0 ];
    }
    if( sizeof( T ) >= 2 )
    {
        p[ 1 ] = src[ 1 ];
    }
    if( sizeof( T ) >= 4 )
    {
        p[ 2 ] = src[ 2 ];
        p[ 3 ] = src[ 3 ];
    }
    if( sizeof( T ) == 8 )
    {
        p[ 4 ] = src[ 4 ];
        p[ 5 ] = src[ 5 ];
        p[ 6 ] = src[ 6 ];
        p[ 7 ] = src[ 7 ];
    }
#endif
}



XX_HAS_FUNC( HasFunc_FastWriteTo, FastWriteTo, void ( T::* )( ByteBuffer& ) const );
template<typename T>
typename std::enable_if<HasFunc_FastWriteTo<T>::value, void>::type FastWriteSwitch( ByteBuffer& b, T const& v )
{
#if XX_BYTEBUFFER_PTR_SUPPORT_ENABLED
    if( b.ptrStore )
    {
        auto rtv = b.ptrStore->Add( (void*)&v, (uint)b.dataLen );
        b.VarWrite( rtv.first->value );
        if( !rtv.second ) return;
    }
#endif
    v.FastWriteTo( b );
};
template<typename T>
typename std::enable_if<!HasFunc_FastWriteTo<T>::value, void>::type FastWriteSwitch( ByteBuffer& b, T const& v )
{
    static_assert( std::is_pod<T>::value, "forget impl FastWriteTo func ?" );
    ByteBuffer::WriteCore( b.buf + b.dataLen, v );
    b.dataLen += sizeof( v );
};
template<typename T>
void ByteBuffer::FastWrite( T const& v )
{
    FastWriteSwitch( *this, v );
}

template<typename T, int len>
void ByteBuffer::FastWrite( T const( &a )[ len ] )
{
    if( std::is_pod<T>::value )
    {
        FastWrite( (char*)a, len * sizeof( T ) );
    }
    else
    {
        for( int i = 0; i < len; ++i )
        {
            FastWriteSwitch( *this, a[ i ] );
        }
    }
}

template<int bits>
void ByteBuffer::FastWrite( bool const( &v )[ bits ] )
{
    int bytes = int( ( bits + 7 ) >> 3 );
    // Reserve( dataLen + bytes );      // fast version do not need Reserve
    int mod = bits % 8;
    if( mod > 0 ) --bytes;
    byte n = 0;
    for( int i = 0; i < bytes; ++i )
    {
        int offset = i << 3;
        if( v[ offset + 0 ] ) n |= 1;
        if( v[ offset + 1 ] ) n |= 2;
        if( v[ offset + 2 ] ) n |= 4;
        if( v[ offset + 3 ] ) n |= 8;
        if( v[ offset + 4 ] ) n |= 16;
        if( v[ offset + 5 ] ) n |= 32;
        if( v[ offset + 6 ] ) n |= 64;
        if( v[ offset + 7 ] ) n |= 128;
        buf[ dataLen++ ] = n;
    }
    if( mod > 0 )
    {
        n = 0;
        int offset = bytes << 3;
        for( int i = 0; i < mod; ++i )
        {
            if( v[ offset + i ] ) n |= (byte)( 1 << i );
        }
        buf[ dataLen++ ] = n;
    }
}

template<typename T>
void ByteBuffer::FastWriteMultiCore( T const& v )
{
    FastWrite( v );
}
template<typename T, typename...TS>
void ByteBuffer::FastWriteMultiCore( T const& v, TS const&...vs )
{
    FastWrite( v );
    FastWriteMultiCore( vs... );
}
template<typename...TS>
void ByteBuffer::FastWriteMulti( TS const&...vs )
{
    FastWriteMultiCore( vs... );
}






template<typename T>
void ByteBuffer::FastRead( T& v )
{
    assert( std::is_pod<T>::value );
    ByteBuffer::ReadCore( v, buf + offset );
    offset += sizeof( v );
}

template<typename T>
void ByteBuffer::FastReadMultiCore( T& v )
{
    FastRead( v );
}
template<typename T, typename...TS>
void ByteBuffer::FastReadMultiCore( T& v, TS&...vs )
{
    FastRead( v );
    FastReadMultiCore( vs... );
}

template<typename...TS>
void ByteBuffer::FastReadMulti( TS&...vs )
{
    FastReadMultiCore( vs... );
}







XX_HAS_FUNC( HasFunc_WriteTo, WriteTo, void ( T::* )( ByteBuffer& ) const );
template<typename T>
typename std::enable_if<HasFunc_WriteTo<T>::value, void>::type WriteSwitch( ByteBuffer& b, T const& v )
{
#if XX_BYTEBUFFER_PTR_SUPPORT_ENABLED
    if( b.ptrStore )
    {
        auto rtv = b.ptrStore->Add( (void*)&v, (uint)b.dataLen );
        b.VarWrite( rtv.first->value );
        if( !rtv.second ) return;
    }
#endif
    v.WriteTo( b );
};
template<typename T>
typename std::enable_if<!HasFunc_WriteTo<T>::value, void>::type WriteSwitch( ByteBuffer& b, T const& v )
{
    static_assert( std::is_pod<T>::value, "forget impl WriteTo func ?" );
    b.Reserve( b.dataLen + sizeof( T ) );
    ByteBuffer::WriteCore( b.buf + b.dataLen, v );
    b.dataLen += sizeof( T );
};
template<typename T>
void ByteBuffer::Write( T const& v )
{
    WriteSwitch( *this, v );
}

template<typename T, int len>
void ByteBuffer::Write( T const( &a )[ len ] )
{
    if( std::is_pod<T>::value )
    {
        auto siz = len * ( int )sizeof( T );
        Reserve( dataLen + siz );
        FastWrite( (char*)a, len * sizeof( T ) );
    }
    else
    {
        for( int i = 0; i < len; ++i )
        {
            Write( a[ i ] );
        }
    }
}

template<int bits>
void ByteBuffer::Write( bool const( &a )[ bits ] )
{
    Reserve( dataLen + int( ( bits + 7 ) >> 3 ) );
    FastWrite( a );
}

template<typename T>
void ByteBuffer::WriteMultiCore( T const& v )
{
    Write( v );
}
template<typename T, typename...TS>
void ByteBuffer::WriteMultiCore( T const& v, TS const&...vs )
{
    Write( v );
    WriteMultiCore( vs... );
}
template<typename...TS>
void ByteBuffer::WriteMulti( TS const&...vs )
{
    WriteMultiCore( vs... );
}


#if XX_BYTEBUFFER_PTR_SUPPORT_ENABLED
template<typename T>
void ByteBuffer::Write( T* v )
{
    if( v )
    {
        Write( true );
        Write( *v );
    }
    else
    {
        Write( false );
    }
}
template<typename T>
void xxlib::ByteBuffer::RootWrite( T const& v )
{
    PtrStoreInit();
    Write( v );
}


template<typename T>
bool xxlib::ByteBuffer::Read( T*& v )
{
    bool b;
    if( !Read( b ) ) return false;
    if( b )
    {
        uint _offset, _offset_bak = (uint)offset;
        if( !VarRead( _offset ) ) return false;
        if( _offset == _offset_bak )
        {
            assert( !v );                       // todo: obj pool support ?
            v = new T();
            if( !Read( *v ) )
            {
                delete v;
                v = nullptr;
                return false;
            }
            idxStore->Add( _offset, v );     // todo: check
        }
        else
        {
            v = (T*)idxStore->At( _offset );
        }
    }
    else
    {
        v = nullptr;
    }
    return true;
}
template<typename T>
bool xxlib::ByteBuffer::RootRead( T& v )
{
    IdxStoreInit();
    return Read( v );
}
#endif



XX_HAS_FUNC( HasFunc_ReadFrom, ReadFrom, bool ( T::* )( ByteBuffer& ) );
template<typename T>
typename std::enable_if<HasFunc_ReadFrom<T>::value, bool>::type ReadSwitch( ByteBuffer& b, T& v )
{
#if XX_BYTEBUFFER_PTR_SUPPORT_ENABLED
    if( b.idxStore )
    {
        uint _offset;
        if( !b.VarRead( _offset ) ) return false;
        b.idxStore->Add( _offset, (void*)&v );   // todo: assert?
    }
#endif
    return v.ReadFrom( b );
};
template<typename T>
typename std::enable_if<!HasFunc_ReadFrom<T>::value, bool>::type ReadSwitch( ByteBuffer& b, T& v )
{
    assert( std::is_pod<T>::value );
    if( b.offset + ( int )sizeof( T ) > b.dataLen ) return false;
    ByteBuffer::ReadCore( v, b.buf + b.offset );
    b.offset += sizeof( T );
    return true;
};
template<typename T>
bool ByteBuffer::Read( T& v )
{
    return ReadSwitch( *this, v );
}

template<typename T>
bool ByteBuffer::Read( T& v, int minLen, int maxLen )
{
    return v.ReadFrom( *this, minLen, maxLen );
}

template<typename T, int len>
bool ByteBuffer::Read( T( &a )[ len ] )
{
    if( std::is_pod<T>::value )
    {
        int siz = len * ( int )sizeof( T );
        if( offset + siz > dataLen ) return false;
        memcpy( &a, buf + offset, siz );
        offset += siz;
        return true;
    }
    for( int i = 0; i < len; ++i )
    {
        if( !Read( a[ i ] ) ) return false;
    }
    return true;
}

template<int bits>
bool ByteBuffer::Read( bool( &a )[ bits ] )
{
    int bytes = int( ( bits + 7 ) >> 3 );
    if( dataLen < offset + bytes ) return false;
    int p = offset;
    offset += bytes;
    int mod = bits % 8;
    if( mod > 0 ) --bytes;
    int idx = 0;
    for( int i = 0; i < bytes; ++i )
    {
        byte n = (byte)buf[ p + i ];
        a[ idx + 0 ] = ( ( n & 1 ) > 0 );
        a[ idx + 1 ] = ( ( n & 2 ) > 0 );
        a[ idx + 2 ] = ( ( n & 4 ) > 0 );
        a[ idx + 3 ] = ( ( n & 8 ) > 0 );
        a[ idx + 4 ] = ( ( n & 16 ) > 0 );
        a[ idx + 5 ] = ( ( n & 32 ) > 0 );
        a[ idx + 6 ] = ( ( n & 64 ) > 0 );
        a[ idx + 7 ] = ( ( n & 128 ) > 0 );
        idx += 8;
    }
    if( mod > 0 )
    {
        byte n = (byte)buf[ p + bytes ];
        for( int i = 0; i < mod; ++i )
        {
            a[ idx++ ] = ( ( n & ( 1 << i ) ) > 0 );
        }
    }
    return true;
}

template<typename T>
bool ByteBuffer::ReadMultiCore( T& v )
{
    return Read( v );
}

template<typename T, typename...TS>
bool ByteBuffer::ReadMultiCore( T& v, TS&...vs )
{
    if( !Read( v ) ) return false;
    return ReadMultiCore( vs... );
}

template<typename...TS>
bool ByteBuffer::ReadMulti( TS&...vs )
{
    return ReadMultiCore( vs... );
}









XX_HAS_FUNC( HasFunc_VarWriteTo, VarWriteTo, void ( T::* )( ByteBuffer& ) const );
template<typename T>
typename std::enable_if<HasFunc_VarWriteTo<T>::value, void>::type VarWriteSwitch( ByteBuffer& b, T const& v )
{
#if XX_BYTEBUFFER_PTR_SUPPORT_ENABLED
    if( b.ptrStore )
    {
        auto rtv = b.ptrStore->Add( (void*)&v, (uint)b.dataLen );
        b.VarWrite( rtv.first->value );
        if( !rtv.second ) return;
    }
#endif
    v.VarWriteTo( b );
};
template<typename T>
typename std::enable_if<!HasFunc_VarWriteTo<T>::value, void>::type VarWriteSwitch( ByteBuffer& b, T const& v )
{
    static_assert( std::is_pod<T>::value, "forget impl VarWriteTo func ?" );
    b.Reserve( b.dataLen + sizeof( T ) );
    ByteBuffer::WriteCore( b.buf + b.dataLen, v );
    b.dataLen += sizeof( T );
};
template<typename T>
void ByteBuffer::VarWrite( T const& v )
{
    VarWriteSwitch( *this, v );
}

template<typename T, int len>
void ByteBuffer::VarWrite( T const( &a )[ len ] )
{
    for( int i = 0; i < len; ++i )
    {
        VarWrite( a[ i ] );
    }
}

template<int bits>
void ByteBuffer::VarWrite( bool const( &a )[ bits ] )
{
    Write( a );
}


template<typename T>
void ByteBuffer::VarWriteMultiCore( T const& v )
{
    VarWrite( v );
}
template<typename T, typename...TS>
void ByteBuffer::VarWriteMultiCore( T const& v, TS const&...vs )
{
    VarWrite( v );
    VarWriteMultiCore( vs... );
}
template<typename...TS>
void ByteBuffer::VarWriteMulti( TS const&...vs )
{
    VarWriteMultiCore( vs... );
}







XX_HAS_FUNC( HasFunc_VarReadFrom, VarReadFrom, bool ( T::* )( ByteBuffer& ) );
template<typename T>
typename std::enable_if<HasFunc_VarReadFrom<T>::value, bool>::type VarReadSwitch( ByteBuffer& b, T& v )
{
#if XX_BYTEBUFFER_PTR_SUPPORT_ENABLED
    if( b.idxStore )
    {
        uint _offset;
        if( !b.VarRead( _offset ) ) return false;
        b.idxStore->Add( _offset, (void*)&v );   // todo: assert?
    }
#endif
    return v.VarReadFrom( b );
};
template<typename T>
typename std::enable_if<!HasFunc_VarReadFrom<T>::value, bool>::type VarReadSwitch( ByteBuffer& b, T& v )
{
    assert( std::is_pod<T>::value );
    if( b.offset + ( int )sizeof( T ) > b.dataLen ) return false;
    ByteBuffer::ReadCore( v, b.buf + b.offset );
    b.offset += sizeof( T );
    return true;
};
template<typename T>
bool ByteBuffer::VarRead( T& v )
{
    return VarReadSwitch( *this, v );
}
template<typename T>
bool ByteBuffer::VarRead( T& v, int minLen, int maxLen )
{
    return v.VarReadFrom( *this, minLen, maxLen );
}


template<typename T, int len>
bool ByteBuffer::VarRead( T( &a )[ len ] )
{
    for( int i = 0; i < len; ++i )
    {
        if( !VarRead( a[ i ] ) ) return false;
    }
    return true;
}



template<typename T>
bool ByteBuffer::VarReadMultiCore( T& v )
{
    return VarRead( v );
}

template<typename T, typename...TS>
bool ByteBuffer::VarReadMultiCore( T& v, TS&...vs )
{
    if( !VarRead( v ) ) return false;
    return VarReadMultiCore( vs... );
}

template<typename...TS>
bool ByteBuffer::VarReadMulti( TS&...vs )
{
    return VarReadMultiCore( vs... );
}






}
