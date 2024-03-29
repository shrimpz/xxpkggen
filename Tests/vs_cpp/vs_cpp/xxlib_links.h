﻿#pragma once
#include "xxlib_utils.h"
#include "xxlib_mp.h"
#include "xxlib_list.h"

namespace xxlib
{

    template <typename T>
    class Links : Memmoveable
    {
    public:
        struct Node
        {
            T               value;
            size_t          index;                          // nodes[ index ]
        };
        List<Node*>         nodes;

        Links();											// Links( 64 )
        explicit Links(size_t capacity);
        Links(Links&& o);
        Links(Links const& o);
        Links& operator=(Links&& o);
        Links& operator=(Links const& o);
        ~Links();
        template<typename...PTS>
        Node* EmplaceAdd(PTS&&...vps);						// same as Add but need value type's construct parameters
        Node* Add(T&& v);
        Node* Add(T const& v);
        Node* Find(T const& v);								// foreach scan. if exists, return Node*. else return nullptr
        void Remove(T const& v);
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
        MemPool pool;                                       // mempool for nodes
    };



    template <typename T>
    Links<T>::Links(size_t capacity)
    {
        if (capacity <= 0)
        {
            capacity = 8;
        }
        else
        {
            capacity = Utils::Round2n(capacity);
        }
        pool.Init<Node>(capacity);
        nodes.Reserve(capacity);
    }

    template <typename T>
    Links<T>::Links()
        : Links(64)
    {
    }

    template <typename T>
    Links<T>::Links(Links const& o)
        : Links(o.Size())
    {
        operator=(o);
    }

    template <typename T>
    Links<T>::Links(Links&& o)
    {
        operator=((Links&&)o);
    }

    template <typename T>
    Links<T>& Links<T>::operator=(Links const& o)
    {
        if (this == &o) return *this;
        Clear();
        Reserve(o.Size());
        for (size_t i = 0; i < o.Size(); ++i)
        {
            Add(o[i]->value);
        }
        return *this;
    }

    template <typename T>
    Links<T>& Links<T>::operator=(Links&& o)
    {
        nodes = std::move(o.nodes);
        pool = std::move(o.pool);
        return *this;
    }

    template <typename T>
    Links<T>::~Links()
    {
        for (size_t i = 0; i < nodes.dataLen; ++i)
        {
            Dispose(nodes[i]);
        }
    }

    template <typename T>
    template<typename...PTS>
    typename Links<T>::Node* Links<T>::EmplaceAdd(PTS&&...ps)
    {
        auto n = (Node*)pool.Alloc();
        new (&n->value) T(std::forward<PTS>(ps)...);
        n->index = nodes.dataLen;
        nodes.Add(n);
        return n;
    }

    template <typename T>
    typename Links<T>::Node* Links<T>::Add(T&& v)
    {
        return EmplaceAdd((T&&)v);
    }
    template <typename T>
    typename Links<T>::Node* Links<T>::Add(T const& v)
    {
        return EmplaceAdd(v);
    }

    template <typename T>
    typename Links<T>::Node* Links<T>::Find(T const& v)
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
    void Links<T>::Remove(T const& v)
    {
        auto n = Find(v);
        if (!n) return;
        Remove(n);
    }

    template <typename T>
    void Links<T>::Remove(Node* n)
    {
        auto last = nodes.Top();
        nodes.Pop();
        if (n != last)
        {
            nodes[n->index] = last;
        }
        last->index = n->index;

        Dispose(n);
        pool.Free(n);
    }

    template <typename T>
    void Links<T>::Clear()
    {
        for (size_t i = 0; i < nodes.dataLen; ++i)
        {
            Dispose(nodes[i]);
            pool.Free(nodes[i]);
        }
        nodes.Clear();
    }


    template <typename T>
    void Links<T>::Reserve(size_t capacity)
    {
        if (capacity <= nodes.bufLen) return;
        capacity = Utils::Round2n(capacity);
        nodes.Reserve(capacity);
        if (pool.allPageItemCounter < capacity)
        {
            pool.SetPageSizeByItemCount(capacity - pool.allPageItemCounter);
            pool.NewPage();
            pool.SetPageSizeByItemCount(nodes.bufLen * 2);
        }
    }


    template <typename T>
    size_t Links<T>::Size() const
    {
        return nodes.dataLen;
    }

    template <typename T>
    void Links<T>::Dispose(Node* n)
    {
        n->value.~T();
    }


    template <typename T>
    bool Links<T>::Empty()
    {
        return nodes.dataLen == 0;
    }


    template <typename T>
    typename Links<T>::Node const* Links<T>::operator[](size_t idx) const
    {
        assert(idx < nodes.dataLen);
        return nodes[idx];
    }
    template <typename T>
    typename Links<T>::Node* Links<T>::operator[](size_t idx)
    {
        assert(idx < nodes.dataLen);
        return nodes[idx];
    }
    template <typename T>
    typename Links<T>::Node const* Links<T>::At(size_t idx) const
    {
        assert(idx < nodes.dataLen);
        return nodes[idx];
    }
    template <typename T>
    typename Links<T>::Node* Links<T>::At(size_t idx)
    {
        assert(idx < nodes.dataLen);
        return nodes[idx];
    }

}

