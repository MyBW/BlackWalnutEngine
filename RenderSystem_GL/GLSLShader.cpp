#include "GLSLShader.h"
#include "../BWEngineCore/BWLog.h"
#include "GL/include/glew.h"
#include "GLPreDefine.h"
#include "../BWEngineCore/BWStringOperation.h"
#include "../BWEngineCore/BWStringConverter.h"
#include "GLSLGpuProgram.h"




GLSLShader::GLSLShader(GLSLShaderManager *manager, const std::string &name, const std::string &groupName) :
BWShader(manager , name ,groupName),
namedConstantes(new GpuNamedConstants),
mIsCreateNamedConstant(false)
{
	GLenum tmp = glGetError();
}
GLSLShader::~GLSLShader()
{
	CHECK_GL_ERROR(glDeleteShader(mID));
}
GLenum GLSLShader::shaderGLType() const
{
	switch (mShaderType)
	{
	case ST_VERTEX_SHADER:
		return GL_VERTEX_SHADER;
	case ST_FRAGMENT_SHADER:
		return GL_FRAGMENT_SHADER;
	}
	Log::GetInstance()->logMessage("There is wrong  shader type", false);
	assert(0);
}
bool GLSLShader::compileShader()
{
	if (!mIsLoadResource)
	{
		loadFromFile();
	}
	if (!mIsCompileFinish)
	{
		mID = glCreateShader(shaderGLType());
		GLsizei *sizes = new GLsizei[mShaderList.size() + 1];
		const GLchar**sources = new const GLchar* [mShaderList.size() + 1];
	    
		sizes[0] = mShaderSource.length();
		sources[0] = new GLchar[sizes[0]];
		memcpy((void*)(sources[0]), mShaderSource.c_str(), sizes[0]);

		BWShaderList::iterator itor = mShaderList.begin();
		int i = 0;
		while (itor != mShaderList.end())
		{
			i++;
			sizes[i] = (*itor)->getShaderSource().length();
			sources[i] = new GLchar[sizes[i]];
			memcpy((void*)(sources[i]), (*itor)->getShaderSource().c_str(), sizes[i]);
			itor++;
		}
		CHECK_GL_ERROR(glShaderSource(mID, mShaderList.size() +1 , sources, sizes));
		glCompileShader(mID);
		mIsCompileFinish = printShaderInforLog(mFileName, mID);

		delete [] sizes;
		for (int i = 0; i < mShaderList.size() + 1 ; i++)
		{
			delete[] sources[i];
		}
		delete[] sources;

		return mIsCompileFinish;
	}
}
GpuNamedConstantsPtr GLSLShader::getNamedConstant() const
{
	GpuNamedConstantsPtr tmp = new GpuNamedConstants();
	BWShaderList::const_iterator shader = mShaderList.begin();
	while (shader != mShaderList.end())
	{
		GpuNamedConstantsPtr src = dynamic_cast<GLSLShader*>((*shader).Get())->getNamedConstant();
		GpuConstantDefinitionMap::iterator  itor = src->map.begin();
		while (itor != src->map.end())
		{
			if (tmp->map.find(itor->first) == tmp->map.end())
			{
				GpuConstantDefinition def = itor->second;
				if (def.isFloat())
				{
					def.physicalIndex = tmp->floatBufferSize;
					tmp->floatBufferSize += def.arraySize * def.elementSize;
				}
				else
				{
					def.physicalIndex = tmp->intBufferSize;
					tmp->intBufferSize += def.arraySize * def.elementSize;
				}
				tmp->map.insert(GpuConstantDefinitionMap::value_type(itor->first, def));
				if (def.arraySize != 1)
				{
					tmp->generateConstantDefinitionArrayEntries(name, def);
				}
			}
			itor++;
		}
		shader++;
	}
	GpuConstantDefinitionMap::iterator  itor = namedConstantes->map.begin();
	while (itor != namedConstantes->map.end())
	{
		if (tmp->map.find(itor->first) == tmp->map.end())
		{
			GpuConstantDefinition def = itor->second;
			if (def.isFloat())
			{
				def.physicalIndex = tmp->floatBufferSize;
				tmp->floatBufferSize += def.arraySize * def.elementSize;
			}
			else
			{
				def.physicalIndex = tmp->intBufferSize;
				tmp->intBufferSize += def.arraySize * def.elementSize;
			}
			tmp->map.insert(GpuConstantDefinitionMap::value_type(itor->first, def));
			if (def.arraySize != 1)
			{
				tmp->generateConstantDefinitionArrayEntries(name, def);
			}
		}
		itor++;
	}
	return tmp;
}
UBOInforMap GLSLShader::getUBOInfor() 
{
	UBOInforMap UBOInfor;
	BWShaderList::const_iterator shader = mShaderList.begin();
	while (shader != mShaderList.end())
	{
		UBOInforMap::const_iterator itor = dynamic_cast<GLSLShader*>((*shader).Get())->getUBOInfor().begin();
		UBOInforMap::const_iterator itorEnd = dynamic_cast<GLSLShader*>((*shader).Get())->getUBOInfor().end();
		while (itor != itorEnd)
		{
			UBOInfor.insert(UBOInforMap::value_type(itor->first, itor->second));
			itor++;
		}
		shader++;
	}

	UBOInforMap::const_iterator itor = mUBOInfor.begin();
	UBOInforMap::const_iterator itorEnd = mUBOInfor.end();
	while (itor != itorEnd)
	{
		UBOInfor.insert(UBOInforMap::value_type(itor->first, itor->second));
		itor++;
	}

	return UBOInfor;
}
 NameLocation GLSLShader::getInConstant()
{
	 NameLocation tmp;
	 BWShaderList::const_iterator shader = mShaderList.begin();
	 while (shader != mShaderList.end())
	 {
		 NameLocation src = dynamic_cast<GLSLShader*>((*shader).Get())->getInConstant();
		 NameLocation::iterator  itor = src.begin();
		 while (itor != src.end())
		 {
			 tmp.insert(NameLocation::value_type(itor->first, itor->second));
			 itor++;
		 }
		 shader++;
	 }
	 NameLocation::iterator  itor = mNameLoaction.begin();
	 while (itor != mNameLoaction.end())
	 {
		 tmp.insert(NameLocation::value_type(itor->first, itor->second));
		 itor++;
	 }
	return tmp;
}
 NameLocation GLSLShader::GetFragmentOutConstant()
 {
	 NameLocation tmp;
	 BWShaderList::const_iterator shader = mShaderList.begin();
	 while (shader != mShaderList.end())
	 {
		 NameLocation src = dynamic_cast<GLSLShader*>((*shader).Get())->GetFragmentOutConstant();
		 NameLocation::iterator  itor = src.begin();
		 while (itor != src.end())
		 {
			 tmp.insert(NameLocation::value_type(itor->first, itor->second));
			 itor++;
		 }
		 shader++;
	 }
	 NameLocation::iterator  itor = mFragOutNameLocation.begin();
	 while (itor != mFragOutNameLocation.end())
	 {
		 tmp.insert(NameLocation::value_type(itor->first, itor->second));
		 itor++;
	 }
	 return tmp;
 }
 void GLSLShader::createOutConstant(std::string& outCosntant)
 {
	 StringUtil::DeleteChar(outCosntant, '\t');
	 StringUtil::DeleteChar(outCosntant, '\r');
	 StringUtil::DeleteChar(outCosntant, '\n');
	 GLint location = -1;
	 // 首先排除是函数定义中的out 变量
	 if (outCosntant.find_first_of("(") != std::string::npos &&
		 outCosntant.find("location") == std::string::npos)
	 {
		 return;
	 }
	 
	 size_t inBeging = outCosntant.find("out ");
	 std::string layout = outCosntant.substr(0, inBeging);
	 if (!layout.empty())
	 {
		 size_t locationPos = layout.find("location");
		 if (locationPos != std::string::npos)
		 {
			 size_t numBegin = layout.find_first_not_of(" ", layout.find_first_of("=", locationPos) + 1);
			 location = StringConverter::ParseInt(layout.substr(numBegin, layout.find_first_not_of(" ", numBegin)));
		 }
	 }
	 std::string tmpInConstant = outCosntant.substr(inBeging, outCosntant.find_last_of(";", inBeging) - inBeging);
	 StringVector vec = StringUtil::Split(tmpInConstant);

	 // 一般我们只要知道in 类型变量的名字和loaction就可以了
	 mFragOutNameLocation.insert(NameLocation::value_type(vec[2], location));
 }
