#ifndef HARDWAREBUFFER_H_
#define HARDWAREBUFFER_H_
#include <string>
class BWHardwareBuffer
{
public:
	/// Enums describing buffer usage; not mutually exclusive
	//这些设置的是buffer将会被用什么样的频率访问进行只读 只写 还是读写访问  这个buffer生命周期过程中保持不变
	enum Usage
	{
		/** Static buffer which the application rarely modifies once created. Modifying
		the contents of this buffer will involve a performance hit.
		*/
		HBU_STATIC = 1,
		/** Indicates the application would like to modify this buffer with the CPU
		fairly often.
		Buffers created with this flag will typically end up in AGP memory rather
		than video memory.
		*/
		HBU_DYNAMIC = 2,
		/** Indicates the application will never read the contents of the buffer back,
		it will only ever write data. Locking a buffer with this flag will ALWAYS
		return a pointer to new, blank memory rather than the memory associated
		with the contents of the buffer; this avoids DMA stalls because you can
		write to a new memory area while the previous one is being used.
		*/
		HBU_WRITE_ONLY = 4,
		/** Indicates that the application will be refilling the contents
		of the buffer regularly (not just updating, but generating the
		contents from scratch), and therefore does not mind if the contents
		of the buffer are lost somehow and need to be recreated. This
		allows and additional level of optimisation on the buffer.
		This option only really makes sense when combined with
		HBU_DYNAMIC_WRITE_ONLY.
		*/
		HBU_DISCARDABLE = 8,
		/// Combination of HBU_STATIC and HBU_WRITE_ONLY
		HBU_STATIC_WRITE_ONLY = 5,
		/** Combination of HBU_DYNAMIC and HBU_WRITE_ONLY. If you use
		this, strongly consider using HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE
		instead if you update the entire contents of the buffer very
		regularly.
		*/
		HBU_DYNAMIC_WRITE_ONLY = 6,
		/// Combination of HBU_DYNAMIC, HBU_WRITE_ONLY and HBU_DISCARDABLE
		HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE = 14


	};
	// 这些设置的是如何对待上一帧遗留在buffer中数据   这个部分可以在每一帧的时候都设置不同的参数 但是usage却是要求在buffer生命周期中保持不变的
	enum LockOptions
	{
		HBL_NORMAL , //在原有数据上读写
		HBL_DISCARD ,//放弃原有数据  重新分配buffer
		HBL_READ_ONLY ,//对原有数据只读
		HBL_NO_OVERWRITE //对原有数据覆盖写
	};
	BWHardwareBuffer(const std::string& name ,Usage usage , bool systemMemory , bool isUseShadowBuffer);
	virtual ~BWHardwareBuffer();
	void* lock(BWHardwareBuffer::LockOptions option);
	virtual void* lock(size_t offset, size_t length, BWHardwareBuffer::LockOptions option); //向gpu 或者 系统（如果使用了shadowbuffer）申请内存 并返回内存的首地址
	virtual void  unlock(); // 将数据上传至buffer
	bool isLock();
	virtual void write(size_t offset, size_t size, void *src){ }
	virtual void read(size_t offset, size_t size, void *dest){ }
	unsigned int getSizeInBytes(){ return mSizeInBytes; }
	Usage getUsage() const { return mUsage; }
	const std::string& getName() const;
	void SetName(std::string &name);
protected:
	virtual void* lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option) = 0 ;
	virtual void  unLockImp() = 0;
	virtual void  updateFromShadowBuffer();
	size_t mSizeInBytes;
	bool mIsLocked;   //这里锁住的意思是 本buffer中有一段buffer在被读写过程中 不能被外界使用 一旦调用lock 就被锁住了
	bool mUseShadowBuffer;
	BWHardwareBuffer *mShadowHardwareBuffer;
	bool mShadowUpdated;
	Usage mUsage;
	size_t mLockStart;
	size_t mLockLenth;
	std::string mName;
private:
	
};
#endif