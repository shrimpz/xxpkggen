#pragma once
#include "xxlib_bb.h"
#include "xxlib_sb.h"

namespace xxlib
{
	namespace Serialization
	{
		template<>
		struct Limit<StringBuilder>
		{
			static const bool value = true;
		};

		inline size_t Calc(StringBuilder const &in)
		{
			return 9 + in.dataLen;
		}

		inline size_t WriteTo(char *dstBuf, StringBuilder const &in)
		{
			size_t offset = VarWriteTo(dstBuf, in.dataLen);
			memcpy(dstBuf + offset, in.buf, in.dataLen);
			return offset + in.dataLen;
		}

		inline bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, StringBuilder &out, size_t minLen, size_t maxLen, bool isRef = false)
		{
			size_t len;
			if (!VarReadFrom(srcBuf, dataLen, offset, len)) return false;
			if (len < minLen || (maxLen > 0 && len > maxLen)) return false;
			if (offset + len > dataLen) return false;
			// 重要：引用模式多半会破坏 buf 对齐规则，于计算 hash 不利( 除非未来更换不要求对齐的 hash 算法 )
			out.Assign((char*)srcBuf + offset, len, len, isRef);
			offset += len;
			return true;
		}

	}
}
