#pragma once
#include "xxlib_utils.h"
#include "xxlib_bb_utils.h"
#include <string>


namespace xxlib
{
	struct ByteBuffer;
    namespace Serialization
    {
        /*
        signed char
        short
        int                     // size_t               in MSVC  32 bit
        long long               // ptrdiff_t            in MSVC  64 bit
        unsigned char
        unsigned short
        unsigned int            // size_t               in MSVC  32 bit
        unsigned long long      // size_t               in MSVC  64 bit

        long                    // ptrdiff_t / ssize_t  in GCC 32/64 bit
        unsigned long           // size_t               in GCC 32/64 bit

        bool
        char
        float
        double

        enum
        std::string
        */


		// todo: xxlib_std_vector_bb_ext.h
		// todo: xxlib_std_string_bb_ext.h

		// todo: 加个接口：自带 placement new 的 ReadFrom

		// todo: T[] support ( use fixed list ? )

        // todo: wchar_t  wchar_t*  utf8 ??




        // 是否为长度受限制的数据类型( ReadFrom 时将调用 minLen, maxLen 版本 )
        template<typename T>
        struct Limit
        {
            static const bool value = false;
        };


		// readfrom switch
		template<typename T>
		typename std::enable_if<Serialization::Limit<T>::value, bool>::type
			ByteBuffer::ReadFromSwitch(char const *srcBuf, size_t const &dataLen, size_t &offset, T &v)
		{
			return Serialization::ReadFrom(srcBuf, dataLen, offset, v, 0, 0);
		}

		template<typename T>
		typename std::enable_if<!Serialization::Limit<T>::value, bool>::type
			ByteBuffer::ReadFromSwitch(char const *srcBuf, size_t const &dataLen, size_t &offset, T &v)
		{
			return Serialization::ReadFrom(srcBuf, dataLen, offset, v);
		}


		// var
		template<typename T>
		typename std::enable_if<Serialization::Limit<T>::value, bool>::type
			ByteBuffer::VarReadFromSwitch(char const *srcBuf, size_t const &dataLen, size_t &offset, T &v)
		{
			return Serialization::VarReadFrom(srcBuf, dataLen, offset, v, 0, 0);
		}

		template<typename T>
		typename std::enable_if<!Serialization::Limit<T>::value, bool>::type
			ByteBuffer::VarReadFromSwitch(char const *srcBuf, size_t const &dataLen, size_t &offset, T &v)
		{
			return Serialization::VarReadFrom(srcBuf, dataLen, offset, v);
		}



        // signed char
        size_t Calc(signed char const &in);
        size_t WriteTo(char *dstBuf, signed char const &in);
        bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, signed char &out);

