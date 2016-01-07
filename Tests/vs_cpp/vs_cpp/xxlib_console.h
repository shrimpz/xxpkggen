#pragma once
namespace xxlib
{

class Console_t final
{
public:
	Console_t() {}

	template<typename...TS>
	static void Write( TS const&...parms );

	template<typename...TS>
	static void WriteLine( TS const&...parms );

	template<typename...TS>
	static void PosWrite( short x, short y, TS const&...parms );

	template<typename T>
	Console_t const& operator<<( T const& t ) const
	{
		Write( t );
		return *this;
	}

    template<typename...TS>
    Console_t operator()( TS const&...parms ) const;

} const Console;
Console_t const Cout;

}

