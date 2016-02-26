#include "xxlib_bb.h"   // xxlib::ByteBuffer
#include <assert.h>


namespace xxlib
{
	namespace Serialization
	{
		// signed char
		size_t Calc(signed char const &in)
		{
			return 1;
		}
		size_t WriteTo(char *dstBuf, signed char const &in)
		{
			((signed char*)dstBuf)[0] = in;
			return 1;
		}
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, signed char &out)
		{
			if (dataLen < 1) return false;
			out = ((signed char*)(srcBuf + offset))[0];
			offset += 1;
			return true;
		}



		// short
		size_t Calc(short const &in)
		{
			return 2;
		}
		size_t WriteTo(char *dstBuf, short const &in)
		{
#ifdef XXLIB_IA
			*(short*)dstBuf = in;
#else
			auto p = (char*)&in;
			dstBuf[0] = p[0];
			dstBuf[1] = p[1];
#endif
			return 2;
		}
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, short &out)
		{
			if (dataLen < 2) return false;
#ifdef XXLIB_IA
			out = *(short*)(srcBuf + offset);
#else
			auto p = (char*)&out;
			out[0] = srcBuf[offset + 0];
			out[1] = srcBuf[offset + 1];
#endif
			offset += 2;
			return true;
		}



		// int
		size_t Calc(int const &in)
		{
			return 5;
		}
		size_t WriteTo(char *dstBuf, int const &in)
		{
			size_t offset = 0;
			Utils::VarWrite7(dstBuf, offset, Utils::ZigZagEncode(in));
			return offset;
		}
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, int &out)
		{
			unsigned int i;
			if (!Utils::VarRead7(srcBuf, dataLen, offset, i)) return false;
			out = Utils::ZigZagDecode(i);
			return true;
		}




		// long long
		size_t Calc(long long const &in)
		{
			return 9;
		}
		size_t WriteTo(char *dstBuf, long long const &in)
		{
			size_t offset = 0;
			Utils::VarWrite764(dstBuf, offset, Utils::ZigZagEncode64(in));
			return offset;
		}
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, long long &out)
		{
			unsigned long long i;
			if (!Utils::VarRead764(srcBuf, dataLen, offset, i)) return false;
			out = Utils::ZigZagDecode64(i);
			return true;
		}






		// unsigned char
		size_t Calc(unsigned char const &in)
		{
			return 1;
		}
		size_t WriteTo(char *dstBuf, unsigned char const &in)
		{
			((unsigned char*)dstBuf)[0] = in;
			return 1;
		}
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned char &v)
		{
			if (dataLen < 1) return false;
			v = ((unsigned char*)(srcBuf + offset))[0];
			offset += 1;
			return true;
		}




		// unsigned short
		size_t Calc(unsigned short const &in)
		{
			return 2;
		}
		size_t WriteTo(char *dstBuf, unsigned short const &in)
		{
#ifdef XXLIB_IA
			*(unsigned short*)dstBuf = in;
#else
			auto p = (char*)&v;
			dstBuf[0] = p[0];
			dstBuf[1] = p[1];
#endif
			return 2;
		}
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned short &out)
		{
			if (dataLen < 2) return false;
#ifdef XXLIB_IA
			out = *(unsigned short*)(srcBuf + offset);
#else
			auto p = (char*)&out;
			out[0] = srcBuf[offset + 0];
			out[1] = srcBuf[offset + 1];
#endif
			offset += 2;
			return true;
		}




		// unsigned int
		size_t Calc(unsigned int const &in)
		{
			return 5;
		}
		size_t WriteTo(char *dstBuf, unsigned int const &in)
		{
			size_t offset = 0;
			Utils::VarWrite7(dstBuf, offset, in);
			return offset;
		}
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned int &out)
		{
			return Utils::VarRead7(srcBuf, dataLen, offset, out);
		}




		// unsigned long long
		size_t Calc(unsigned long long const &in)
		{
			return 9;
		}
		size_t WriteTo(char *dstBuf, unsigned long long const &in)
		{
			size_t offset = 0;
			Utils::VarWrite764(dstBuf, offset, in);
			return offset;
		}
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned long long &out)
		{
			return Utils::VarRead764(srcBuf, dataLen, offset, out);
		}





		// long
		size_t Calc(long const &in)
		{
#ifdef _MSC_VER
			return 5;
#else
# ifdef XXLIB_64BIT
			return 9;
# else
			return 5;
# endif
#endif
		}
		size_t WriteTo(char *dstBuf, long const &in)
		{
#ifdef _MSC_VER
			return WriteTo(dstBuf, (int const&)in);
#else
# ifdef XXLIB_64BIT
			return WriteTo(dstBuf, (long long const&)in);
# else
			return WriteTo(dstBuf, (int const&)in);
# endif
#endif
		}
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, long &v)
		{
#ifdef _MSC_VER
			return ReadFrom(srcBuf, dataLen, offset, (int&)v);
#else
# ifdef XXLIB_64BIT
			return ReadFrom(srcBuf, dataLen, offset, (long long&)v);
# else
			return ReadFrom(srcBuf, dataLen, offset, (int&)v);
# endif
#endif
		}


		// unsigned long
		size_t Calc(unsigned long const &in)
		{
#ifdef _MSC_VER
			return 5;
#else
# ifdef XXLIB_64BIT
			return 9;
# else
			return 5;
# endif
#endif
		}
		size_t WriteTo(char *dstBuf, unsigned long const &in)
		{
#ifdef _MSC_VER
			return WriteTo(dstBuf, (unsigned int const&)in);
#else
# ifdef XXLIB_64BIT
			return WriteTo(dstBuf, (unsigned long long const&)in);
# else
			return WriteTo(dstBuf, (unsigned int const&)in);
# endif
#endif
		}
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned long &v)
		{
#ifdef _MSC_VER
			return ReadFrom(srcBuf, dataLen, offset, (unsigned int&)v);
#else
# ifdef XXLIB_64BIT
			return ReadFrom(srcBuf, dataLen, offset, (unsigned long long&)v);
# else
			return ReadFrom(srcBuf, dataLen, offset, (unsigned int&)v);
# endif
#endif
		}





		// char
		size_t Calc(char const &in)
		{
			return 1;
		}
		size_t WriteTo(char *dstBuf, char const &in)
		{
			dstBuf[0] = in;
			return 1;
		}
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, char &v)
		{
			if (dataLen < 1) return false;
			v = srcBuf[offset + 0];
			offset += 1;
			return true;
		}




		// bool
		size_t Calc(bool const &in)
		{
			return 1;
		}
		size_t WriteTo(char *dstBuf, bool const &in)
		{
			dstBuf[0] = in ? 1 : 0;
			return 1;
		}
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, bool &v)
		{
			if (dataLen < 1) return false;
			v = srcBuf[offset + 0] ? true : false;
			offset += 1;
			return true;
		}




		// float
		size_t Calc(float const &in)
		{
			return 4;
		}
		size_t WriteTo(char *dstBuf, float const &in)
		{
#ifdef XXLIB_IA
			*(float*)dstBuf = in;
#else
			auto p = (char*)&in;
			dstBuf[0] = p[0];
			dstBuf[1] = p[1];
			dstBuf[2] = p[2];
			dstBuf[3] = p[3];
#endif
			return 4;
		}
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, float &out)
		{
			if (dataLen < 4) return false;
#ifdef XXLIB_IA
			out = *(float*)(srcBuf + offset);
#else
			auto p = (char*)&out;
			out[0] = srcBuf[offset + 0];
			out[1] = srcBuf[offset + 1];
			out[2] = srcBuf[offset + 2];
			out[3] = srcBuf[offset + 3];
#endif
			offset += 4;
			return true;
		}





		// double
		size_t Calc(double const &in)
		{
			return 8;
		}
		size_t WriteTo(char *dstBuf, double const &in)
		{
#ifdef XXLIB_IA
			*(double*)dstBuf = in;
#else
			auto p = (char*)&in;
			dstBuf[0] = p[0];
			dstBuf[1] = p[1];
			dstBuf[2] = p[2];
			dstBuf[3] = p[3];
			dstBuf[4] = p[4];
			dstBuf[5] = p[5];
			dstBuf[6] = p[6];
			dstBuf[7] = p[7];
#endif
			return 8;
		}
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, double &out)
		{
			if (dataLen < 8) return false;
#ifdef XXLIB_IA
			out = *(double*)(srcBuf + offset);
#else
			auto p = (char*)&out;
			out[0] = srcBuf[offset + 0];
			out[1] = srcBuf[offset + 1];
			out[2] = srcBuf[offset + 2];
			out[3] = srcBuf[offset + 3];
			out[4] = srcBuf[offset + 4];
			out[5] = srcBuf[offset + 5];
			out[6] = srcBuf[offset + 6];
			out[7] = srcBuf[offset + 7];
#endif
			offset += 8;
			return true;
		}





		// std::string
		size_t Calc(std::string const &in)
		{
			return 9 + in.size();
		}
		size_t WriteTo(char *dstBuf, std::string const &in)
		{
			size_t offset = VarWriteTo(dstBuf, in.size());
			memcpy(dstBuf + offset, in.data(), in.size());
			return offset + in.size();
		}
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, std::string &out, size_t minLen, size_t maxLen)
		{
			size_t len;
			if (!VarReadFrom(srcBuf, dataLen, offset, len)) return false;
			if (offset + len > dataLen) return false;
			if (len < minLen || (maxLen > 0 && len > maxLen)) return false;
			out.assign(srcBuf + offset, len);
			offset += len;
			return true;
		}






		// ByteBuffer
		size_t Calc(ByteBuffer const &in)
		{
			return 9 + in.dataLen;
		}
		size_t WriteTo(char *dstBuf, ByteBuffer const &in)
		{
			size_t offset = VarWriteTo(dstBuf, in.dataLen);
			memcpy(dstBuf + offset, in.buf, in.dataLen);
			return offset + in.dataLen;
		}
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, ByteBuffer &out, size_t minLen, size_t maxLen, bool isRef)
		{
			size_t len;
			if (!VarReadFrom(srcBuf, dataLen, offset, len)) return false;
			if (offset + len > dataLen) return false;
			if (len < minLen || (maxLen > 0 && len > maxLen)) return false;
			out.Assign((char*)srcBuf + offset, len, len, isRef);
			offset += len;
			return true;
		}









		// var 系列

		// int
		size_t VarCalc(int const &in)
		{
			return 5;
		}
		size_t VarWriteTo(char *dstBuf, int const &in)
		{
			size_t offset = 0;
			Utils::VarWrite7(dstBuf, offset, Utils::ZigZagEncode(in));
			return offset;
		}
		bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, int &out)
		{
			unsigned int i;
			if (!Utils::VarRead7(srcBuf, dataLen, offset, i)) return false;
			out = Utils::ZigZagDecode(i);
			return true;
		}


		// long long
		size_t VarCalc(long long const &in)
		{
			return 9;
		}
		size_t VarWriteTo(char *dstBuf, long long const &in)
		{
			size_t offset = 0;
			Utils::VarWrite764(dstBuf, offset, Utils::ZigZagEncode64(in));
			return offset;
		}
		bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, long long &out)
		{
			unsigned long long i;
			if (!Utils::VarRead764(srcBuf, dataLen, offset, i)) return false;
			out = Utils::ZigZagDecode64(i);
			return true;
		}


		// unsigned int
		size_t VarCalc(unsigned int const &in)
		{
			return 5;
		}
		size_t VarWriteTo(char *dstBuf, unsigned int const &in)
		{
			size_t offset = 0;
			Utils::VarWrite7(dstBuf, offset, in);
			return offset;
		}
		bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned int &out)
		{
			return Utils::VarRead7(srcBuf, dataLen, offset, out);
		}


		// unsigned long long
		size_t VarCalc(unsigned long long const &in)
		{
			return 9;
		}
		size_t VarWriteTo(char *dstBuf, unsigned long long const &in)
		{
			size_t offset = 0;
			Utils::VarWrite764(dstBuf, offset, in);
			return offset;
		}
		bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned long long &out)
		{
			return Utils::VarRead764(srcBuf, dataLen, offset, out);
		}


		// long
		size_t VarCalc(long const &in)
		{
#ifdef _MSC_VER
			return 5;
#else
# ifdef XXLIB_64BIT
			return 9;
# else
			return 5;
# endif
#endif
		}
		size_t VarWriteTo(char *dstBuf, long const &in)
		{
#ifdef _MSC_VER
			return VarWriteTo(dstBuf, (int const&)in);
#else
# ifdef XXLIB_64BIT
			return VarWriteTo(dstBuf, (long long const&)in);
# else
			return VarWriteTo(dstBuf, (int const&)in);
# endif
#endif
		}
		bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, long &v)
		{
#ifdef _MSC_VER
			return VarReadFrom(srcBuf, dataLen, offset, (int&)v);
#else
# ifdef XXLIB_64BIT
			return VarReadFrom(srcBuf, dataLen, offset, (long long&)v);
# else
			return VarReadFrom(srcBuf, dataLen, offset, (int&)v);
# endif
#endif
		}


		// unsigned long
		size_t VarCalc(unsigned long const &in)
		{
#ifdef _MSC_VER
			return 5;
#else
# ifdef XXLIB_64BIT
			return 9;
# else
			return 5;
# endif
#endif
		}
		size_t VarWriteTo(char *dstBuf, unsigned long const &in)
		{
#ifdef _MSC_VER
			return VarWriteTo(dstBuf, (unsigned int const&)in);
#else
# ifdef XXLIB_64BIT
			return VarWriteTo(dstBuf, (unsigned long long const&)in);
# else
			return VarWriteTo(dstBuf, (unsigned int const&)in);
# endif
#endif
		}
		bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned long &v)
		{
#ifdef _MSC_VER
			return VarReadFrom(srcBuf, dataLen, offset, (unsigned int&)v);
#else
# ifdef XXLIB_64BIT
			return VarReadFrom(srcBuf, dataLen, offset, (unsigned long long&)v);
# else
			return VarReadFrom(srcBuf, dataLen, offset, (unsigned int&)v);
# endif
#endif
		}


		// double
		size_t VarCalc(double const &in)
		{
			return 9;
		}
		size_t VarWriteTo(char *dstBuf, double const &in)
		{
			if (in == 0)
			{
				dstBuf[0] = 0;
				return 1;
			}
			else
			{
				auto i = (int)in;
				if (in == (double)i)
				{
					dstBuf[0] = 1;
					return 1 + VarWriteTo(dstBuf + 1, i);
				}
				else
				{
					dstBuf[0] = 2;
					return 1 + WriteTo(dstBuf + 1, in);
				}
			}
		}
		bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, double &out)
		{
			if (offset >= dataLen) return false;	// 确保还有 1 字节可读
			switch (srcBuf[offset++])				// 跳过 1 字节
			{
			case 0:
				out = 0;
				return true;
			case 1:
			{
				int rtv = 0;
				if (!VarReadFrom(srcBuf, dataLen, offset, rtv)) return false;
				out = rtv;
				return true;
			}
			case 2:
			{
				return ReadFrom(srcBuf, dataLen, offset, out);
			}
			default:
				return false;
			}
		}


	}

}





















