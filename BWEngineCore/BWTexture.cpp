#include "BWTexture.h"
#include "BWMemoryDataStream.h"
#include "BWResourceGroupMananger.h"

// 纯粹是为了测试纹理 
#include "../RenderSystem_GL/GLTexture.h"
#include "../RenderSystem_GL/GL/include/GLAUX.H"

BWTexture::BWTexture(BWResourceManager* creator, const std::string &name, const std::string &groupName):
BWResource(creator , name , groupName)
, mHeight(512)
, mWidth(512)
, mDepth(1)
, mNumRequestedMipmaps(0)
, mNumMipmaps(0)
, mMipmapsHardwareGenerated(false)
, mGamma(1.0)
, mHwGamma(false)
, mTextureType(TEX_TYPE_2D)
, mFormat(PF_UNKNOWN)
, mUsage(TU_DEFAULT)
, mSrcFormat(PF_UNKNOWN)
, mSrcWidth(0)
, mSrcHeight(0)
, mSrcDepth(0)
, mDesiredFormat(PF_UNKNOWN)
, mDesiredIntegerBitDepth(0)
, mDesiredFloatBitDepth(0)
, mTreatLuminanceAsAlpha(false)
, mInternalResourcesCreated(false)
,mRenderTarget(NULL)
{

}

bool BWTexture::getTreatLuminanceAsAlpha() const 
{
	return mTreatLuminanceAsAlpha;
}

bool BWTexture::hasAlpha() const
{
	return mTreatLuminanceAsAlpha;
}

size_t BWTexture::getNumFaces() const
{
	return GetTextureType() == TEX_TYPE_CUBE_MAP ? 6 : 1;
}
unsigned short BWTexture::getDesiredFloatBitDepth() const 
{
	return mDesiredFloatBitDepth;
}

unsigned short BWTexture::getDesiredIntegerBitDepth() const
{
	return mDesiredIntegerBitDepth;
}

void BWTexture::_loadImages(const ConstImagePtrList& images)
{
	if (images.size() < 1)
	{
		assert(0);
	}
	//设置纹理的长宽高 位深 每个像素的格式  以及产生Mipmap的方式  自动 还是手动
	mSrcWidth = mWidth = images[0]->getWidth();
	mSrcHeight = mHeight = images[0]->getHeight();
	mSrcDepth = mDepth = images[0]->getDepth();
	mSrcFormat = images[0]->getFormat();
	
	if (mTreatLuminanceAsAlpha && mSrcFormat == PF_L8)
	{
		mSrcFormat = PF_A8;
	}
	if (mDesiredFormat != PF_UNKNOWN)
	{
		mFormat = mDesiredFormat;
	}
	else
	{
		mFormat = PixelUtil::getFormatForBitDepths(mSrcFormat, mDesiredIntegerBitDepth, mDesiredFloatBitDepth);
	}
	size_t imageMipNum = images[0]->getNumMipmaps();
	if (imageMipNum > 0)
	{
		mNumMipmaps = mNumRequestedMipmaps = images[0]->getNumMipmaps();
		mUsage &= ~TU_AUTOMIPMAP;
	}

	createInternalResourcesWithImage(images);// 创建GPU资源  以下为将资源载传入GPU
	return;
	//createInternalResources();  
	// 测试纹理代码
/*	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	AUX_RGBImageRec *textureImage[1];
	memset(textureImage, 0, sizeof(void*)* 1);
	textureImage[0] = auxDIBImageLoad("image/Tex.bmp");
	if (textureImage[0])
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, textureImage[0]->sizeX, textureImage[0]->sizeY,
			0, GL_RGB, GL_UNSIGNED_BYTE, textureImage[0]->data);

		free(textureImage[0]->data);
		free(textureImage[0]);
	}*/
	// 这里仅仅是为了简化纹理载入流程 所以先这么做了  以后要改
	/*glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, images[0]->getWidth(), images[0]->getHeight(),
		0, GL_BGR, GL_UNSIGNED_BYTE, images[0]->getData());
	return;
*/
	size_t faces;
	bool multiImage = false;
	if (images.size() > 1)
	{
		faces = images.size();
		multiImage = true;
	}
	else
	{
		faces = images[0]->getNumFaces();
	}
	if (faces > getNumFaces())
	{
		faces = getNumFaces();
	}
	for (size_t mip = 0; mip <= imageMipNum; mip++)
	{
		for (size_t i = 0; i < faces; i++)
		{
			BWPixelBox src;
			if (multiImage)
			{
				src = images[i]->getPixelBox(0, mip);
			}
			else
			{
				src = images[0]->getPixelBox(0, mip);
			}
			src.mPixelFormat = images[0]->getFormat();
			if (mGamma != 1.0)
			{
				MemoryDataStreamPtr buf;
				buf.bind(new MemoryDataStream(PixelUtil::getMemorySize(src.getWidth() , src.getHeight(),src.getDepth(),src.mPixelFormat)));
				BWPixelBox currentPixelBox = BWPixelBox(src.getWidth(), src.getHeight(), src.getDepth(), src.mPixelFormat, buf->getPtr());
				PixelUtil::bulkPixelConversion(src, currentPixelBox);
				Image::applyGamma(static_cast<unsigned char*>(currentPixelBox.mData), 
					             mGamma, currentPixelBox.getConsecutiveSize(), 
								 static_cast<unsigned char>(PixelUtil::getNumElemBits(currentPixelBox.mPixelFormat)));
				getBuffer(i, mip)->blitFromMemory(currentPixelBox);
			}
			else
			{
				getBuffer(i, mip)->blitFromMemory(src);
			}
		}
	}
	mSize = getNumFaces() * PixelUtil::getMemorySize(mWidth, mHeight, mDepth, mFormat);
}
void BWTexture::convertToImage(Image& destImage, bool includeMipMaps /* = false */)
{

}

