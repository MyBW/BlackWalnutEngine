#include "BWStringInterface.h"
StringInterfaceMap BWStringInterface::stringInterfaceMap;
void BWStringInterface::SetParameter(const std::string &name, const std::string &value)
{
	if (myParamCommandMap == NULL)
	 {
		 return;
	 }
	ParamCommandMap::iterator  cmd = myParamCommandMap->find(name);
	if (cmd != myParamCommandMap->end())
	{
		cmd->second->DoSet(this, value);
	}
}

bool BWStringInterface::CreateStringInterface(const std::string &name)
{
	StringInterfaceMap::iterator stringInterface = stringInterfaceMap.find(name);
	if (stringInterface != stringInterfaceMap.end())
	{
		myParamCommandMap = stringInterface->second;
		return false;
	}
	else
	{
		myParamCommandMap = new ParamCommandMap;
		stringInterfaceMap.insert(StringInterfaceMap::value_type(name, myParamCommandMap));
	}
}

ParamCommandMap* BWStringInterface::GetStringInterface()
{
	return myParamCommandMap;
}