#ifndef CONFIGOPTIONMAP_H_
#define CONFIGOPTIONMAP_H_
#include <string>
#include <vector>
#include <map>
typedef std::vector<std::string> StringVector;
typedef struct _ConfigOption
{
	std::string name;
	std::string currentValue;
	StringVector possibleValues;
	bool immutable;
} ConfigOption;

typedef std::map<std::string, ConfigOption> ConfigOptionMap;
#endif