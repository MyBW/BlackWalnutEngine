#ifndef BWSHADER_H_
#define BWSHADER_H_


/*
    ������ BWShader
	���ܣ� BWShader��Ҫ�����������shader����Ҫ�У�����Դ���򣬴�������ȵȡ�
    ͬһ��shader�������һ��shader���ϣ�����֮����Ի�����ñ˴˵ĺ���������ʵ
	��ĳһ�ֹ��ܡ�
	BWShader ���ṩ�κ�����shader�г�����uniform�������Ľӿ� ��Ϊ��BWShader�׶ζԳ�����uniform������������û��
	����ģ����г��������ö���program���������
	�����ھ���ʵ�ֹ����� ������ GLSLshader������������Ѽ�������Ϣ�Ĺ���  ���ṩ��program����VAO��UBO
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