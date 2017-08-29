#include"GLTextureManager.h"
#include "GLTexture.h"

GLTextureManager::GLTextureManager()
{
	createWarningTexture();
}
GLuint GLTextureManager::getWarningTexture()
{
	return mWarningTexture;
}
BWResourcePtr GLTextureManager::CreateImp(const std::string &name, const std::string &groupName)
{
	return new GLTexture(this, name, groupName);
}
PixelFormat GLTextureManager::getNativeFormat(TextureType textureType, PixelFormat format, int usage)
{
	 // assert(0);
	if (PixelUtil::isCompressed(format))
	{
		return PF_A8R8G8B8;
	}
	if (PixelUtil::isFloatingPoint(format))
	{
		return PF_A8R8G8B8;
	}
	if (usage & TU_RENDERTARGET)
	{
		assert(0);
	}
	return format;
}
void GLTextureManager::createWarningTexture()
{
	size_t width = 8;
	size_t height = 8;
	unsigned int *data = new unsigned int[width * height];
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			data[y*width + x] = (((x + y) % 8) < 4) ? 0x000000 : 0xFFFF00;
		}
	}
	glGenTextures(1, &mWarningTexture);
	glBindTexture(GL_TEXTURE_2D, mWarningTexture);
	if (GLEW_VERSION_1_2)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, (void*)data);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGBA, GL_UNSIGNED_INT, (void*)data);
	}
	delete [] data;
}