
#include "BWMemoryDataStream.h"

MemoryDataStream::MemoryDataStream(void* pMem, int inSize, bool freeOnClose, bool readOnly)
:BWDataStream(static_cast<unsigned short>(readOnly ? READ : (READ | WRITE)))
{
	mData = mPos = static_cast<unsigned char*>(pMem);
	size = inSize;
	mEnd = mData + size;
	mFreeOnClose = freeOnClose;
	assert(mEnd >= mPos);
}
//-----------------------------------------------------------------------
MemoryDataStream::MemoryDataStream(const std::string& name, void* pMem, int inSize,
	bool freeOnClose, bool readOnly)
	: BWDataStream(name, static_cast<unsigned short>(readOnly ? READ : (READ | WRITE)))
{
	mData = mPos = static_cast<unsigned char*>(pMem);
	size = inSize;
	mEnd = mData + size;
	mFreeOnClose = freeOnClose;
	assert(mEnd >= mPos);
}
//-----------------------------------------------------------------------
MemoryDataStream::MemoryDataStream(BWDataStream& sourceStream,
	bool freeOnClose, bool readOnly)
	: BWDataStream(static_cast<unsigned short>(readOnly ? READ : (READ | WRITE)))
{
	// Copy data from incoming stream
	size = sourceStream.Size();
	if (size == 0 && !sourceStream.Eof())
	{
		// size of source is unknown, read all of it into memory
		std::string contents = sourceStream.GetAsString();
		size = contents.size();
		mData = new unsigned char[size];
		mPos = mData;
		memcpy(mData, contents.data(), size);
		mEnd = mData + size;
	}
	else
	{
		mData = new unsigned char[size];
		mPos = mData;
		mEnd = mData + sourceStream.Read(mData, size);
		mFreeOnClose = freeOnClose;
	}
	assert(mEnd >= mPos);
}
//-----------------------------------------------------------------------
MemoryDataStream::MemoryDataStream(BWDataStreamPrt& sourceStream,
	bool freeOnClose, bool readOnly)
	: BWDataStream(static_cast<unsigned short>(readOnly ? READ : (READ | WRITE)))
{
	// Copy data from incoming stream
	size = sourceStream->Size();
	if (size == 0 && !sourceStream->Eof())
	{
		// size of source is unknown, read all of it into memory
		std::string contents = sourceStream->GetAsString();
		size = contents.size();
		mData = new unsigned char[size];

		mPos = mData;
		memcpy(mData, contents.data(), size);
		mEnd = mData + size;
	}
	else
	{
		mData = new unsigned char[size];

		mPos = mData;
		mEnd = mData + sourceStream->Read(mData, size);
		mFreeOnClose = freeOnClose;
	}
	assert(mEnd >= mPos);
}
//-----------------------------------------------------------------------
MemoryDataStream::MemoryDataStream(const std::string& name, BWDataStreamPrt& sourceStream,
	bool freeOnClose, bool readOnly)
	: BWDataStream(name, static_cast<unsigned short>(readOnly ? READ : (READ | WRITE)))
{
	// Copy data from incoming stream
	size = sourceStream->Size();
	if (size == 0 && !sourceStream->Eof())
	{
		// size of source is unknown, read all of it into memory
		std::string contents = sourceStream->GetAsString();
		size = contents.size();
		mData = new unsigned char[size];

		mPos = mData;
		memcpy(mData, contents.data(), size);
		mEnd = mData + size;
	}
	else
	{
		mData = new unsigned char[size];

		mPos = mData;
		mEnd = mData + sourceStream->Read(mData, size);
		mFreeOnClose = freeOnClose;
	}
	assert(mEnd >= mPos);
}
//-----------------------------------------------------------------------
MemoryDataStream::MemoryDataStream(const std::string& name, const BWDataStreamPrt& sourceStream,
	bool freeOnClose, bool readOnly)
	: BWDataStream(name, static_cast<unsigned short>(readOnly ? READ : (READ | WRITE)))
{
	// Copy data from incoming stream
	size = sourceStream->Size();
	if (size == 0 && !sourceStream->Eof())
	{
		// size of source is unknown, read all of it into memory
		std::string contents = sourceStream->GetAsString();
		size = contents.size();
		mData = new unsigned char[size];
		mPos = mData;
		memcpy(mData, contents.data(), size);
		mEnd = mData + size;
	}
	else
	{

		mData = new unsigned char[size];
		mPos = mData;
		mEnd = mData + sourceStream->Read(mData, size);
		mFreeOnClose = freeOnClose;
	}
	assert(mEnd >= mPos);
}
//-----------------------------------------------------------------------
MemoryDataStream::MemoryDataStream(int inSize, bool freeOnClose, bool readOnly)
: BWDataStream(static_cast<unsigned short>(readOnly ? READ : (READ | WRITE)))
{
	size = inSize;
	mFreeOnClose = freeOnClose;
	mData = new unsigned char[size];

	mPos = mData;
	mEnd = mData + size;
	assert(mEnd >= mPos);
}
//-----------------------------------------------------------------------
MemoryDataStream::MemoryDataStream(const std::string& name, int inSize,
	bool freeOnClose, bool readOnly)
	: BWDataStream(name, static_cast<unsigned short>(readOnly ? READ : (READ | WRITE)))
{
	size = inSize;
	mFreeOnClose = freeOnClose;
	mData = new unsigned char[size];

	mPos = mData;
	mEnd = mData + size;
	assert(mEnd >= mPos);
}
//-----------------------------------------------------------------------
MemoryDataStream::~MemoryDataStream()
{
	Close();
}
//-----------------------------------------------------------------------
int MemoryDataStream::Read(void* buf, int count)
{
	int cnt = count;
	// Read over end of memory?
	if (mPos + cnt > mEnd)
		cnt = mEnd - mPos;
	if (cnt == 0)
		return 0;

	assert(cnt <= count);

	memcpy(buf, mPos, cnt);
	mPos += cnt;
	return cnt;
}
//---------------------------------------------------------------------
int MemoryDataStream::write(const void* buf, int count)
{
	int written = 0;
	if (IsWriteable())
	{
		written = count;
		// we only allow writing within the extents of allocated memory
		// check for buffer overrun & disallow
		if (mPos + written > mEnd)
			written = mEnd - mPos;
		if (written == 0)
			return 0;

		memcpy(mPos, buf, written);
		mPos += written;
	}
	return written;
}
//-----------------------------------------------------------------------
int MemoryDataStream::ReadLine(char* buf, int maxCount,
	const std::string& delim)
{
	// Deal with both Unix & Windows LFs
	bool trimCR = false;
	if (delim.find_first_of('\n') != std::string::npos)
	{
		trimCR = true;
	}

	int pos = 0;

	// Make sure pos can never go past the end of the data 
	while (pos < maxCount && mPos < mEnd)
	{
		if (delim.find(*mPos) != std::string::npos)
		{
			// Trim off trailing CR if this was a CR/LF entry
			if (trimCR && pos && buf[pos - 1] == '\r')
			{
				// terminate 1 character early
				--pos;
			}

			// Found terminator, skip and break out
			++mPos;
			break;
		}

		buf[pos++] = *mPos++;
	}

	// terminate
	buf[pos] = '\0';

	return pos;
}
//-----------------------------------------------------------------------
int MemoryDataStream::skipLine(const std::string& delim)
{
	int pos = 0;

	// Make sure pos can never go past the end of the data 
	while (mPos < mEnd)
	{
		++pos;
		if (delim.find(*mPos++) != std::string::npos)
		{
			// Found terminator, break out
			break;
		}
	}

	return pos;

}
//-----------------------------------------------------------------------
void MemoryDataStream::Skip(int count)
{
	int newpos = (int)((mPos - mData) + count);
	assert(mData + newpos <= mEnd);

	mPos = mData + newpos;
}
//-----------------------------------------------------------------------
void MemoryDataStream::Seek(int pos)
{
	assert(mData + pos <= mEnd);
	mPos = mData + pos;
}
//-----------------------------------------------------------------------
int MemoryDataStream::Tell(void)
{
	//mData is start, mPos is current location
	return mPos - mData;
}
//-----------------------------------------------------------------------
bool MemoryDataStream::Eof(void) 
{
	return mPos >= mEnd;
}
//-----------------------------------------------------------------------
void MemoryDataStream::Close(void)
{
	if (mFreeOnClose && mData)
	{
		delete[] mData;
		mData = 0;
	}

}