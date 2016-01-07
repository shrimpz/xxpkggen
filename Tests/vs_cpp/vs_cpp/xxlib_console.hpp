#pragma once
namespace xxlib
{

template<typename...TS>
void Console_t::Write( TS const&...parms )
{
	String s;
	s.Append( parms... );
	//printf( "%s", s.C_str() );
	fwrite( s.buf, 1, s.dataLen, stdout );
}

template<typename...TS>
void Console_t::WriteLine( TS const&...parms )
{
	Write( parms..., '\n' );
}

template<typename...TS>
void Console_t::PosWrite( short x, short y, TS const&...parms )
{
#ifdef XX_WINDOWS
	static auto h = GetStdHandle( STD_OUTPUT_HANDLE );
	static COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition( h, pos );
#else
	//printf( "\033[%d;%dH", x + 1, y + 1 );
	Write( "\033[", x + 1, ";", y + 1, "H" );
#endif
	Write( parms... );
}



template<typename...TS>
Console_t xxlib::Console_t::operator()( TS const&...parms ) const
{
    Write( parms... );
    return *this;
}


}
