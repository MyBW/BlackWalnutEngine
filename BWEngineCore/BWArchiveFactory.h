#ifndef ARCHIVEFACTORY_H_
#define ARCHIVEFACTORY_H_

#include "BWArchive.h"
#include <list>
class BWArchiveFactory
{
public:
	BWArchiveFactory(const std::string &tp) :type(tp)
	{

	}
	virtual ~BWArchiveFactory(){ }
	virtual BWArchive* GetArchive()=0;
	const std::string & GetType(){ return type; }
private:
	std::string type;
};


#endif
