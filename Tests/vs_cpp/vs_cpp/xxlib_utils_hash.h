#pragma once
namespace xxlib
{

int GetHash_CS( byte const* buf, int len );             // if arm, the buf must be align of 4(32bit)/8(64bit)
int GetHash_Lua( byte const* buf, int len );            // if len <=4, will be faster than CS
template<typename T>
int GetHashCode( T const &in );

}

