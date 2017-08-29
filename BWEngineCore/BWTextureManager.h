#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_
#include "BWResourceManager.h"
#include "BWSingleton.h"
#include "BWTexture.h"
class BWTextureManager : public BWResourceManager, public BWSingleton<BWTextureManager>
{
public:
	static BWTextureManager* GetInstance();
	BWTextureManager();
	virtual ~BWTextureManager();
	BWTexturePtr load(const std::string& name, const std::string& group, TextureUsage usage = TU_DYNAMIC, TextureType texType = TEX_TYPE_2D, int numMipmaps = MIP_DEFAULT, float gamma = 1.0f, bool isAlpha = false, PixelFormat desiredFormat = PF_UNKNOWN, bool hwGammaCorrection = false);
	ResourceCreateOrRetrieveResult CreateOrRetrieve(const std::string &name, const std::string &groupName, const NameValuePairList *createParams, TextureType texType,
		int numMipmap, float gama, bool isAlpha, PixelFormat desiredFormat, bool hwGama , TextureUsage usage);
	virtual PixelFormat getNativeFormat(TextureType textureType, PixelFormat format, int usage) = 0;	
private:
};



#endif