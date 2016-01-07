#pragma once
namespace xxlib
{

XX_HAS_FUNC( HasFunc_GetHashCode, GetHashCode, int ( T::* )( ) const );
template<typename T>
typename std::enable_if<HasFunc_GetHashCode<T>::value, int>::type GetHashCodeSwitch( T const& v )
{
	return v.GetHashCode();
};

template<typename T>
typename std::enable_if<std::is_pointer<T>::value, int>::type GetHashCodeSwitch2( T const& v )
{
	return (int)(uint)( (size_t)(void*)v / ( sizeof( size_t ) * 2 ) );    // 经验数据. 经测试发现 x64 下 vc new 至少是 16 的倍数, x86 至少是 8 的倍数
}

template<typename T>
typename std::enable_if<!std::is_pointer<T>::value, int>::type GetHashCodeSwitch2( T const& v )
{
	if( sizeof(T) == 4 ) return (int)v;
	return (int)std::hash<T>()( v );
}

template<typename T>
typename std::enable_if<!HasFunc_GetHashCode<T>::value, int>::type GetHashCodeSwitch( T const& v )
{
	static_assert( std::is_pod<T>::value, "forget impl GetHashCode func ?" );
	return GetHashCodeSwitch2( v );
};

template<typename T>
int GetHashCode( T const &v )
{
	return GetHashCodeSwitch( v );
}

}
