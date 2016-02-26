#pragma once
#include "xxlib_utils.h"
#include "xxlib_mp.h"
#include "xxlib_list.h"

namespace xxlib
{
    template <typename T>
    class LinksEx : Memmoveable
    {
    public:
        struct Node
        {
            T               value;
            size_t          index;                          // nodes[ index ]
        };
        List<Node*>         nodes;
        MemPool            *pool;                           // mempool for nodes

        LinksEx(MemPool *pool);				         		// Links( pool, 64 )
        explicit LinksEx(MemPool *pool, size_t capacity);
        LinksEx(LinksEx &&o);
        LinksEx(LinksEx const &o) = delete;
        LinksEx& operator=(LinksEx &&o);
        LinksEx& operator=(LinksEx const &o) = delete;
        ~LinksEx();
        template<typename...PTS>
        Node* EmplaceAdd(PTS&&...vps);						// same as Add but need value type's construct parameters
        Node* Add(T&& v);
        Node* Add(T const& v);
        Node* Find(T const& v);								// foreach scan. if exists, return Node*. else return nullptr
        void Remove(Node* n);
        void Clear();
        bool Empty();
        void Reserve(size_t capacity);
        size_t Size() const;
        Node const* operator[](size_t idx) const;
        Node* operator[](size_t idx);
        Node const* At(size_t idx) const;
        Node* At(size_t idx);

    protected:
        void Dispose(Node* n);
    };



    template <typename T>
    LinksEx<T>::LinksEx(MemPool* pool, size_t capacity)
        : pool(pool)
    {
        if (capacity <= 0)
        {
            capacity = 8;
        }
        else
        {
            capacity = Utils::Round2n(capacity);
        }
        nodes.Reserve(capacity);
    }

    template <typename T>
    LinksEx<T>::LinksEx(MemPool* pool)
        : LinksEx(pool, 64)
    {
    }

    template <typename T>
    LinksEx<T>::LinksEx(LinksEx&& o)
    {
        operator=((LinksEx&&)o);
    }

    template <typename T>
    LinksEx<T>& LinksEx<T>::operator=(LinksEx&& o)
    {
        nodes = std::move(o.nodes);
        pool = std::move(o.pool);
        return *this;
    }

    template <typename T>
    LinksEx<T>::~LinksEx()
    {
        for (size_t i = 0; i < nodes.dataLen; ++i)
        {
            Dispose(nodes[i]);
        }
    }

    template <typename T>
    template<typename...PTS>
    typename LinksEx<T>::Node* LinksEx<T>::EmplaceAdd(PTS&&...ps)
    {
        auto n = (Node*)pool->Alloc();
        new (&n->value) T(std::forward<PTS>(ps)...);
        n->index = nodes.dataLen;
        nodes.Add(n);
        return n;
    }

    template <typename T>
    typename LinksEx<T>::Node* LinksEx<T>::Add(T&& v)
    {
        return EmplaceAdd((T&&)v);
    }
    template <typename T>
    typename LinksEx<T>::Node* LinksEx<T>::Add(T const& v)
    {
        return EmplaceAdd(v);
    }

    template <typename T>
    typename LinksEx<T>::Node* LinksEx<T>::Find(T const& v)
    {
        for (size_t i = 0; i < nodes.dataLen; ++i)
        {
            if (nodes[i]->value == v)
            {
                return nodes[i];
            }
        }
        return nullptr;
    }

    template <typename T>
    void LinksEx<T>::Remove(Node* n)
    {
        auto last = nodes.Top();
        nodes.Pop();
        if (n != last)
        {
            nodes[n->index] = last;
        }
        last->index = n->index;

        Dispose(n);
        pool->Free(n);
    }

    template <typename T>
    void LinksEx<T>::Clear()
    {
        for (size_t i = 0; i < nodes.dataLen; ++i)
        {
            Dispose(nodes[i]);
            pool->Free(nodes[i]);
        }
        nodes.Clear();
    }


    template <typename T>
    void LinksEx<T>::Reserve(size_t capacity)
    {
        if (capacity <= nodes.bufLen) return;
        capacity = Utils::Round2n(capacity);
        nodes.Reserve(capacity);
    }


    template <typename T>
    size_t LinksEx<T>::Size() const
    {
        return nodes.dataLen;
    }

    template <typename T>
    void LinksEx<T>::Dispose(Node* n)
    {
        n->value.~T();
    }


    template <typename T>
    bool LinksEx<T>::Empty()
    {
        return nodes.dataLen == 0;
    }


    template <typename T>
    typename LinksEx<T>::Node const* LinksEx<T>::operator[](size_t idx) const
    {
        assert(idx < nodes.dataLen);
        return nodes[idx];
    }
    template <typename T>
    typename LinksEx<T>::Node* LinksEx<T>::operator[](size_t idx)
    {
        assert(idx < nodes.dataLen);
        return nodes[idx];
    }
    template <typename T>
    typename LinksEx<T>::Node const* LinksEx<T>::At(size_t idx) const
    {
        assert(idx < nodes.dataLen);
        return nodes[idx];
    }
    template <typename T>
    typename LinksEx<T>::Node* LinksEx<T>::At(size_t idx)
    {
        assert(idx < nodes.dataLen);
        return nodes[idx];
    }

}

