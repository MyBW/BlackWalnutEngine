#ifndef STRINGCONVERTER_H_
#define STRINGCONVERTER_H_
#include <string>
class StringConverter
{
public:
	static bool  IsNumber(const std::string &src );
	static int   ParseInt(const std::string &src, int defaultValue = 0);
	static float ParseReal(const std::string &src, float defaultValue = 0.0f);
	static bool  ParseBool(const std::string &src, bool defaultValue = false);
	static unsigned int ParseUnsignedInt(const std::string &src, unsigned int defaultValue = 0);
	static unsigned long ParseUnsignedLong(const std::string &src, unsigned int defaultValue = 0);
	static std::string ToString(bool b);
	static std::string ToString(int i);
	static std::string ToString(size_t i);
	static std::string ToString(float i);

protected:
private:
};

#endif