namespace xxlib
{
	ByteBuffer::ByteBuffer(size_t capacity)
		: dataLen(0)
		, offset(0)
		, isRef(false)
	{
		if (capacity < 64)
		{
			bufLen = 64;
		}
		else
		{
			bufLen = Utils::Round2n(capacity);
		}
		buf = (char*)malloc(bufLen);
	}

	ByteBuffer::ByteBuffer()
		: ByteBuffer(64)
	{
	}

	ByteBuffer::ByteBuffer(char *buf_, size_t bufLen_, size_t dataLen_, bool isRef_)
	{
		dataLen = dataLen_;
		offset = 0;
		if (isRef_)
		{
			buf = buf_;
			bufLen = bufLen_;
			isRef = true;
		}
		else
		{
			bufLen = Utils::Round2n(dataLen_);
			buf = (char*)malloc(bufLen);
			isRef = false;
			memcpy(buf, buf_, dataLen_);
		}
	}

	ByteBuffer::ByteBuffer(ByteBuffer const& o)
		: ByteBuffer(o.dataLen)
	{
		memcpy(buf, o.buf, o.dataLen);
		dataLen = o.dataLen;
		offset = o.offset;
	}

	ByteBuffer::ByteBuffer(ByteBuffer&& o)
		: buf(o.buf)
		, bufLen(o.bufLen)
		, dataLen(o.dataLen)
		, offset(o.offset)
		, isRef(o.isRef)
	{
		o.buf = nullptr;
	}