void GLSLShader::createInConstant(std::string &inConstant)
{
	//该函数本来是想把In 变量的location取出来的  后来改成在GPUProgram的loadimp中实现了 该函数只是记录了shader中包含的in 变量的名称
	StringUtil::DeleteChar(inConstant, '\t');
	StringUtil::DeleteChar(inConstant, '\r');
	StringUtil::DeleteChar(inConstant, '\n');
	GLint location = -1;
	// 首先排除是函数定义中的in 变量
	if (inConstant.find_first_of("(") != std::string::npos && 
		inConstant.find("location") == std::string::npos)
	{
		return;
	}
	size_t inBeging = inConstant.find_first_of("in ");
	std::string layout = inConstant.substr(0, inBeging);
	if (!layout.empty())
	{
		size_t locationPos = layout.find("location");
		if (locationPos != std::string::npos)
		{
			size_t numBegin = layout.find_first_not_of(" ", layout.find_first_of("=", locationPos) + 1);
			location = StringConverter::ParseInt(layout.substr(numBegin, layout.find_first_not_of(" ", numBegin)));
		}
	}
	std::string tmpInConstant = inConstant.substr(inBeging, inConstant.find_last_of(";", inBeging) - inBeging);
	StringVector vec = StringUtil::Split(tmpInConstant);

	mNameLoaction.insert(NameLocation::value_type(vec[2], location));
}
void GLSLShader::createNamedConstant()
{
	// 这里并没有考虑layout关键字

	if (compileShader() && mIsCreateNamedConstant == false)
	{
		namedConstantes->clearData();
		mUBOInfor.clear();
		if (mShaderType == ST_VERTEX_SHADER)
		{
			std::size_t currPos = mShaderSource.find("in ");//这里要加空格 因为可能和int冲突
			while (currPos != std::string::npos)
			{
				std::string sentence = mShaderSource.substr(currPos, mShaderSource.find_first_of(';', currPos) - currPos);
				createInConstant(sentence);
				currPos = mShaderSource.find("in ", currPos + 3);
			}
		}
		if (mShaderType == ST_FRAGMENT_SHADER)
		{
			// 解析fragment shader的out变量
			std::size_t prePos = 0;
			std::size_t currPos = mShaderSource.find(";");
			while (currPos != std::string::npos)
			{
				std::string sentnce = mShaderSource.substr(prePos, currPos - prePos);
				if (sentnce.find("out ") != std::string::npos)
				{
					createOutConstant(sentnce);
				}
				prePos = currPos + 1;
				currPos = mShaderSource.find_first_of(";", prePos);
			}
			//std::size_t currPos = mShaderSource.find("out ");//这里要加空格 因为可能和其他关键字冲突
			//while (currPos != std::string::npos)
			//{
			//	std::string sentence = mShaderSource.substr(currPos, mShaderSource.find_first_of(';', currPos) - currPos);
			//	createOutConstant(sentence);
			//	currPos = mShaderSource.find("out ", currPos + 4);
			//}
		}
		// 解析uniform变量
		std::size_t currPos = mShaderSource.find("uniform");
		while (currPos != std::string::npos)
		{
			std::string sentence = mShaderSource.substr(currPos, mShaderSource.find_first_of(';', currPos) - currPos);
			if (sentence.find("{") != std::string::npos)
			{
				sentence = mShaderSource.substr(currPos, mShaderSource.find_first_of("}", currPos) - currPos + 1);
				createUniformBuffer(sentence);
			}
			else
			{
				createUniformConstant(sentence);
			}
			currPos = mShaderSource.find("uniform", currPos + 7);
		}
		mIsCreateNamedConstant = true;
	}
}
void GLSLShader::preLoadImpl()
{
	loadFromFile();
}
void GLSLShader::loadImpl()
{
	if (!mIsCompileFinish)
	{
		compileShader();
	}
	createNamedConstant();
}
void GLSLShader::createUniformBuffer(std::string &uniformBuffer)
{
	UBOInfor  mUbo;
	StringUtil::DeleteChar(uniformBuffer, '\t');
	StringUtil::DeleteChar(uniformBuffer, '\r');
	StringUtil::DeleteChar(uniformBuffer, '\n');

	size_t uniformBeging = uniformBuffer.find_first_of("uniform");
	std::string layout = uniformBuffer.substr(0, uniformBeging);
	if (!layout.empty())
	{
		size_t bindingPos = layout.find("binding");
		if (bindingPos != std::string::npos)
		{
			size_t numBegin = layout.find_first_not_of(" ", layout.find_first_of("=", bindingPos) + 1);
			mUbo.mBinding = StringConverter::ParseInt(layout.substr(numBegin, layout.find_first_not_of(" ", numBegin)));
		}
	}
	mUbo.mUBOName = uniformBuffer.substr(uniformBeging + 7, uniformBuffer.find_first_of("{") - uniformBeging - 7);
	StringUtil::DeleteChar(mUbo.mUBOName, ' ');
	size_t endPos = uniformBuffer.find_last_of(';');
	size_t beginPos = uniformBuffer.find("{");
	while (beginPos < endPos)
	{
		beginPos = uniformBuffer.find_first_not_of(" ", beginPos + 1);
		std::string type = uniformBuffer.substr(beginPos , uniformBuffer.find_first_of(" ", beginPos + 1) - beginPos);
		beginPos = uniformBuffer.find_first_of(" ", beginPos);
		std::string name = uniformBuffer.substr(beginPos, uniformBuffer.find_first_of(";", beginPos) - beginPos);
		StringUtil::DeleteChar(name, ' ');
		createUniformConstant("uniform " + type + " " + name); // UBO 对象中的uniform也是通过传统的uniform方式赋值 所以也要放入namedconstant 中
		mUbo.mContentMember.push_back(name);
		beginPos = uniformBuffer.find_first_of(';', beginPos);
	}
	mUBOInfor.insert(UBOInforMap::value_type(mUbo.mUBOName, mUbo));
}
void GLSLShader::createUniformConstant(std::string &uniformSentence)
{
	StringUtil::DeleteChar(uniformSentence, '\t');
	std::size_t begin = uniformSentence.find_first_not_of(" ", uniformSentence.find("uniform") + 7);
	std::string type = uniformSentence.substr(begin, uniformSentence.find_first_of(" ", begin) - begin);
	std::string name = uniformSentence.substr(uniformSentence.find(type) + type.length());
	StringUtil::DeleteChar(type, ' ');
	StringUtil::DeleteChar(name, ' ');
	StringToEnumMap::iterator itor = mTypeEnumMap.find(type);
	GpuConstantDefinition def;
	if (itor != mTypeEnumMap.end())
	{
		CompleteConstantDefine(itor->second, def);
	}
	else
	{
		assert(0);
	}
	std::size_t pos = name.find("[");
	if (pos != std::string::npos)
	{
		std::string num = name.substr(pos + 1, name.find(']', pos) - pos - 1);
		name = name.substr(0, pos);
		def.arraySize = StringConverter::ParseInt(num);
	}
	else
	{
		def.arraySize = 1;
	}
	def.logicalIndex = 0;
	if (def.isFloat())
	{
		def.physicalIndex = namedConstantes->floatBufferSize;
		namedConstantes->floatBufferSize += def.arraySize * def.elementSize;
	}
	else
	{
		def.physicalIndex = namedConstantes->intBufferSize;
		namedConstantes->intBufferSize += def.arraySize * def.elementSize;
	}
	namedConstantes->map.insert(GpuConstantDefinitionMap::value_type(name, def));
	if (def.arraySize != 1)
	{
		namedConstantes->generateConstantDefinitionArrayEntries(name, def);
	}
}