void BWTexture::RemoveFromRenderTarget()
{

}

void BWTexture::copyToTexture(BWTexturePtr& target)
{

}
void BWTexture::createInternalResourcesWithImage(const ConstImagePtrList& images)
{
	if (!mInternalResourcesCreated)
	{
		createInternalResourcesWithImageImpl(images);
		mInternalResourcesCreated = true;
	}
}

void BWTexture::CreateInternalResourcesWithData(const void * Data)
{
	if (!mInternalResourcesCreated)
	{
		createInternalResourcesImpl(Data);
		mInternalResourcesCreated = true;
	}
}

void BWTexture::createInternalResources()
{
	if (!mInternalResourcesCreated)
	{
		createInternalResourcesImpl(NULL);
		mInternalResourcesCreated = true;
	}
}
void BWTexture::freeInternalResources()
{

}
void BWTexture::loadImage(const Image &img)
{

}
void BWTexture::loadRawData(BWDataStream& stream, unsigned short uWidth, unsigned short uHeight, PixelFormat eFormat)
{

}
void BWTexture::setDesiredBitDepths(unsigned short integerBits, unsigned short floatBits)
{
	mDesiredIntegerBitDepth = integerBits;
	mDesiredFloatBitDepth = floatBits;
}
void BWTexture::setDesiredFloatBitDepth(unsigned short bits)
{
	mDesiredFloatBitDepth = bits;
}
void BWTexture::setDesiredIntegerBitDepth(unsigned short bits)
{
	mDesiredIntegerBitDepth = bits;
}
void BWTexture::setFormat(PixelFormat pf)
{
	mFormat = pf;
	mDesiredFormat = pf;
	mSrcFormat = pf;
}

void BWTexture::setTreatLuminanceAsAlpha(bool b)
{
	mTreatLuminanceAsAlpha = b;
}
std::string  BWTexture::getSourceFileType() const
{
       if (name.empty())
       {
		   return StringUtil::BLANK;
       }
	   size_t pos = name.find_last_of(".");
	   if (pos != std::string::npos && pos < (name.length() - 1))
	   {
		   std::string ext = name.substr(pos + 1);
		   StringUtil::ToLowerCase(ext);
		   return ext;
	   }
	   else
	   {
		   BWDataStreamPrt daStream;
		   daStream = BWResourceGroupManager::GetInstance()->OpenResource(name, groupName);
		   if (daStream.IsNull() && GetTextureType() == TEX_TYPE_CUBE_MAP)
		   {
			   daStream = BWResourceGroupManager::GetInstance()->OpenResource(name + "_rt", groupName);
		   }
		   if (!daStream.IsNull())
		   {
			   assert(0);
			   //return Image::getFileExtFromMagic(daStream);
		   }
	   }
	   return StringUtil::BLANK;
}