#ifndef MEMORYDATASTREAM_H_
#define MEMORYDATASTREAM_H_
#include "BWDataStream.h"
#include "BWDataStream.h"
class MemoryDataStream : public BWDataStream
{
protected:
	/// Pointer to the start of the data area
	unsigned char* mData;
	/// Pointer to the current position in the memory
	unsigned char* mPos;
	/// Pointer to the end of the memory
	unsigned char* mEnd;
	/// Do we delete the memory on close
	bool mFreeOnClose;
public:

	/** Wrap an existing memory chunk in a stream.
	@param pMem Pointer to the existing memory
	@param size The size of the memory chunk in bytes
	@param freeOnClose If true, the memory associated will be destroyed
	when the stream is destroyed. Note: it's important that if you set
	this option to true, that you allocated the memory using OGRE_ALLOC_T
	with a category of MEMCATEGORY_GENERAL ensure the freeing of memory
	matches up.
	@param readOnly Whether to make the stream on this memory read-only once created
	*/
	MemoryDataStream(void* pMem, int size, bool freeOnClose = false, bool readOnly = false);

	/** Wrap an existing memory chunk in a named stream.
	@param name The name to give the stream
	@param pMem Pointer to the existing memory
	@param size The size of the memory chunk in bytes
	@param freeOnClose If true, the memory associated will be destroyed
	when the stream is destroyed. Note: it's important that if you set
	this option to true, that you allocated the memory using OGRE_ALLOC_T
	with a category of MEMCATEGORY_GENERAL ensure the freeing of memory
	matches up.
	@param readOnly Whether to make the stream on this memory read-only once created
	*/
	MemoryDataStream(const std::string& name, void* pMem, int size,
		bool freeOnClose = false, bool readOnly = false);

	/** Create a stream which pre-buffers the contents of another stream.
	@remarks
	This constructor can be used to intentionally read in the entire
	contents of another stream, copying them to the internal buffer
	and thus making them available in memory as a single unit.
	@param sourceStream Another DataStream which will provide the source
	of data
	@param freeOnClose If true, the memory associated will be destroyed
	when the stream is destroyed.
	@param readOnly Whether to make the stream on this memory read-only once created
	*/
	MemoryDataStream(BWDataStream& sourceStream,
		bool freeOnClose = true, bool readOnly = false);

	/** Create a stream which pre-buffers the contents of another stream.
	@remarks
	This constructor can be used to intentionally read in the entire
	contents of another stream, copying them to the internal buffer
	and thus making them available in memory as a single unit.
	@param sourceStream Weak reference to another DataStream which will provide the source
	of data
	@param freeOnClose If true, the memory associated will be destroyed
	when the stream is destroyed.
	@param readOnly Whether to make the stream on this memory read-only once created
	*/
	MemoryDataStream(BWDataStreamPrt& sourceStream,
		bool freeOnClose = true, bool readOnly = false);

	/** Create a named stream which pre-buffers the contents of
	another stream.
	@remarks
	This constructor can be used to intentionally read in the entire
	contents of another stream, copying them to the internal buffer
	and thus making them available in memory as a single unit.
	@param name The name to give the stream
	@param sourceStream Another DataStream which will provide the source
	of data
	@param freeOnClose If true, the memory associated will be destroyed
	when the stream is destroyed.
	@param readOnly Whether to make the stream on this memory read-only once created
	*/
	MemoryDataStream(const std::string& name, BWDataStreamPrt& sourceStream,
		bool freeOnClose = true, bool readOnly = false);

	/** Create a named stream which pre-buffers the contents of
	another stream.
	@remarks
	This constructor can be used to intentionally read in the entire
	contents of another stream, copying them to the internal buffer
	and thus making them available in memory as a single unit.
	@param name The name to give the stream
	@param sourceStream Another DataStream which will provide the source
	of data
	@param freeOnClose If true, the memory associated will be destroyed
	when the stream is destroyed.
	@param readOnly Whether to make the stream on this memory read-only once created
	*/
	MemoryDataStream(const std::string& name, const BWDataStreamPrt& sourceStream,
		bool freeOnClose = true, bool readOnly = false);

	/** Create a stream with a brand new empty memory chunk.
	@param size The size of the memory chunk to create in bytes
	@param freeOnClose If true, the memory associated will be destroyed
	when the stream is destroyed.
	@param readOnly Whether to make the stream on this memory read-only once created
	*/
	MemoryDataStream(int size, bool freeOnClose = true, bool readOnly = false);
	/** Create a named stream with a brand new empty memory chunk.
	@param name The name to give the stream
	@param size The size of the memory chunk to create in bytes
	@param freeOnClose If true, the memory associated will be destroyed
	when the stream is destroyed.
	@param readOnly Whether to make the stream on this memory read-only once created
	*/
	MemoryDataStream(const std::string& name, int size,
		bool freeOnClose = true, bool readOnly = false);

	~MemoryDataStream();

	/** Get a pointer to the start of the memory block this stream holds. */
	unsigned char* getPtr(void) { return mData; }

	/** Get a pointer to the current position in the memory block this stream holds. */
	unsigned char* getCurrentPtr(void) { return mPos; }

	/** @copydoc DataStream::read
	*/
	int Read(void* buf, int count);

	/** @copydoc DataStream::write
	*/
	int write(const void* buf, int count);

	/** @copydoc DataStream::readLine
	*/
	int ReadLine(char* buf, int maxCount, const std::string& delim = "\n");

	/** @copydoc DataStream::skipLine
	*/
	int skipLine(const std::string& delim = "\n");

	/** @copydoc DataStream::skip
	*/
	void Skip(int count);

	/** @copydoc DataStream::seek
	*/
	void Seek(int pos);

	/** @copydoc DataStream::tell
	*/
	int Tell() ;

	/** @copydoc DataStream::eof
	*/
	bool Eof(void) ;

	/** @copydoc DataStream::close
	*/
	void Close(void);

	/** Sets whether or not to free the encapsulated memory on close. */
	void setFreeOnClose(bool free) { mFreeOnClose = free; }
};

/** Shared pointer to allow memory data streams to be passed around without
worrying about deallocation
*/
typedef SmartPointer<MemoryDataStream> MemoryDataStreamPtr;

#endif