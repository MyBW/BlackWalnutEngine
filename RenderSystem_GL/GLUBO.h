#ifndef GLUBO_H_
#define GLUBO_H_
#include <string>
#include <vector>
#include <map>
#include "../BWEngineCore/BWHardwareBuffer.h"
#include "GL/include/glew.h"
typedef std::vector<std::string> ContentMember;
struct UBOInfor
{
	std::string mUBOName;
	GLuint  mBinding;
	ContentMember mContentMember;
	UBOInfor() :mBinding(-1){}
	UBOInfor(const UBOInfor &uboInfo) :mUBOName(uboInfo.mUBOName),mBinding(uboInfo.mBinding),mContentMember(uboInfo.mContentMember)
	{
	}
};
typedef std::map<std::string, UBOInfor> UBOInforMap;

class GLUniformBufferObject : public BWHardwareBuffer
{
public:
	struct Data
	{
		ContentMember mContentMember;
		GLuint *mIndex; 
		GLint *mOffset;
		GLint *mSize;
		GLint *mType;
	};
	GLUniformBufferObject(const UBOInfor & ubo , Usage usage);
	~GLUniformBufferObject();
	void initUBO(GLuint program);
	void setBindPoint(GLint binding);
	bool setUniform(const std::string &name, void *data);
	const std::string & getUBOName() const;
	GLint getBindPoint() const;
	bool isUniformInBuffer(const std::string &name) const;
	void bind();
private:
	void *lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option);
	void unLockImp();
	GLuint mID;
	std::string mUBOName;
	Data mData;
	GLint mBinding;
};

typedef std::map<std::string , GLUniformBufferObject*> UBOMap;

#endif