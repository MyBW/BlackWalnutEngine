#include "GLHardwareIndexBuffer.h"
#include "GL/include/glew.h"
#include "GLHardwareBufferManager.h"
GLHardwareIndexBuffer::GLHardwareIndexBuffer(BWHardwareBufferManagerBase *manager, IndexType indexType, size_t indexNum, Usage usage, bool isUseSystemBuffer, bool isUseShadowBuffer)
:BWHardwareIndexBuffer(manager, indexType, indexNum, usage, isUseSystemBuffer, isUseShadowBuffer)
{
	glGenBuffers(1, &mBufferID);
	if (!mBufferID)
	{
		assert(0); 
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mSizeInBytes, NULL, GLHardwareBufferManager::getGLUsage(usage));
}
GLuint GLHardwareIndexBuffer::getGLBufferID() const
{
	return mBufferID;
}
void* GLHardwareIndexBuffer::lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option)
{
	assert(!isLock());
	void *ptr;
	unsigned int access;
	// 在进行应用和显卡数据交换的时候  有一个长度限制  当length的长度小于该限制的时候 使用glBufferSudData
	//当数据长度大于限制的时候 使用glMapBuffer   这样比较快  但是这种要求只有对win32平台有用 其他平台使用
	//glMapBuffer   这里我们一致使用glMapBuffer  查阅相关资料后可以开始更改一下
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID);
    if (option == HBL_DISCARD)
    {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mSizeInBytes, NULL, GLHardwareBufferManager::getGLUsage(mUsage));
    }
	if (mUsage & HBU_WRITE_ONLY)
	{
		access = GL_WRITE_ONLY;
	}else if (option == HBL_READ_ONLY)
	{
		access = GL_READ_ONLY;
	}
	else
	{
		access = GL_READ_WRITE;
	}
	void *pBuffer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, access);
	if (pBuffer == 0)
	{
		assert(0);
	}
	ptr = static_cast<void *>(static_cast<unsigned char*>(pBuffer) + offset);
	mIsLocked = true;
	return ptr;
}
void GLHardwareIndexBuffer::unLockImp()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID);
	if (!glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER))
	{
		assert(0);
	}
	mIsLocked = false;
}
void GLHardwareIndexBuffer::updateFromShadowBuffer()
{
	if (mUseShadowBuffer && mShadowUpdated)
	{
		const void *src = mShadowHardwareBuffer->lock(mLockStart, mLockLenth, HBL_READ_ONLY);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID);
		if (mLockStart == 0 && mLockLenth == mSizeInBytes)
		{
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mSizeInBytes, src, GLHardwareBufferManager::getGLUsage(mUsage));
		}
		else
		{
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, mLockStart, mLockLenth, src);
		}
		mShadowHardwareBuffer->unlock();
		mShadowUpdated = false;
	}
}