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
    // 启用该上下文  以后所有的渲染命令都从这里开始
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