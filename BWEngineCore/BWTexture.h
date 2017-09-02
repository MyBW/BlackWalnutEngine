#ifndef TEXTURE_H_
#define TEXTURE_H_
#include "BWResource.h"
#include "BWHardwareBuffer.h"
#include "BWImage.h"
#include "BWHardwarePixelBuffer.h"
enum TextureUsage
{
	/// @copydoc HardwareBuffer::Usage
	TU_STATIC = BWHardwareBuffer::HBU_STATIC,
	TU_DYNAMIC = BWHardwareBuffer::HBU_DYNAMIC,
	TU_WRITE_ONLY = BWHardwareBuffer::HBU_WRITE_ONLY,
	TU_STATIC_WRITE_ONLY = BWHardwareBuffer::HBU_STATIC_WRITE_ONLY,
	TU_DYNAMIC_WRITE_ONLY = BWHardwareBuffer::HBU_DYNAMIC_WRITE_ONLY,
	TU_DYNAMIC_WRITE_ONLY_DISCARDABLE = BWHardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,
	/// mipmaps will be automatically generated for this texture
	TU_AUTOMIPMAP = 0x100,
	/// this texture will be a render target, i.e. used as a target for render to texture
	/// setting this flag will ignore all other texture usages except TU_AUTOMIPMAP
	TU_RENDERTARGET = 0x200,
	/// default to automatic mipmap generation static textures
	TU_DEFAULT = TU_AUTOMIPMAP | TU_STATIC_WRITE_ONLY

};

/** Enum identifying the texture type
*/
enum TextureType
{
	/// 1D texture, used in combination with 1D texture coordinates
	TEX_TYPE_1D = 1,
	/// 2D texture, used in combination with 2D texture coordinates (default)
	TEX_TYPE_2D = 2,
	/// 3D volume texture, used in combination with 3D texture coordinates
	TEX_TYPE_3D = 3,
	/// 3D cube map, used in combination with 3D texture coordinates
	TEX_TYPE_CUBE_MAP = 4,
	/// 2D texture array
	TEX_TYPE_2D_ARRAY = 5
};

/** Enum identifying special mipmap numbers
*/
enum TextureMipmap
{
	/// Generate mipmaps up to 1x1
	MIP_UNLIMITED = 0x7FFFFFFF,
	/// Use TextureManager default
	MIP_DEFAULT = -1
};


class BWTexturePtr;

class BWTexture : public BWResource
{
public:
	BWTexture(BWResourceManager* creator, const std::string &name, const std::string &groupName);
	virtual void setTextureType(TextureType ttype) { mTextureType = ttype; }
	virtual TextureType GetTextureType(void) const { return mTextureType; }
	virtual size_t getNumMipmaps(void) const { return mNumMipmaps; }
	virtual void setNumMipmaps(size_t num) { mNumRequestedMipmaps = mNumMipmaps = num; }
	virtual bool getMipmapsHardwareGenerated(void) const { return mMipmapsHardwareGenerated; }
	virtual float getGamma(void) const { return mGamma; }
	virtual void setGamma(float g) { mGamma = g; }
	virtual void setHardwareGammaEnabled(bool enabled) { mHwGamma = enabled; }
	virtual bool isHardwareGammaEnabled() const { return mHwGamma; }
	virtual void setFSAA(unsigned int  fsaa, const std::string &fsaaHint) { mFSAA = fsaa; mFSAAHint = fsaaHint; }

	virtual unsigned int getFSAA() const { return mFSAA; }

	virtual const std::string& getFSAAHint() const { return mFSAAHint; }


	virtual size_t getHeight(void) const { return mHeight; }


	virtual size_t getWidth(void) const { return mWidth; }

	/** Returns the depth of the texture (only applicable for 3D textures).
	*/
	virtual size_t getDepth(void) const { return mDepth; }

	/** Returns the height of the original input texture (may differ due to hardware requirements).
	*/
	virtual size_t getSrcHeight(void) const { return mSrcHeight; }

	/** Returns the width of the original input texture (may differ due to hardware requirements).
	*/
	virtual size_t getSrcWidth(void) const { return mSrcWidth; }

	/** Returns the original depth of the input texture (only applicable for 3D textures).
	*/
	virtual size_t getSrcDepth(void) const { return mSrcDepth; }

	/** Set the height of the texture; can only do this before load();
	*/
	virtual void setHeight(size_t h) { mHeight = mSrcHeight = h; }

	/** Set the width of the texture; can only do this before load();
	*/
	virtual void setWidth(size_t w) { mWidth = mSrcWidth = w; }

	/** Set the depth of the texture (only applicable for 3D textures);
	can only do this before load();
	*/
	virtual void setDepth(size_t d) { mDepth = mSrcDepth = d; }

