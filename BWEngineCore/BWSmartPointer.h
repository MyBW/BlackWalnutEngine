#ifndef SMARTPOINTER_H_
#define SMARTPOINTER_H_
#include <assert.h>
#include <stdlib.h>
//尽量保证两个只能指针之间赋值  避免诸如 smarpoint<t> tmp = new t ;  smartpoint<t> tmp1 = tmp.Get() ; 这样就导致smartpoint 的计数失效
//也就是在使用过程避免使用将get得到后的指针赋值给另一个只能指针   那么 在应用程序中只要使用指针指向get得到的指针就可以了 不用delete掉 也不存在new
template<class T>
class SmartPointer
{
public:
	SmartPointer() :mPointer(NULL), counter(NULL)
	{

	}
	SmartPointer(T* p) :mPointer(NULL), counter(NULL)
	{
		mPointer = p;
		counter = new unsigned int;
		*counter = 0;
		++(*counter);
	}
	//这里还有一种情况 如果多个空的smartepointer相互赋值 怎么办？
	SmartPointer(const SmartPointer &sP) :mPointer(NULL), counter(NULL)
	{
		if (&sP == this)
		{
			return;
		}
		counter = sP.counter;
		mPointer = sP.mPointer;
		if (counter)
		{
			++(*counter);
		}
	}


	SmartPointer<T>& operator=(const SmartPointer& sP)
	{
		if (&sP == this)
		{
			return *this;
		}
		//保证了 如果是同一个SmartPointer<T> 但是 mpointer指向的内容不同 这种情况下的赋值  也能够保证指针的一致性
		SmartPointer<T> tmp = sP;
		swap(tmp);
		return *this;
	}

	// ??为什么使用模板？？ 如果类型转换出错怎么办
	template<class Y>
	SmartPointer(const SmartPointer<Y> &r) :mPointer(NULL), counter(NULL)
	{
		mPointer = r.Get();
		counter = r.GetCounterPointer();
		if (counter)
		{
			++(*counter);
		}
	}
	template<class Y>
	SmartPointer& operator=(const SmartPointer<Y> &r)
	{
		if (mPointer == r.Get())
		{
			return *this;
		}
		SmartPointer<T> tmp(r);
		swap(tmp);
		return *this;
	}/**/
	~SmartPointer()
	{
		if (counter)
		{
			(*counter)--;
			if ((*counter) == 0)
			{
				delete mPointer;
				delete counter;
			}
		}
		return;
	}
	void swap(SmartPointer<T>& tmp)
	{
		std::swap(mPointer, tmp.mPointer);
		std::swap(counter, tmp.counter);
	}
	void bind(T* rep)
	{
		assert(!counter && !mPointer);
		counter = new unsigned int;
		(*counter) = 1;
		mPointer = rep;

	}
	inline T* operator->()
	{
		assert(mPointer);
		return mPointer;
	}
	inline T* operator->() const
	{
		assert(mPointer);
		return mPointer;
	}
   T* Get() const 
	{
		return mPointer;
	}
   T* Get()
	{
		return mPointer;
	}
	bool IsNull()
	{
		return mPointer ? false : true;
	}
	bool IsNull() const
	{
		return mPointer ? false : true;
	}
	void SetNull()
	{
		if (mPointer == NULL)
		{
			return;
		}
		(*counter)--;
		if (!(*counter))
		{
			delete mPointer;
		}
		mPointer = NULL;
		counter = NULL;
	}
	unsigned int GetCounter()const
	{
		return *counter;
	}
	unsigned int GetCounter()
	{
		return *counter;
	}
	unsigned int* GetCounterPointer()
	{
		return counter;
	}
	unsigned int* GetCounterPointer() const
	{
		return counter;
	}
protected:
	T* mPointer;
    unsigned int *counter;
};

#endif