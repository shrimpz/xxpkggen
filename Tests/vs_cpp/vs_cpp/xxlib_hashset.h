#pragma once
#include "xxlib_utils.h"            // Utils::Round2n GetPrime
#include "xxlib_utils_hash.h"       // Utils::GetHashCode
#include <string.h>                 // memset

namespace xxlib
{
    // 从 Dict 精简而来( 复制小改 )

    template <typename TK>
    struct HashSet : Memmoveable
    {
        struct Node
        {
            unsigned int    hashCode;
            int             next;
        };
        struct Data
        {
            TK              key;
            int             prev;
        };


        // 只是为了能 for( auto c : 
        struct Iter
        {
            HashSet& hs;
            int i;
            bool operator!=(Iter const& other) { return i != other.i; }
            Iter& operator++()
            {
                while (++i < hs.count)
                {
                    if (hs.items[i].prev != -2) break;
                }
                return *this;
            }
            TK& operator*() { return hs.items[i].key; }
        };
        Iter begin()
        {
            if (Empty()) return end();
            for (int i = 0; i < count; ++i)
            {
                if (items[i].prev != -2) return Iter{ *this, i };
            }
            return end();
        }
        Iter end()
        {
            return Iter{ *this, count };
        }




        int                 freeList;               // 自由空间链表头( next 指向下一个未使用单元 )
        int                 freeCount;              // 自由空间链长
        int                 count;                  // 已使用空间数
        int                 bucketsLen;             // 桶数组长( 质数 )
        int                *buckets;                // 桶数组
        Node               *nodes;                  // 节点数组
        Data               *items;                  // 数据数组( 与节点数组同步下标 )

        explicit HashSet(int capacity)
            : freeList(-1)
            , freeCount(0)
            , count(0)
            , bucketsLen((int)Utils::GetPrime(capacity, sizeof(Data)))
        {
            Alloc();
        }

        HashSet()
            : HashSet(16)
        {
        }

        HashSet(HashSet const &o)
            : HashSet(o.count)
        {
            FastAdd(o);
        }

        HashSet(HashSet &&o)
            : freeList(o.freeList)
            , freeCount(o.freeCount)
            , count(o.count)
            , bucketsLen(o.bucketsLen)
            , buckets(o.buckets)
            , nodes(o.nodes)
            , items(o.items)
        {
            o.buckets = nullptr;
        }

        HashSet& operator=(HashSet const &o)
        {
            if (bucketsLen < o.count)           // 确保 this.bucketsLen 大于等于 o.count( 不够就扩容 )
            {
                DeleteAll();
                bucketsLen = (int)Utils::GetPrime(o.count, sizeof(Data));
                Alloc();
            }
            else
            {
                DeleteKVs();
                memset(buckets, -1, bucketsLen * sizeof(int));
            }
            freeList = -1;
            freeCount = 0;

            FastAdd(o);     // 复制数据 并设置 count 的值
            return *this;
        }

        HashSet& operator=(HashSet &&o)
        {
            DeleteAll();

            freeList = o.freeList;
            freeCount = o.freeCount;
            count = o.count;
            bucketsLen = o.bucketsLen;
            buckets = o.buckets;
            nodes = o.nodes;
            items = o.items;
            o.buckets = nullptr;
        }

        ~HashSet()
        {
            if (!buckets) return;               // 判断一个就够了. 右值的情况下会为 nullptr
            DeleteAll();
        }


        template<typename K>
        std::pair<bool, int> EmplaceAdd(K &&key)
        {
            // hash 按桶数取模 定位到具体 链表, 扫找
            auto hashCode = Utils::GetHashCode(key);
            auto targetBucket = hashCode % bucketsLen;
            for (int i = buckets[targetBucket]; i >= 0; i = nodes[i].next)
            {
                if (nodes[i].hashCode == hashCode && Utils::Equals(items[i].key, key))
                {
                    return std::make_pair(false, i);
                }
            }
            // 没找到则新增
            int index;
            if (freeCount > 0)                          // 如果 自由节点链表 不空, 取一个来当容器
            {                                           // 这些节点来自 Remove 操作. next 指向下一个
                index = freeList;
                freeList = nodes[index].next;
                freeCount--;
            }
            else
            {
                if (count == bucketsLen)               // 所有空节点都用光了, Resize
                {
                    Reserve();
                    targetBucket = hashCode % bucketsLen;
                }
                index = count;                         // 指向 Resize 后面的空间起点
                count++;
            }

            // 执行到此处时, freeList 要么来自 自由节点链表, 要么为 Reserve 后新增的空间起点.
            nodes[index].hashCode = hashCode;
            nodes[index].next = buckets[targetBucket];

            // 如果当前 bucket 中有 index, 则令其 prev 等于即将添加的 index
            if (buckets[targetBucket] >= 0)
            {
                items[buckets[targetBucket]].prev = index;
            }
            buckets[targetBucket] = index;

            // 移动复制构造写 key
            new (&items[index].key) TK(std::forward<K>(key));
            items[index].prev = -1;

            return std::make_pair(true, index);
        }


        std::pair<bool, int> Add(TK const &k)
        {
            return EmplaceAdd(k);
        }
        std::pair<bool, int> Add(TK &&k)
        {
            return EmplaceAdd((TK&&)k);
        }


