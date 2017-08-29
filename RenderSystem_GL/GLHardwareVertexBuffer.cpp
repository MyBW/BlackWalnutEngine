#include "GLHardwareVertexBuffer.h"
#include "GLHardwareBufferManager.h"
#include "GL/include/glew.h"
#include "GLPreDefine.h"
GLHardwareVertexBuffer::GLHardwareVertexBuffer(BWHardwareBufferManagerBase *manager, size_t vertexSize, size_t vertexNum, Usage usage, bool isSystemMemory, bool isUseShadowBuffer)
:BWHardwareVertexBuffer(manager , vertexSize , vertexNum , usage , isSystemMemory , isUseShadowBuffer)
{
	glGenBuffers(1, &mBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
	glBufferData(GL_ARRAY_BUFFER, mSizeInBytes, NULL, GLHardwareBufferManager::getGLUsage(usage));
}
GLuint GLHardwareVertexBuffer::getGLHardwareBufferID() const
{
	return mBufferID;
}
void* GLHardwareVertexBuffer::lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option)
{
	unsigned int access;
	glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
	if (option == HBL_DISCARD)
	{
		glBufferData(GL_ARRAY_BUFFER, mSizeInBytes, NULL, GLHardwareBufferManager::getGLUsage(mUsage));
	}
	if (mUsage & HBU_WRITE_ONLY)
	{
		access = GL_WRITE_ONLY;
	}else if (option == BWHardwareBuffer::HBL_READ_ONLY)
	{
		access = GL_READ_ONLY;
	}
	else
	{
		access = GL_READ_WRITE;
	}
	void *pBuffer = nullptr;
	CHECK_GL_ERROR(pBuffer = glMapBuffer(GL_ARRAY_BUFFER, access));
	assert(!pBuffer ? 0 : 1);
	void * ptr = static_cast<void*>(static_cast<unsigned char*>(pBuffer)+offset);
	return ptr;
}
void GLHardwareVertexBuffer::unLockImp()
{
	glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
	if (!glUnmapBuffer(GL_ARRAY_BUFFER))
	{
		assert(0);
	}
}
void GLHardwareVertexBuffer::updateFromShadowBuffer()
{
	if (mShadowUpdated && mUseShadowBuffer)
	{
		void * src = mShadowHardwareBuffer->lock(mLockStart, mLockLenth, BWHardwareBuffer::HBL_READ_ONLY);
		glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
		if (mLockStart == 0 && mLockLenth == mSizeInBytes)
		{
			glBufferData(GL_ARRAY_BUFFER, mSizeInBytes, src, GLHardwareBufferManager::getGLUsage(mUsage));
		}
		else
		{
			glBufferSubData(GL_ARRAY_BUFFER, mLockStart, mLockLenth, src);
		}
		mShadowHardwareBuffer->unlock();
	}
	mShadowUpdated = false;
}