#pragma once
#include "xxlib_utils.h"
#include "assert.h"

namespace xxlib
{
	template<typename T>
	class List : Memmoveable
	{
	public:
		T*          buf;
		size_t      bufLen;
		size_t      dataLen;
		bool        needFree;      // 是否需要释放 buf 内存( 引用外部 buf, 或 buf 为 nullptr 时就不需要释放, Reserve 实际发生后 List 有 malloc 行为就变为需要 )


		// 只是为了能 for( auto c : 
		struct Iter
		{
			T *ptr;
			bool operator!=(Iter const& other) { return ptr != other.ptr; }
			Iter& operator++()
			{
				++ptr;
				return *this;
			}
			T& operator*() { return *ptr; }
		};
		Iter begin()
		{
			return Iter{ buf };
		}
		Iter end()
		{
			return Iter{ &buf[dataLen] };
		}
		Iter begin() const
		{
			return Iter{ buf };
		}
		Iter end() const
		{
			return Iter{ &buf[dataLen] };
		}



		List();
		explicit List(size_t capacity);
		List(std::initializer_list<T> items);

		List(char* buf, size_t bufLen);     // 以 buf 引用模式初始化
		template<size_t bufLen>
		List(char(&buf)[bufLen]) : List(buf, bufLen) {}


		~List();
		List(List&& o);
		List(List const& o);
		List& operator=(List&& o);
		List& operator=(List const& o);

		bool operator==(List const& other) const;
		bool operator!=(List const& other) const;

		template<typename...PTS>
		T& EmplaceAdd(PTS&&...ps);

		template<typename...VS>
		void Add(VS&&...vs);

		template<typename...PTS>
		T& EmplaceInsertAt(size_t idx, PTS&&...ps);
		void InsertAt(size_t idx, T&& v);
		void InsertAt(size_t idx, T const& v);
		size_t Find(T const& v) const;
		void RemoveAt(size_t idx);
		void FastRemoveAt(size_t idx);		// swap with last item + Pop()
		void FastAdd(T&& v);				// unsafe: do not check space enough
		void FastAdd(T const& v);			// unsafe: do not check space enough

		T& Top();
		T const& Top() const;
		void Pop();
		bool Pop(T& output);
		void Clear(bool freeBuf = false);
		void Reserve(size_t capacity);
		void Resize(size_t len, bool init = true);
		size_t DataByteLen() const;
		size_t BufByteLen() const;
		T const& operator[](size_t idx) const;
		T& operator[](size_t idx);
		T const& At(size_t idx) const;
		T& At(size_t idx);
		void Set(size_t idx, T&& v);
		void Set(size_t idx, T const& v);
		size_t Size() const;

	private:
		void Init();
		void ShallowCopy(List const& o);
		void CopyItems(List const& o);

		template<typename V>
		void AddCore(size_t idx, V&& v);
		template<typename V, typename...VS>
		void AddCore(size_t idx, V&& v, VS&&...vs);

		void ReserveCore(size_t capacity);
	};



	// 2进制排列为 从右到左，由低到高位，即小尾机可 memcpy
	template<>
	class List<bool> : Memmoveable
	{
	public:
		char*       buf = nullptr;
		size_t      bufLen = 0;
		size_t      dataLen = 0;

		List();							// List( 512 )
		explicit List(size_t capacity);
		List(std::initializer_list<bool> items);
		~List();
		List(List<bool>&& o);
		List(List<bool> const& o);
		List& operator=(List<bool>&& o);
		List& operator=(List<bool> const& o);
		void Add(bool bit);
		void Pop();
		bool Top() const;
		void Clear();
		void Reserve(size_t capacity);
		static size_t CalcByteLen(size_t dataLen);
		size_t DataByteLen() const;
		size_t BufByteLen() const;
		bool operator[](size_t idx) const;
		bool At(size_t idx) const;
		void Set(size_t idx, bool bit);
		void SetTrue(size_t idx);
		void SetFalse(size_t idx);
		void FillTrue();
		void FillFalse();
		void Fill(bool bit, size_t idxFrom = 0, size_t idxTo = 0);
		void Resize(size_t capacity, bool init = true);
		void CleanUp() const;   // clear last byte's unused bits ( for byte buffer memcpy )

		// todo: Remove? Insert? 外置 buf 支持
		// todo: iter?
	};

















	template<typename T>
	List<T>::List(std::initializer_list<T> items)
		: List(items.size())
	{
		for (auto& i : items)
		{
			Add(i);
		}
	}

	template<typename T>
	List<T>::List(size_t capacity)
	{
		if (capacity <= 0)
		{
			capacity = 8;
		}
		auto byteLen = Utils::Round2n(capacity * sizeof(T));
		buf = (T*)malloc(byteLen);
		bufLen = byteLen / sizeof(T);
		dataLen = 0;
		needFree = true;
	}

	template<typename T>
	List<T>::List()
	{
		Init();
	}

