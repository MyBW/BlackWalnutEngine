#ifndef GLCONTEXT_H_
#define GLCONTEXT_H_
class GLContext
{
public:
	GLContext()
	{

	}
	virtual ~GLContext()
	{

	}
    // ���ø�������  �Ժ����е���Ⱦ��������￪ʼ
	virtual void setCurrent() = 0;
	virtual void endCurrent() = 0;
	bool getInitialized() { return initialized; }
	void setInitialized(){ initialized = true; }
	virtual GLContext * clone() const = 0;
	virtual void releaseContext() { }
private:
	bool initialized;
};

#endif