	/** Returns the TextureUsage identifier for this Texture
	*/
	virtual int getUsage() const
	{
		return mUsage;
	}

	/** Sets the TextureUsage identifier for this Texture; only useful before load()

	@param u is a combination of TU_STATIC, TU_DYNAMIC, TU_WRITE_ONLY
	TU_AUTOMIPMAP and TU_RENDERTARGET (see TextureUsage enum). You are
	strongly advised to use HBU_STATIC_WRITE_ONLY wherever possible, if you need to
	update regularly, consider HBU_DYNAMIC_WRITE_ONLY.
	*/
	virtual void setUsage(int u) { mUsage = u; }

	/** Creates the internal texture resources for this texture.
	@remarks
	This method creates the internal texture resources (pixel buffers,
	texture surfaces etc) required to begin using this texture. You do
	not need to call this method directly unless you are manually creating
	a texture, in which case something must call it, after having set the
	size and format of the texture (e.g. the ManualResourceLoader might
	be the best one to call it). If you are not defining a manual texture,
	or if you use one of the self-contained load...() methods, then it will be
	called for you.
	*/
	virtual void createInternalResources(void);

	virtual void createInternalResourcesWithImage(const ConstImagePtrList& images);
	/** Frees internal texture resources for this texture.
	*/
	virtual void freeInternalResources(void);

	/** Copies (and maybe scales to fit) the contents of this texture to
	another texture. */
	virtual void copyToTexture(BWTexturePtr& target);

	/** Loads the data from an image.
	@note Important: only call this from outside the load() routine of a
	Resource. Don't call it within (including ManualResourceLoader) - use
	_loadImages() instead. This method is designed to be external,
	performs locking and checks the load status before loading.
	*/
	virtual void loadImage(const Image &img);

	/** Loads the data from a raw stream.
	@note Important: only call this from outside the load() routine of a
	Resource. Don't call it within (including ManualResourceLoader) - use
	_loadImages() instead. This method is designed to be external,
	performs locking and checks the load status before loading.
	@param stream Data stream containing the raw pixel data
	@param uWidth Width of the image
	@param uHeight Height of the image
	@param eFormat The format of the pixel data
	*/
	virtual void loadRawData(BWDataStream& stream,
		unsigned short uWidth, unsigned short uHeight, PixelFormat eFormat);

	/** Internal method to load the texture from a set of images.
	@note Do NOT call this method unless you are inside the load() routine
	already, e.g. a ManualResourceLoader. It is not threadsafe and does
	not check or update resource loading status.
	 将图片从image载入到GPU中
	*/
	virtual void _loadImages(const ConstImagePtrList& images);

	/** Returns the pixel format for the texture surface. */
	virtual PixelFormat getFormat() const
	{
		return mFormat;
	}

	/** Returns the desired pixel format for the texture surface. */
	virtual PixelFormat getDesiredFormat(void) const
	{
		return mDesiredFormat;
	}

	/** Returns the pixel format of the original input texture (may differ due to
	hardware requirements and pixel format conversion).
	*/
	virtual PixelFormat getSrcFormat(void) const
	{
		return mSrcFormat;
	}

	/** Sets the pixel format for the texture surface; can only be set before load(). */
	virtual void setFormat(PixelFormat pf);

	/** Returns true if the texture has an alpha layer. */
	virtual bool hasAlpha(void) const;

	/** Sets desired bit depth for integer pixel format textures.
	@note
	Available values: 0, 16 and 32, where 0 (the default) means keep original format
	as it is. This value is number of bits for the pixel.
	*/
	virtual void setDesiredIntegerBitDepth(unsigned short bits);

	/** gets desired bit depth for integer pixel format textures.
	*/
	virtual unsigned short getDesiredIntegerBitDepth(void) const;

	/** Sets desired bit depth for float pixel format textures.
	@note
	Available values: 0, 16 and 32, where 0 (the default) means keep original format
	as it is. This value is number of bits for a channel of the pixel.
	*/
	virtual void setDesiredFloatBitDepth(unsigned short bits);

	/** gets desired bit depth for float pixel format textures.
	*/
	virtual unsigned short getDesiredFloatBitDepth(void) const;

	/** Sets desired bit depth for integer and float pixel format.
	*/
	virtual void setDesiredBitDepths(unsigned short integerBits, unsigned short floatBits);

	/** Sets whether luminace pixel format will treated as alpha format when load this texture.
	*/
	virtual void setTreatLuminanceAsAlpha(bool asAlpha);

	/** Gets whether luminace pixel format will treated as alpha format when load this texture.
	*/
	virtual bool getTreatLuminanceAsAlpha(void) const;

