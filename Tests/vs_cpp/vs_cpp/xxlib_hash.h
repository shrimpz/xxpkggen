#pragma once
namespace xxlib
{
// 直接由 Dict 去 value 简化而来

template <typename TK>
struct Hash : Memmoveable
{
	struct Node
	{
		Node*           next;
		uint            hash;                               // key's hashCode
		int             index;                              // nodes[ index ]
		TK              key;
	};
	List<Node*>         nodes;

	Hash();													// Hash( 64 )
	explicit Hash( int capacity);
	Hash( Hash&& o );
	Hash( Hash const& o );
	Hash& operator=( Hash&& o );
	Hash& operator=( Hash const& o );
	~Hash();
	template<typename...PTS>
	std::pair<Node*, bool> EmplaceAdd( PTS&&...ps );		// same as Add but need value type's construct parameters
	std::pair<Node*, bool> Add( TK&& v );					// if exists, return Node* + false. else add and return Node* + true
	std::pair<Node*, bool> Add( TK const& v );
	Node* Find( TK const& k );                              // if exists, return Node*. else return nullptr
	void Remove( TK const& k );
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
	void Resize();
	List<Node*>         buckets;
	Pool                pool;
};

}

