#include "BWShaderManager.h"
#include "BWStringOperation.h"
#include "BWShader.h"
#include "BWLog.h"
enum ShaderID
{
	ID_Vertex_Shader ,
	ID_Fragment_Shader,
	ID_Geometry_Shader,
	ID_Compute_Shader,
	ID_Use_Shader ,
	ID_GLSL_Shader,
	ID_Source_Shader
};
std::map<std::string, unsigned int> ParseMap;

BWShaderPtr parseShader(BWDataStream &dataStream, const std::string &type, std::string &name ,const std::string &groupName)
{
	ParseMap["vertex_shader"] = ID_Vertex_Shader;
	ParseMap["fragment_shader"] = ID_Fragment_Shader;
	ParseMap["geometry_shader"] = ID_Geometry_Shader;
	ParseMap["compute_shader"] = ID_Compute_Shader;
	ParseMap["User"] = ID_Use_Shader;
	ParseMap["glsl"] = ID_GLSL_Shader;
	ParseMap["Source"] = ID_Source_Shader;
	Log::GetInstance()->logMessage(std::string(" load Shader " + name));
	BWShaderPtr shader = BWShaderManager::GetInstance()->CreateOrRetrieve(name, groupName).first;
	switch (ParseMap[type])
	{
	case ID_Vertex_Shader:    shader->setShaderType(ST_VERTEX_SHADER);   break;
	case ID_Fragment_Shader : shader->setShaderType(ST_FRAGMENT_SHADER); break;
	case ID_Compute_Shader:  shader->setShaderType(ST_COMPUTE_SHADER); break;
	case ID_Geometry_Shader:  shader->setShaderType(ST_GEOMETRY_SHADER); break; 
	default:
		assert(0);
	}
	std::string line;
	int brackets = 0;
	while (!dataStream.Eof())
	{
		line = dataStream.GetLine();
		StringUtil::DeleteChar(line, '\t');
		StringVector value = StringUtil::Split(line);
		if (value.size() == 0)
		{
			continue;
		}
		if (value.size() == 1)
		{
			if (value[0] == "{")
			{
				++brackets;
				continue;
			}
			if (value[0] == "}")
			{
				--brackets;
				break;
			}
		}
		switch (ParseMap[value[0]])
		{
		case  ID_Source_Shader:
			shader->setFileName(value[1]);
			break;
		case  ID_Use_Shader:
			if (!shader->addShader(
				BWShaderManager::GetInstance()->CreateOrRetrieve(value[1], groupName).first
				))
			{
				Log::GetInstance()->logMessage("Wrong:  parseShader ,  addShader" , false);
				assert(0);
			}
			break;
		default:
			assert(0);
			break;
		}
	}
	if (brackets != 0)
	{
		return NULL;
	}
	return shader;

}
BWShaderManager*  BWShaderManager::GetInstance()
{
	assert(instance);
	return instance;
}

BWShaderManager::BWShaderManager()
{
	pattern = ".shader";
	order = SLO_1;
	BWResourceGroupManager::GetInstance()->RegisterScriptLoader(this , order);
	BWResourceGroupManager::GetInstance()->RegisterResourceManager(pattern, this);
}
void BWShaderManager::ParseScript(BWDataStream& dataStream , const std::string &groupName)
{
	std::string line;
    while (!dataStream.Eof())
    {
		line = dataStream.GetLine();
		if (line.find("vertex_shader") != std::string::npos || line.find("fragment_shader") != std::string::npos 
			|| line.find("geometry_shader" ) != std::string::npos || line.find("compute_shader") != std::string ::npos)
		{
			StringVector stringVec = StringUtil::Split(line);
			if (stringVec.size() != 3)
			{
				assert(0);
			}
			BWShaderPtr shader = parseShader(dataStream, stringVec[0], stringVec[1] , groupName);
			if (!shader.Get())
			{
				assert(0);
			}
		}
    }
}

