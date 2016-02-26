#pragma once
#include "xxlib_utils.h"
#include <type_traits>
#include <memory>
#include "assert.h"

namespace xxlib
{
    // 可切换内置定长 buf 数组或  传入 buf 指针的 定长版 List( 各种超长检查扩容都没有 )
    // 经实测发现, 外置 buf 可以得到更加彻底的编译器优化
    // todo: bool 特化版

    using std::move;
    using std::forward;
    using std::initializer_list;
    using std::aligned_storage;
    using std::alignment_of;
    using std::enable_if;
    using std::is_same;
    using std::is_trivial;
    using std::is_base_of;
    using std::is_convertible;


    // 内置定长数组 buf
    template<typename DataType, size_t len>
    struct ArrBuf
    {
        typename aligned_storage<sizeof(DataType), alignment_of<DataType>::value>::type buf[len];
        operator DataType*() { return (DataType*)buf; }
        operator DataType*() const { return (DataType*)buf; }
    };


    // 传入 buf 指针
    template<typename DataType>
    struct PtrBuf
    {
        DataType* buf;
        operator DataType*() { return buf; }
        operator DataType*() const { return buf; }
        PtrBuf(void* p = nullptr) : buf((DataType*)p) {}
    };


    // 定长版 List
    template<typename DataType, size_t bufLen_, typename SizeType = int, typename IndexType = size_t, typename Buf = ArrBuf<DataType, bufLen_>, bool rangeCheck = true>
    struct FixedList
    {
        // 成员变量区
        SizeType    dataLen = 0;    // 这个在前方便部分复制
        Buf         buf;
        static const size_t bufLen = bufLen_;   // 伪成员( 方便取模板参数 )

        // 路由标志区
        static const bool isArr = is_same<Buf, ArrBuf<DataType, bufLen>>::value;   // buf 类型路由
        static const bool moveable = is_trivial<DataType>::value || is_base_of<Memmoveable, DataType>::value;   // 数据移动特性路由

        // 一般构造
        FixedList() {}

        template<class TT = int> FixedList(typename enable_if<is_convertible<TT, int>::value && !isArr, void*>::type buf)
            : buf(buf)
        {}


        // 带初始值列表的构造
        template<class TT = int> FixedList(typename enable_if<is_convertible<TT, int>::value && isArr, initializer_list<DataType>&&>::type items)
        {
            for (auto&& i : items)
            {
                Add(move(i));
            }
        }
        template<class TT = int> FixedList(typename enable_if<is_convertible<TT, int>::value && !isArr, void*>::type buf, initializer_list<DataType>&& items)
            : buf(buf)
        {
            assert(buf);
            for (auto&& i : items)
            {
                Add(move(i));
            }
        }


        // 复制构造相关
        FixedList(FixedList&& o)
        {
            operator=((FixedList&&)o);
        }
        FixedList(FixedList const& o)
        {
            operator=(o);
        }

        template<class TT = int> typename enable_if<is_convertible<TT, int>::value && isArr, FixedList&>::type& operator=(TT&& o)
        {
            //buf = o.buf;
            if (is_trivial<DataType>::value)
            {
                memcpy(buf, o.buf, o.dataLen * sizeof(DataType));
            }
            else
            {
                for (size_t i = 0; i < o.dataLen; ++i)
                {
                    new (buf + i) DataType((DataType&&)o.buf[i]);
                }
            }
            dataLen = o.dataLen;
            //o.buf = nullptr;
            o.dataLen = 0;
            return *this;
        }
        template<class TT = int> typename enable_if<is_convertible<TT, int>::value && !isArr, FixedList&>::type& operator=(TT&& o)
        {
            buf = o.buf;
            dataLen = o.dataLen;
            o.buf = nullptr;
            o.dataLen = 0;
            return *this;
        }
        FixedList& operator=(FixedList const& o)
        {
            assert(bufLen > o.dataLen);
            Clear();
            if (is_trivial<DataType>::value)
            {
                memcpy(buf, o.buf, o.dataLen * sizeof(DataType));
            }
            else
            {
                for (size_t i = 0; i < o.dataLen; ++i)
                {
                    new (buf + i) DataType(o.buf[i]);
                }
            }
            dataLen = o.dataLen;
            return *this;
        }

        // 清理相关
        ~FixedList()
        {
            if (!isArr && buf == nullptr) return;
            Clear();
        }

        void Clear()
        {
            for (auto& i : *this)
            {
                i.~DataType();
            }
            dataLen = 0;
        }


        // 追加 / 插入 系列

        template<typename...PTS>
        DataType& Emplace(PTS&&...ps)
        {
            assert(dataLen < bufLen);
            return *new (buf + dataLen++) DataType(forward<PTS>(ps)...);
        }

