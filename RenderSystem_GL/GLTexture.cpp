#include "GLTexture.h"
#include "../BWEngineCore/BWResourceGroupMananger.h"
#include "../BWEngineCore/BWRenderSystem.h"
#include "../BWEngineCore/BWHardwarePixelBuffer.h"
#include "GLTextureManager.h"
#include "GLPixelUtil.h"
#include "GLHardwarePixelBuffer.h"
#include "GL/include/GLAUX.H"
#include "GLRenderTarget.h"
#include "../BWEngineCore/BWLog.h"
#include "GLPreDefine.h"
GLTexture::GLTexture(BWResourceManager* creator, const std::string &name, const std::string &gorupName) 
	:BWTexture(creator, name, gorupName)
	,mTextureID(0)
{
	CubeMapTextureBufferAttachment.clear();
	CubeMapTextureBufferAttachment.insert(std::pair<std::string, GLenum>(std::string("+X"), 0));
	CubeMapTextureBufferAttachment.insert(std::pair<std::string, GLenum>(std::string("-X"), 0));
	CubeMapTextureBufferAttachment.insert(std::pair<std::string, GLenum>(std::string("+Y"), 0));
	CubeMapTextureBufferAttachment.insert(std::pair<std::string, GLenum>(std::string("-Y"), 0));
	CubeMapTextureBufferAttachment.insert(std::pair<std::string, GLenum>(std::string("+Z"), 0));
	CubeMapTextureBufferAttachment.insert(std::pair<std::string, GLenum>(std::string("-Z"), 0));
}

GLTexture::~GLTexture()
{
	glDeleteTextures(1, &mTextureID);
}

BWHardwarePixelBufferPtr GLTexture::getBuffer(size_t face /* = 0 */, size_t mipmap /* = 0 */)
{
	if (face >= getNumFaces())
	{
		assert(0);
	}
	if (mipmap > mNumMipmaps)
	{
		assert(0);
	}
	unsigned int idx = face *(mNumMipmaps + 1) + mipmap;
	assert(idx < mSurfaceList.size());
	return mSurfaceList[idx];
}

void GLTexture::freeInternalResourcesImpl()
{

}

void GLTexture::ResizeInteranl(int Width, int Height ,const void *Data)
{
	mWidth = Width; mHeight = Height;
	size_t maxMipmaps = GLPixelUtil::getMaxMipmaps(mWidth, mHeight, mDepth);
	mNumMipmaps = mNumRequestedMipmaps;
	if (mNumMipmaps > maxMipmaps)
	{
		mNumMipmaps = maxMipmaps;
	}
	if (mNumMipmaps == 0)
	{
		mNumMipmaps = 1;
	}
	if (mTextureType == TEX_TYPE_2D)
	{
		GLenum InternalFormat, Format;
		switch (mFormat)
		{
		case PF_B8G8R8A8:    InternalFormat = GL_RGBA8; Format = GL_RGBA; break;
		case PF_FLOAT32_RGB: InternalFormat = GL_RGB32F; Format = GL_RGB; break;
		case PF_FLOAT32_R:   InternalFormat = GL_R32F; Format = GL_R; break;
		default:
			InternalFormat = GL_RGBA32F; Format = GL_RGBA; break;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, mWidth, mHeight, 0, Format, GL_FLOAT, Data);
	}
	if (mTextureType == TEX_TYPE_CUBE_MAP)
	{
		// 这里的cubemap都是为HDR做准备的
		for (unsigned int i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, mWidth, mHeight, 0, GL_RGB, GL_FLOAT, Data);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	CHECK_GL_ERROR(glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_BASE_LEVEL, 0));
	CHECK_GL_ERROR(glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_MAX_LEVEL, mNumMipmaps - 1));//从0开始计数//开启这一项后会导致绑定到framebuffer上出现问题
	glGenerateMipmap(GetGLTextureTarget());
}

void GLTexture::do_image_io(std::string &name, std::string &groupName, std::string &ext, ImageVectorPtr &images, BWTexture *texture)
{
	size_t imageIndex = images->size();
	images->push_back(Image());
	BWDataStreamPrt dataStream = BWResourceGroupManager::GetInstance()->OpenResource(name, groupName);
	(*images.Get())[imageIndex].load(dataStream, ext);
	
}

