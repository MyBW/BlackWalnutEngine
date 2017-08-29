#ifndef GLSUPPORT_H_
#define GLSUPPORT_H_
#include <string>
#include <set>
#include "../BWEngineCore/BWConfigOptionMap.h"
#include "../BWEngineCore/BWPixlFormat.h"
#include "../BWEngineCore/BWRenderWindow.h"
#include "GL/include/glew.h"
class GLRenderSystem;
class GLPBuffer;
class GLSupport
{
public:
	GLSupport(){ }
	virtual ~GLSupport(){ }
	virtual void addConfig() = 0;
	virtual void setConfigOption(const std::string &name, const std::string &value);
	virtual std::string validateConfig() = 0;
	virtual ConfigOptionMap& getConfigOptions();
	virtual BWRenderWindow* createWindow(bool autoCreateWindow, GLRenderSystem *system, const std::string &windowTitle) = 0;
	virtual BWRenderWindow* newWindow(const std::string &name, unsigned int width, unsigned int height,
		bool fullScreen, const NameValuePairList *miscParams = 0) = 0;
	virtual bool supportsPBuffers();
	virtual GLPBuffer* createPBuffer(PixelComponentType format, size_t width, size_t  height);
	virtual void start() = 0;
	virtual void stop() = 0;
	const std::string & getGlVendor() const
	{
		return mVendor;
	}
	const std::string& getGLVersioin() const
	{
		return mVersion;
	}
	bool checkMinGLVersion(const std::string &v) const;
	virtual bool checkExtension(const std::string &ext) const;
	virtual void* getProcAddress(const std::string &procname) = 0;
	virtual void initialiseExtension();
	virtual unsigned int getDisplayMonitorCount() const
	{
		return  1;
	}
protected:
	ConfigOptionMap mOptions;
	std::set<std::string> extensionList;
private:
	std::string mVersion;
	std::string mVendor;
};

#endif