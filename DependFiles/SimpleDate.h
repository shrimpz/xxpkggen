#pragma once
class SimpleDate
{
public:
	short year = 0;
	byte month = 0;
	byte day = 0;
	byte hour = 0;
	byte minute = 0;
	byte second = 0;
	SimpleDate() = default;
	SimpleDate( SimpleDate const &other ) = default;
	SimpleDate( short year, byte month, byte day, byte hour, byte minute, byte second )
		: year( year )
		, month( month )
		, day( day )
		, hour( hour )
		, minute( minute )
		, second( second )
	{
	}
	// interface for ByteBuffer
	inline void WriteTo( xxlib::ByteBuffer& bb ) const
	{
		bb.WriteMulti( year, month, day, hour, minute, second );
	}
    
	inline bool ReadFrom( xxlib::ByteBuffer& bb )
	{
		return bb.ReadMulti( year, month, day, hour, minute, second );
	}
};