#pragma once
namespace xxlib
{

// 直接由 Hash 去 buckets 简化而来
// 特点是 Add 返回的 Node* 值不会变化，对象可存储该值以实现高速 Remove
template <typename T>
class Links : Memmoveable
{
public:
	struct Node
	{
		T               value;
		int             index;                              // nodes[ index ]
	};
	List<Node*>         nodes;

	Links();												// Links( 64 )
	explicit Links( int capacity);
	Links( Links&& o );
	Links( Links const& o );
	Links& operator=( Links&& o );
	Links& operator=( Links const& o );
	~Links();
	template<typename...PTS>
	Node* EmplaceAdd( PTS&&...vps );						// same as Add but need value type's construct parameters
	Node* Add( T&& v );
	Node* Add( T const& v );
	Node* Find( T const& v );								// foreach scan. if exists, return Node*. else return nullptr
	void Remove( T const& v );
	void Remove( Node* n );
	void Clear();
	bool Empty();
	void Reserve( int capacity );
	int Size() const;
	Node const* operator[]( int idx ) const;
	Node* operator[]( int idx );
	Node const* IndexAt( int idx ) const;
	Node* IndexAt( int idx );

protected:
	void Dispose( Node* n );
	Pool                pool;
};

}

