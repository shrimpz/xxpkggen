#pragma once
#define XX_BYTEBUFFER_PTR_SUPPORT_ENABLED 0

namespace xxlib
{
template <typename TK, typename TV>
class Dict;

// utils_str ext for ByteBuffer
int GetToStringMaxLength( ByteBuffer const& v );
int ToString( char* dstBuf, ByteBuffer const& v );

class ByteBuffer : Memmoveable
{
public:
	char*       buf;
	int         bufLen;
	int         dataLen;
	int         offset;
	bool        isRef;

#if XX_BYTEBUFFER_PTR_SUPPORT_ENABLED
	Dict<void*, uint>* ptrStore = nullptr;                                      // for 序列化含指针的类实例
	Dict<uint, void*>* idxStore = nullptr;                                      // for 反序列化含指针的类实例
#endif

	ByteBuffer();																// ByteBuffer( 1024 )
	explicit ByteBuffer( int capacity );										// prepare
	ByteBuffer( char* _buf, int _bufLen, int _dataLen = 0, bool isRef = false );   // Copy or ref
	ByteBuffer( ByteBuffer const& other );                                      // Copy
	ByteBuffer( ByteBuffer&& other );                                           // move
	ByteBuffer& operator=( ByteBuffer const& other );                           // Copy
	ByteBuffer& operator=( ByteBuffer&& other );                                // move
	void Destroy();                                                             // delete[] buf if need
	~ByteBuffer();
	void Assign( char const* _buf, int _bufLen, int _dataLen = 0, bool isRef = false );  // Copy or ref
	void Reserve( int capacity );
	void Resize( int widx );
	void Clear();                                                               // dataLen = offset = 0
	bool Empty() const;                                                         // dataLen == 0
	bool Full() const;                                                          // dataLen == offset
	char operator[] ( int idx ) const;                                          // buf[ idx ]
	char& operator[] ( int idx );                                               // buf[ idx ]
	char IndexAt( int idx ) const;                                              // buf[ idx ]
	char& IndexAt( int idx );                                                   // buf[ idx ]

#if XX_BYTEBUFFER_PTR_SUPPORT_ENABLED
		// ptrStore, idxStore 相关
	void PtrStoreInit();                                                        // RootWrite 时自动调用 
	void PtrStoreDestroy();

	void IdxStoreInit();                                                        // RootRead 时自动调用
	void IdxStoreDestroy();
#endif

	// 最基础的读写函数实现体

	// 基本读写系列
	template<typename T>
	static void WriteCore( char* dest, T const& src );
	template<typename T>
	static void ReadCore( T& dest, char const* src );

	// 变长读写系列
	static void Write7Core( char* buf, int& offset, uint v );                   // need ensure 7 space
	static void FastRead7Core( uint& v, char* buf, int& offset );
	static bool Read7Core( uint& v, char* buf, int len, int& offset );

	// 64 位长版
	static void Write7Core( char* buf, int & offset, uint64 v );                // need ensure 9 space
	static void FastRead7Core( uint64& v, char* buf, int& offset );
	static bool Read7Core( uint64& v, char* buf, int len, int& offset );

	// negative -> ZigZag positive
	// 效果：负数变正，正数 *= 2
	static uint32 ZigZagEncode( int    i );
	static uint64 ZigZagEncode( int64  i );

	// ZigZag positive -> negative
	static int32  ZigZagDecode( uint   i );
	static int64  ZigZagDecode( uint64 i );


	// 快速写入系列：不预申请内存（危险，主供生成物代码调用）
	void FastWrite( char const* _buf, int _dataLen );                           // do not Write _dataLen
	template<typename T, int len>
	void FastWrite( T const( &a )[ len ] );
	template<typename T>
	void FastWrite( T const& v );
	// 多值变参版
	template<typename...TS>
	void FastWriteMulti( TS const&...vs );
    template<int len>
    void FastWrite( bool const( &a )[ len ] );



	// 快速读出系列，不判断数据长度是否足够满足读取操作（危险，安全环节才用）
	template<typename T>
	void FastRead( T& v );
	template<typename...TS>
	void FastReadMulti( TS&...vs );


