#ifndef STRINGINTERFACE_H_
#define STRINGINTERFACE_H_

#include <string>
#include <map>
class ParamCommand
{
public:
 	 virtual  std::string DoGet(const void *target) const  = 0;
	 virtual void DoSet(void *target, const std::string& value) const  = 0;
	 virtual ~ParamCommand();
};
typedef std::map<std::string, ParamCommand*> ParamCommandMap;
typedef std::map<std::string, ParamCommandMap*> StringInterfaceMap;
class BWStringInterface
{
private:
	static StringInterfaceMap stringInterfaceMap;
public:
	void   SetParameter(const std::string &name, const std::string &value);
protected:
	void   RegisterCommand(ParamCommand *cmd);
	ParamCommand* GetCommand(const std::string &name);
	 bool  CreateStringInterface(const std::string &name);
	 ParamCommandMap* GetStringInterface();
private:
	ParamCommandMap* myParamCommandMap;
};

#endif