        // 只支持没数据时扩容或空间用尽扩容( 如果不这样限制, 扩容时的 遍历损耗 略大 )
        void Reserve(int capacity = 0)
        {
            assert(count == 0 || count == bucketsLen);          // 确保扩容函数使用情型

            // 得到空间利用率最高的扩容长度并直接修改 bucketsLen( count 为当前数据长 )
            if (capacity == 0)
            {
                capacity = count * 2;                           // 2倍扩容
            }
            if (capacity <= bucketsLen) return;
            bucketsLen = (int)Utils::GetPrime(capacity, sizeof(Data));

            // 桶扩容并全部初始化( 后面会重新映射一次 )
            free(buckets);
            buckets = (int*)malloc(bucketsLen * sizeof(int));
            memset(buckets, -1, bucketsLen * sizeof(int));

            // 节点数组扩容( 保留老数据 )
            nodes = (Node*)realloc(nodes, bucketsLen * sizeof(Node));

            // item 数组扩容
            if (std::is_base_of<Memmoveable, TK>::value || std::is_pod<TK>::value)
            {
                items = (Data*)realloc(items, bucketsLen * sizeof(Data));
            }
            else
            {
                auto newItems = (Data*)malloc(bucketsLen * sizeof(Data));
                for (int i = 0; i < count; ++i)
                {
                    new (&newItems[i].key) TK((TK&&)items[i].key);
                    items[i].key.TK::~TK();
                }
                free(items);
                items = newItems;
            }

            // 遍历所有节点, 重构桶及链表( 扩容情况下没有节点空洞 )
            for (int i = 0; i < count; i++)
            {
                auto index = nodes[i].hashCode % bucketsLen;
                if (buckets[index] >= 0)
                {
                    items[buckets[index]].prev = i;
                }
                items[i].prev = -1;
                nodes[i].next = buckets[index];
                buckets[index] = i;
            }
        }


        int Find(TK const &key) const
        {
            auto hashCode = Utils::GetHashCode(key);
            for (int i = buckets[hashCode % bucketsLen]; i >= 0; i = nodes[i].next)
            {
                if (nodes[i].hashCode == hashCode && Utils::Equals(items[i].key, key))
                {
                    return i;
                }
            }
            return -1;
        }

        bool Contains(TK const &key) const
        {
            return Find(key) != -1;
        }
        bool Exists(TK const &key) const
        {
            return Find(key) != -1;
        }

        void RemoveIndexAt(int idx)
        {
            assert(items[idx].prev != -2);
            if (items[idx].prev < 0)
            {
                buckets[nodes[idx].hashCode % bucketsLen] = nodes[idx].next;
            }
            else
            {
                nodes[items[idx].prev].next = nodes[idx].next;
            }
            if (nodes[idx].next >= 0)       // 如果存在当前节点的下一个节点, 令其 prev 指向 上一个节点
            {
                items[nodes[idx].next].prev = items[idx].prev;
            }
            
            nodes[idx].next = freeList;     // 当前节点已被移出链表, 令其 next 指向  自由节点链表头( next 有两种功用 )
            freeList = idx;
            freeCount++;

            items[idx].key.TK::~TK();       // 析构内存结构
            items[idx].prev = -2;           // foreach 时的无效标志
        }


        void Remove(TK const &key)
        {
            auto idx = Find(key);
            if (idx != -1)
            {
                RemoveIndexAt(idx);
            }
        }


        void Clear()
        {
            if (!count) return;
            DeleteKVs();
            memset(buckets, -1, bucketsLen * sizeof(int));
            freeList = -1;
            freeCount = 0;
            count = 0;
        }


        size_t Size() const
        {
            return size_t(count - freeCount);
        }

        bool Empty()
        {
            return count == 0;
        }

        TK& IndexAtKey(int idx)
        {
            assert(idx < count);
            return items[idx].key;
        }
        Data& At(int idx)
        {
            assert(idx < count);
            return items[idx];
        }

        TK const& IndexAtKey(int idx) const
        {
            return const_cast<HashSet*>(this)->IndexAtKey(idx);
        }
        Data const& At(int idx) const
        {
            return const_cast<HashSet*>(this)->At(idx);
        }

    protected:
        void Alloc()                                        // 用于初始化和复制
        {
            items = (Data*)malloc(bucketsLen * sizeof(Data));
            nodes = (Node*)malloc(bucketsLen * sizeof(Node));
            buckets = (int*)malloc(bucketsLen * sizeof(int));
            memset(buckets, -1, bucketsLen * sizeof(int));          // -1 代表 "空"
        }

        void FastAdd(HashSet const& o)                         // 用于 复制构造 和 operator=
        {
            int i = 0;
            for (int oi = 0; oi < o.count; ++oi)
            {
                if (items[i].prev != -2)
                {
                    auto targetBucket = o.nodes[oi].hashCode % bucketsLen;
                    nodes[i].hashCode = o.nodes[oi].hashCode;
                    nodes[i].next = buckets[targetBucket];
                    if (buckets[targetBucket] >= 0)
                    {
                        items[buckets[targetBucket]].prev = i;
                    }
                    buckets[targetBucket] = i;
                    new (&items[i].key) TK(o.items[oi].key);
                    items[i].prev = -1;
                    ++i;
                }
            }
            count = i;
        }
        
        void DeleteKVs()                                    // 用于 析构, 复制, Clear
        {
            for (int i = 0; i < count; ++i)
            {
                if (items[i].prev != -2)
                {
                    items[i].key.TK::~TK();
                }
            }
        }
        
        void DeleteAll()                                    // 用于析构, 移动复制
        {
            DeleteKVs();
            free(buckets);
            free(nodes);
            free(items);
        }
    };
}
