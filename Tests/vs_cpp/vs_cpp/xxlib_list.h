#pragma once
namespace xxlib
{

class ByteBuffer;

template<typename T>
class List : Memmoveable
{
public:
	T*          buf;
	int         bufLen;
	int         dataLen;

	List();
	explicit List( int capacity );
	List( std::initializer_list<T> items );
	~List();
	List( List&& o );
	List( List const& o );
	List& operator=( List&& o );
	List& operator=( List const& o );
	template<typename...PTS>
	T& EmplaceAdd( PTS&&...ps );
	void Add( T&& v );
	void Add( T const& v );
	template<typename...PTS>
	T& EmplaceInsertAt( int idx, PTS&&...ps );
	void InsertAt( int idx, T&& v );
	void InsertAt( int idx, T const& v );
	int Find( T const& v ) const;
	void RemoveAt( int idx );
	void FastRemoveAt( int idx );			// swap with last item + Pop()
	void FastAdd( T&& v );				// unsafe: do not check space enough
	void FastAdd( T const& v );			// unsafe: do not check space enough

	T& Top();
	T const& Top() const;
	void Pop();
	bool Pop( T& output );
	void Clear();
	void Reserve( int capacity );
	void Resize( int capacity, bool init = true );
	int DataByteLen() const;
	int BufByteLen() const;
	T const& operator[]( int idx ) const;
	T& operator[]( int idx );
	T const& IndexAt( int idx ) const;
	T& IndexAt( int idx );
	void Set( int idx, T&& v );
	void Set( int idx, T const& v );

	// ByteBuffer interface
	void WriteTo( ByteBuffer& bb ) const;
	void FastWriteTo( ByteBuffer& bb ) const;
	bool ReadFrom( ByteBuffer& bb );
	bool ReadFrom( ByteBuffer& bb, int minLen, int maxLen );
	void VarWriteTo( ByteBuffer& bb ) const;
	bool VarReadFrom( ByteBuffer& bb );
	bool VarReadFrom( ByteBuffer& bb, int minLen, int maxLen );
};



// 2进制排列为 从右到左，由低到高位，即小尾机可 memcpy
template<>
class List<bool> : Memmoveable
{
public:
	char*       buf = nullptr;
	int         bufLen = 0;
	int         dataLen = 0;

	List();							// List( 512 )
	explicit List( int capacity );
    List( std::initializer_list<bool> items );
	~List();
	List( List<bool>&& o );
	List( List<bool> const& o );
	List& operator=( List<bool>&& o );
	List& operator=( List<bool> const& o );
	void Add( bool bit );
	void Pop();
	bool Top() const;
	void Clear();
	void Reserve( int capacity );
	int DataByteLen() const;
	int BufByteLen() const;
	bool operator[]( int idx ) const;
	bool IndexAt( int idx ) const;
	void Set( int idx, bool bit );
	void SetTrue( int idx );
	void SetFalse( int idx );
	void FillTrue();
	void FillFalse();
	void Fill( bool bit, int idxFrom = 0, int idxTo = 0 );
	void Resize( int capacity, bool init = true );
	void CleanUp() const;   // clear last byte's unused bits
	// todo: Remove? Insert?

	// ByteBuffer interface
	void WriteTo( ByteBuffer& bb ) const;
	void FastWriteTo( ByteBuffer& bb ) const;
	bool ReadFrom( ByteBuffer& bb );
	bool ReadFrom( ByteBuffer& bb, int minLen, int maxLen );
	void VarWriteTo( ByteBuffer& bb ) const;
	bool VarReadFrom( ByteBuffer& bb );
	bool VarReadFrom( ByteBuffer& bb, int minLen, int maxLen );

};


}