        template<typename...PTS>
        DataType& EmplaceAt(size_t idx, PTS&&...ps)
        {
            assert(idx <= dataLen);
            if (moveable)
            {
                memmove(buf + idx + 1, buf + idx, (dataLen - idx) * sizeof(DataType));
            }
            else
            {
                if (idx < dataLen)
                {
                    new (buf + dataLen) DataType((DataType&&)buf[dataLen - 1]);
                    for (size_t i = dataLen - 1; i > idx; --i)
                    {
                        buf[i] = (DataType&&)buf[i - 1];
                    }
                    buf[idx].~DataType();
                }
            }
            ++dataLen;
            return *new (buf + idx) DataType(forward<PTS>(ps)...);
        }


        template<typename V>
        void Add(V &&v)
        {
            Emplace(std::forward<V>(v));
        }
        template<typename V, typename ...VS>
        void Add(V &&v, VS &&...vs)
        {
            Add(std::forward<V>(v));
            Add(std::forward<VS>(vs)...);
        }

        void InsertAt(size_t idx, DataType&& v)
        {
            EmplaceAt(idx, (DataType&&)v);
        }
        void InsertAt(size_t idx, DataType const& v)
        {
            EmplaceAt(idx, v);
        }


        void RemoveAt(size_t idx)
        {
            assert(idx < dataLen);
            --dataLen;
            if (moveable)
            {
                buf[idx].~DataType();
                memmove(buf + idx, buf + idx + 1, (dataLen - idx) * sizeof(DataType));
            }
            else
            {
                for (size_t i = idx; i < dataLen; ++i)
                {
                    buf[i] = (DataType&&)buf[i + 1];
                }
                buf[dataLen].~DataType();
            }
        }


        // 快删, 副作用: 将最后一个移到 idx, 会导致乱序
        void FastRemoveAt(size_t idx)
        {
            assert(idx < dataLen);
            --dataLen;
            if (dataLen == idx)	// last one
            {
                buf[idx].~DataType();
                return;
            }
            if (moveable)
            {
                buf[idx].~DataType();
                memcpy(buf + idx, buf + dataLen, sizeof(DataType));
            }
            else
            {
                buf[idx] = (DataType&&)buf[dataLen];
                buf[dataLen].~DataType();
            }
        }





        // 找指定元素所在索引
        size_t Find(DataType const& v) const
        {
            for (size_t i = 0; i < dataLen; ++i)
            {
                if (buf[i] == v) return i;
            }
            return -1;  // size_t 的最大值
        }


        bool operator==(FixedList const& o) const
        {
            if (dataLen != o.dataLen) return false;
            if (dataLen == 0) return true;
            for (size_t i = 0; i < dataLen; ++i)
            {
                if (!(buf[i] == o.buf[i])) return false;
            }
            return true;
        }
        bool operator!=(FixedList const& o) const
        {
            return !operator==(o);
        }



        DataType& Top()
        {
            assert(dataLen > 0);
            return buf[dataLen - 1];
        }
        DataType const& Top() const
        {
            assert(dataLen > 0);
            return buf[dataLen - 1];
        }

        void Pop()
        {
            assert(dataLen > 0);
            buf[--dataLen].~DataType();
        }
        bool TryPop(DataType& output)
        {
            if (!dataLen) return false;
            output = move(buf[dataLen - 1]);
            buf[dataLen - 1].~DataType();
            --dataLen;
            return true;
        }

        size_t Size() const
        {
            return dataLen;
        }



        DataType& At(size_t idx)
        {
            if (rangeCheck)
            {
                assert(idx < (size_t)dataLen);
            }
            return buf[idx];
        }
        DataType const& At(size_t idx) const
        {
            return ((FixedList*)(this))->At(idx);
        }
        DataType& operator[](size_t idx)
        {
            return At(idx);
        }
        DataType const& operator[](size_t idx) const
        {
            return ((FixedList*)(this))->At(idx);
        }





        // 如果索引类型并非 size_t, 则再生成一套匹配以方便使用
        template<class TT = int>
        void InsertAt(typename enable_if<is_convertible<TT, int>::value && !is_same<IndexType, size_t>::value, IndexType>::type idx, DataType &&v)
        {
            EmplaceAt((size_t)idx, (DataType&&)v);
        }
        template<class TT = int>
        void InsertAt(typename enable_if<is_convertible<TT, int>::value && !is_same<IndexType, size_t>::value, IndexType>::type idx, DataType const &v)
        {
            EmplaceAt((size_t)idx, v);
        }
        template<class TT = int>
        void RemoveAt(typename enable_if<is_convertible<TT, int>::value && !is_same<IndexType, size_t>::value, IndexType>::type idx)
        {
            RemoveAt((size_t)idx);
        }
        template<class TT = int>
        void FastRemoveAt(typename enable_if<is_convertible<TT, int>::value && !is_same<IndexType, size_t>::value, IndexType>::type idx)
        {
            FastRemoveAt((size_t)idx);
        }
        template<class TT = int>
        DataType& At(typename enable_if<is_convertible<TT, int>::value && !is_same<IndexType, size_t>::value, IndexType>::type idx)
        {
            return At((size_t)idx);
        }
        template<class TT = int>
        DataType const& At(typename enable_if<is_convertible<TT, int>::value && !is_same<IndexType, size_t>::value, IndexType>::type idx) const
        {
            return At((size_t)idx);
        }
        template<class TT = int>
        DataType& operator[](typename enable_if<is_convertible<TT, int>::value && !is_same<IndexType, size_t>::value, IndexType>::type idx)
        {
            return At((size_t)idx);
        }
        template<class TT = int>
        DataType const& operator[](typename enable_if<is_convertible<TT, int>::value && !is_same<IndexType, size_t>::value, IndexType>::type idx) const
        {
            return At((size_t)idx);
        }




