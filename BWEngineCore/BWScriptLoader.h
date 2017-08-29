#ifndef SCRIPTLOADER_H_
#define SCRIPTLOADER_H_
#include "BWDataStream.h"
#include <string>
class BWScriptLoader
{

public:

	BWScriptLoader(){ }
	virtual ~BWScriptLoader(){ }
	virtual void ParseScript( BWDataStream& dataStream , const std::string &groupName) ;
    virtual const std::string & GetPattern();
	
	virtual int GetLoadingOrder(void)  const;

	std::string pattern; //该脚本分析器支持的脚本后缀
	int  order;
};


#endif