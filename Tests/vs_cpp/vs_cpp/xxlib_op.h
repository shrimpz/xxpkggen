#pragma once
#include <vector>
#include <unordered_set>
#include <assert.h>
#include <stdint.h>     // SIZE_MAX

namespace xxlib
{
    struct Obj
    {
        size_t objiter;                                 // 如果 Obj 要放入 ObjManager, 需要声明它. 在 Alloc 时, 它将被赋值, 用于快速删除
        size_t versionNumber;                           // 在派生类中声明, 用于 ObjPool + ObjPtr 做对象有效性检查
        //static const size_t typeId;                   // 用于 ObjPool 分类管理( 派生类自己声明和赋自增值, 值不可重叠不可断层, 从 0 开始 )
        virtual size_t GetTypeId() = 0;                 // return typeId( 必须继承并实现 ), 用以弱类型时的正确回收
        // T() {};                                      // 生命周期 -- 物理初始化( 派生类自己实现 )
        // void Init( ... );                            // 生命周期 -- 逻辑初始化( 派生类自己实现 )
        virtual bool Process(size_t ticks) = 0;         // 生命周期 -- 执行( 必须继承并实现 ), 返回 false 表示 "要自杀", 将由调用者继续调用其 Destroy() 函数
        virtual void Destroy() = 0;                     // 生命周期 -- 逻辑销毁
        virtual ~Obj() {};                              // 生命周期 -- 物理销毁( 必须继承并实现 )
    };


    // todo: 在这个宏里进一步做文章, 以实现 typeId 的正确性检测, 甚至实现自增 id 的效果
    // 此宏可以写在 Obj 派生类的上方
#define XXLIB_OBJ_TYPEID( n ) \
    static const size_t typeId = n;\
    size_t GetTypeId() override { return typeId; }


    // 池对象的 带版本控制需求的指针 容器
    // sample: 
    /*
    PoolPtr<Foo> foo;
    foo = xxxx;
    if( auto o = foo.Ptr() ) ...
    */
    struct ObjPtr
    {
        Obj* pointer = nullptr;
        size_t versionNumber = 0;

        ObjPtr() {}
        ObjPtr(ObjPtr const& o) = default;
        ObjPtr& operator=(ObjPtr const& o) = default;

        ObjPtr(Obj* const p)
            : pointer(p)
            , versionNumber(p->versionNumber)
        {
        }
        ObjPtr& operator=(Obj* const o)
        {
            pointer = o;
            if (o)
            {
                versionNumber = o->versionNumber;
            }
            return *this;
        }
        bool operator==(ObjPtr const& o) const
        {
            return Ptr() == o.Ptr();
        }
        Obj* Ptr() const
        {
            if (!pointer || pointer->versionNumber != versionNumber)
            {
                return nullptr;
            }
            return pointer;
        }
    };


    template<typename T>
    struct ObjPtrEx : public ObjPtr
    {
        ObjPtrEx() {}
        ObjPtrEx(ObjPtrEx const& o) = default;
        ObjPtrEx& operator=(ObjPtrEx const& o) = default;

        ObjPtrEx(T* const p)
            : ObjPtr(p)
        {
        }
        ObjPtrEx& operator=(T* const o)
        {
            return *(ObjPtrEx*)this->ObjPtr::operator=(o);
        }
        bool operator==(ObjPtrEx const& o) const
        {
            return Ptr() == o.Ptr();
        }
        T* Ptr() const
        {
            return (T*)this->ObjPtr::Ptr();
        }
    };



    // 对象池，提供创建 / 分配 / 回收, 版本管理
    struct ObjPool
    {
        std::vector<std::vector<Obj*>> pools;           // 池数组( 以 typeId 为下标 )
        size_t versionNumber;                           // 全局自增版本号
        size_t numObjTypes;                             // 保存 一共有多少个 obj

        ObjPool(size_t numObjTypes)
            : versionNumber(0)
            , numObjTypes(numObjTypes)
        {
            pools.resize(numObjTypes + 1);
        }
        ~ObjPool()
        {
            Clear();
        }

        // 通过池试分配一个 Obj派生类
        // TypeName: Obj派生类名
        // 调用方式: op.Alloc<TypeName>( 传给 分配后的实例.Alloc 函数的参数表 )
        template<typename T, typename ...PTS>
        T* Alloc(PTS&& ...ps)
        {
            T* rtv;
            auto& pool = pools[T::typeId];              // 根据 typeId 定位到目标池
            if (!pool.empty())
            {
                rtv = (T*)pool.back();                  // 池不空就从池取
                pool.pop_back();
            }
            else
            {
                rtv = new T();                          // 调用默认构造函数
            }
            rtv->T::versionNumber = ++versionNumber;    // 设置版本号
            rtv->Init(std::forward<PTS>(ps)...);        // 调用逻辑构造函数
            return rtv;
        }

