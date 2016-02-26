#pragma once
#include "xxlib_utils.h"
#include "assert.h"
#include <vector>

namespace xxlib
{
    class MemPool : Memmoveable
    {
    public:

        MemPool() {};							        // do nothing. need Init
        MemPool(size_t itemSize, size_t pageSize);
        ~MemPool();
        MemPool(MemPool&& o) = default;
        MemPool& operator=(MemPool&& o) = default;
        MemPool(MemPool const& o) = delete;
        MemPool& operator=(MemPool const& o) = delete;

        void Init(size_t itemSize, size_t pageSize);	// set itemSize, pageSize, NewPage
        template<typename T>
        inline void Init(size_t pageItemCount = 8)
        {
            Init(MAX(sizeof(T), sizeof(void*)), pageItemCount * MAX(sizeof(T), sizeof(void*)));
        }

        void SetPageSize(size_t newPageSize);
        void SetPageSizeByItemCount(size_t itemCount);
        void NewPage();								    // spaceCounter += xxx, new & fill mem ptrs, pageSize *= 2

        // c style funcs
        void* Alloc();
        void* FastAlloc();							    // unsafe: do not ensure the space is enough
        void Free(void* p);

        // c++ style funcs	
        template<typename T, typename...PTS>
        inline T* New(PTS&&...ps)
        {
            assert(itemSize >= sizeof(T));
            auto p = Alloc();
            return new (p) T(std::forward<PTS>(ps)...);
        }

        template<typename T>
        inline void Delete(T* p)
        {
            p->T::~T();
            Free(p);
        }

    private:
        void* header = nullptr;					        // current alloc ptr( *header = next ptr )
    public:
        size_t itemSize = 0, pageSize = 0;
        size_t allPageItemCounter = 0;
    private:
        std::vector<void*> pages;

#ifdef _DEBUG
    public:
        size_t allocatedCounter = 0;
#endif
    } const emptyPool;
}

