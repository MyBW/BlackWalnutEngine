#ifndef GLTEXTUREMANAGER_H_
#define GLTEXTUREMANAGER_H_

#include "../BWEngineCore/BWTextureManager.h"
#include "GL/include/glew.h"
class GLTextureManager : public BWTextureManager
{
public:
	GLTextureManager();
	BWResourcePtr CreateImp(const std::string &name, const std::string &groupName);
	PixelFormat getNativeFormat(TextureType textureType, PixelFormat format, int usage);
	GLuint getWarningTexture();
protected:
	void createWarningTexture();
private:
	GLuint mWarningTexture;
};
#endif