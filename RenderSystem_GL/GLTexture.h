#ifndef GLTEXTURE_H_
#define GLTEXTURE_H_
#include "../BWEngineCore/BWTexture.h"
#include "GL/include/glew.h"
class GLTexture : public BWTexture
{
public:
	typedef std::vector<Image> ImageVector;
	typedef SmartPointer<ImageVector> ImageVectorPtr;

	GLTexture(BWResourceManager* creator,const std::string &name, const std::string &gorupName);
	~GLTexture();
	virtual BWHardwarePixelBufferPtr getBuffer(size_t face = 0, size_t mipmap = 0) override;
	size_t calculateSize() { return 0; }
	GLenum GetGLTextureTarget();
	GLenum getTextureBufferAttachment() const;
	GLuint GetHIID(){ return mTextureID; }
	void Resize(int Width, int Height) override;
	void GenerateMipmapForTest(int MipmapNum) override;
	FORCEINLINE void SetHIID(GLuint NewHIID) { mTextureID = NewHIID; }
	virtual void attachToRenderTarget(BWRenderTarget* renderTarget ,int Index , int MipLevel = 0) override;
	static void do_image_io(std::string &name, std::string &groupName, std::string &ext, ImageVectorPtr &images, BWTexture *texture);
	void RemoveFromRenderTarget() override;
	// cube map 相关接口
	GLenum GetCubeMapTextureBufferAttachment(const std::string & InDir);
	std::map <std::string, GLenum> CubeMapTextureBufferAttachment;
protected:
	void _createSurfaceList();
	void createRenderTexture();
	void createInternalResourcesImpl(const void *Data) override;
	void freeInternalResourcesImpl();
	void ResizeInteranl(int Width, int Height , const void *Data = nullptr);
	void createInternalResourcesWithImageImpl(const ConstImagePtrList& images) override;

	void prepareImpl();
	void preLoadImpl(){ }
	void loadImpl();
	void postLoadImpl() { }
private:
	ImageVectorPtr mLoadedImage;
	GLuint mTextureID;
	GLenum mAttachment;
	typedef std::vector<BWHardwarePixelBufferPtr> SurfaceList;  
	SurfaceList mSurfaceList;
	int CurrentMipLevel;
};
class GLTexturePtr : public SmartPointer<GLTexture>
{
public:
	GLTexturePtr()
	{
		
	}
	GLTexturePtr(const BWTexturePtr& texture)
	{
		if (texture.IsNull() || texture.Get() == dynamic_cast<BWTexture*>(Get()))
		{
			return;
		}
		if (texture.GetCounterPointer())
		{
			counter = texture.GetCounterPointer();
			(*counter)++;
			mPointer = dynamic_cast<GLTexture*>(texture.Get());
		}
	}
	~GLTexturePtr(){ }

private:

};

#endif