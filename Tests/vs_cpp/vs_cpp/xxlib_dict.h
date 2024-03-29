﻿#pragma once
#include "xxlib_utils.h"            // Utils::Round2n GetPrime
#include "xxlib_utils_hash.h"       // Utils::GetHashCode
#include <string.h>                 // memset

namespace xxlib
{
    // 抄自 .net 的 Dict 的代码
    // 注: 所有 assert(buckets  字样的代码都是防止 对象被 移动 后, 还试图正常使用( 调用除了析构功能的其他函数 ). 真需要用, 应该使用 Ensure 确保其重建

    // todo: 同步改造代码到 hashset 以及将确保机制应用到所有已存在的容器

    // 去掉了链表深度达到 100 时随机更换 hash 算法的设计
    // 去掉了枚举器支持, 版本检查
    // 能 for

    // 内存优化原则, 亲和 cpu cache
    // hashCode, next 与 key value prev 分离, 查找热点相关内存相对集中

    // todo: 提供真正连续的内存的分配方案, 传入 buf( buckets, nodes, items 三段合一 , 按长度比例瓜分.  load_factor 机制考虑 作用于 buckets 似乎可以吃多点内存提升性能减少浪费 )
    // todo: 提供 vs 调试支持文件
    // todo: 将 各种 int 换为 uint


    template <typename TK, typename TV>
    struct Dict : Memmoveable
    {
        struct Node
        {
            unsigned int    hashCode;
            int             next;
        };
        struct Data
        {
            TK              key;
            TV              value;
            int             prev;
        };


        // 只是为了能 for( auto c : 
        struct Iter
        {
            Dict& hs;
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
            Data& operator*() { return hs.items[i]; }
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

        explicit Dict(int capacity)
        {
            Init();
            Ensure(capacity);
        }

        Dict()
            : Dict(16)
        {
        }

        Dict(Dict const &o)
        {
            Init();
            if (o.buckets)
            {
                Ensure(o.Size());
                FastAdd(o);
            }
        }

        Dict(Dict &&o)
        {
            ShalowCopy(o);
            o.Init();
        }

        Dict& operator=(Dict const &o)
        {
            if (!buckets && !o.buckets)         // 两边都空: 啥都不干, 返回
            {
                return *this;
            }
            else if (buckets && !o.buckets)     // 我方不空, 目标为空: 清空自己, 返回
            {
                DeleteAll();
                Init();
                return *this;
            }
            else if (!buckets)                  // 我方为空, 目标不空: 重建
            {
                Ensure(o.Size());
            }
            else if (bucketsLen < o.Size())     // 都不空: 我方装不下就 重建
            {
                DeleteAll();
                Ensure(o.Size());
            }
            else                                // 都不空: 装得下就清空数据
            {
                DeleteKVs();
                memset(buckets, -1, bucketsLen * sizeof(int));
                freeList = -1;
                freeCount = 0;
                // bucketsLen 及指针变量 维持原值, count 值经由 FastAdd 填充
            }

            FastAdd(o);     // 复制数据 并设置 count 的值
            return *this;
        }

        Dict& operator=(Dict &&o)
        {
            if (buckets)
            {
                DeleteAll();
            }
            ShalowCopy(o);
            o.Init();
            return *this;
        }

        ~Dict()
        {
            if (!buckets) return;
            DeleteAll();
            Init();
        }


        void Swap(Dict &o)
        {
            std::swap(freeList, o.freeList);
            std::swap(freeCount, o.freeCount);
            std::swap(count, o.count);
            std::swap(bucketsLen, o.bucketsLen);
            std::swap(buckets, o.buckets);
            std::swap(nodes, o.nodes);
            std::swap(items, o.items);
        }


        // 仅确保 dict 重新 "可用" ( move 后 ), 不能拿来扩容. 内部也用于 Init 后的内存初建
        void Ensure(int capacity = 16)
        {
            if (buckets) return;
            bucketsLen = (int)Utils::GetPrime(capacity, sizeof(Data));
            Alloc();
        }

