#pragma once

# include "CS.h"

namespace CS_EnumToString
{
static char const* const Get( ::CS::Enum1 e );
static char const* const GetName( ::CS::Enum1 e );
static char const* const GetDesc( ::CS::Enum1 e );
static char const* const Get( ::CS::Ns2::Enum1 e );
static char const* const GetName( ::CS::Ns2::Enum1 e );
static char const* const GetDesc( ::CS::Ns2::Enum1 e );
static char const* const Get( ::CS::Ns1::Enum1 e );
static char const* const GetName( ::CS::Ns1::Enum1 e );
static char const* const GetDesc( ::CS::Ns1::Enum1 e );
} // CS_EnumToString
