#ifndef CONFIGFILE_H_
#define CONFIGFILE_H_

#include<map>
#include "BWDataStream.h"
class BWConfigFile
{
public:
	typedef std::multimap<std::string, std::string> Mutimap;
	typedef std::map<std::string, Mutimap*> Sections;
	BWConfigFile();
	~BWConfigFile();
	bool LoadFromFile(const std::string &fileName,const std::string &sperator = ":=");
	bool LoadDirect(const BWDataStreamPrt &dataStream, const std::string &sperator = ":=");
	void Clear();
	Sections& GetSectionSet();
private:
	Sections sections;	
};

#endif