        template<typename K, typename ...VPS>
        std::pair<bool, int> EmplaceAdd(bool override, K &&key, VPS &&... vps)
        {
            assert(bucketsLen);

            // hash 按桶数取模 定位到具体 链表, 扫找
            auto hashCode = Utils::GetHashCode(key);
            auto targetBucket = hashCode % bucketsLen;
            for (int i = buckets[targetBucket]; i >= 0; i = nodes[i].next)
            {
                if (nodes[i].hashCode == hashCode && Utils::Equals(items[i].key, key))
                {
                    if (override)                       // 允许覆盖 value
                    {
                        items[i].value = TV(std::forward<VPS>(vps)...);
                        return std::make_pair(true, i);
                    }
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

            // 移动复制构造写 key, value
            new (&items[index].key) TK(std::forward<K>(key));
            new (&items[index].value) TV(std::forward<VPS>(vps)...);
            items[index].prev = -1;

            return std::make_pair(true, index);
        }


        std::pair<bool, int> Add(TK const &k, TV const &v, bool override = false)
        {
            return EmplaceAdd(override, k, v);
        }
        std::pair<bool, int> Add(TK const &k, TV &&v, bool override = false)
        {
            return EmplaceAdd(override, k, (TV&&)v);
        }
        std::pair<bool, int> Add(TK &&k, TV const &v, bool override = false)
        {
            return EmplaceAdd(override, (TK&&)k, v);
        }
        std::pair<bool, int> Add(TK &&k, TV &&v, bool override = false)
        {
            return EmplaceAdd(override, (TK&&)k, (TV&&)v);
        }


        // 只支持没数据时扩容或空间用尽扩容( 如果不这样限制, 扩容时的 遍历损耗 略大 )
        void Reserve(int capacity = 0)
        {
            assert(buckets);
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
            if ((std::is_base_of<Memmoveable, TK>::value || std::is_pod<TK>::value)
                && (std::is_base_of<Memmoveable, TV>::value || std::is_pod<TV>::value))
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
                    new (&newItems[i].value) TV((TV&&)items[i].value);
                    items[i].value.TV::~TV();
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
            assert(buckets);
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


        void RemoveIndexAt(int idx)
        {
            assert(buckets);
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
            items[idx].value.TV::~TV();
            items[idx].prev = -2;           // foreach 时的无效标志
        }


        void Remove(TK const &key)
        {
            assert(buckets);
            auto idx = Find(key);
            if (idx != -1)
            {
                RemoveIndexAt(idx);
            }
        }


        void Clear()
        {
            assert(buckets);
            if (!count) return;
            DeleteKVs();
            memset(buckets, -1, bucketsLen * sizeof(int));
            freeList = -1;
            freeCount = 0;
            count = 0;
        }


        int Size() const
        {
            assert(buckets);
            return int(count - freeCount);
        }

        bool Empty()
        {
            assert(buckets);
            return count == 0;
        }

        TV* TryGetValue(TK const &key)
        {
            int idx = Find(key);
            if (idx >= 0) return items[idx].value;
            return nullptr;
        }

        template<typename K>
        TV& operator[](K &&key)
        {
            assert(buckets);
            int idx = Find(key);
            if (idx < 0)
            {
                idx = EmplaceAdd(true, std::forward<K>(key), TV()).second;
            }
            return items[idx].value;
        }
        template<typename K>
        TV& At(K &&key)
        {
            return operator[](std::forward<K>(key));
        }

        TK& IndexAtKey(int idx)
        {
            assert(buckets);
            assert(idx < count);
            return items[idx].key;
        }
        TV& IndexAtValue(int idx)
        {
            assert(buckets);
            assert(idx < count);
            return items[idx].value;
        }
        Data& At(int idx)
        {
            assert(buckets);
            assert(idx < count);
            return items[idx];
        }

        TK const& IndexAtKey(int idx) const
        {
            return const_cast<Dict*>(this)->IndexAtKey(idx);
        }
        TV const& IndexAtValue(int idx) const
        {
            return const_cast<Dict*>(this)->IndexAtValue(idx);
        }
        Data const& At(int idx) const
        {
            return const_cast<Dict*>(this)->At(idx);
        }

    protected:
        void Alloc()                                        // 用于初始化和复制
        {
            items = (Data*)malloc(bucketsLen * sizeof(Data));
            nodes = (Node*)malloc(bucketsLen * sizeof(Node));
            buckets = (int*)malloc(bucketsLen * sizeof(int));
            memset(buckets, -1, bucketsLen * sizeof(int));  // -1 代表 "空"
        }

        void FastAdd(Dict const& o)                         // 用于 复制构造 和 operator=
        {
            assert(buckets && o.buckets);
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
                    new (&items[i].value) TV(o.items[oi].value);
                    items[i].prev = -1;
                    ++i;
                }
            }
            count = i;
        }

        void DeleteKVs()                                    // 用于 析构, 复制, Clear
        {
            assert(buckets);
            for (int i = 0; i < count; ++i)
            {
                if (items[i].prev != -2)
                {
                    items[i].key.TK::~TK();
                    items[i].value.TV::~TV();
                }
            }
        }

        void DeleteAll()                                    // 用于析构, 移动复制
        {
            assert(buckets);
            DeleteKVs();
            free(buckets);
            free(nodes);
            free(items);
            buckets = nullptr;                              // 各种空判断的依据
        }

        void Init()
        {
            freeList = -1;
            freeCount = 0;
            count = 0;
            bucketsLen = 0;
            buckets = nullptr;
            nodes = nullptr;
            items = nullptr;
        }

        void ShalowCopy(Dict const& o)
        {
            freeList = o.freeList;
            freeCount = o.freeCount;
            count = o.count;
            bucketsLen = o.bucketsLen;
            buckets = o.buckets;
            nodes = o.nodes;
            items = o.items;
        }
    };
}
