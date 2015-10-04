#ifndef CORE_ALLOCATOR_H_
#define CORE_ALLOCATOR_H_

#include "Core/Singleton.h"
#include "Core/MemAllocator.h"

// 分配器
template<typename T, int TAG = MEM_TAG_DEFAULT>
class Allocator : public SingletonDynamic<Allocator<T, TAG> >
{
public:
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    template<class U, int UTAG>
    struct rebind
    {
        typedef Allocator<U, UTAG> other;
    };

    Allocator()
    {

    }

    Allocator(const Allocator<T, TAG>&)
    {

    }

    template<class U, int UTAG>
    Allocator(const Allocator<U, UTAG>&, int = 0)
    {

    }

    virtual ~Allocator()
    {

    }

    pointer address(reference x) const
    {
        return &x;
    }

    const_pointer address(const_reference x) const
    {
        return &x;
    }

    pointer allocate(size_type n, const void * = NULL, const char* fileName = "", int line = 0)
    {
        pointer p = (T*)(MemAllocator::Inst()->AllocMem(n * sizeof(T), TAG, fileName, line));
        return p;
    }

    void deallocate(void* p, size_type n, const char* fileName = "", int line = 0)
    {
        MemAllocator::Inst()->FreeMem(p, fileName, line);
    }

    size_type max_size() const
    {
        return (size_type)(-1);
    }

    void construct(pointer p, const T& val)
    {
        new(p)T(val);
    }

    void construct(pointer p)
    {
        new(p)T;
    }

    template <typename P1>
    void construct(pointer p, P1 p1)
    {
        new(p)T(p1);
    }

    template <typename P1, typename P2>
    void construct(pointer p, P1 p1, P2 p2)
    {
        new(p)T(p1, p2);
    }

    template <typename P1, typename P2, typename P3>
    void construct(pointer p, P1 p1, P2 p2, P3 p3)
    {
        new(p)T(p1, p2, p3);
    }

    template <typename P1, typename P2, typename P3, typename P4>
    void construct(pointer p, P1 p1, P2 p2, P3 p3, P4 p4)
    {
        new(p)T(p1, p2, p3, p4);
    }