        // 逻辑回收一个 Obj派生类 到池( 将执行其 Destroy 并改版本号 )
        void Free(Obj* o, size_t typeId)
        {
            o->Destroy();
            o->versionNumber = 0;
            pools[typeId].push_back(o);
        }
        void Free(Obj* o)
        {
            Free(o, o->GetTypeId());
        }
        template<typename T>
        void Free(T* o)
        {
            Free(o, T::typeId);
        }
        template<typename T>
        void Free(ObjPtrEx<T> o)
        {
            if (auto p = o.Ptr())
            {
                Free(p);
            }
        }
        void Free(ObjPtr o)
        {
            if (auto p = o.Ptr())
            {
                Free(p);
            }
        }

        // 为指定类型预创建够 capacity 个空对象到相应池中( 当前池中不会超出 capacity 个 )
        template<typename T>
        void Prepare(size_t capacity)
        {
            auto& pool = pools[T::typeId];
            count -= pool.size();
            for (size_t i = 0; i < count; ++i)
            {
                pool.push_back(new T());
            }
        }

        // 物理回收掉每种类型的池中的对象( 不清掉池本身 )
        void Clear()
        {
            for (size_t i = 0; i < pools.size(); ++i)
            {
                auto& os = pools[i];
                for (size_t j = 0; j < os.size(); ++j)
                {
                    delete os[j];
                }
                os.clear();
            }
        }
    };


    // 对象管理器
    struct ObjMgr
    {
        std::vector<std::vector<Obj*>> oss;     // objects s
        ObjPool op;                             // object pool
        size_t count;                           // oss's obj's count

        ObjMgr(size_t numObjTypes)
            : op(numObjTypes)
            , count(0)
        {
            oss.resize(numObjTypes);
        }
        ~ObjMgr()
        {
            Clear();
        }

        template<typename T>
        std::vector<T*>& RefObjs()
        {
            return *(std::vector<T*>*)&oss[T::typeId];
        }
        std::vector<Obj*>& RefObjs(size_t typeId)
        {
            return oss[typeId];
        }


        template<typename T, typename ...PTS>
        T* Alloc(PTS&& ...ps)
        {
            auto& os = RefObjs<T>();
            auto rtv = op.Alloc<T>(std::forward<PTS>(ps)...);
            rtv->objiter = os.size();       // for fast erase
            os.push_back(rtv);
            ++count;
            return rtv;
        }

        void Free(Obj* o, size_t typeId)
        {
            auto& os = RefObjs(typeId);
            op.Free(o, typeId);
            // 快速删除: 如果指定的 o 后面还有 obj, 则与最后一个交换位置, 令最后一个 objiter 等于自己的 iter, 容器 pop_back
            if (o->objiter + 1 < os.size())
            {
                os.back()->objiter = o->objiter;
                os[o->objiter] = os.back();
            }
            os.pop_back();
            --count;
        }
        void Free(Obj* o)
        {
            Free(o, o->GetTypeId());
        }
        template<typename T>
        void Free(T* o)
        {
            Free(o, T::typeId);
        }
        template<typename T>
        void Free(ObjPtrEx<T> o)
        {
            if (auto p = o.Ptr())
            {
                Free(p);
            }
        }
        void Free(ObjPtr o)
        {
            if (auto p = o.Ptr())
            {
                Free(p);
            }
        }

        bool Process(size_t ticks = 0)
        {
            for (size_t j = 0; j < oss.size(); ++j)
            {
                auto& os = oss[j];
                for (size_t i = os.size() - 1; i != SIZE_MAX; --i)  // -1 的无符号 size_t 为 SIZE_MAX
                {
                    auto& o = os[i];
                    if (!o->Process(ticks))
                    {
                        Free(o, j);
                    }
                }
            }
            return count > 0;
        }

        void Clear()
        {
            for (size_t j = 0; j < oss.size(); ++j)
            {
                auto& os = oss[j];
                for (size_t i = os.size() - 1; i != SIZE_MAX; --i)
                {
                    Free(os[i], j);
                }
                assert(os.size() == 0);
            }
            op.versionNumber = 0;
        }
    };

}
