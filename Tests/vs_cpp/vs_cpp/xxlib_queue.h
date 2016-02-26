#pragma once
#include "xxlib_utils.h"        // Round2n
#include <assert.h>

namespace xxlib
{
    // 注: 在 linux 下于 std queue 而言, 并无性能优势. windows 下倒是快出不少
    // 能随机检索...能批量 pop
    // todo: 传入 buf 的支持

    // memory model: ring buffer
    // move to RIGHT way = forward
    // 3 situation:
    //...............FR...............		    // Front == Rear
    //......Front+++++++++++Rear......		    // DataLen = Rear - Front
    // ++++++Rear...........Front++++++		    // DataLen = BufLen - Front + Rear
    template <class T>
    class Queue
    {
    public:
        Queue();								// Queue( 8 )
        explicit Queue(size_t capacity);
        ~Queue();
        Queue(Queue const& o);
        Queue(Queue&& o);
        Queue& operator=(Queue const& o);
        Queue& operator=(Queue&& o);

        T const& operator[](size_t idx) const;	// [0] = Front()
        T& operator[](size_t idx);
        T const& At(size_t idx) const;		// like []
        T& At(size_t idx);

        size_t Size() const;
        bool Empty() const;
        void Clear();
        void Reserve(size_t capacity, bool afterPush = false);

        template<typename...PTS>
        T& EmplacePush(PTS&&...ps);			    // [ Rear++ ] = T( ps )
        void Push(T const& v);
        void Push(T&& v);
        T& Front();
        T const& Front() const;
        void Pop();								// ++Front
        bool Pop(T& outVal);
        size_t PopMulti(size_t count);			// Front += count

    private:
        size_t front, rear;
        size_t bufLen;
        T* buf;
    };






    template <class T>
    Queue<T>::Queue(size_t capacity)
    {
        if (capacity <= 0)
        {
            capacity = 8;
        }
        auto byteLen = Utils::Round2n(capacity * sizeof(T));
        bufLen = byteLen / sizeof(T);
        buf = (T*)malloc(byteLen);
        // FR..............................
        front = 0;
        rear = 0;
    }

    template <class T>
    Queue<T>::Queue()
        : Queue(8)
    {
    }

    template <class T>
    Queue<T>::~Queue()
    {
        if (buf)
        {
            Clear();
            free(buf);
        }
    }

    template <class T>
    size_t Queue<T>::Size() const
    {
        //......Front+++++++++++Rear......
        //...............FR...............
        if (front <= rear)
        {
            return rear - front;
        }
        // ++++++Rear...........Front++++++
        else
        {
            return rear + (bufLen - front);
        }
    }

    template <class T>
    bool Queue<T>::Empty() const
    {
        return front == rear;
    }

    template <class T>
    void Queue<T>::Pop()
    {
        assert(front != rear);
        buf[front++].~T();
        if (front == bufLen)
        {
            front = 0;
        }
    }

    template <class T>
    T const& Queue<T>::Front() const
    {
        assert(front != rear);
        return buf[front];
    }

    template <class T>
    T& Queue<T>::Front()
    {
        assert(front != rear);
        return buf[front];
    }


    template <class T>
    template<typename...PTS>
    T& Queue<T>::EmplacePush(PTS&&...ps)
    {
        auto idx = rear;
        new (buf + rear++) T(std::forward<PTS>(ps)...);

        if (rear == bufLen)		    // cycle
        {
            rear = 0;
        }
        if (rear == front)			// no more space
        {
            idx = bufLen;
            Reserve(bufLen + 1, true);
        }
        return buf[idx];
    }
    template <class T>
    void Queue<T>::Push(T const& v)
    {
        EmplacePush(v);
    }
    template <class T>
    void Queue<T>::Push(T&& v)
    {
        EmplacePush((T&&)v);
    }


    template <class T>
    void Queue<T>::Clear()
    {
        if (front == rear)
        {
            return;
        }
        if (!std::is_pod<T>::value)
        {
            //......Front+++++++++++Rear......
            if (front < rear)
            {
                for (size_t i = front; i < rear; ++i)
                {
                    buf[i].~T();
                }
            }
            // ++++++Rear...........Front++++++
            else
            {
                size_t i = 0;
                for (; i < rear; ++i)
                {
                    buf[i].~T();
                }
                for (i = front; i < bufLen; ++i)
                {
                    buf[i].~T();
                }
            }
        }
        //........FR......................
        front = rear;
    }