        // short
        size_t Calc(short const &in);
        size_t WriteTo(char *dstBuf, short const &in);
        bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, short &out);

        // int
        size_t Calc(int const &in);
        size_t WriteTo(char *dstBuf, int const &in);
        bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, int &out);

        // long long
        size_t Calc(long long const &in);
        size_t WriteTo(char *dstBuf, long long const &in);
        bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, long long &out);


        // unsigned char
        size_t Calc(unsigned char const &in);
        size_t WriteTo(char *dstBuf, unsigned char const &in);
        bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned char &out);

        // unsigned short
        size_t Calc(unsigned short const &in);
        size_t WriteTo(char *dstBuf, unsigned short const &in);
        bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned short &out);

        // unsigned int
        size_t Calc(unsigned int const &in);
        size_t WriteTo(char *dstBuf, unsigned int const &in);
        bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned int &out);

        // unsigned long long
        size_t Calc(unsigned long long const &in);
        size_t WriteTo(char *dstBuf, unsigned long long const &in);
        bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned long long &out);

        // long
        size_t Calc(unsigned long long const &in);
        size_t WriteTo(char *dstBuf, unsigned long long const &in);
        bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned long long &out);

        // unsigned long
        size_t Calc(unsigned long long const &in);
        size_t WriteTo(char *dstBuf, unsigned long long const &in);
        bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned long long &out);


        // char
        size_t Calc(char const &in);
        size_t WriteTo(char *dstBuf, char const &in);
        bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, char &out);

        // bool
        size_t Calc(bool const &in);
        size_t WriteTo(char *dstBuf, bool const &in);
        bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, bool &out);

        // float
        size_t Calc(float const &in);
        size_t WriteTo(char *dstBuf, float const &in);
        bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, float &out);

        // double
        size_t Calc(double const &in);
        size_t WriteTo(char *dstBuf, double const &in);
        bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, double &out);

        // enum
        template<typename ET>
        size_t Calc(ET const &in)
        {
            return Calc((typename std::underlying_type<ET>::type)in);
        }
        template<typename ET>
        size_t WriteTo(char *dstBuf, ET const &in)
        {
            return WriteTo(dstBuf, (typename std::underlying_type<ET>::type)in);
        }
        template<typename ET>
        bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, ET &out)
        {
            return ReadFrom(srcBuf, dataLen, offset, (typename std::underlying_type<ET>::type)out);
        }


		
		// ByteBuffer
		template<>
		struct Limit<ByteBuffer>
		{
			static const bool value = true;
		};
		size_t Calc(ByteBuffer const &in);
		size_t WriteTo(char *dstBuf, ByteBuffer const &in);
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, ByteBuffer &out, size_t minLen, size_t maxLen, bool isRef = false);

		
		
		
		// std::string
        template<>
        struct Limit<std::string>
        {
            static const bool value = true;
        };
        size_t Calc(std::string const &in);
        size_t WriteTo(char *dstBuf, std::string const &in);
        bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, std::string &out, size_t minLen, size_t maxLen);








		// var

		// int
		size_t VarCalc(int const &in);
		size_t VarWriteTo(char *dstBuf, int const &in);
		bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, int &out);

		// long long
		size_t VarCalc(long long const &in);
		size_t VarWriteTo(char *dstBuf, long long const &in);
		bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, long long &out);

		// unsigned int
		size_t VarCalc(unsigned int const &in);
		size_t VarWriteTo(char *dstBuf, unsigned int const &in);
		bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned int &out);

		// unsigned long long
		size_t VarCalc(unsigned long long const &in);
		size_t VarWriteTo(char *dstBuf, unsigned long long const &in);
		bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned long long &out);

		// long
		size_t VarCalc(unsigned long long const &in);
		size_t VarWriteTo(char *dstBuf, unsigned long long const &in);
		bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned long long &out);

		// unsigned long
		size_t VarCalc(unsigned long long const &in);
		size_t VarWriteTo(char *dstBuf, unsigned long long const &in);
		bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, unsigned long long &out);

		// double
		size_t VarCalc(double const &in);
		size_t VarWriteTo(char *dstBuf, double const &in);
		bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, double &out);




		// 变参系列



		// var
		template<typename T, typename ...TS>
		size_t Calc(T const &v, TS const & ...vs);

		template<typename T, typename ...TS>
		size_t WriteTo(char *dstBuf, T const &v, TS const & ...vs);

		template<typename T, typename ...TS>
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, T &out, TS &...outs);




		template<typename T, typename ...TS>
		size_t VarCalc(T const &v, TS const & ...vs);

		template<typename T, typename ...TS>
		size_t VarWriteTo(char *dstBuf, T const &v, TS const & ...vs);

		template<typename T, typename ...TS>
		bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, T &out, TS &...outs);




        // todo: more containers, vector<bool> ? map? ...
    }
}










namespace xxlib
{
    namespace Serialization
    {
		// normal

        template<typename T, typename ...TS>
        size_t Calc(T const &v, TS const & ...vs)
        {
            return Calc(v) + Calc(vs...);
        }

        template<typename T, typename ...TS>
        size_t WriteTo(char *dstBuf, T const &v, TS const & ...vs)
        {
            auto offset = WriteTo(dstBuf, v);
            return offset + WriteTo(dstBuf + offset, vs...);
        }

		template<typename T, typename ...TS>
		bool ReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, T &out, TS &...outs)
		{
			if (!ReadFrom(srcBuf, dataLen, offset, out)) return false;
			return ReadFrom(srcBuf, dataLen, offset, outs...);
		}



		// var

		template<typename T, typename ...TS>
		size_t VarCalc(T const &v, TS const & ...vs)
		{
			return VarCalc(v) + VarCalc(vs...);
		}

		template<typename T, typename ...TS>
		size_t VarWriteTo(char *dstBuf, T const &v, TS const & ...vs)
		{
			auto offset = VarWriteTo(dstBuf, v);
			return offset + VarWriteTo(dstBuf + offset, vs...);
		}

		template<typename T, typename ...TS>
		bool VarReadFrom(char const *srcBuf, size_t const &dataLen, size_t &offset, T &out, TS &...outs)
		{
			if (!VarReadFrom(srcBuf, dataLen, offset, out)) return false;
			return VarReadFrom(srcBuf, dataLen, offset, outs...);
		}


    }
}








namespace xxlib
{
    struct ByteBuffer
    {
        char   *buf;
        size_t  bufLen;
        size_t  dataLen;    // "写"操作偏移量
        size_t  offset;     // "读"操作偏移量
        bool    isRef;      // 是否为引用模式( 并非 BB 自己管理当前 buf 内存 )

