#include "BWShader.h"
#include "BWLog.h"
BWShader::BWShader(BWShaderManager *manager,const std::string &name, const std::string &groupName) :
BWResource(manager, name, groupName)  ,
mIsLoadResource(false)
{

}

void BWShader::setShaderType(ShaderType shaderType)
{
	mShaderType = shaderType;
}
ShaderType BWShader::getShaderType() const 
{
	return mShaderType;
}
void BWShader::setFileName(const std::string &name)
{
	mFileName = name;
	mIsLoadResource = false;
	mShaderSource.clear();
	mIsCompileFinish = false;
	unLoad();
}
bool BWShader::addShader(BWShaderPtr shader)
{
	if (shader->getShaderType() != mShaderType)
	{
		return false;
	}
	BWShaderList::iterator  itor = mShaderList.begin();
	while (itor != mShaderList.end())
	{
		if (itor->Get() == shader.Get())
		{
			return false;
		}
		itor++;
	}
	mShaderList.push_back(shader);
	return true;
}

bool BWShader::isLoadFromFile()
{
	return mIsLoadResource;
}
const std::string& BWShader::getShaderSource()
{
	loadFromFile();
	return mShaderSource;
}
void BWShader::loadFromFile()
{
	if (!mFileName.empty() && mIsLoadResource == false)
	{
		BWDataStreamPrt stream = BWResourceGroupManager::GetInstance()->OpenResource(mFileName, groupName);
		if (!stream.Get())
		{
			std::string Meg = "Can not find " + mFileName;
			Log::GetInstance()->logMessage(Meg);
		}
		mShaderSource = stream->GetAsString();
		mIsLoadResource = true;
	}
	BWShaderList::iterator itor = mShaderList.begin();
	while (itor != mShaderList.end())
	{
		(*itor)->loadFromFile();
		itor++;
	}
}
BWShader::~BWShader()
{

}