	/** Return the number of faces this texture has. This will be 6 for a cubemap
	texture and 1 for a 1D, 2D or 3D one.
	*/
	virtual size_t getNumFaces() const;

	/** Return hardware pixel buffer for a surface. This buffer can then
	be used to copy data from and to a particular level of the texture.
	@param face 	Face number, in case of a cubemap texture. Must be 0
	for other types of textures.
	For cubemaps, this is one of
	+X (0), -X (1), +Y (2), -Y (3), +Z (4), -Z (5)
	@param mipmap	Mipmap level. This goes from 0 for the first, largest
	mipmap level to getNumMipmaps()-1 for the smallest.
	@return	A shared pointer to a hardware pixel buffer
	@remarks	The buffer is invalidated when the resource is unloaded or destroyed.
	Do not use it after the lifetime of the containing texture.
	*/
	virtual BWHardwarePixelBufferPtr getBuffer(size_t face = 0, size_t mipmap = 0) = 0;


	/** Populate an Image with the contents of this texture.
	@param destImage The target image (contents will be overwritten)
	@param includeMipMaps Whether to embed mipmaps in the image
	*/
	virtual void convertToImage(Image& destImage, bool includeMipMaps = false);

	/** Retrieve a platform or API-specific piece of information from this texture.
	This method of retrieving information should only be used if you know what you're doing.
	@param name The name of the attribute to retrieve
	@param pData Pointer to memory matching the type of data you want to retrieve.
	*/
	virtual void getCustomAttribute(const std::string& name, void* pData) {}
	virtual void attachToRenderTarget(BWRenderTarget* renderTarget, int Num, int MipLevel = 0) = 0;
	virtual void RemoveFromRenderTarget();
	int SetIndex(int Index) { mIndex = Index; }
	FORCEINLINE bool isAttachToRenderTarget() const { return mRenderTarget != NULL; }
	FORCEINLINE int GetIndex() const { return mIndex };
	

protected:
	size_t mHeight;
	size_t mWidth;
	size_t mDepth;  //这个参数的意义是 如果是3D纹理 它表示有该纹理由多少张2D纹理组成
	size_t mIndex;   // 所属的纹理单元在Pass中的坐标
	BWRenderTarget* mRenderTarget;

	size_t mNumRequestedMipmaps;
	size_t mNumMipmaps;
	bool mMipmapsHardwareGenerated;
	float mGamma;
	bool mHwGamma;
	unsigned int mFSAA;
	std::string mFSAAHint;

	TextureType mTextureType;
	PixelFormat mFormat;
	int mUsage; // Bit field, so this can't be TextureUsage

	PixelFormat mSrcFormat;
	size_t mSrcWidth, mSrcHeight, mSrcDepth;

	PixelFormat mDesiredFormat;

	BWHardwarePixelBufferPtr mHardwarePixelBufferPtr;

	unsigned short mDesiredIntegerBitDepth;
	unsigned short mDesiredFloatBitDepth;
	bool mTreatLuminanceAsAlpha;

	bool mInternalResourcesCreated;
	/// @copydoc Resource::calculateSize
	size_t calculateSize(void) const;


	/** Implementation of creating internal texture resources
	*/
	virtual void createInternalResourcesImpl(void) = 0;
	virtual void createInternalResourcesWithImageImpl(const ConstImagePtrList& images) = 0;
	/** Implementation of freeing internal texture resources
	*/
	virtual void freeInternalResourcesImpl(void) = 0;

	/** Default implementation of unload which calls freeInternalResources */
	void unloadImpl(void);

	/** Identify the source file type as a string, either from the extension
	or from a magic number.
	*/
	std::string getSourceFileType() const;

};

class BWTexturePtr : public SmartPointer<BWTexture>
{
public:
	BWTexturePtr() :SmartPointer<BWTexture>()
	{

	}
	BWTexturePtr(const BWResourcePtr & resource)
	{
		mPointer = dynamic_cast<BWTexture*>(resource.Get());
		counter = resource.GetCounterPointer();
		if (counter)
		{
			(*counter)++;
		}
	}
	const BWTexturePtr& operator=(const BWResourcePtr &resource)
	{
		if (mPointer == dynamic_cast<BWTexture*>(resource.Get()))
		{
			return *this;
		}
		if (mPointer != NULL)
		{
			(*counter)--;
			if ((*counter) == 0)
			{
				delete mPointer;
			}
		}
		mPointer = dynamic_cast<BWTexture*>(resource.Get());
		counter = resource.GetCounterPointer();
		(*counter)++;
		return *this;
	}
};
#endif // !TEXTURE_H_