	ByteBuffer& ByteBuffer::operator=(ByteBuffer const& o)
	{
		if (this == &o) return *this;
		dataLen = o.dataLen;
		offset = o.offset;
		if (bufLen >= o.dataLen)
		{
			memcpy(buf, o.buf, o.dataLen);
		}
		else
		{
			if (!isRef)
			{
				free(buf);
			}
			else
			{
				isRef = false;
			}
			bufLen = Utils::Round2n(o.dataLen);
			buf = (char*)malloc(bufLen);
			memcpy(buf, o.buf, o.dataLen);
		}
		return *this;
	}

	ByteBuffer& ByteBuffer::operator=(ByteBuffer&& o)
	{
		if (!isRef)
		{
			free(buf);
		}
		buf = o.buf;
		bufLen = o.bufLen;
		dataLen = o.dataLen;
		offset = o.offset;
		isRef = o.isRef;
		o.buf = nullptr;
		return *this;
	}

	ByteBuffer::~ByteBuffer()
	{
		if (buf && !isRef)
		{
			free(buf);
		}
	}

	void ByteBuffer::Reserve(size_t capacity)
	{
		if (capacity <= bufLen) return;
		ReserveCore(capacity);
	}
	void ByteBuffer::ReserveCore(size_t capacity)
	{
		bufLen = Utils::Round2n(capacity);
		auto newBuf = (char*)malloc(capacity);
		memcpy(newBuf, buf, dataLen);
		if (!isRef)
		{
			free(buf);
		}
		else
		{
			isRef = false;
		}
		buf = newBuf;
	}


