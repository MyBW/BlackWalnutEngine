#include "GLUniformBufferObject.h"

void NewGLUniformBufferObject::UploadData(char* Content, int Size)
{
	if (Size != mSizeInBytes)
	{
		mSizeInBytes = Size;
		CHECK_GL_ERROR(glBindBuffer(GL_UNIFORM_BUFFER, BufferID));
		CHECK_GL_ERROR(glBufferData(GL_UNIFORM_BUFFER, mSizeInBytes, NULL, GL_DYNAMIC_DRAW));
	}
	void* HardwareBuffer = lockImp(0, mSizeInBytes, BWHardwareBuffer::HBL_NORMAL);
	memcpy(HardwareBuffer, Content, mSizeInBytes);
	unLockImp();
}

void * NewGLUniformBufferObject::lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option)
{
	if ((offset + length) > mSizeInBytes)
	{
		Log::GetInstance()->logMessage("GLUniformBufferObject::lockImp() : the buffer dont fit the need .\n", false);
		return NULL;
	}
	CHECK_GL_ERROR(glBindBuffer(GL_UNIFORM_BUFFER, BufferID));
	void *buffer = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	buffer = (unsigned char *)buffer + offset;
	return buffer;
}

void NewGLUniformBufferObject::unLockImp()
{
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

