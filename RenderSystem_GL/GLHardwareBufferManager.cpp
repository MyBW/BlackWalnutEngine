#include "GLHardwareBufferManager.h"
#include "../BWEngineCore/BWHardwareBuffer.h"
#include "GLHardwareIndexBuffer.h"
#include "GLHardwareVertexBuffer.h"
GLenum GLHardwareBufferManager::getGLUsage(unsigned int usage)
{
	switch (usage)
	{
	case BWHardwareBuffer::HBU_STATIC_WRITE_ONLY:
	case BWHardwareBuffer::HBU_STATIC:
		return GL_STATIC_DRAW;
	case BWHardwareBuffer::HBU_DYNAMIC_WRITE_ONLY:
	case BWHardwareBuffer::HBU_DYNAMIC:
		return GL_DYNAMIC_DRAW;
	case BWHardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE:
		return GL_STREAM_DRAW;
	default:
		return GL_DYNAMIC_DRAW;
	}
}
GLenum GLHardwareBufferManager::getGLType(unsigned int type)
{
	switch (type)
	{
	case VET_FLOAT1:
	case VET_FLOAT2:
	case VET_FLOAT3:
	case VET_FLOAT4:
		return GL_FLOAT;
		break;
	case VET_SHORT1:
	case VET_SHORT2:
	case VET_SHORT3:
	case VET_SHORT4:
		return GL_SHORT;
	case VET_COLOR:
	case VET_UBYTE4:
	case VET_COLOUR_ARGB:
	case VET_COLOUR_ABGR:
		return GL_UNSIGNED_BYTE;
	}
	return 0;
}
BWHardwareIndexBufferPtr GLHardwareBufferManager::createIndexBuffer(BWHardwareIndexBuffer::IndexType type, size_t numIndexs,
	BWHardwareBuffer::Usage usage, bool useShadowBuffer)
{
	BWHardwareIndexBuffer *tmp = new GLHardwareIndexBuffer(this, type, numIndexs, usage, false, useShadowBuffer);
	mHardwareIndexBufferList.push_back(tmp);
	return tmp;
}
BWHardwareVertexBufferPtr GLHardwareBufferManager::createVertexBuffer(unsigned int vertexSize, unsigned int count,
	BWHardwareBuffer::Usage usage, bool useShadowBuffer)
{
	BWHardwareVertexBuffer *tmp = new GLHardwareVertexBuffer(this, vertexSize, count, usage, false, useShadowBuffer);
	mHardwareVertexBufferList.push_back(tmp);
	return tmp;
}