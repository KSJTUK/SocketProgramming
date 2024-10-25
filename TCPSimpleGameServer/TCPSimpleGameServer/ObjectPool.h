#pragma once

#include <queue>


template <typename ObjectType, size_t size>
class ObjectPool {
public:
    ObjectPool();
    ~ObjectPool();

public:
    ObjectType* AcquireObject();

    void ReturnObject(ObjectType* object);

    size_t Size() const;

private:
    std::queue<ObjectType*> mPool;
};

template<typename ObjectType, size_t size>
inline ObjectPool<ObjectType, size>::ObjectPool()
{
    for (size_t i = 0; i < size; ++i) {
        mPool.push(new ObjectType());
    }
}

template<typename ObjectType, size_t size>
inline ObjectPool<ObjectType, size>::~ObjectPool()
{
    while (false == mPool.empty()) {
        auto p = mPool.front();
        mPool.pop();
        delete p;
    }
}

template<typename ObjectType, size_t size>
inline ObjectType* ObjectPool<ObjectType, size>::AcquireObject()
{
    if (mPool.empty()) return nullptr;

    auto p = mPool.front();
    mPool.pop();
    return p;
}

template<typename ObjectType, size_t size>
inline void ObjectPool<ObjectType, size>::ReturnObject(ObjectType* object)
{
    object->Reset();

    mPool.push(object);
}

template<typename ObjectType, size_t size>
inline size_t ObjectPool<ObjectType, size>::Size() const
{
    return size;
}
