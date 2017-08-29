#ifndef CODEC_H_
#define CODEC_H_
#include <map>
#include "BWSmartPointer.h"
#include "BWMemoryDataStream.h"
class BWCodec
{
public:
	typedef std::map<std::string, BWCodec*> CodecMap;
	class BWCodecData
	{
	public:
		virtual ~BWCodecData(){}
		virtual std::string dataType(){ return "CodecData"; }
	};
	typedef SmartPointer<BWCodecData> BWCodecDataPrt;
	typedef std::pair<MemoryDataStreamPtr, BWCodecDataPrt> DecodeResult;
protected:
	static CodecMap mCodecMap;
public:
	virtual ~BWCodec(){ }
	virtual std::string getType() const
	{
		return "BaseType";
	}
	static void registerCodec(BWCodec *pCodec)
	{
		CodecMap::iterator  reslut = mCodecMap.find(pCodec->getType());
		if (reslut != mCodecMap.end())
		{
			assert(0);
		}
		mCodecMap[pCodec->getType()] = pCodec;
	}
	static bool isCodecIsRegister(const std::string &type)
	{
		return mCodecMap.find(type) != mCodecMap.end();
	}
	static void unRegisterCodec(const BWCodec *codec)
	{
		mCodecMap.erase(codec->getType());
	}
	static  BWCodec* getCodec(const std::string &type)
	{
		CodecMap::iterator itor = mCodecMap.find(type);
		if (itor != mCodecMap.end())
		{
			return itor->second;
		}
		return NULL;
	}
	static BWCodec* getCodec(char *magicNumber, size_t maxByte)
	{
		assert(0);
		return NULL;
	}
	 virtual DecodeResult decode(BWDataStreamPrt& input) const = 0;

};
#endif