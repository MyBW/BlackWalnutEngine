#ifndef SERIALIZER_H_
#define SERIALIZER_H_
#include <stdio.h>
#include <string>
#include "BWDataStream.h"

const int  STREAM_OVERHEAD_SIZE = sizeof(unsigned short)+sizeof(unsigned int);
const unsigned short HEAD_STREAM_ID = 0x1000;
const unsigned short OTHER_ENDIAN_HEADER_STREAM_ID = 0x1000;

class BWSerializer
{
public:
	BWSerializer();
	virtual ~BWSerializer();
	void setVersion(const std::string& version){ mVersion = version; }
	const std::string& getVersion(){ return mVersion; }
	enum Endian
	{
		ENDIAN_NATIVE ,
		ENDIAN_BING ,
		ENDIAN_LITTLE
	};
protected:
	unsigned int mCurrentstreamLen;
	FILE *mpfFile;
	std::string mVersion;
	bool mFlipEndian;

	virtual void writeFileHeader();
	virtual void writeChunkHeader(unsigned short id, size_t size);

	void writeFloats(const float * const pfloat, size_t count = 1);
	void writeFloats(const double *const pfloat, size_t count = 1);
	void writeShorts(const  unsigned short * const pshort, size_t count = 1);
	void writeInts(const  unsigned int * const pint, size_t count = 1);
	void writeBools(const bool * const pLong, size_t count = 1);
	void writeString(const std::string &string);
	void writeData(const void * const buf, size_t size, size_t count = 1);

	virtual void  readFileHeader(BWDataStreamPrt &stream);
	virtual unsigned short readChunk(BWDataStreamPrt &stream);

	void readBool(BWDataStreamPrt &stream, bool *dest, size_t count = 1);
	void readFloats(BWDataStreamPrt &stream, float *dest, size_t count = 1);
	void readFloats(BWDataStreamPrt &stream, double *dest, size_t count = 1);
	void readShorts(BWDataStreamPrt &stream, unsigned short *dest, size_t count = 1);
	void readInts(BWDataStreamPrt &stream, unsigned int *dest, size_t count = 1);
	std::string readString(BWDataStreamPrt &stream);
	std::string readString(BWDataStreamPrt &stream, size_t numChars);

	virtual void flipToLittleEndian(void *data, size_t size, size_t count = 1);
	virtual void flipFromLittleEndian(void *data, size_t size, size_t count = 1);

	virtual void flipEndian(void *data, size_t size, size_t count);
	virtual void flipEndian(void *data, size_t  size);

	virtual void determineEndianness( BWDataStreamPrt &data);
	virtual void determineEndianness(Endian requestedEndian);
private:

};


#endif