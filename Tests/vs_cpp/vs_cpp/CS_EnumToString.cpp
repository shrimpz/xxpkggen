//#include "precompile.h"
# include "CS_EnumToString.h"
namespace CS_EnumToString
{
char const* const Get( ::CS::Enum1 e )
{
    switch(e)
    {
    case ::CS::Enum1::Item1: 
        return u8"::CS::Enum1::Item1 RootEnum1 Item1's desc";
    case ::CS::Enum1::Item2: 
        return u8"::CS::Enum1::Item2 RootEnum1 Ite2's desc";
    case ::CS::Enum1::Item3: 
        return u8"::CS::Enum1::Item3 ";
    case ::CS::Enum1::Item4: 
        return u8"::CS::Enum1::Item4 ";
    case ::CS::Enum1::Item5: 
        return u8"::CS::Enum1::Item5 ";
    }
    assert( false );
    return u8"";
}
char const* const GetName( ::CS::Enum1 e )
{
    switch(e)
    {
    case ::CS::Enum1::Item1: 
        return u8"Item1";
    case ::CS::Enum1::Item2: 
        return u8"Item2";
    case ::CS::Enum1::Item3: 
        return u8"Item3";
    case ::CS::Enum1::Item4: 
        return u8"Item4";
    case ::CS::Enum1::Item5: 
        return u8"Item5";
    }
    assert( false );
    return u8"";
}
char const* const GetDesc( ::CS::Enum1 e )
{
    switch(e)
    {
    case ::CS::Enum1::Item1: 
        return u8"RootEnum1 Item1's desc";
    case ::CS::Enum1::Item2: 
        return u8"RootEnum1 Ite2's desc";
    case ::CS::Enum1::Item3: 
        return u8"";
    case ::CS::Enum1::Item4: 
        return u8"";
    case ::CS::Enum1::Item5: 
        return u8"";
    }
    assert( false );
    return u8"";
}
char const* const Get( ::CS::Ns2::Enum1 e )
{
    switch(e)
    {
    case ::CS::Ns2::Enum1::item1: 
        return u8"::CS::Ns2::Enum1::item1 ";
    }
    assert( false );
    return u8"";
}
char const* const GetName( ::CS::Ns2::Enum1 e )
{
    switch(e)
    {
    case ::CS::Ns2::Enum1::item1: 
        return u8"item1";
    }
    assert( false );
    return u8"";
}
char const* const GetDesc( ::CS::Ns2::Enum1 e )
{
    switch(e)
    {
    case ::CS::Ns2::Enum1::item1: 
        return u8"";
    }
    assert( false );
    return u8"";
}
char const* const Get( ::CS::Ns1::Enum1 e )
{
    switch(e)
    {
    case ::CS::Ns1::Enum1::item1: 
        return u8"::CS::Ns1::Enum1::item1 ";
    }
    assert( false );
    return u8"";
}
char const* const GetName( ::CS::Ns1::Enum1 e )
{
    switch(e)
    {
    case ::CS::Ns1::Enum1::item1: 
        return u8"item1";
    }
    assert( false );
    return u8"";
}
char const* const GetDesc( ::CS::Ns1::Enum1 e )
{
    switch(e)
    {
    case ::CS::Ns1::Enum1::item1: 
        return u8"";
    }
    assert( false );
    return u8"";
}
} // CS_EnumToString
