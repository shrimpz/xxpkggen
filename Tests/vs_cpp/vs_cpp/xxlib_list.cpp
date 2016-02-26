#include "xxlib_list.h"
#include "xxlib_utils.h"
#include "assert.h"
#include "string.h"

namespace xxlib
{
    List<bool>::List(size_t capacity)
    {
        assert(capacity > 0);
        auto len = capacity / 8;
        if (len < 64) len = 64;
        else len = Utils::Round2n(len);
        buf = (char*)malloc(len);
        bufLen = len * 8;
    }


    List<bool>::List(std::initializer_list<bool> items)
        : List(items.size())
    {
        for (auto& i : items)
        {
            Add(i);
        }
    }

    List<bool>::List()
        : List(512)
    {
    }

    List<bool>::~List()
    {
        if (buf)
        {
            Clear();
            free(buf);
        }
    }

    List<bool>::List(List<bool> && o)
        : buf(o.buf)
        , bufLen(o.bufLen)
        , dataLen(o.dataLen)
    {
        o.buf = nullptr;
    }

    List<bool>::List(List<bool> const& o)
        : List<bool>(o.dataLen)
    {
        memcpy(buf, o.buf, o.DataByteLen());
    }

    List<bool>& List<bool>::operator=(List<bool> && o)
    {
        free(buf);
        buf = o.buf;
        dataLen = o.dataLen;
        bufLen = o.bufLen;
        o.buf = nullptr;
        return *this;
    }

    List<bool>& List<bool>::operator=(List<bool> const& o)
    {
        if (this == &o)
        {
            return *this;
        }
        dataLen = o.dataLen;
        if (bufLen < o.dataLen)
        {
            auto len = o.dataLen / 8;
            if (len < 64) len = 64;
            else len = Utils::Round2n(len);
            bufLen = len * 8;
            free(buf);
            buf = (char*)malloc(len);
        }
        memcpy(buf, o.buf, o.DataByteLen());
        return *this;
    }

    void List<bool>::Add(bool v)
    {
        if (dataLen == bufLen) Reserve(dataLen + 1);
        Set(dataLen++, v);
    }

    void List<bool>::Pop()
    {
        assert(dataLen > 0);
        --dataLen;
    }

    bool List<bool>::Top() const
    {
        assert(dataLen > 0);
        return operator[](dataLen - 1);
    }

    void List<bool>::Clear()
    {
        dataLen = 0;
    }

    void List<bool>::Reserve(size_t capacity)
    {
        assert(capacity > 0);
        if (capacity <= bufLen) return;
        auto len = Utils::Round2n((capacity - 1) / 8 + 1);
        bufLen = len * 8;
        auto newBuf = (char*)malloc(len);
        memcpy(newBuf, buf, DataByteLen());
        free(buf);
        buf = newBuf;
    }


	size_t List<bool>::CalcByteLen(size_t len)
	{
		return (len - 1) / 8 + 1;
	}

    size_t List<bool>::DataByteLen() const
    {
		return dataLen ? CalcByteLen(dataLen) : 0;
    }

    size_t List<bool>::BufByteLen() const
    {
		return bufLen ? CalcByteLen(bufLen) : 0;
    }

    bool List<bool>::operator[](size_t idx) const
    {
        return At(idx);
    }

    bool List<bool>::At(size_t idx) const
    {
        return (((size_t*)buf)[idx / (sizeof(size_t) * 8)] &
            (size_t(1) << (idx % (sizeof(size_t) * 8)))) > 0;
    }

    void List<bool>::Set(size_t idx, bool v)
    {
        assert(idx >= 0 && idx < dataLen);
        if (v) SetTrue(idx);
        else SetFalse(idx);
    }

    void List<bool>::SetTrue(size_t idx)
    {
        assert(idx >= 0 && idx < dataLen);
        ((size_t*)buf)[idx / (sizeof(size_t) * 8)] |=
            (size_t(1) << (idx % (sizeof(size_t) * 8)));
    }

    void List<bool>::SetFalse(size_t idx)
    {
        assert(idx >= 0 && idx < dataLen);
        ((size_t*)buf)[idx / (sizeof(size_t) * 8)] &=
            ~(size_t(1) << (idx % (sizeof(size_t) * 8)));
    }

    void List<bool>::FillTrue()
    {
        memset(buf, 0xFFFFFFFFu, (dataLen - 1) / 8 + 1);
    }

    void List<bool>::FillFalse()
    {
        memset(buf, 0, (dataLen - 1) / 8 + 1);
    }

    void List<bool>::Fill(bool v, size_t idxFrom, size_t idxTo)
    {
        assert(dataLen > 0 && idxFrom >= 0 && idxTo >= 0 && idxFrom < dataLen && idxTo < dataLen);
        if (idxFrom == idxTo)
        {
            Set(idxFrom, v);
            return;
        }
        if (idxFrom > idxTo)
        {
            std::swap(idxFrom, idxTo);
        }
        auto byteIdxFrom = idxFrom >> 3;
        auto byteIdxTo = idxTo >> 3;

        if (byteIdxFrom == byteIdxTo)
        {
            // 搞一个 中间一段是 v 的 unsigned char 出来
            if (v)
            {
                buf[byteIdxFrom] |= (unsigned char)0xFFu >> (7 - (idxTo - idxFrom)) << (idxFrom & 7);
            }
            else
            {
                buf[byteIdxFrom] &= ~((unsigned char)0xFFu >> (7 - (idxTo - idxFrom)) << (idxFrom & 7));
            }
        }
        else
        {
            // 分别搞一头一尾, 再 memset 中间
            auto idxFrom7 = idxFrom & 7;
            auto idxTo7 = idxTo & 7;
            if (v)
            {
                buf[byteIdxFrom] |= (unsigned char)0xFFu << idxFrom7;
                buf[byteIdxTo] |= (unsigned char)0xFFu >> (7 - idxTo7);
            }
            else
            {
                buf[byteIdxFrom] &= ~((unsigned char)0xFFu << idxFrom7);
                buf[byteIdxTo] &= ~((unsigned char)0xFFu >> (7 - idxTo7));
            }
            if (idxFrom7) ++byteIdxFrom;
            if (idxTo7) --byteIdxTo;
            if (byteIdxFrom <= byteIdxTo)
            {
                memset(buf + byteIdxFrom, v ? 0xFFFFFFFFu : 0, byteIdxTo - byteIdxFrom);
            }
        }
    }

    void List<bool>::Resize(size_t capacity, bool init)
    {
        if (capacity == dataLen) return;
        if (capacity < dataLen)
        {
            dataLen = capacity;
            return;
        }
        Reserve(capacity);
        auto oldLen = dataLen;
        dataLen = capacity;
        if (init)
        {
            Fill(false, oldLen, capacity - 1);
        }
    }

    void List<bool>::CleanUp() const
    {
        if (auto mod = dataLen % 8)
        {
            buf[(dataLen - 1) / 8] &= ~(0xFF << mod);
        }
    }

}