	void ByteBuffer::Resize(size_t len)
	{
		if (len == dataLen)
		{
			return;
		}
		else if (len < dataLen)
		{
			dataLen = len;
		}
		else
		{
			Reserve(len);
			dataLen = len;
		}
	}

	void ByteBuffer::Pop()
	{
		assert(dataLen > 0);
		--dataLen;
	}

	char& ByteBuffer::Top()
	{
		assert(dataLen > 0);
		return buf[dataLen - 1];
	}

	char const& ByteBuffer::Top() const
	{
		assert(dataLen > 0);
		return buf[dataLen - 1];
	}

	void ByteBuffer::Clear()
	{
		dataLen = 0;
		offset = 0;
	}

	size_t ByteBuffer::Size() const
	{
		return dataLen;
	}

	bool ByteBuffer::Empty() const
	{
		return dataLen == 0;
	}


	char ByteBuffer::operator[](size_t idx) const
	{
		assert(idx < dataLen);
		return buf[idx];
	}

	char& ByteBuffer::operator[](size_t idx)
	{
		assert(idx < dataLen);
		return buf[idx];
	}

	char ByteBuffer::At(size_t idx) const
	{
		assert(idx < dataLen);
		return buf[idx];
	}

	char& ByteBuffer::At(size_t idx)
	{
		assert(idx < dataLen);
		return buf[idx];
	}

