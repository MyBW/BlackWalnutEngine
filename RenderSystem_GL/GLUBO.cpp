#include "GLUBO.h"
#include "GLPreDefine.h"

GLUniformBufferObject::GLUniformBufferObject(const UBOInfor & ubo, Usage usage):
BWHardwareBuffer(ubo.mUBOName, usage, false , false)
{
	CHECK_GL_ERROR(glGenBuffers(1, &mID));
	mData.mContentMember = ubo.mContentMember;
	mData.mIndex = new GLuint[ubo.mContentMember.size()];
	mData.mOffset = new GLint[ubo.mContentMember.size()];
	mData.mSize = new GLint[ubo.mContentMember.size()];
	mData.mType = new GLint[ubo.mContentMember.size()];
	mUBOName = ubo.mUBOName;
	mBinding = ubo.mBinding;
}

GLUniformBufferObject::~GLUniformBufferObject()
{
	glDeleteBuffers(1, &mID);
}

void GLUniformBufferObject::initUBO(GLuint program)
{

	GLuint BI = glGetUniformBlockIndex(program, mUBOName.c_str());
	CHECK_GL_ERROR(glUniformBlockBinding(program, BI, mBinding));
	GLint size;
	CHECK_GL_ERROR(glGetActiveUniformBlockiv(program, BI, GL_UNIFORM_BLOCK_DATA_SIZE, &size));
	mSizeInBytes = size;
	const char** names = new const char*[mData.mContentMember.size()];
	for (int i = 0; i < mData.mContentMember.size(); i++)
	{
		names[i] = mData.mContentMember[i].c_str();
	}
	glGetUniformIndices(program, mData.mContentMember.size(), names, mData.mIndex);
	glGetActiveUniformsiv(program, mData.mContentMember.size(), mData.mIndex, GL_UNIFORM_OFFSET, mData.mOffset);
	glGetActiveUniformsiv(program, mData.mContentMember.size(), mData.mIndex, GL_UNIFORM_SIZE, mData.mSize);
	glGetActiveUniformsiv(program, mData.mContentMember.size(), mData.mIndex, GL_UNIFORM_TYPE, mData.mType);
	
	CHECK_GL_ERROR(glBindBuffer(GL_UNIFORM_BUFFER, mID));
	CHECK_GL_ERROR(glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW));
	CHECK_GL_ERROR(glBindBufferBase(GL_UNIFORM_BUFFER, mBinding, mID));
	delete[] names;
}
void GLUniformBufferObject::bind()
{
	CHECK_GL_ERROR(glBindBuffer(GL_UNIFORM_BUFFER, mID));
}

void GLUniformBufferObject::setBindPoint(GLint binding)
{
	mBinding = binding;

}
const std::string & GLUniformBufferObject::getUBOName() const 
{
	return mUBOName;
}
GLint GLUniformBufferObject::getBindPoint() const 
{
	return  mBinding;
}
bool GLUniformBufferObject::isUniformInBuffer(const std::string &name) const
{
	ContentMember::const_iterator itor = mData.mContentMember.begin();
	while (itor != mData.mContentMember.end())
	{
		if ((*itor) == name)
		{
			return true;
		}
		itor++;
	}
	return false;
}
bool GLUniformBufferObject::setUniform(const std::string &name, void *data)
{
	for (int i = 0; i < mData.mContentMember.size(); i++)
	{
		if (name == mData.mContentMember[i])
		{

			void *buffer = lock(mData.mOffset[i] , mData.mSize[i] * GLTypeSize(mData.mType[i]), BWHardwareBuffer::HBL_READ_ONLY);
			if (buffer)
			{
				memcpy(buffer, data, mData.mSize[i] * GLTypeSize(mData.mType[i]));
			}
			unlock();
			return true;
		}
	}
	return false;
}
void* GLUniformBufferObject::lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option)
{
    if ((offset + length) > mSizeInBytes)
    {
		Log::GetInstance()->logMessage("GLUniformBufferObject::lockImp() : the buffer dont fit the need .\n", false);
		return NULL;
    }
	CHECK_GL_ERROR(glBindBuffer(GL_UNIFORM_BUFFER, mID));
	void *buffer = glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_ONLY);
	buffer =(unsigned char *)buffer + offset;
	return buffer;
}
void GLUniformBufferObject::unLockImp()
{
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}