	// 正常写入系列：会预申请内存
	void Write( char const* _buf, int _dataLen );
	template<typename T, int len>
	void Write( T const( &a )[ len ] );
	template<typename T>
	void Write( T const& v );
	// 多值变参版
	template<typename...TS>
	void WriteMulti( TS const&...vs );
    template<int len>
    void Write( bool const( &a )[ len ] );

#if XX_BYTEBUFFER_PTR_SUPPORT_ENABLED
	// 带指针写入系列
	template<typename T>
	void Write( T* v );
	template<typename T>
	void RootWrite( T const& v );

	// 带指针读出系列
	template<typename T>
	bool Read( T*& v );
	template<typename T>
	bool RootRead( T& v );
#endif

	// 变长写入系列：会预申请内存
	void VarWrite( int v );
	void VarWrite( uint v );
	void VarWrite( int64 v );
	void VarWrite( uint64 v );
	//void VarWrite( float v );
	//void VarWrite( double v );	// if( v == (int)v ) 按整数写... 最低位放标志位
	template<typename T, int len>
	void VarWrite( T const( &a )[ len ] );
	template<typename T>
	void VarWrite( T const& v );
	//// 多值变参版
	template<typename...TS>
	void VarWriteMulti( TS const&...vs );
    template<int len>
    void VarWrite( bool const( &a )[ len ] );

	bool VarRead( int& v );
	bool VarRead( uint& v );
	bool VarRead( int64& v );
	bool VarRead( uint64& v );
	//void VarRead( float v );
	//void VarRead( double v );
	template<typename T>
	bool VarRead( T& v );
	template<typename T>
	bool VarRead( T& v, int minLen, int maxLen );       // 限长版
	template<typename T, int len>
	bool VarRead( T( &a )[ len ] );
	template<typename...TS>
	bool VarReadMulti( TS&...vs );





	void FastVarWrite( int v );
	void FastVarWrite( uint v );
	void FastVarWrite( int64 v );
	void FastVarWrite( uint64 v );

	void FastVarRead( int& v );
	void FastVarRead( uint& v );
	void FastVarRead( int64& v );
	void FastVarRead( uint64& v );



	// 正常读出系列：返回 true 表示读出成功
	template<typename T>
	bool Read( T& v );
	template<typename T>
	bool Read( T& v, int minLen, int maxLen );       // 限长版
	bool Read( char* _buf, int _dataLen );
	template<typename T, int len>
	bool Read( T( &a )[ len ] );
	template<typename...TS>
	bool ReadMulti( TS&...vs );
    template<int len>
    bool Read( bool( &a )[ len ] );


	// interface for ByteBuffer
	void WriteTo( ByteBuffer& bb ) const;
	void FastWriteTo( ByteBuffer& bb ) const;
	bool ReadFrom( ByteBuffer& bb );
	bool ReadFrom( ByteBuffer& bb, int minLen, int maxLen );
	void VarWriteTo( ByteBuffer& bb ) const;
	bool VarReadFrom( ByteBuffer& bb );
	bool VarReadFrom( ByteBuffer& bb, int minLen, int maxLen );

protected:
	template<typename T>
	void FastWriteMultiCore( T const& v );
	template<typename T, typename...TS>
	void FastWriteMultiCore( T const& v, TS const&...vs );

	template<typename T>
	void FastReadMultiCore( T& v );
	template<typename T, typename...TS>
	void FastReadMultiCore( T& v, TS&...vs );

	template<typename T>
	void WriteMultiCore( T const& v );
	template<typename T, typename...TS>
	void WriteMultiCore( T const& v, TS const&...vs );

	template<typename T>
	bool ReadMultiCore( T& v );
	template<typename T, typename...TS>
	bool ReadMultiCore( T& v, TS&...vs );

	template<typename T>
	void VarWriteMultiCore( T const& v );
	template<typename T, typename...TS>
	void VarWriteMultiCore( T const& v, TS const&...vs );

	template<typename T>
	bool VarReadMultiCore( T& v );
	template<typename T, typename...TS>
	bool VarReadMultiCore( T& v, TS&...vs );
};

}

