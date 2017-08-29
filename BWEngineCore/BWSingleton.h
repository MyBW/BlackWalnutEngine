#ifndef SINGLETON_H_
#define SINGLETON_H_
#include <stdlib.h>
#include <cassert>
template<class T>
class BWSingleton
{
public:
	static T* GetInstance()
	{
		assert(instance);
		return instance;
	}
	virtual ~BWSingleton()
	{
		
	}
   
protected:
	
	BWSingleton()
	{
		assert(!instance);
		instance = static_cast<T*>(this);
	}
	static T* instance;
private:
	BWSingleton(const BWSingleton<T> &);
	BWSingleton<T>& operator=(const BWSingleton<T>&);
};

template<class T>
T* BWSingleton<T>::instance = NULL;

#endif