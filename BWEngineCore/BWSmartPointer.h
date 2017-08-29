#ifndef SMARTPOINTER_H_
#define SMARTPOINTER_H_
#include <assert.h>
#include <stdlib.h>
//������֤����ֻ��ָ��֮�丳ֵ  �������� smarpoint<t> tmp = new t ;  smartpoint<t> tmp1 = tmp.Get() ; �����͵���smartpoint �ļ���ʧЧ
//Ҳ������ʹ�ù��̱���ʹ�ý�get�õ����ָ�븳ֵ����һ��ֻ��ָ��   ��ô ��Ӧ�ó�����ֻҪʹ��ָ��ָ��get�õ���ָ��Ϳ����� ����delete�� Ҳ������new
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
	//���ﻹ��һ����� �������յ�smartepointer�໥��ֵ ��ô�죿
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
		//��֤�� �����ͬһ��SmartPointer<T> ���� mpointerָ������ݲ�ͬ ��������µĸ�ֵ  Ҳ�ܹ���ָ֤���һ����
		SmartPointer<T> tmp = sP;
		swap(tmp);
		return *this;
	}

	// ??Ϊʲôʹ��ģ�壿�� �������ת��������ô��
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