	template<typename T>
	List<T>::List(char* buf_, size_t bufLen_)
	{
		assert(bufLen_ / sizeof(T) >= 1);
		buf = (T*)buf_;
		bufLen = bufLen_ / sizeof(T);
		dataLen = 0;
		needFree = false;
	}

	template<typename T>
	List<T>::~List()
	{
		Clear(true);
	}

	template<typename T>
	List<T>::List(List&& o)
	{
		ShallowCopy(o);
		o.Init();
	}

	template<typename T>
	List<T>::List(List const& o)
	{
		Init();
		if (!o.dataLen) return;
		Reserve(o.dataLen);
		CopyItems(o);
	}

	template<typename T>
	List<T>& List<T>::operator=(List&& o)
	{
		assert(this != &o);
		Clear(true);
		ShallowCopy(o);
		o.Init();
		return *this;
	}

	template<typename T>
	List<T>& List<T>::operator=(List const& o)
	{
		assert(this != &o);
		Clear();
		Reserve(o.dataLen);
		CopyItems(o);
		return *this;
	}

	template<typename T>
	bool List<T>::operator==(List const& o) const
	{
		if (dataLen != o.dataLen) return false;
		if (dataLen == 0) return true;
		for (size_t i = 0; i < dataLen; ++i)
		{
			if (!(buf[i] == o.buf[i])) return false;
		}
		return true;
	}
	template<typename T>
	bool List<T>::operator!=(List const& o) const
	{
		return !operator==(o);
	}

	template<typename T>
	void List<T>::FastAdd(T&& v)
	{
		new (buf + dataLen++) T((T&&)v);
	}

	template<typename T>
	void List<T>::FastAdd(T const& v)
	{
		new (buf + dataLen++) T(v);
	}



	template<typename T>
	template<typename...VS>
	void List<T>::Add(VS&&...vs)
	{
		if (dataLen + sizeof...(vs) > bufLen)
		{
			ReserveCore(dataLen + sizeof...(vs));
		}
		AddCore(dataLen, std::forward<VS>(vs)...);
		dataLen += sizeof...(vs);
	}
	template<typename T>
	template<typename V>
	void List<T>::AddCore(size_t idx, V&& v)
	{
		new (buf + idx) T(std::forward<V>(v));
	}
	template<typename T>
	template<typename V, typename...VS>
	void List<T>::AddCore(size_t idx, V&& v, VS&&...vs)
	{
		new (buf + idx) T(std::forward<V>(v));
		AddCore(idx + 1, std::forward<VS>(vs)...);
	}



	template<typename T>
	template<typename...PTS>
	T& List<T>::EmplaceAdd(PTS&&...ps)
	{
		if (dataLen == bufLen)
		{
			ReserveCore(dataLen + 1);
		}
		return *new (buf + dataLen++) T(std::forward<PTS>(ps)...);
	}




	template<typename T>
	void List<T>::Pop()
	{
		assert(dataLen > 0);
		buf[--dataLen].~T();
	}

	template<typename T>
	T& List<T>::Top()
	{
		assert(dataLen > 0);
		return buf[dataLen - 1];
	}
	template<typename T>
	T const& List<T>::Top() const
	{
		assert(dataLen > 0);
		return buf[dataLen - 1];
	}

	template<typename T>
	bool List<T>::Pop(T& output)
	{
		if (!dataLen)
		{
			return false;
		}
		output = std::move(buf[--dataLen]);
		buf[dataLen].~T();
		return true;
	}

	template<typename T>
	void List<T>::Clear(bool freeBuf)
	{
		if (!buf) return;
		for (size_t i = 0; i < dataLen; ++i)
		{
			buf[i].~T();
		}
		dataLen = 0;
		if (freeBuf)
		{
			if (needFree)
			{
				free(buf);
				buf = nullptr;
				bufLen = 0;
				needFree = false;
			}
		}
	}

	template<typename T>
	void List<T>::ReserveCore(size_t capacity)
	{
		// if (capacity <= bufLen) return;  这句被提取到外面判断
		auto byteLen = Utils::Round2n(capacity * sizeof(T));
		bufLen = byteLen / sizeof(T);
		auto newBuf = (T*)malloc(byteLen);
		if (std::is_pod<T>::value || std::is_base_of<Memmoveable, T>::value)
		{
			memcpy(newBuf, buf, DataByteLen());
		}
		else
		{
			for (size_t i = 0; i < dataLen; ++i)
			{
				new (&newBuf[i]) T(std::move(buf[i]));
				buf[i].~T();
			}
		}
		if (needFree)
		{
			free(buf);
		}
		else
		{
			needFree = true;
		}
		buf = newBuf;
	}
	template<typename T>
	void List<T>::Reserve(size_t capacity)
	{
		if (capacity <= bufLen) return;
		ReserveCore(capacity);
	}

