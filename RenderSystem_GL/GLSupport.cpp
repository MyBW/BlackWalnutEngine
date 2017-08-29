#include"GLSupport.h"
#include "../BWEngineCore/BWStringOperation.h"
bool GLSupport::checkExtension(const std::string &ext) const
{
   if (extensionList.find(ext) == extensionList.end())
   {
	   return false;
   }
   return true;
}

bool GLSupport::supportsPBuffers()
{
	return (GLEW_ARB_pixel_buffer_object || GLEW_EXT_pixel_buffer_object) != GL_FALSE;
}
GLPBuffer* GLSupport::createPBuffer(PixelComponentType format, size_t width, size_t height) 
{
	return 0;
}
void  GLSupport::initialiseExtension()
{
	  const GLubyte* pcVer = glGetString(GL_VERSION);
	std::string tmpStr = (const char*)pcVer;
	mVersion = tmpStr.substr(0, tmpStr.find(" "));
	const GLubyte* pcVendor = glGetString(GL_VENDOR);
	tmpStr = (const char*)pcVendor;
	mVendor = tmpStr.substr(0, tmpStr.find(" "));

	const GLubyte* render = glGetString(GL_RENDERER);

	StringStream ext;
	std::string str;
	const GLubyte* pcExt = glGetString(GL_EXTENSIONS);

	ext << pcExt;
	while (ext >> str)
	{
		extensionList.insert(str);
	}
}
void GLSupport::setConfigOption(const std::string &name, const std::string &value)
{
	ConfigOptionMap::iterator itor = mOptions.find(name);
	if (itor != mOptions.end())
	{
		itor->second.currentValue = name;
	}
}
ConfigOptionMap& GLSupport::getConfigOptions()
{
	return mOptions;
}


