#include "BWScriptLoader.h"

void BWScriptLoader::ParseScript(BWDataStream& dataStream, const std::string &groupName)
{

}
const std::string & BWScriptLoader::GetPattern()
{
	return pattern;
}

int BWScriptLoader::GetLoadingOrder() const
{
	return order;
}