#pragma once
namespace xxlib
{
class ScopeGuard
{
public:
	typedef std::function<void()> FT;
	template<typename T>
	ScopeGuard( T&& f )
		: func( std::forward<T>( f ) )
	{
	}
	~ScopeGuard();
	void RunAndCancel();
	void Run();
	void Cancel();
	template<typename T>
	inline void Set( T&& f )
	{
		func = std::forward<T>( f );
	}
private:
	FT func;
	ScopeGuard( ScopeGuard const& ) = delete;
	ScopeGuard &operator=( ScopeGuard const& ) = delete;
};

}
