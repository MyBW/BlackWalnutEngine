

template<typename UniformBufferStruct>
void TGLUniformBufferObject<UniformBufferStruct>::UploadData()
{
	void* HardwareBuffer = lockImp(0, sizeof(UniformBufferStruct), BWHardwareBuffer::HBL_NORMAL);
	memcpy(HardwareBuffer, Content, sizeof(UniformBufferStruct));
	unLockImp();
}

template<typename UniformBufferStruct>
void * TGLUniformBufferObject<UniformBufferStruct>::lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option)
{
	if ((offset + length) > mSizeInBytes)
	{
		Log::GetInstance()->logMessage("GLUniformBufferObject::lockImp() : the buffer dont fit the need .\n", false);
		return NULL;
	}
	CHECK_GL_ERROR(glBindBuffer(GL_UNIFORM_BUFFER, mID));
	void *buffer = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	buffer = (unsigned char *)buffer + offset;
	return buffer;
}
template<typename UniformBufferStruct>
void TGLUniformBufferObject<UniformBufferStruct>::unLockImp()
{
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}