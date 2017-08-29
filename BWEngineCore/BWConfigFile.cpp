#include "BWConfigFile.h"
#include <fstream>
#include "BWDataStream.h"
#include "BWFileStreamDataStream.h"
#include "BWCommon.h"
BWConfigFile::BWConfigFile()
{

}
BWConfigFile::~BWConfigFile()
{
	Clear();
}
bool BWConfigFile::LoadFromFile(const std::string &fileName, const std::string& sperator /* = ":/* =" */)
{
	std::ifstream filestream;
	filestream.open(fileName.c_str(), std::ios_base::in|std::ios_base::binary );
	if (!filestream.is_open())
	{
		//抛出异常  配置文件出错
		return false;
	}
	BWDataStreamPrt dataStream = new BWFileStreamDataStream(fileName ,&filestream , false);
    return	LoadDirect(dataStream, sperator);
}
bool BWConfigFile::LoadDirect(const BWDataStreamPrt& dataStream, const std::string& sperator /* = ":/* =" */)
{
	Clear();
	BWDataStream* stream = dataStream.Get();
	std::string line;
	std::string section, key, value;
	Mutimap *multimap;
	section = DEFAULT_RESOURCE_GROUP;
	multimap = new Mutimap;
	sections.insert(std::make_pair(section, multimap));
	while (!stream->Eof())
	{
		line.clear();
		line = stream->GetLine();
		if (line.empty()|| line[0]=='#')
		{
			continue;
		}
		if (line[0] =='[')
		{
			section.clear();
			section = line.substr(line.find_first_of('[') + 1, line.find_first_of(']') - line.find_first_of('[') -1);
			multimap = new Mutimap;
			sections.insert(std::make_pair(section, multimap));
			continue;
		}
		int pos = line.find(sperator.c_str());
		key = line.substr(0, pos);
		value = line.substr(pos + 1, line.length() - pos - sperator.length());
		multimap = sections.find(section)->second;
		multimap->insert(std::make_pair(key, value));
	}
	return true;
}
BWConfigFile::Sections& BWConfigFile::GetSectionSet()
{
	return sections;
}
void BWConfigFile::Clear()
{
	Sections::iterator itor = sections.begin();
	while (itor != sections.end())
	{
		delete (itor->second);
		itor->second = NULL;
		++itor;
	}
	sections.clear();
}