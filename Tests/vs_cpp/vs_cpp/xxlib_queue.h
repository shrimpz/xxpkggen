#pragma once
namespace xxlib
{

// memory model: ring buffer
// move to RIGHT way = forward
// 3 situation:
//...............FR...............		// Front == Rear
//......Front+++++++++++Rear......		// DataLen = Rear - Front
// ++++++Rear...........Front++++++		// DataLen = BufLen - Front + Rear
template <class T>
class Queue
{
public:
	Queue();								// Queue( 8 )
	explicit Queue( int capacity );
	~Queue();
	Queue( Queue const& o );
	Queue( Queue&& o );
	Queue& operator=( Queue const& o );
	Queue& operator=( Queue&& o );

	T const& operator[]( int idx ) const;	// [0] = Front()
	T& operator[]( int idx );
	T const& IndexAt( int idx ) const;		// like []
	T& IndexAt( int idx );

	int Size() const;
	bool Empty() const;
	void Clear();
	void Reserve( int capacity, bool afterPush = false );

	template<typename...PTS>
	T& EmplacePush( PTS&&...ps );			// [ Rear++ ] = T( ps )
	void Push( T const& v );
	void Push( T&& v );
	T& Front();
	T const& Front() const;
	void Pop();								// ++Front
	bool Pop( T& outVal );
	int PopMulti( int count );				// Front += count

private:
	int front, rear;						
	int bufLen;								
	T* buf;									
};


}

