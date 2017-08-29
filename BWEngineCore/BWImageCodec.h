#ifndef IMAGECODEC_H_
#define IMAGECODEC_H_
#include "BWCodec.h"
#include "BWPixlFormat.h"
class ImageCodec : public BWCodec
{
public:
	virtual ~ImageCodec(){ }
	
	class ImageData : public BWCodec::BWCodecData
	{
	public:
		ImageData() :height(0), width(0), depth(1), size(0), num_mipMap(0), flag(0), format(PF_UNKNOWN)
		{ 
		}
		std::string dataType()
		{
				return "ImageData";
		}
	
		size_t height;
		size_t width;
		size_t depth;
		size_t size;
		unsigned short num_mipMap;
		unsigned int flag;
		PixelFormat format;
	};
	std::string getDataType()
	{
		return "ImageData";
	}
private:

};

#endif