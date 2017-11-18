#include "BWImage.h"
#include "BWCodec.h"
#include "BWImageCodec.h"
#include "BWPrimitive.h"
#include <algorithm>

Image::Image()
:mWidth(0)
, mHeight(0)
, mDepth(0)
, mBufSize(0)
, mNumMipmaps(0)
, mFlags(0)
, mFormat(PixelFormat::PF_UNKNOWN)
, mPixelSize(0)
, mBuffer(NULL)
, mAutoDelete(true)
{

}
Image::Image(const Image &img) :mWidth(0)
, mHeight(0)
, mDepth(0)
, mBufSize(0)
, mNumMipmaps(0)
, mFlags(0)
, mFormat(PixelFormat::PF_UNKNOWN)
, mPixelSize(0)
, mBuffer(NULL)
, mAutoDelete(true)
{
	*this = img;
}
Image::~Image()
{
	if (mAutoDelete)
	{
		delete[] mBuffer;
	}
}
PixelFormat Image::getFormat() const
{
	return mFormat;
}
size_t Image::getDepth() const
{
	return mDepth;
}
size_t Image::getWidth() const
{
	return mWidth;
}
size_t Image::getHeight() const
{
	return mHeight;
}
const unsigned char* Image::getData() const
{
	return mBuffer;
}
Image & Image::operator=(const Image &img)
{
	freeMemory();
	mWidth = img.getWidth();
	mHeight = img.getHeight();
	mDepth = img.getDepth();
	mFormat = img.getFormat();
	mBufSize = img.mBufSize;
	mFlags = img.mFlags;
	mPixelSize = img.mPixelSize;
	mNumMipmaps = img.mNumMipmaps;
	mAutoDelete = img.mAutoDelete;
	if (mAutoDelete)
	{
		mBuffer = new unsigned char[mBufSize];
		memcpy(mBuffer, img.mBuffer, mBufSize);
	}
	else
	{
		mBuffer = img.mBuffer;
	}
	return *this;
}
Image& Image::load(BWDataStreamPrt& stream, const std::string& type /* = StringUtil::BLANK */)
{
	freeMemory();
	BWCodec *dec = NULL;
	if (!type.empty())
	{
		dec = BWCodec::getCodec(type);
	}
	else
	{
		size_t magicLen = std::min((size_t)stream->Size(), (size_t)32);
		char magicBuff[32];
		stream->Read(magicBuff, 32);
		stream->Seek(0);
		dec = BWCodec::getCodec(magicBuff, magicLen);	
	}
	if (dec == NULL)
	{
		assert(0);
	}
	BWCodec::DecodeResult reslut = dec->decode(stream);
	ImageCodec::ImageData *data = static_cast<ImageCodec::ImageData*>(reslut.second.Get());
	mWidth = data->width;
	mHeight = data->height;
	mDepth = data->depth;
	mBufSize = data->size;
	mNumMipmaps = data->num_mipMap;
	mFlags = data->flag;
	mFormat = data->format;
	mPixelSize = static_cast<unsigned char>(PixelUtil::getNumElemBytes(mFormat));
	mBuffer = reslut.first->getPtr();
	reslut.first->setFreeOnClose(false);
	mAutoDelete = true;
	return *this;
}

void Image::scale(const BWPixelBox &src, const BWPixelBox &scaled, Filter filter /* = FILTER_BILINEAR */)
{
	assert(0);
	/*assert(PixelUtil::isAccessible(src.mPixelFormat));
	assert(PixelUtil::isAccessible(scaled.mPixelFormat));

	MemoryDataStreamPtr buf;
	BWPixelBox  tmpPixelBox;
	switch (filter)
	{
	case Image::FILTER_NEAREST:
		if (src.mPixelFormat == scaled.mPixelFormat)
		{
			tmpPixelBox = scaled;
		}
		else
		{
			tmpPixelBox = BWPixelBox(scaled.getWidth(), scaled.getHeight(), scaled.getDepth(), scaled.mPixelFormat);
			buf.bind(new MemoryDataStream(tmpPixelBox.getConsecutiveSize()));
			tmpPixelBox.mData = buf->getPtr();
		}
		switch (PixelUtil::getNumElemBytes(src.mPixelFormat))
		{
		case 1 : 
		default:
			break;
		}
		break;
	case Image::FILTER_LINEAR:
		break;
	case Image::FILTER_BILINEAR:
		break;
	case Image::FILTER_BOX:
		break;
	case Image::FILTER_TRIANGLE:
		break;
	case Image::FILTER_BICUBIC:
		break;
	default:
		break;
	}*/
}
BWPixelBox Image::getPixelBox(size_t face /* = 0 */, size_t mipmap /* = 0 */) const
{
    //iamge data is arranged as 
	//face 0  mip 0
	//face 0  mip 1
    //face 1  mip 0
    //face 1  mip1
	if (mipmap > getNumMipmaps())
	{
		assert(0);
	}
	if (face > getNumFaces())
	{
		assert(0);
	}
	unsigned char * offset = const_cast<unsigned char*>(getData());
	size_t width = getWidth(), height = getHeight(), depth = getDepth();
	size_t numMip = getNumMipmaps();
	size_t fullFaceSize = 0;
	size_t finalFaceSize = 0;
	size_t finalWidth = 0, finalHeight = 0, finalDepth = 0;
	for (size_t mip = 0; mip <= numMip ; mip++)
	{
		if (mip == mipmap)
		{
			finalFaceSize = fullFaceSize;
			finalWidth = width;
			finalHeight = height;
			finalDepth = depth;
		}
		fullFaceSize += PixelUtil::getMemorySize(width, height, depth, mFormat);
		if (width != 1) { width /= 2; }
		if (height != 1){ height /= 2; }
		if (depth != 1) { depth /= 2; }
	}
	offset += face*fullFaceSize;
	offset += finalFaceSize;
	BWPixelBox src(finalWidth, finalHeight, finalDepth, getFormat(), offset);
	return src;
}
bool Image::hasFlag(const ImageFlags imgFlag) const
{
	return mFlags & imgFlag == 1;
}
size_t Image::getNumFaces() const
{
	return hasFlag(IF_CUBEMAP) ? 6 : 1;
}
size_t Image::getNumMipmaps() const
{
	return mNumMipmaps;
}
void Image::freeMemory()
{
	if (mAutoDelete && mBuffer)
	{
		delete[] mBuffer;
	}
	mBuffer = NULL;
}
void Image::applyGamma(unsigned char *buffer, float gamma, size_t size, unsigned char bpp)
{
	if (gamma == 1.0)
	{
		return;
	}
	if (bpp != 24 && bpp != 32)
	{
		return;
	}
	unsigned int stride = bpp >> 3;
	for (size_t i = 0, j = size / stride; i < j; i++ , buffer += stride)
	{
		float r, g, b;
		r = (float)buffer[0];
		g = (float)buffer[1];
		b = (float)buffer[2];

		float scale = 1.0f, tmp;
		r = r * gamma; b = b *gamma; g = g *gamma;
		if (r > 255.0f && (tmp = (255.0/r))< scale)
		{
			scale = tmp;
		}
		if (g > 255.0f && (tmp = (255.0 / g)) < scale)
		{
			scale = tmp;
		}
		if (b > 255.0f && (tmp = (255.0 / b)) < scale)
		{
			scale = tmp;
		}
		r *= scale; g *= scale; b *= scale;
		buffer[0] = (unsigned char)r;
		buffer[1] = (unsigned char)g;
		buffer[2] = (unsigned char)b;
	}
}