#ifndef BWSHADER_H_
#define BWSHADER_H_


/*
    类名： BWShader
	功能： BWShader主要用来处理各种shader。主要有，载入源程序，处理，编译等等。
    同一类shader可以组成一个shader集合，他们之间可以互相调用彼此的函数，最终实
	现某一种功能。
	BWShader 不提供任何设置shader中常量（uniform变量）的接口 因为在BWShader阶段对常量（uniform变量）设置是没有
	意义的，所有常量的设置都在program函数中完成
	但是在具体实现过程中 （例如 GLSLshader），可以完成搜集常量信息的功能  以提供给program产生VAO和UBO
*/


#include "BWResource.h"
#include "BWSingleton.h"
#include "BWShaderManager.h"
enum ShaderType
{
	ST_VERTEX_SHADER ,
	ST_FRAGMENT_SHADER
};
class BWShaderPtr;

class BWShader : public BWResource 
{
public:
	BWShader(BWShaderManager *manager , const std::string &name , const std::string &groupName);
	virtual ~BWShader();
	void       setShaderType(ShaderType shaderType);
	ShaderType getShaderType() const;
	bool       addShader(BWShaderPtr shader);
	void       setFileName(const std::string& fileName);
	bool       isLoadFromFile();
	void       loadFromFile();
	bool       isCompile() const;
	const std::string&  getShaderSource();
protected:
	typedef std::list<BWShaderPtr> BWShaderList;
	ShaderType mShaderType;
	BWShaderList mShaderList;
	std::string mFileName;
	std::string mShaderSource;
	bool mIsLoadResource;
	bool mIsCompileFinish;
};
class BWShaderPtr : public SmartPointer<BWShader>
{
public:
	BWShaderPtr()
	{

	}
	BWShaderPtr(const BWResourcePtr resource)
	{
		if (resource.IsNull())
		{
			mPointer = NULL;
			counter = NULL;
			return;
		}
		mPointer = dynamic_cast<BWShader*>(resource.Get());
		counter = resource.GetCounterPointer();
		(*counter)++;
	}
	~BWShaderPtr()
	{

	}
private:

};

#endif