#include "BWTextureManager.h"
template<> BWTextureManager *  BWSingleton<BWTextureManager>::instance = NULL;
BWTextureManager* BWTextureManager::GetInstance()
{
	assert(instance);
	return instance;
}
BWTextureManager::BWTextureManager()
{
	resourceType = "texture";
}
BWTextureManager::~BWTextureManager()
{

}
BWTexturePtr BWTextureManager::load(const std::string& name, const std::string& group, TextureUsage usage, TextureType texType /* = TEX_TYPE_2D */, int numMipmaps /* = MIP_DEFAULT */, float gamma /* = 1.0f */, bool isAlpha /* = false */, PixelFormat desiredFormat /* = PF_UNKNOWN */, bool hwGammaCorrection /* = false */)
{
	ResourceCreateOrRetrieveResult res = CreateOrRetrieve(name, group, NULL, texType, numMipmaps, gamma, isAlpha, desiredFormat, hwGammaCorrection, usage);
	if (res.second)
	{
		res.first->Load();
	}
	return res.first;
}


BWResourceManager::ResourceCreateOrRetrieveResult BWTextureManager::CreateOrRetrieve(const std::string &name, const std::string &groupName, const NameValuePairList *createParams, 
	TextureType texType, int numMipmap, float gama, bool isAlpha, PixelFormat desiredFormat, bool hwGama , TextureUsage usage)
{
	ResourceCreateOrRetrieveResult res = BWResourceManager::CreateOrRetrieve(name, groupName, createParams);
	if (res.second)
	{
		BWTexturePtr texturePtr = res.first;
		texturePtr->setTextureType(texType);
		texturePtr->setNumMipmaps(numMipmap);
		texturePtr->setGamma(gama);
		texturePtr->setTreatLuminanceAsAlpha(isAlpha);
		texturePtr->setFormat(desiredFormat);
		texturePtr->setHardwareGammaEnabled(hwGama);
		texturePtr->setUsage(usage);
	}
	return res;
}