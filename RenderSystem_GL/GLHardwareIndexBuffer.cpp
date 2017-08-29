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
	// �ڽ���Ӧ�ú��Կ����ݽ�����ʱ��  ��һ����������  ��length�ĳ���С�ڸ����Ƶ�ʱ�� ʹ��glBufferSudData
	//�����ݳ��ȴ������Ƶ�ʱ�� ʹ��glMapBuffer   �����ȽϿ�  ��������Ҫ��ֻ�ж�win32ƽ̨���� ����ƽ̨ʹ��
	//glMapBuffer   ��������һ��ʹ��glMapBuffer  ����������Ϻ���Կ�ʼ����һ��
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