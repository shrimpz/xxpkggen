#include "xxlib_mp.h"
#include "xxlib_utils.h"
#include "assert.h"

namespace xxlib
{

    MemPool::MemPool(size_t itemSize, size_t pageSize)
    {
        assert(itemSize >= sizeof(void*));
        Init(itemSize, pageSize);
    }

    MemPool::~MemPool()
    {
        for (size_t i = 0; i < pages.size(); ++i)
        {
            Utils::AlignedFree(pages[i]);
        }
    }

    void MemPool::Init(size_t itemSize_, size_t pageSize_)
    {
        assert(!itemSize);				    // ensure init only execute 1 times
        if (itemSize_ < sizeof(void*))
        {
            itemSize_ = sizeof(void*);
        }
        itemSize = Utils::Round2n(itemSize_);
        SetPageSize(pageSize_);
        NewPage();
    }

    void MemPool::SetPageSize(size_t newPageSize)
    {
        assert(itemSize);					// need init
        if (newPageSize < itemSize)
        {
            newPageSize = itemSize;
        }
        pageSize = Utils::Round2n(newPageSize);
    }

    void MemPool::SetPageSizeByItemCount(size_t itemCount)
    {
        assert(itemSize);					// need init
        assert(itemCount);
        pageSize = Utils::Round2n(itemCount * itemSize);
    }

    XXLIB_NOINLINE void MemPool::NewPage()
    {
        assert(itemSize);					// need init
        allPageItemCounter += (pageSize / itemSize);
        auto p = (char*)Utils::AlignedAlloc(itemSize, pageSize);
        if (!p)
        {
            return;
        }
        pages.push_back(p);
        auto bak = p;
        auto pp = p + pageSize - itemSize;	// - itemSize: skip last cell
        for (; p < pp; p += itemSize)
        {
            *(void**)p = p + itemSize;		// cell1 = &cell2
        }
        *(void**)p = header;				// last cell link to header or set nullptr
        header = bak;
        pageSize *= 2;
    }

    void* MemPool::Alloc()
    {
        assert(itemSize);					// need init
        if (!header)
        {
            NewPage();
        }
        auto rtv = header;
        header = *(void**)header;
#ifdef _DEBUG
        ++allocatedCounter;
#endif
        return rtv;
    }

    void* MemPool::FastAlloc()
    {
        assert(itemSize);					// need init
        auto rtv = header;
        header = *(void**)header;
#ifdef _DEBUG
        ++allocatedCounter;
#endif
        return rtv;
    }

    void MemPool::Free(void* p)
    {
        assert(itemSize);					// need init
        assert(p);
        *(void**)p = header;
        header = p;
#ifdef _DEBUG
        --allocatedCounter;
#endif
    }

}
