#ifndef SINGLETON_H
#define SINGLETON_H

#include "global.h"

template<typename T>
class Singleton {

public:
    static std::shared_ptr<T> GetInstance() {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {
            // 为什么不用make_shared构造
            // 继承模版单例类的子类的构造会设置为private，make_shared无法访问私有的构造函数
            _instance = std::shared_ptr<T>(new T);
        });

        return _instance;
    }

    void PrintAddr() {
        std::cout << _instance.get() << std::end;
    }

    ~Singleton() {
        std::cout << "This is Singleton destruct" << std::endl;
    }
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>&) = delete;

    static std::shared_ptr<T> _instance;
};

template<typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;


#endif // SINGLETON_H