        // 只是为了能 for( auto c : 
        struct Iter
        {
            DataType *ptr;
            bool operator!=(Iter const& other) const { return ptr != other.ptr; }
            Iter& operator++() { ++ptr; return *this; }
            DataType& operator*() const { return *ptr; }
        };
        Iter begin() { return Iter{ buf }; }
        Iter begin() const { return Iter{ buf }; }
        Iter end() { return Iter{ (DataType*)buf + dataLen }; }
        Iter end() const { return Iter{ (DataType*)buf + dataLen }; }

    };


    template<typename DataType, size_t bufLen, typename SizeType = int, typename IndexType = size_t, bool rangeCheck = true>
    using FixedList_Buf = FixedList<DataType, bufLen, SizeType, IndexType, PtrBuf<DataType>, rangeCheck>;

    template<typename DataType, size_t bufLen, typename SizeType = int, typename IndexType = size_t, bool rangeCheck = true>
    using FixedList_Arr = FixedList<DataType, bufLen, SizeType, IndexType, ArrBuf<DataType, bufLen>, rangeCheck>;


    template<typename DataType, size_t bufSiz, typename SizeType = long long, typename IndexType = size_t>
    FixedList_Buf<DataType, bufSiz / sizeof(DataType), SizeType, IndexType> MakeFixedList(char(&buf)[bufSiz])
    {
        return FixedList_Buf<DataType, bufSiz / sizeof(DataType), SizeType, IndexType>(buf);
    }
    template<typename DataType, size_t bufSiz, typename SizeType = long long, typename IndexType = size_t>
    FixedList_Buf<DataType, bufSiz / sizeof(DataType), SizeType, IndexType> MakeFixedList(char(&buf)[bufSiz], initializer_list<DataType>&& items)
    {
        return FixedList_Buf<DataType, bufSiz / sizeof(DataType), SizeType, IndexType>(buf, move(items));
    }

    template<typename DataType, size_t maxLen, typename SizeType = long long, typename IndexType = size_t>
    FixedList_Buf<DataType, maxLen, SizeType, IndexType> MakeFixedList(char *buf)
    {
        return FixedList_Buf<DataType, maxLen, SizeType, IndexType>(buf);
    }
    template<typename DataType, size_t maxLen, typename SizeType = long long, typename IndexType = size_t>
    FixedList_Buf<DataType, maxLen, SizeType, IndexType> MakeFixedList(char *buf, initializer_list<DataType>&& items)
    {
        return FixedList_Buf<DataType, maxLen, SizeType, IndexType>(buf, move(items));
    }





    ////// 2进制排列为 从右到左，由低到高位，即小尾机可 memcpy
    ////template<>
    ////class FixedList<bool> : Memmoveable
    ////{
    ////public:
    ////    char*       buf = nullptr;
    ////    size_t      bufLen = 0;
    ////    size_t      dataLen = 0;

    ////    FixedList();							// FixedList( 512 )
    ////    explicit FixedList(size_t capacity);
    ////    FixedList(initializer_list<bool> items);
    ////    ~FixedList();
    ////    FixedList(FixedList<bool>&& o);
    ////    FixedList(FixedList<bool> const& o);
    ////    FixedList& operator=(FixedList<bool>&& o);
    ////    FixedList& operator=(FixedList<bool> const& o);
    ////    void Add(bool bit);
    ////    void Pop();
    ////    bool Top() const;
    ////    void Clear();
    ////    void Reserve(size_t capacity);
    ////    size_t DataByteLen() const;
    ////    size_t BufByteLen() const;
    ////    bool operator[](size_t idx) const;
    ////    bool At(size_t idx) const;
    ////    void Set(size_t idx, bool bit);
    ////    void SetTrue(size_t idx);
    ////    void SetFalse(size_t idx);
    ////    void FillTrue();
    ////    void FillFalse();
    ////    void Fill(bool bit, size_t idxFrom = 0, size_t idxTo = 0);
    ////    void Resize(size_t capacity, bool init = true);
    ////    void CleanUp() const;   // clear last byte's unused bits

    ////    // todo: Remove? Insert? 外置 buf 支持
    ////    // todo: iter?
    ////};

}

