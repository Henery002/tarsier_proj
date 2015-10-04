#ifndef CORE_SINGLETON_H_
#define CORE_SINGLETON_H_

#include "Core/Common.h"

// 动态单例, 无基类
template<typename T>
class SingletonDynamic
{
public:
    SingletonDynamic() {}
    virtual ~SingletonDynamic() {}

    // 获得实例
    static T* Inst()
    {
        if (!m_Inst)
        {
            m_Inst = boost::shared_ptr<T>(new T());
        }

        return m_Inst.get();
    }

    // 销毁实例
    static void DestroyInst()
    {
        m_Inst = boost::shared_ptr<T>();
    }

private:

    // 实例
    static boost::shared_ptr<T> m_Inst;
};

template<typename T>
boost::shared_ptr<T> SingletonDynamic<T>::m_Inst;

// 动态单例, 有基类
template<typename T, typename B>
class SingletonDynamicDerived : public B
{
public:
    SingletonDynamicDerived() {}
    virtual ~SingletonDynamicDerived() {}

    // 获得实例
    static T* Inst()
    {
        if (!m_Inst)
        {
            m_Inst = boost::shared_ptr<T>(new T());
        }

        return m_Inst.get();
    }

    // 销毁实例
    static void DestroyInst()
    {
        m_Inst = boost::shared_ptr<T>();
    }

    // 销毁自己
    virtual void DestroySelf()
    {
        m_Inst = boost::shared_ptr<T>();
    }

private:

    // 实例
    static boost::shared_ptr<T> m_Inst;
};

template<typename T, typename B>
boost::shared_ptr<T> SingletonDynamicDerived<T, B>::m_Inst;

#endif // CORE_SINGLETON_H_
