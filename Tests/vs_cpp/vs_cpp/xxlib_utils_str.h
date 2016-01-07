#pragma once
namespace xxlib
{

// 取长度系列，值为 ToString 后的最大长. 10 进制。
int GetToStringMaxLength( uint8   v );
int GetToStringMaxLength( uint16  v );
int GetToStringMaxLength( uint    v );
int GetToStringMaxLength( uint64  v );
int GetToStringMaxLength( int8    v );
int GetToStringMaxLength( int16   v );
int GetToStringMaxLength( int     v );
int GetToStringMaxLength( int64   v );
int GetToStringMaxLength( double  v );
int GetToStringMaxLength( float   v );
int GetToStringMaxLength( bool    v );
int GetToStringMaxLength( char    v );
int GetToStringMaxLength( char const* v );
//#if defined(XX_LINUX) && defined(XX_64)
//int GetToStringMaxLength( size_t  v);	// unsigned long int
//#endif

// 将值类型转换为字符串 返回转换后的串长 (主要为 Append, Write 等函数服务)
int ToString( char * dstBuf, uint8   v );
int ToString( char * dstBuf, uint16  v );
int ToString( char * dstBuf, uint    v );
int ToString( char * dstBuf, uint64  v );
int ToString( char * dstBuf, int8    v );
int ToString( char * dstBuf, int16   v );
int ToString( char * dstBuf, int     v );
int ToString( char * dstBuf, int64   v );
int ToString( char * dstBuf, double  v );
int ToString( char * dstBuf, float   v );
int ToString( char * dstBuf, bool    v );
int ToString( char * dstBuf, char    v );
int ToString( char * dstBuf, char const* v );
//#if defined(XX_LINUX) && defined(XX_64)
//int ToString( char * dstBuf, size_t  v);
//#endif


// HEX 版
//
int ToHexString( char * dstBuf, uint8  v );
int ToHexString( char * dstBuf, uint16 v );
int ToHexString( char * dstBuf, uint   v );
int ToHexString( char * dstBuf, uint64 v );
int ToHexString( char * dstBuf, char   v );
int ToHexString( char * dstBuf, int8   v );
int ToHexString( char * dstBuf, int16  v );
int ToHexString( char * dstBuf, int    v );
int ToHexString( char * dstBuf, int64  v );
//#if defined(XX_LINUX) && defined(XX_64)
//int ToHexString( char * dstBuf, size_t v );
//#endif



// 字串转值（值后面需要以 0 为结尾且不能有乱七八糟的 空格, TAB 等排版符）
void FromString( uint8   & dstVar, char const * s );
void FromString( uint16  & dstVar, char const * s );
void FromString( uint32  & dstVar, char const * s );
void FromString( uint64  & dstVar, char const * s );
void FromString( int8    & dstVar, char const * s );
void FromString( int16   & dstVar, char const * s );
void FromString( int     & dstVar, char const * s );
void FromString( int64   & dstVar, char const * s );
void FromString( double  & dstVar, char const * s );
void FromString( float   & dstVar, char const * s );
void FromString( bool    & dstVar, char const * s );
//#if defined(XX_LINUX) && defined(XX_64)
//void FromString( size_t  & dstVar, char const * s );
//#endif

}
