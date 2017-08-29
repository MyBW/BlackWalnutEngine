#include "BWStringConverter.h"
#include "BWStringOperation.h"
int StringConverter::ParseInt(const std::string &src , int defaultValue)
{
	StringStream str(src);
	int ret = defaultValue;
	str >> ret;
	return ret;
}
bool StringConverter::IsNumber(const std::string &src)
{
	StringStream str(src);
	float test;
	str >> test;
	return !str.fail() && str.eof();
}

float StringConverter::ParseReal(const std::string &src, float defaultValue /* = 0.0f */)
{
	StringStream str(src);
	float ret = defaultValue;
	str >> ret;
	return ret;
}
bool StringConverter::ParseBool(const std::string &src, bool defaultValue /* = false */)
{
	StringStream str(src);
	bool ret = defaultValue;
	str >> ret;
	return ret;
}
unsigned int StringConverter::ParseUnsignedInt(const std::string &src, unsigned int defaultValue /* = 0 */)
{
	StringStream str(src);
	unsigned int ret = defaultValue;
	str >> ret;
	return ret;
}
unsigned long StringConverter::ParseUnsignedLong(const std::string &src, unsigned int defaultValue /* = 0 */)
{
	StringStream str(src);
	unsigned long ret = defaultValue;
	str >> ret;
	return  ret;
}
std::string StringConverter::ToString(int i)
{
	StringStream str;
	str << i;
	return str.str();
}
std::string StringConverter::ToString(bool b)
{
	if (b)
	{
		return "true";
	}
	return "false";
}
std::string StringConverter::ToString(size_t i)
{
	return ToString((int)i);
}
std::string StringConverter::ToString(float i)
{
	StringStream str;
	str << i;
	return str.str();
}