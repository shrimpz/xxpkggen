#pragma once
#include "xxlib_utils.h"
#include "assert.h"
#include <memory>
#include <functional>
#include <tuple>
#include <vector>

namespace xxlib
{
    // 平铺内存分段对象池
    // T1 长度必须大于等于 4( 未分配时要用来放 next 值 )
    // Alloc 不会对内存做任何的初始化操作, Free 也啥都不做.
    // 扩容是用 resize, 故只有无内置 buf 的类型, pod 之类才是安全的
    template<typename ... TS>
    struct FlatPool : Memmoveable
    {
    public:
        int         freeList;               // 自由空间链表头( next 指向下一个未使用单元 )
        int         freeCount;              // 已 Free 数( 如果 Alloc 是从 freeList 分配, 该值将 -1 )
        int         count;                  // 已 Alloc 数
        int         bufLen;                 // 数组能放置的数据总数( 下列数组共享一个长度 )
        void       *bufs[sizeof...(TS)];

        typedef std::tuple<TS...> TupleType;

        static_assert(sizeof(typename std::tuple_element<0, TupleType>::type) >= 4, "sizeof( T1 ) need >= 4");

        FlatPool()
            : FlatPool(512)
        {}

        template<typename Tuple, std::size_t N>
        struct TupleHelper
        {
            static void Malloc(int bufLen, void** bufs, Tuple const &t)
            {
                TupleHelper<Tuple, N - 1>::Malloc(bufLen, bufs, t);
                bufs[N - 1] = malloc(bufLen * sizeof(std::get<N - 1>(t)));
            }
            static void Free(void** bufs, Tuple const &t)
            {
                TupleHelper<Tuple, N - 1>::Free(bufs, t);
                free(bufs[N - 1]);
            }
            static void Realloc(int bufLen, void** bufs, Tuple const &t)
            {
                TupleHelper<Tuple, N - 1>::Realloc(bufLen, bufs, t);
                bufs[N - 1] = realloc(bufs[N - 1], bufLen * sizeof(std::get<N - 1>(t)));
            }
        };
        template<typename Tuple>
        struct TupleHelper<Tuple, 1>
        {
            static void Malloc(int bufLen, void** bufs, Tuple const &t)
            {
                bufs[0] = malloc(bufLen * sizeof(std::get<0>(t)));
            }
            static void Free(void** bufs, Tuple const &t)
            {
                free(bufs[0]);
            }
            static void Realloc(int bufLen, void** bufs, Tuple const &t)
            {
                bufs[0] = realloc(bufs[0], bufLen * sizeof(std::get<0>(t)));
            }
        };

        FlatPool(int capacity)
            : freeList(-1)
            , freeCount(0)
            , count(0)
            , bufLen((int)Utils::Round2n(capacity))
        {
            TupleHelper<TupleType, sizeof...(TS)>::Malloc(bufLen, bufs, TupleType());
        }

        ~FlatPool()
        {
            TupleHelper<TupleType, sizeof...(TS)>::Free(bufs, TupleType());
        }

        FlatPool(FlatPool const &other) = delete;
        FlatPool operator=(FlatPool const &other) = delete;

        FlatPool(FlatPool &&other) = default;
        FlatPool operator=(FlatPool &&other)
        {
            std::swap(*this, other);
        }

        template<size_t bufIndex>
        typename std::tuple_element<bufIndex, TupleType>::type* Ref()
        {
            return (typename std::tuple_element<bufIndex, TupleType>::type*)bufs[bufIndex];
        }

        template<size_t bufIndex>
        typename std::tuple_element<bufIndex, TupleType>::type& Ref(int itemIndex)
        {
            assert(itemIndex < bufLen);
            return Ref<bufIndex>()[itemIndex];
        }

        void Reserve(int capacity = 0)
        {
            if (capacity == 0)
            {
                capacity = count * 2;   // 此时 count == itemLen
            }
            else if (capacity <= bufLen)
            {
                return;
            }
            else
            {
                capacity = (int)Utils::Round2n(capacity);
            }
            bufLen = capacity;

            TupleHelper<TupleType, sizeof...(TS)>::Realloc(bufLen, bufs, TupleType());
        }

        int Alloc()
        {
            int index;
            if (freeCount > 0)                  // 如果 自由节点链表 不空, 取一个来当容器
            {                                   // 这些节点来自 Free 操作. T1 硬转成 int 指向下一个
                index = freeList;
                freeList = *(int*)&Ref<0>(index);
                freeCount--;
            }
            else
            {
                if (count == bufLen)            // 所有空节点都用光了, Resize
                {
                    Reserve();
                }
                index = count;                  // 指向 Resize 后面的空间起点
                count++;
            }
            return index;
        }

        void Free(int index)
        {
            *(int*)&Ref<0>(index) = freeList;
            freeList = index;
            freeCount++;
        }

        int Size() const
        {
            return count - freeCount;
        }
    };

}
