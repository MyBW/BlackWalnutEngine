#ifndef BWSHADERMANAGER_H_
#define BWSHADERMANAGER_H_
#include "BWResourceManager.h"
#include "BWSingleton.h"
class BWShaderManager : public BWResourceManager , public BWSingleton<BWShaderManager>
{
public:
	BWShaderManager();
	virtual ~BWShaderManager()
	{

	}
	static BWShaderManager* GetInstance();
	virtual void ParseScript(BWDataStream& dataStream , const std::string &groupName);
};


#endif