void GLTexture::RemoveFromRenderTarget()
{
	 if (mRenderTarget)
	 {
		 GLRenderTarget* RenderTarget = dynamic_cast<GLRenderTarget*>(mRenderTarget);
		 GLenum Attachment = getTextureBufferAttachment();
		 GLint CurrentFrameBuffer;
		 glGetIntegerv(GL_FRAMEBUFFER_BINDING, &CurrentFrameBuffer);
		 glBindFramebuffer(GL_FRAMEBUFFER, RenderTarget->getID());
		 if (TEX_TYPE_2D == mTextureType)
		 {
			 CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, Attachment,
				 GL_TEXTURE_2D, 0, 0));
			 RenderTarget->CollectAttachment(Attachment);
		 }
		 if (TEX_TYPE_CUBE_MAP == mTextureType)
		 {
			 std::vector<std::string> Dir{ "+X" , "-X", "+Y", "-Y" , "+Z" , "-Z" };
			 for (int Index = 0 ; Index < 6 ; Index++)
			 {
				 GLenum& Attachment = CubeMapTextureBufferAttachment[Dir[Index]];
				 if (Attachment)
				 {
					 CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, Attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + Index, 0, 0));
					 RenderTarget->CollectAttachment(Attachment);
					 Attachment = 0;
				 }
			 }
		 }
		 glBindFramebuffer(GL_FRAMEBUFFER, CurrentFrameBuffer);
		 mRenderTarget = NULL;
	 }
}

GLenum GLTexture::GetCubeMapTextureBufferAttachment(const std::string & InDir)
{
	if (CubeMapTextureBufferAttachment.find(InDir) != CubeMapTextureBufferAttachment.end())
	{
		return CubeMapTextureBufferAttachment.find(InDir)->second;
	}
	return 0;
}

void GLTexture::prepareImpl()
{
	if (mUsage & TU_RENDERTARGET)
	{
		return;
	}
	std::string baseName , ext;
	size_t pos = name.find_last_of(".");
	if (pos != std::string::npos)
	{
		baseName = name.substr(0, pos);
		ext = name.substr(pos + 1, name.length() - pos);
	}
	ImageVectorPtr images(new ImageVector);
	if (mTextureType == TEX_TYPE_2D || mTextureType == TEX_TYPE_1D 
		|| mTextureType == TEX_TYPE_3D)
	{
		do_image_io(name, groupName, ext, images, this);
		if ((*images.Get())[0].hasFlag(IF_CUBEMAP))
		{
			mTextureType = TEX_TYPE_CUBE_MAP;
		}
		if ((*images.Get())[0].getDepth() > 1)
		{
			mTextureType = TEX_TYPE_3D;
		}
	}
	else if (mTextureType == TEX_TYPE_CUBE_MAP)
	{
		if (getSourceFileType() == "dds")
		{
			do_image_io(name, groupName, ext, images, this);
		}
		else
		{
			static std::string subfix[6] = { "_rt", "_lf", "_up", "_dn", "_fr", "_bk" };
			for (int i = 0; i < 6; i++)
			{
				std::string fullName = baseName + subfix[i] + ".";
				fullName += ext;
				do_image_io(fullName, groupName, ext, images, this);
			}
		}
	}
	else
	{
		assert(0);
	}
	mLoadedImage = images;
}
void  GLTexture::loadImpl()
{
	if (mUsage & TU_RENDERTARGET)
	{
		createRenderTexture();
		return;
	}
	ImageVectorPtr images = mLoadedImage;
	mLoadedImage.SetNull();
	 ConstImagePtrList constImagePtrList;
	 for (int i = 0; i < images->size(); i++)
	 {
		 Image *tmp = &(*images.Get())[i];
		 constImagePtrList.push_back(tmp);
	 }
	 _loadImages(constImagePtrList); // 从imagelist 真正的载入到GPU
}
void GLTexture::attachToRenderTarget(BWRenderTarget* renderTarget, int Index, int MipLevel)
{
	if (!mInternalResourcesCreated) createInternalResources();
	 if (!mTextureID)
	 {
		 //这个mTextureID的值最好能用系统的最大纹理数量来代替检测
		 assert(0);
	 }
	 int CurrentBindFrameBuffer;
	 glGetIntegerv(GL_FRAMEBUFFER_BINDING, &CurrentBindFrameBuffer);
	 if (mRenderTarget && mRenderTarget != renderTarget)
	 {
		 mRenderTarget->removeTextureBuffer(name);
	 }
	 mRenderTarget = renderTarget;
	 GLRenderTarget *frameBuffer = dynamic_cast<GLRenderTarget*>(renderTarget);
	 CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->getID()));
	 if (mTextureType == TEX_TYPE_2D)
	 {
		 
		 GLint CurrentBindTextureID;
		 glGetIntegerv(GL_TEXTURE_BINDING_2D, &CurrentBindTextureID);

		 //// 不这样处理 2D纹理都不对
		 glBindTexture(GL_TEXTURE_2D, mTextureID);
		 CHECK_GL_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, NULL));
		 mAttachment = frameBuffer->getColorAttachment();
		 CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, mAttachment, GL_TEXTURE_2D, mTextureID, 0));  //暂时使用2D纹理

		 glBindTexture(GL_TEXTURE_2D, CurrentBindTextureID);
	 }
	 if (mTextureType == TEX_TYPE_CUBE_MAP)
	 {
		 std::vector<std::string> Dir{ "+X" , "-X", "+Y", "-Y" , "+Z" , "-Z" };
		 GLenum Attachment = frameBuffer->getColorAttachment();
		 CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, Attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + Index, mTextureID, MipLevel));
		 CubeMapTextureBufferAttachment[Dir[Index]] = Attachment;
		 CurrentMipLevel = MipLevel;
	 }
	 GLenum statues = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	 if (statues != GL_FRAMEBUFFER_COMPLETE)//某个绑定到attachment上的元素无效
	 {
		 Log::GetInstance()->logMessage("GLTexture::attachToRenderTarget() : Error");
	 }
	 glBindFramebuffer(GL_FRAMEBUFFER, CurrentBindFrameBuffer);
}
GLenum GLTexture::GetGLTextureTarget()
{
	//纹理数组怎么办
	switch (mTextureType)
	{
	case TEX_TYPE_1D:
		return GL_TEXTURE_1D;
	case TEX_TYPE_2D:
		return GL_TEXTURE_2D;
	case TEX_TYPE_3D:
		return GL_TEXTURE_3D;
	case TEX_TYPE_CUBE_MAP:
		return GL_TEXTURE_CUBE_MAP;
	default:
		return 0;
	}
}
GLenum GLTexture::getTextureBufferAttachment() const
{
	return mAttachment;
}
void GLTexture::Resize(int Width, int Height)
{
	if (!glIsTexture(mTextureID)) return;
	if (Width == mWidth && Height == mHeight) return;
	GLint CurrentBindTextureID;
	glGetIntegerv(GetGLTextureTarget(), &CurrentBindTextureID);
	glBindTexture(GetGLTextureTarget(), mTextureID);
	ResizeInteranl(Width, Height);
	glBindTexture(GetGLTextureTarget(), CurrentBindTextureID);
}