    template <typename P1, typename P2, typename P3, typename P4, typename P5>
    void construct(pointer p, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
    {
        new(p)T(p1, p2, p3, p4, p5);
    }

    template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
    void construct(pointer p, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
    {
        new(p)T(p1, p2, p3, p4, p5, p6);
    }

    template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
    void construct(pointer p, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
    {
        new(p)T(p1, p2, p3, p4, p5, p6, p7);
    }

    template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
    void construct(pointer p, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
    {
        new(p)T(p1, p2, p3, p4, p5, p6, p7, p8);
    }

    template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
    void construct(pointer p, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
    {
        new(p)T(p1, p2, p3, p4, p5, p6, p7, p8, p9);
    }

    void destroy(pointer p)
    {
        p->~T();
    }

    friend bool operator==(const Allocator&, const Allocator&)
    {
        return true;
    }

    friend bool operator!=(const Allocator&, const Allocator&)
    {
        return false;
    }

    static T* New(const char* fileName, int line)
    {
        Allocator<T, TAG>* allocator = Allocator<T, TAG>::Inst();
        T* p = allocator->allocate(1, NULL, fileName, line);
        if (p != NULL)
        {
            allocator->construct(p);
        }
        return p;
    }

    template <typename P1>
    static T* New(P1 p1, const char* fileName, int line)
    {
        Allocator<T, TAG>* allocator = Allocator<T, TAG>::Inst();
        T* p = allocator->allocate(1, NULL, fileName, line);
        if (p != NULL)
        {
            allocator->construct(p, p1);
        }
        return p;
    }

    template <typename P1, typename P2>
    static T* New(P1 p1, P2 p2, const char* fileName, int line)
    {
        Allocator<T, TAG>* allocator = Allocator<T, TAG>::Inst();
        T* p = allocator->allocate(1, NULL, fileName, line);
        if (p != NULL)
        {
            allocator->construct(p, p1, p2);
        }
        return p;
    }

    template <typename P1, typename P2, typename P3>
    static T* New(P1 p1, P2 p2, P3 p3, const char* fileName, int line)
    {
        Allocator<T, TAG>* allocator = Allocator<T, TAG>::Inst();
        T* p = allocator->allocate(1, NULL, fileName, line);
        if (p != NULL)
        {
            allocator->construct(p, p1, p2, p3);
        }
        return p;
    }

    template <typename P1, typename P2, typename P3, typename P4>
    static T* New(P1 p1, P2 p2, P3 p3, P4 p4, const char* fileName, int line)
    {
        Allocator<T, TAG>* allocator = Allocator<T, TAG>::Inst();
        T* p = allocator->allocate(1, NULL, fileName, line);
        if (p != NULL)
        {
            allocator->construct(p, p1, p2, p3, p4);
        }
        return p;
    }

    template <typename P1, typename P2, typename P3, typename P4, typename P5>
    static T* New(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, const char* fileName, int line)
    {
        Allocator<T, TAG>* allocator = Allocator<T, TAG>::Inst();
        T* p = allocator->allocate(1, NULL, fileName, line);
        if (p != NULL)
        {
            allocator->construct(p, p1, p2, p3, p4, p5);
        }
        return p;
    }

    template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
    static T* New(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, const char* fileName, int line)
    {
        Allocator<T, TAG>* allocator = Allocator<T, TAG>::Inst();
        T* p = allocator->allocate(1, NULL, fileName, line);
        if (p != NULL)
        {
            allocator->construct(p, p1, p2, p3, p4, p6);
        }
        return p;
    }

    template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
    static T* New(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, const char* fileName, int line)
    {
        Allocator<T, TAG>* allocator = Allocator<T, TAG>::Inst();
        T* p = allocator->allocate(1, NULL, fileName, line);
        if (p != NULL)
        {
            allocator->construct(p, p1, p2, p3, p4, p6, p7);
        }
        return p;
    }

    template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
    static T* New(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, const char* fileName, int line)
    {
        Allocator<T, TAG>* allocator = Allocator<T, TAG>::Inst();
        T* p = allocator->allocate(1, NULL, fileName, line);
        if (p != NULL)
        {
            allocator->construct(p, p1, p2, p3, p4, p6, p7, p8);
        }
        return p;
    }

    template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
    static T* New(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, const char* fileName, int line)
    {
        Allocator<T, TAG>* allocator = Allocator<T, TAG>::Inst();
        T* p = allocator->allocate(1, NULL, fileName, line);
        if (p != NULL)
        {
            allocator->construct(p, p1, p2, p3, p4, p6, p7, p8, p9);
        }
        return p;
    }

    static void Delete(T* p, const char* fileName, int line)
    {
        Allocator<T, TAG>* allocator = Allocator<T, TAG>::Inst();
        if (p != NULL)
        {
            allocator->destroy(p);
            allocator->deallocate(p, 1, fileName, line);
        }
    }
};

template<>
class Allocator<void, 0> : public SingletonDynamic<Allocator<void, 0> >
{
public:
    typedef void* pointer;
    typedef const void* const_pointer;
    typedef void value_type;

    template<class U, int UTAG>
    struct rebind
    {
        typedef Allocator<U, UTAG> other;
    };
};

template<>
class Allocator<void, 1> : public SingletonDynamic<Allocator<void, 1> >
{
public:
    typedef void* pointer;
    typedef const void* const_pointer;
    typedef void value_type;

    template<class U, int UTAG>
    struct rebind
    {
        typedef Allocator<U, UTAG> other;
    };
};

#define NEW(T, tag) Allocator<T, tag>::New(__FILE__, __LINE__)
#define NEW_P1(T, p1, tag) Allocator<T, tag>::New(p1, __FILE__, __LINE__)
#define NEW_P2(T, p1, p2, tag) Allocator<T, tag>::New(p1, p2, __FILE__, __LINE__)
#define NEW_P3(T, p1, p2, p3, tag) Allocator<T, tag>::New(p1, p2, p3, __FILE__, __LINE__)
#define NEW_P4(T, p1, p2, p3, p4, tag) Allocator<T, tag>::New(p1, p2, p3, p4, __FILE__, __LINE__)
#define NEW_P5(T, p1, p2, p3, p4, p5, tag) Allocator<T, tag>::New(p1, p2, p3, p4, p5, __FILE__, __LINE__)
#define NEW_P6(T, p1, p2, p3, p4, p5, p6, tag) Allocator<T, tag>::New(p1, p2, p3, p4, p5, p6, __FILE__, __LINE__)
#define NEW_P7(T, p1, p2, p3, p4, p5, p6, p7, tag) Allocator<T, tag>::New(p1, p2, p3, p4, p5, p6, p7, __FILE__, __LINE__)
#define NEW_P8(T, p1, p2, p3, p4, p5, p6, p7, p8, tag) Allocator<T, tag>::New(p1, p2, p3, p4, p5, p6, p7, p8, __FILE__, __LINE__)
#define NEW_P9(T, p1, p2, p3, p4, p5, p6, p7, p8, p9, tag) Allocator<T, tag>::New(p1, p2, p3, p4, p5, p6, p7, p8, p9, __FILE__, __LINE__)

#define SAFE_DELETE(T, p) Allocator<T, MEM_TAG_DEFAULT>::Delete(p, __FILE__, __LINE__); p = NULL

#endif // CORE_ALLOCATOR_H_