	void ByteBuffer::Assign(char *buf_, size_t bufLen_, size_t dataLen_, bool isRef_)
	{
		assert(buf != buf_);
		if (isRef_)
		{
			if (!isRef)
			{
				free(this->buf);
				isRef = true;
			}
			buf = buf_;
			bufLen = bufLen_;
			dataLen = dataLen_;
			return;
		}
		else if (bufLen <= dataLen_)
		{
			if (!this->isRef)
			{
				free(this->buf);
			}
			else
			{
				this->isRef = false;
			}
			bufLen = Utils::Round2n(dataLen_);
			buf = (char*)malloc(bufLen);
		}
		dataLen = dataLen_;
		memcpy(buf, buf_, dataLen_);
	}




	bool ByteBuffer::operator==(ByteBuffer const& o) const
	{
		if (this == &o) return true;
		if (dataLen != o.dataLen) return false;
		return memcmp(buf, o.buf, dataLen) == 0;
	}

	bool ByteBuffer::operator!=(ByteBuffer const& o) const
	{
		return !operator==(o);
	}

	bool ByteBuffer::operator<(ByteBuffer const& o) const
	{
		if (this == &o) return false;
		auto r = memcmp(buf, o.buf, dataLen < o.dataLen ? dataLen : o.dataLen);
		if (r == 0) return dataLen < o.dataLen;
		return r < 0;
	}

	bool ByteBuffer::operator>(ByteBuffer const& o) const
	{
		if (this == &o) return false;
		auto r = memcmp(buf, o.buf, dataLen < o.dataLen ? dataLen : o.dataLen);
		if (r == 0) return dataLen > o.dataLen;
		return r > 0;
	}

	bool ByteBuffer::operator<=(ByteBuffer const& o) const
	{
		return !operator>(o);
	}

	bool ByteBuffer::operator>=(ByteBuffer const& o) const
	{
		return !operator<(o);
	}



	bool ByteBuffer::ReadByteBuffer(ByteBuffer &v, size_t minLen, size_t maxLen, bool isRef)
	{
		return Serialization::ReadFrom(buf, dataLen, offset, v, minLen, maxLen, isRef);
	}

}
