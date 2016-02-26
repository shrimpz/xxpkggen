#pragma once
#include "xxlib_bb.h"
#include "xxlib_list.h"

namespace xxlib
{
	namespace Serialization
	{

		template<typename T>
		struct Limit<List<T>>
		{
			static const bool value = true;
		};

		template<typename T>
		inline size_t Calc(List<T> const &in)
		{
			if (std::is_arithmetic<T>::value || std::is_enum<T>::value)
			{
				return 9 + in.dataLen * sizeof(T);
			}
			else
			{
				size_t len = 9;
				for (auto const& o : in) len += Calc(o);
				return len;
			}
		}

		template<typename T>
		inline size_t WriteTo(char *dstBuf, List<T> const &in)
		{
			size_t offset = VarWriteTo(dstBuf, in.dataLen);
			if (std::is_arithmetic<T>::value || std::is_enum<T>::value)
			{
				memcpy(dstBuf + offset, in.buf, in.dataLen * sizeof(T));
				offset += in.dataLen * sizeof(T);
			}
			else
			{
				for (auto const& o : in)
				{
					offset += WriteTo(dstBuf + offset, o);
				}
			}
			return offset;
		}

		template<typename T>
		inline bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, List<T> &out, size_t minLen, size_t maxLen)
		{
			size_t len;
			if (!VarReadFrom(srcBuf, dataLen, offset, len)) return false;
			if (len < minLen || (maxLen > 0 && len > maxLen)) return false;

			out.Clear();
			if (!len) return true;

			if (std::is_arithmetic<T>::value || std::is_enum<T>::value)
			{
				if (offset + len * sizeof(T) > dataLen) return false;
				out.Resize(len, false);
				memcpy(out.buf, srcBuf + offset, len * sizeof(T));
			}
			else
			{
				out.Reserve(len);
				for (size_t i = 0; i < len; ++i)
				{
					auto p = new (out.buf + i) T();
					out.dataLen = i + 1;
					if (!ReadFromSwitch(srcBuf, dataLen, offset, *p)) return false;
				}
			}
			return true;
		}












		// bool

		template<>
		inline size_t Calc(List<bool> const &in)
		{
			return 9 + in.DataByteLen();
		}

		template<>
		inline size_t WriteTo(char *dstBuf, List<bool> const &in)
		{
			size_t offset = VarWriteTo(dstBuf, in.dataLen);
			if (in.dataLen)
			{
				in.CleanUp();
				auto byteLen = in.DataByteLen();
				memcpy(dstBuf + offset, in.buf, byteLen);
				offset += byteLen;
			}
			return offset;
		}

		template<>
		inline bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, List<bool> &out, size_t minLen, size_t maxLen)
		{
			size_t len;
			if (!VarReadFrom(srcBuf, dataLen, offset, len)) return false;
			if (len < minLen || (maxLen > 0 && len > maxLen)) return false;

			out.Clear();
			if (!len) return true;

			auto byteLen = out.CalcByteLen(len);
			if (offset + byteLen > dataLen) return false;
			out.Resize(len, false);
			memcpy(out.buf, srcBuf + offset, byteLen);
			return true;
		}











		// var

		template<typename T>
		inline size_t VarCalc(List<T> const &in)
		{
			if (std::is_arithmetic<T>::value || std::is_enum<T>::value)
			{
				return 9 + in.dataLen * (sizeof(T) + 1);	// int, long long, double : 5 - 9
			}
			else
			{
				size_t len = 9;
				for (auto const& o : in) len += VarCalc(o);
				return len;
			}
		}

		template<typename T>
		inline size_t VarWriteTo(char *dstBuf, List<T> const &in)
		{
			size_t offset = VarWriteTo(dstBuf, in.dataLen);
			for (auto const& o : in)
			{
				offset += VarWriteTo(dstBuf + offset, o);
			}
			return offset;
		}

		template<typename T>
		inline bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, List<T> &out, size_t minLen, size_t maxLen)
		{
			size_t len;
			if (!VarReadFrom(srcBuf, dataLen, offset, len)) return false;
			if (len < minLen || (maxLen > 0 && len > maxLen)) return false;

			out.Clear();
			if (!len) return true;

			out.Reserve(len);
			for (size_t i = 0; i < len; ++i)
			{
				auto p = new (out.buf + i) T();
				out.dataLen = i + 1;
				if (!VarReadFromSwitch(srcBuf, dataLen, offset, *p)) return false;
			}
			return true;
		}

	}
}