void GLTexture::GenerateMipmapForTest(int MipmapNum)
{
	if (MipmapNum != mNumMipmaps)
	{
		mNumMipmaps = MipmapNum;
		GLint CurrentBindTextureID;
		glGetIntegerv(GetGLTextureTarget(), &CurrentBindTextureID);
		glBindTexture(GetGLTextureTarget(), mTextureID);
		size_t maxMipmaps = GLPixelUtil::getMaxMipmaps(mWidth, mHeight, mDepth);
		if (mNumMipmaps > maxMipmaps)
		{
			mNumMipmaps = maxMipmaps;
		}
		if (mNumMipmaps == 0)
		{
			mNumMipmaps = 1;
		}
		CHECK_GL_ERROR(glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_BASE_LEVEL, 0));
		CHECK_GL_ERROR(glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_MAX_LEVEL, mNumMipmaps - 1));//从0开始计数//开启这一项后会导致绑定到framebuffer上出现问题
		glGenerateMipmap(GetGLTextureTarget());
		glBindTexture(GetGLTextureTarget(), CurrentBindTextureID);
	}
}

void GLTexture::createInternalResourcesWithImageImpl(const ConstImagePtrList& images)
{
	if (mTextureType == TEX_TYPE_2D)
	{
		mFormat = BWTextureManager::GetInstance()->getNativeFormat(mTextureType, mFormat, mUsage);
		size_t maxMipmaps = GLPixelUtil::getMaxMipmaps(mWidth, mHeight, mDepth);
		mNumMipmaps = mNumRequestedMipmaps;
		if (mNumMipmaps > maxMipmaps)
		{
			mNumMipmaps = maxMipmaps;
		}
		glGenTextures(1, &mTextureID);
		glBindTexture(GetGLTextureTarget(), mTextureID);

		//  这里使用glTextureParamter的 是为了提前准备各种mipmap等 而不是纹理针对某一个纹理单元
		if (GLEW_VERSION_1_2)
		{
			//glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_MAX_LEVEL, mNumMipmaps);
		}

		glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (GLEW_VERSION_1_2)
		{
			glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		// 发现使用GLPixelUtil得到的数据不正确
		GLenum InternalFormat = GLPixelUtil::getGLInternalFormat(mSrcFormat);
		GLenum OriginFormat = GLPixelUtil::getGLOriginFormat(mSrcFormat);
		GLenum OriginDataType = GLPixelUtil::getGLOriginDataType(mSrcFormat);

		if (mSrcFormat == PF_L8)
		{
			CHECK_GL_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, images[0]->getWidth(), images[0]->getHeight(),
				0, OriginFormat, OriginDataType, images[0]->getData()));
			return;
		}
		if (mSrcFormat == PF_R8G8B8)
		{
			if (isHardwareGammaEnabled())
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, images[0]->getWidth(), images[0]->getHeight(),
					0, GL_RGB, GL_UNSIGNED_BYTE, images[0]->getData());
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, images[0]->getWidth(), images[0]->getHeight(),
					0, GL_RGB, GL_UNSIGNED_BYTE, images[0]->getData());
			}
			return;
		}
		if (mSrcFormat == PF_FLOAT32_RGB)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, images[0]->getWidth(), images[0]->getHeight(),
				0, GL_RGB, GL_FLOAT, images[0]->getData());
			return;
		}
	/*	GLenum InternalFormat = GLPixelUtil::getGLInternalFormat(mSrcFormat);
		GLenum OriginFormat = GLPixelUtil::getGLOriginFormat(mSrcFormat);
		GLenum OriginDataType = GLPixelUtil::getGLOriginDataType(mSrcFormat);
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, images[0]->getWidth(), images[0]->getHeight(),
			0, OriginFormat, OriginDataType, images[0]->getData());*/

		/*if (mSrcFormat == PF_FLOAT32_RGB)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, images[0]->getWidth(), images[0]->getHeight(),
				0, GL_RGB, GL_FLOAT, images[0]->getData());
		}
		if (mSrcFormat == PF_R8G8B8)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, images[0]->getWidth(), images[0]->getHeight(),
				0, GL_RGB, GL_UNSIGNED_BYTE, images[0]->getData());
		}
		else if (mSrcFormat == PF_L8)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, images[0]->getWidth(), images[0]->getHeight(),
				0, GL_RED, GL_UNSIGNED_BYTE, images[0]->getData());
		}*/
		//简化纹理载入流程 暂时这样处理
		
	}
	if (mTextureType == TEX_TYPE_CUBE_MAP)
	{
		mFormat = BWTextureManager::GetInstance()->getNativeFormat(mTextureType, mFormat, mUsage);
		size_t maxMipmaps = GLPixelUtil::getMaxMipmaps(mWidth, mHeight, mDepth);
		mNumMipmaps = mNumRequestedMipmaps;
		if (mNumMipmaps > maxMipmaps)
		{
			mNumMipmaps = maxMipmaps;
		}

		 GLenum types[6] = 
		 { GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 
		 };
		glGenTextures(1, &mTextureID);
		glBindTexture(GetGLTextureTarget(), mTextureID);
		GLenum InternalFormat;
		isHardwareGammaEnabled() ? InternalFormat = GL_SRGB : InternalFormat = GL_RGB;
		for (int i = 0; i < 6; i++)
		{
			glTexImage2D(types[i], 0, InternalFormat, images[i]->getWidth(), images[i]->getHeight(),
				0, GL_RGB, GL_UNSIGNED_BYTE, images[i]->getData());
			glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
	}
}
void GLTexture::createInternalResourcesImpl(const void *Data)
{
	if (!GLEW_VERSION_1_2 && mTextureType == TEX_TYPE_3D)
	{
		//1.2版本之前不支持3D纹理
		assert(0);
	}
	//这里主要处理的是 将纹理的大小 位深 从ogre的定义转换到合适的GL定义  然后判断是否能生成mipmap 然后判断是否生成压缩纹
	//将ogre数据转换为GL数据  本来要求是2的次方 但是 目前没有要求了
	//mWidth = GLPixelUtil::optionalPO2(mWidth);
	//mHeight = GLPixelUtil::optionalPO2(mHeight);
	//mDepth = GLPixelUtil::optionalPO2(mDepth);
	//mFormat = BWTextureManager::GetInstance()->getNativeFormat(mTextureType, mFormat, mUsage);

	if (glIsTexture(mTextureID)) glDeleteTextures(1, &mTextureID);
	GLint CurrentBindTextureID;
	glGetIntegerv(GetGLTextureTarget(), &CurrentBindTextureID);
	glGenTextures(1, &mTextureID);
	glBindTexture(GetGLTextureTarget(), mTextureID);
	ResizeInteranl(mWidth, mHeight, Data);
	glBindTexture(GetGLTextureTarget(), CurrentBindTextureID);
	//简化纹理载入流程 暂时这样处理
	return;
	mMipmapsHardwareGenerated = true;//BWRenderSystem::GetInstance()->hasCapability();
	// 为了调试 暂时都不用硬件产生mipmap 
	mMipmapsHardwareGenerated = false;
	if ((mUsage & TU_AUTOMIPMAP) &&
		mNumRequestedMipmaps && mMipmapsHardwareGenerated)
	{
		glTexParameteri(GetGLTextureTarget(), GL_GENERATE_MIPMAP, GL_TRUE);
	}
	GLenum format = GLPixelUtil::getClosestGLInternalFormat(mFormat, mHwGamma);
	size_t width = mWidth;
	size_t height = mHeight;
	size_t depth = mDepth;
	if (PixelUtil::isCompressed(mFormat))
	{
		size_t size = PixelUtil::getMemorySize(width, height, depth, mFormat);
		unsigned char *tmpData = new unsigned char[size];
		memset(tmpData, 0, size);
		for (int i = 0; i <= mNumMipmaps; i++)
		{
			size = PixelUtil::getMemorySize(width, height, depth, mFormat);
			switch (mTextureType)
			{
			case TEX_TYPE_1D:
				glCompressedTexImage1D(GL_TEXTURE_1D, i, format, width, 0, size, tmpData);
				break;
			case TEX_TYPE_2D:
				glCompressedTexImage2D(GL_TEXTURE_2D, i, format, width, height, 0, size, tmpData);
				break;
			case TEX_TYPE_3D:
				glCompressedTextureImage3DEXT(mTextureID ,GL_TEXTURE_3D, i, format, width , height ,  depth, 0, size , tmpData);
				break;
			case TEX_TYPE_CUBE_MAP:

				for (int face = 0; face < 6; face++)
				{
					glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, i, format, width, height, 0, size, tmpData);
				}
				break;
			}
			if (width > 1)
			{
				width /= 2;
			}
			if (height > 1)
			{
				height /= 2;
			}
			if (depth > 1)
			{
				depth /= 2;
			}
		}
		delete[] tmpData;
	}
	else
	{
		for (int mip = 0; mip <= mNumMipmaps; mip++)
		{
			switch (mTextureType)
			{
			case TEX_TYPE_1D:
				glTexImage1D(GL_TEXTURE_1D, mip, format, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			case TEX_TYPE_2D:
				glTexImage2D(GL_TEXTURE_2D, mip, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				break;
			case TEX_TYPE_3D:
				glTexImage3D(GL_TEXTURE_3D, mip, format, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			case TEX_TYPE_CUBE_MAP:
				for (int face = 0; face < 6; face++)
				{
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				}
				break;
			}
			if (width > 1 )
			{
				width /= 2;
			}
			if (height > 1)
			{
				height /= 2;
			}
			if (depth > 1)
			{
				depth /= 2;
			}
		}
	}
	_createSurfaceList();
	mFormat = getBuffer(0, 0)->getFormat();
}
void GLTexture::_createSurfaceList()
{
	/*mSurfaceList.clear();
	bool wantGenerateMips = (mUsage & TU_AUTOMIPMAP) != 0;
	bool dosoftware = wantGenerateMips && !mMipmapsHardwareGenerated && getNumMipmaps();
	for (size_t face = 0; face < getNumFaces(); face++)
	{
		for (size_t mip = 0; mip <= getNumMipmaps(); mip++)
		{
			GLHardwarePixelBuffer *buf = new GLTextureBuffer(name, GetGLTextureTarget(), mTextureID, face, mip,
				static_cast<BWHardwareBuffer::Usage>(mUsage), dosoftware && mip == 0, mHwGamma, mFSAA);
			mSurfaceList.push_back(buf);
			if (buf->getWidth() == 0 || buf->getHeight() == 0|| buf->getWidth() == 0)
			{
				assert(0);
			}
		}
	}*/
}

void GLTexture::createRenderTexture()
{
	glGenTextures(1, &mTextureID);
	glBindTexture(GetGLTextureTarget(), mTextureID);

	//  这里使用glTextureParamter的 是为了提前准备各种mipmap等 而不是纹理针对某一个纹理单元
	if (GLEW_VERSION_1_2)
	{
		//glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_MAX_LEVEL, mNumMipmaps);
	}

	glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (GLEW_VERSION_1_2)
	{
		glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GetGLTextureTarget(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
}