	template<typename T>
	void List<T>::Resize(size_t len, bool init /*= true */)
	{
		if (len == dataLen)
		{
			return;
		}
		else if (len < dataLen)
		{
			if (!std::is_pod<T>::value)
			{
				for (size_t i = len; i < dataLen; ++i)
				{
					buf[i].~T();
				}
			}
		}
		else // len > dataLen
		{
			Reserve(len);
			if (init)
			{
				if (std::is_pod<T>::value)
				{
					memset(buf + dataLen, 0, (len - dataLen) * sizeof(T));
				}
				else
				{
					for (size_t i = dataLen; i < len; ++i)
					{
						new (buf + i) T();
					}
				}
			}
		}
		dataLen = len;
	}


	template<typename T>
	size_t List<T>::DataByteLen() const
	{
		return dataLen * sizeof(T);
	}

	template<typename T>
	size_t List<T>::BufByteLen() const
	{
		return bufLen * sizeof(T);
	}

	template<typename T>
	T const& List<T>::operator[](size_t idx) const
	{
		assert(idx < dataLen);
		return buf[idx];
	}

	template<typename T>
	T& List<T>::operator[](size_t idx)
	{
		assert(idx < dataLen);
		return buf[idx];
	}

	template<typename T>
	T const& List<T>::At(size_t idx) const
	{
		assert(idx < dataLen);
		return buf[idx];
	}

	template<typename T>
	T& List<T>::At(size_t idx)
	{
		assert(idx < dataLen);
		return buf[idx];
	}

	template<typename T>
	void List<T>::Set(size_t idx, T&& v)
	{
		assert(idx < dataLen);
		buf[idx] = (T&&)v;
	}
	template<typename T>
	void List<T>::Set(size_t idx, T const& v)
	{
		assert(idx < dataLen);
		buf[idx] = v;
	}

	template<typename T>
	size_t List<T>::Find(T const& v) const
	{
		for (size_t i = 0; i < dataLen; ++i)
		{
			if (buf[i] == v)
			{
				return i;
			}
		}
		return -1;
	}

	template<typename T>
	void List<T>::RemoveAt(size_t idx)
	{
		assert(idx < dataLen);
		--dataLen;
		if (std::is_pod<T>::value || std::is_base_of<Memmoveable, T>::value)
		{
			buf[idx].~T();
			memmove(buf + idx, buf + idx + 1, (dataLen - idx) * sizeof(T));
		}
		else
		{
			for (size_t i = idx; i < dataLen; ++i)
			{
				buf[i] = (T&&)buf[i + 1];
			}
			buf[dataLen].~T();
		}
	}

	template<typename T>
	void List<T>::FastRemoveAt(size_t idx)
	{
		assert(idx < dataLen);
		--dataLen;
		if (dataLen == idx)	// last one
		{
			buf[idx].~T();
			return;
		}
		if (std::is_pod<T>::value)
		{
			buf[idx] = buf[dataLen];
		}
		else if (std::is_base_of<Memmoveable, T>::value)
		{
			buf[idx].~T();
			memcpy(buf + idx, buf + dataLen, sizeof(T));
		}
		else
		{
			buf[idx] = (T&&)buf[dataLen];
			buf[dataLen].~T();
		}
	}

	template<typename T>
	template<typename...PTS>
	T& List<T>::EmplaceInsertAt(size_t idx, PTS&&...ps)
	{
		assert(idx <= dataLen);
		if (dataLen == bufLen)
		{
			ReserveCore(dataLen + 1);
		}

		if (std::is_pod<T>::value || std::is_base_of<Memmoveable, T>::value)
		{
			memmove(buf + idx + 1, buf + idx, (dataLen - idx) * sizeof(T));
			++dataLen;
			return *new (buf + idx) T(std::forward<PTS>(ps)...);
		}
		else
		{
			if (idx < dataLen)
			{
				new (buf + dataLen) T((T&&)buf[dataLen - 1]);
				for (size_t i = dataLen - 1; i > idx; --i)
				{
					buf[i] = (T&&)buf[i - 1];
				}
				buf[idx].~T();
			}
			++dataLen;
			return *new (buf + idx) T(std::forward<PTS>(ps)...);
		}
	}

	template<typename T>
	void List<T>::InsertAt(size_t idx, T&& v)
	{
		EmplaceInsertAt(idx, (T&&)v);
	}
	template<typename T>
	void List<T>::InsertAt(size_t idx, T const& v)
	{
		EmplaceInsertAt(idx, v);
	}


	template<typename T>
	size_t List<T>::Size() const
	{
		return dataLen;
	}


	template<typename T>
	void List<T>::Init()
	{
		buf = nullptr;
		bufLen = 0;
		dataLen = 0;
		needFree = false;
	}

	template<typename T>
	void List<T>::ShallowCopy(List const &o)
	{
		buf = o.buf;
		bufLen = o.bufLen;
		dataLen = o.dataLen;
		needFree = o.needFree;
	}

	template<typename T>
	void List<T>::CopyItems(List const &o)
	{
		dataLen = o.dataLen;
		if (std::is_pod<T>::value)
		{
			memcpy(buf, o.buf, o.DataByteLen());
		}
		else
		{
			for (size_t i = 0; i < o.dataLen; ++i)
			{
				new (buf + i) T(o.buf[i]);
			}
		}
	}
}

