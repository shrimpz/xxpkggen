#pragma once
namespace xxlib
{

class ByteBuffer;


template <typename TK, typename TV>
class Dict : Memmoveable
{
public:
	struct Node
	{
		Node*           next;
		uint            hash;                               // key's hashCode
		int             index;                              // nodes[ index ]
		TK              key;
		TV              value;
	};
	List<Node*>         nodes;

	Dict();													// Dict( 8 )
	explicit Dict( int capacity );
	~Dict();

	Dict( Dict&& o );
	Dict( Dict const& o );
	Dict& operator=( Dict&& o );
	Dict& operator=( Dict const& o );

	template<typename KT, typename...VPTS>
	std::pair<Node*, bool> EmplaceAdd( bool replace, KT&& k, VPTS&&...vps );		// same as Add but need value type's construct parameters
	std::pair<Node*, bool> Add( TK const& k, TV const& v, bool replace = false );	// if exists, return Node* + false. else add and return Node* + true
	std::pair<Node*, bool> Add( TK const& k, TV&& v, bool replace = false );
	std::pair<Node*, bool> Add( TK&& k, TV const& v, bool replace = false );
	std::pair<Node*, bool> Add( TK&& k, TV&& v, bool replace = false );

	Node const* Find( TK const& k ) const;                  // if exists, return Node*. else return nullptr
	Node* Find( TK const& k );
	TV& At( TK const& k );                                  // Find or Add default value( if not found ) and return
	TV& operator[]( TK const& k );                          // return at( k )

	void Remove( TK const& k );
	void Remove( Node* n );

	void Reserve( int capacity );
	void Clear();
	bool Empty();
	int Size() const;

	Node const* IndexAt( int idx ) const;                   // return nodes[ idx ]
	Node* IndexAt( int idx );


	// ByteBuffer interface
	void WriteTo( ByteBuffer& bb ) const;
	void FastWriteTo( ByteBuffer& bb ) const;
	bool ReadFrom( ByteBuffer& bb );
	bool ReadFrom( ByteBuffer& bb, int minLen, int maxLen );
	void VarWriteTo( ByteBuffer& bb ) const;
	bool VarReadFrom( ByteBuffer& bb );
	bool VarReadFrom( ByteBuffer& bb, int minLen, int maxLen );
protected:
	void Dispose( Node* n );
	List<Node*>         buckets;
	Pool                pool;
};
}

