#pragma once
#include "xxlib_simpledate.h"
#include "xxlib_bb.h"

namespace xxlib
{
	namespace Serialization
	{
		inline size_t Calc(SimpleDate const &in)
		{
			return Calc(in.data);
		}
		inline size_t WriteTo(char *dstBuf, SimpleDate const &in)
		{
			return WriteTo(dstBuf, in.data);
		}
		inline bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, SimpleDate &out)
		{
			return ReadFrom(srcBuf, dataLen, offset, out.data);
		}
	}
}