    template <typename T>
    size_t Queue<T>::PopMulti(size_t count)
    {
        if (count <= 0)
        {
            return 0;
        }
        auto dataLen = Size();
        if (count >= dataLen)
        {
            Clear();
            return dataLen;
        }
        // count < dataLen

        //......Front+++++++++++Rear......
        if (front < rear)
        {
            if (!std::is_pod<T>::value)
            {
                for (size_t i = front; i < count; i++)
                {
                    buf[i].~T();
                }
            }
            front += count;
        }
        // ++++++Rear...........Front++++++
        else
        {
            auto frontDataLen = bufLen - front;
            if (!std::is_pod<T>::value)
            {
                //...Front+++
                if (count < frontDataLen)
                {
                    for (size_t i = front; i < front + count; ++i)
                    {
                        buf[i].~T();
                    }
                    front += count;
                }
                else
                {
                    //...Front++++++
                    size_t i = front;
                    for (; i < bufLen; ++i)
                    {
                        buf[i].~T();
                    }
                    // ++++++Rear...
                    count -= frontDataLen;
                    for (i = 0; i < count; ++i)
                    {
                        buf[i].~T();
                    }
                    front = count;
                }
            }
        }
        return count;
    }

    template <class T>
    void Queue<T>::Reserve(size_t capacity, bool afterPush)
    {
        assert(capacity > 0);
        if (capacity <= bufLen)
        {
            return;
        }

        // afterPush: ++++++++++++++RF++++++++++++++++
        auto dataLen = afterPush ? bufLen : Size();

        auto newBufByteLen = Utils::Round2n(capacity * sizeof(T));
        auto newBufLen = newBufByteLen / sizeof(T);
        auto newBuf = (T*)malloc(newBufByteLen);

        //......Front+++++++++++Rear......
        if (front < rear)
        {
            if (std::is_pod<T>::value || std::is_base_of<Memmoveable, T>::value)
            {
                memcpy(newBuf, buf + front, dataLen * sizeof(T));
            }
            else
            {
                for (size_t i = 0; i < dataLen; ++i)
                {
                    new (newBuf + i) T(std::move(buf[front + i]));
                }
            }
        }
        // ++++++Rear...........Front++++++
        // ++++++++++++++RF++++++++++++++++
        else
        {
            //...Front++++++
            auto frontDataLen = bufLen - front;
            if (std::is_pod<T>::value || std::is_base_of<Memmoveable, T>::value)
            {
                memcpy(newBuf, buf + front, frontDataLen * sizeof(T));
            }
            else
            {
                for (size_t i = 0; i < frontDataLen; ++i)
                {
                    new (newBuf + i) T(std::move(buf[front + i]));
                }
            }
            // ++++++Rear...
            auto dstPos = newBuf + frontDataLen;
            if (std::is_pod<T>::value || std::is_base_of<Memmoveable, T>::value)
            {
                memcpy(dstPos, buf, rear * sizeof(T));
            }
            else
            {
                for (size_t i = 0; i < rear; ++i)
                {
                    new (dstPos + i) T(std::move(buf[i]));
                }
            }
        }

        if (!std::is_pod<T>::value && !std::is_base_of<Memmoveable, T>::value)
        {
            for (size_t i = 0; i < bufLen; ++i)		// clean up old data
            {
                buf[i].~T();
            }
        }
        free(buf);

        buf = newBuf;
        bufLen = newBufLen;
        // Front+++++++++++Rear............
        front = 0;
        rear = dataLen;
    }


    template<typename T>
    T const& Queue<T>::operator[](size_t idx) const
    {
        return At(idx);
    }

    template<typename T>
    T& Queue<T>::operator[](size_t idx)
    {
        return At(idx);
    }

    template<typename T>
    T const& Queue<T>::At(size_t idx) const
    {
        return const_cast<Queue<T>*>(this)->At(idx);
    }

    template<typename T>
    T& Queue<T>::At(size_t idx)
    {
        assert(idx < Size());
        // = buf[ ( front + idx ) % bufLen ];  but slowly 3 times than following code
        if (front + idx < bufLen)
        {
            return buf[front + idx];
        }
        else
        {
            return buf[front + idx - bufLen];
        }
    }


    template <typename T>
    bool Queue<T>::Pop(T& outVal)
    {
        if (front == rear)
        {
            return false;
        }
        outVal = std::move(buf[front]);
        Pop();
        return true;
    }

}
