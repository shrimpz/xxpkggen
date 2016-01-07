#pragma once
namespace xxlib
{

// todo: ToString 接口

class SimpleDate
{
public:
    union
    {
        struct
        {
            short year;
            byte month;
            byte day;
            byte hour;
            byte minute;
            byte second;
            byte dummy;
        };
        uint64 data;
    };

    // 这些默认值是为了兼容 java 序列化
    SimpleDate()
        : year( 1991 )
        , month( 1 )
        , day( 1 )
        , hour( 0 )
        , minute( 0 )
        , second( 0 )
        , dummy( 0 )
    {
    }
    SimpleDate( short year, byte month, byte day, byte hour, byte minute, byte second )
        : year( year )
        , month( month )
        , day( day )
        , hour( hour )
        , minute( minute )
        , second( second )
        , dummy( 0 )
    {
    }
    SimpleDate( SimpleDate const &other )
    {
        data = other.data;
    }
    inline SimpleDate& operator=( SimpleDate const &other )
    {
        data = other.data;
        return *this;
    }

    // ByteBuffer interface
    inline void WriteTo( xxlib::ByteBuffer& bb ) const
    {
        bb.Write( data );
    }
    inline void FastWriteTo( ByteBuffer& bb ) const
    {
        bb.FastWrite( data );
    }
    inline bool ReadFrom( xxlib::ByteBuffer& bb )
    {
        return bb.Read( data );
    }
    inline void VarWriteTo( ByteBuffer& bb ) const
    {
        WriteTo( bb );
    }
    inline bool VarReadFrom( ByteBuffer& bb )
    {
        return ReadFrom( bb );
    }
};
}