        ByteBuffer();                                                           // Reserve( 64 )
        explicit ByteBuffer(size_t capacity);                                   // Reserve( capacity )
        ByteBuffer(char *buf, size_t bufLen, size_t dataLen, bool isRef);       // ref or copy

        ByteBuffer(ByteBuffer const& o);                                        // copy
        ByteBuffer(ByteBuffer&& o);                                             // move
        ByteBuffer& operator=(ByteBuffer const& o);                             // copy
        ByteBuffer& operator=(ByteBuffer&& o);                                  // move
        ~ByteBuffer();                                                          // free buf if needed

        void Assign(char *buf, size_t bufLen, size_t dataLen, bool isRef);      // ref or copy
        void Reserve(size_t capacity);                                          // prepare memory( capacity )
        void Resize(size_t len);									            // cut or grow( do not fill 0 )
        void Clear();                                                           // dataLen = 0

        char operator[] (size_t idx) const;                                     // return buf[ idx ]
        char& operator[] (size_t idx);                                          // return buf[ idx ]

        char At(size_t idx) const;												// return buf[ idx ]
        char& At(size_t idx);													// return buf[ idx ]

        void Pop();
        char& Top();
        char const& Top() const;
        size_t Size() const;
        bool Empty() const;

		bool ReadByteBuffer(ByteBuffer &v, size_t minLen, size_t maxLen, bool isRef);	// 为支持 0 copy( 引用模式 ) 而增加一个 read


		// 直接 读写

        template<typename ...TS>
        void Write(TS const & ...vs);

        template<typename T>
        bool Read(T &v);                // 如果 T 为 Limit 类型, 将传入 0, 0 作为 minLen maxLen 的值.

        template<typename T, typename ...TS>
        bool Read(T &v, TS&...vs);

        template<typename T>
        bool ReadLimit(T &v, size_t minLen, size_t maxLen);




		// 变长 读写

		template<typename ...TS>
		void VarWrite(TS const & ...vs);

		template<typename T>
		bool VarRead(T &v);

		template<typename T, typename ...TS>
		bool VarRead(T &v, TS&...vs);

		template<typename T>
		bool VarReadLimit(T &v, size_t minLen, size_t maxLen);



        template<typename T>
        ByteBuffer& operator<<(T const& v);

        bool operator==(ByteBuffer const &other) const;
        bool operator!=(ByteBuffer const &other) const;
        bool operator<(ByteBuffer const &other) const;
        bool operator>(ByteBuffer const &other) const;
        bool operator<=(ByteBuffer const &other) const;
        bool operator>=(ByteBuffer const &other) const;

    protected:
		XXLIB_NOINLINE void ReserveCore(size_t capacity);
    };

}








namespace xxlib
{

    template<typename ...TS>
    void ByteBuffer::Write(TS const & ...vs)
    {
        auto len = Serialization::Calc(vs...);
        if (len + dataLen > bufLen)
        {
            ReserveCore(len + dataLen);
        }
        dataLen += Serialization::WriteTo(buf + dataLen, vs...);
    }

    template<typename T>
    bool ByteBuffer::Read(T &v)
    {
        return Serialization::ReadFromSwitch(buf, dataLen, offset, v);
    }

    template<typename T, typename ...TS>
    bool ByteBuffer::Read(T &v, TS&...vs)
    {
        if (!Read(v)) return false;
        return Read(vs...);
    }

    template<typename T>
    bool ByteBuffer::ReadLimit(T &v, size_t minLen, size_t maxLen)
    {
        return Serialization::ReadFrom(buf, dataLen, offset, v, minLen, maxLen);
    }




	// var( 暂未做成压缩优先模式, 也就是说 用 var 系列读写， 目标类型 没有 var 版本适配，就会报错 )

	template<typename ...TS>
	void ByteBuffer::VarWrite(TS const & ...vs)
	{
		auto len = Serialization::VarCalc(vs...);
		if (len + dataLen > bufLen)
		{
			ReserveCore(len + dataLen);
		}
		dataLen += Serialization::VarWriteTo(buf + dataLen, vs...);
	}

	template<typename T>
	bool ByteBuffer::VarRead(T &v)
	{
		return Serialization::VarReadFromSwitch(buf, dataLen, offset, v);
	}

	template<typename T, typename ...TS>
	bool ByteBuffer::VarRead(T &v, TS&...vs)
	{
		if (!VarRead(v)) return false;
		return VarRead(vs...);
	}

	template<typename T>
	bool ByteBuffer::VarReadLimit(T &v, size_t minLen, size_t maxLen)
	{
		return Serialization::VarReadFrom(buf, dataLen, offset, v, minLen, maxLen);
	}










    template<typename T>
    ByteBuffer& ByteBuffer::operator<<(T const& v)
    {
        Write(v);
        return *this;
    }

}
