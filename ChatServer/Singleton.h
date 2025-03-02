#pragma once

#include <iostream>
#include <memory>

template <typename T>
class Singleton {
public:
	static std::shared_ptr<T> GetInstance() {
		static std::once_flag s_flag;
		std::call_once(s_flag, [&]() {
			_instance = std::shared_ptr<T>(new T);
			});

		return _instance;
	}

	~Singleton() {
		std::cout << "This is Singleton destruct" << std::endl;
	}

	void PrintAddr() {
		std::cout << _instance.get() << std::endl;
	}

protected:
	Singleton() = default;
	Singleton(const Singleton<T>&) = delete;
	Singleton& operator=(const Singleton<T>&) = delete;

	static std::shared_ptr<T> _instance;
};

template<typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;
