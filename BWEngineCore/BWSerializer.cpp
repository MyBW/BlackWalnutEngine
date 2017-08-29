#include "BWSerializer.h"


BWSerializer::BWSerializer()
{
	mVersion = "[serializer_v1.00]";
	mFlipEndian = false;
}
BWSerializer::~BWSerializer()
{
	mpfFile; //»Á∫Œ Õ∑≈
}
void BWSerializer::writeBools(const bool * const pLong, size_t count)
{
	if (mFlipEndian)
	{
		bool *pBoolToWrite = new bool[count];
		memcpy(pBoolToWrite, pLong, count *sizeof(bool));
		flipToLittleEndian(pBoolToWrite, sizeof(bool), count);
		writeData(pBoolToWrite, sizeof(bool), count);
	}
	else
	{
		writeData(pLong, sizeof(bool), count);
	}
}

void BWSerializer::writeInts(const unsigned int * const pint, size_t count)
{
	if (mFlipEndian)
	{
		unsigned int *pIntsToWrite = new unsigned int[count];
		memcpy(pIntsToWrite, pint, count * sizeof(unsigned int));
		flipToLittleEndian(pIntsToWrite, sizeof(unsigned int), count);
		writeData(pIntsToWrite, sizeof(unsigned int), count);
	}
	else
	{
		writeData(pint, sizeof(unsigned int), count);
	}
}
void BWSerializer::writeFloats(const float * const pfloat, size_t count)
{
	if (mFlipEndian)
	{
		float *pFloatToWrite = new float[count];
		memcpy(pFloatToWrite, pfloat, sizeof(float)* count);
		flipToLittleEndian(pFloatToWrite, sizeof(float), count);
		writeData(pFloatToWrite, sizeof(float), count);
		delete[] pFloatToWrite;
	}
	else
	{
		writeData(pfloat, sizeof(float), count);
	}
}
void BWSerializer::writeFloats(const double *const pfloat, size_t count /* = 1 */)
{
	
}
void BWSerializer::writeShorts(const unsigned short * const pshort, size_t count /* = 1 */)
{
	if (mFlipEndian)
	{
		unsigned short *pshortoWrite = new unsigned short[count];
		memcpy(pshortoWrite, pshort, sizeof(unsigned short)* count);
		flipToLittleEndian(pshortoWrite, sizeof(unsigned short), count);
		writeData(pshortoWrite, sizeof(unsigned short), count);
		delete[] pshortoWrite;
	}
	else
	{
		writeData(pshort, sizeof(unsigned short), count);
	}
}
void BWSerializer::writeString(const std::string &string)
{
	fputs(string.c_str(), mpfFile);
	fputc('\n', mpfFile);
}
void BWSerializer::writeFileHeader()
{
	unsigned short val = HEAD_STREAM_ID;
	writeShorts(&val, 1);
	writeString(mVersion);
}
void BWSerializer::writeChunkHeader(unsigned short id, size_t size)
{
	writeShorts(&id, size);
	unsigned int uint32size = static_cast<unsigned int>(size);
	writeInts(&uint32size, 1);
}
void BWSerializer::flipToLittleEndian(void *data, size_t size, size_t count /* = 1 */)
{
	if (mFlipEndian)
	{
		flipEndian(data, size, count);
	}
}
void BWSerializer::flipEndian(void *data, size_t size, size_t count)
{
	for (unsigned int i = 0; i < count; i++)
	{
		flipEndian((void*)((size_t)data + (i *size)), size);
	}
}
void BWSerializer::flipEndian(void *data, size_t size)
{
	char swapByte;
	for (unsigned int  byteIndex = 0; byteIndex < size / 2; byteIndex++)
	{
		swapByte = *(char *)((size_t)data + byteIndex);
		*(char *)((size_t)data + byteIndex) = *(char *)((size_t)data + size - byteIndex - 1);
		*(char *)((size_t)data + size - byteIndex - 1) = swapByte;
	}
}
void BWSerializer::writeData(const void * const buf, size_t size, size_t count)
{
	fwrite((void * const)buf, size, count, mpfFile);
}

void BWSerializer::readFloats(BWDataStreamPrt &stream, float *dest, size_t count)
{
	stream->Read(dest, sizeof(float)*count);
	flipFromLittleEndian(dest, sizeof(float), count);
}
void BWSerializer::readBool(BWDataStreamPrt &stream, bool *dest, size_t count)
{
	stream->Read(dest, sizeof(bool)*count);
	flipFromLittleEndian(dest, sizeof(bool), count);
}
void BWSerializer::readFloats(BWDataStreamPrt &stream, double *dest, size_t count)
{
	stream->Read(dest, sizeof(float)*count);
	flipFromLittleEndian(dest, sizeof(float), count);
}
void BWSerializer::readShorts(BWDataStreamPrt &stream, unsigned short *dest, size_t count)
{
	stream->Read(dest, sizeof(unsigned short) * count);
	flipFromLittleEndian(dest, sizeof(unsigned int), count);
}
std::string BWSerializer::readString(BWDataStreamPrt &stream)
{
	return stream->GetLine();
}
void BWSerializer::readInts(BWDataStreamPrt &stream, unsigned int *dest, size_t count /* = 1 */)
{
	stream->Read(dest, sizeof(unsigned int)* count);
	flipFromLittleEndian(dest, sizeof(unsigned int), count);
}
std::string BWSerializer::readString(BWDataStreamPrt &stream, size_t numChars)
{
	assert(numChars < 255);
	char str[255];
	stream->Read(str, numChars);
	str[numChars] = '\0';
	return str;
}
void BWSerializer::flipFromLittleEndian(void *data, size_t size, size_t count /* = 1 */)
{
	if (mFlipEndian)
	{
		flipEndian(data, size, count);
	}
}
void BWSerializer::determineEndianness(BWDataStreamPrt &data)
{
	if (data->Tell() != 0)
	{
		assert(0);
	}
	unsigned short dest;
	size_t actually_read = data->Read(&dest, sizeof(unsigned short));
	data->Skip(0 - (long)actually_read);
	if (actually_read != sizeof(unsigned short))
	{
		assert(0);
	}
	if (dest == HEAD_STREAM_ID)
	{
		mFlipEndian = false;
	}
	else if (dest == OTHER_ENDIAN_HEADER_STREAM_ID)
	{
		mFlipEndian = true;
	}
	else
	{
		assert(0);
	}
}
void BWSerializer::readFileHeader(BWDataStreamPrt &stream)
{
	unsigned short heardID;
	readShorts(stream, &heardID, 1);
	if (heardID == HEAD_STREAM_ID)
	{
		std::string ver = readString(stream);
		if (ver != mVersion)
		{
			assert(0);
		}
	}
	else
	{
		assert(0);
	}
}
unsigned short BWSerializer::readChunk(BWDataStreamPrt &stream)
{
	unsigned short  id;
	readShorts(stream, &id, 1);
	readInts(stream, &mCurrentstreamLen, 1);
	return id;
}
void BWSerializer::determineEndianness(Endian requestedEndian)
{
	switch (requestedEndian)
	{
	case BWSerializer::ENDIAN_NATIVE:
		mFlipEndian = false;
		break;
	case BWSerializer::ENDIAN_BING:
		mFlipEndian = true;
		break;
	case BWSerializer::ENDIAN_LITTLE:
		mFlipEndian = false;
		break;
	default:
		break;
	}
}
