#pragma once
#include "BWHardwareBuffer.h"
#include "BWPrimitive.h"

 template<typename TUniformBufferStruct>
 class TBWUniformBufferObject : public BWHardwareBuffer
 {
 public:
 };
 template<typename T>
 class TBWUniformBufferObjectPtr : public SmartPointer<TBWUniformBufferObject<T>>
 {
 public:
	 TBWUniformBufferObjectPtr() :SmartPointer<TBWUniformBufferObject<T>>()
	 {
	 }
 };

 enum EUniformBufferMemberBaseType
 {
	 UBMBT_INVALID,
	 UBMBT_BOOL,
	 UBMBT_INT32,
	 UBMBT_UINT32,
	 UBMBT_FLOAT32,
	 UBMBT_STRUCT
 };

 class UniformBufferStruct
 {
	 struct Member
	 {
		 std::string MemberName;
		 std::string MemberShaderType;
		 int NumRows;
		 int NumColums;
		 int NumElement;
	 };
	 std::vector<Member> AllMembers;
 };
 template<typename Type>
 struct UniformBufferMemberType
 {
	 enum { BaseType = UBMBT_INVALID };
	 enum { NumRows = 0 };
	 enum { NumColums = 0 };
	 enum { NumElements = 0 };
	 enum { Alignment = 0 };
	 enum { IsResource = 0 };
	 static const UniformBufferStruct* GetStruct() { return NULL; }
 };

 template<>
 class UniformBufferMemberType<int>
 {
	 enum { BaseType = UBMBT_INT32};
	 enum { NumRows = 1 };
	 enum { NumColums = 1 };
	 enum { NumElements = 0 };
	 enum { Alignment = 4 };
	 enum { IsResource = 0 };
	 static const UniformBufferStruct* GetStruct() { return NULL; }
 };
 template<>
 class UniformBufferMemberType<float>
 {
	 enum { BaseType = UBMBT_FLOAT32 };
	 enum { NumRows = 1 };
	 enum { NumColums = 1 };
	 enum { NumElements = 0 };
	 enum { Alignment = 4 };
	 enum { IsResource = 0 };
	 static const UniformBufferStruct* GetStruct() { return NULL; }
 };
 template<>
 class UniformBufferMemberType<BWVector3>
 {
	 enum { BaseType = UBMBT_FLOAT32 };
	 enum { NumRows = 1 };
	 enum { NumColums = 3 };
	 enum { NumElements = 0 };
	 enum { Alignment = 16 };
	 enum { IsResource = 0 };
	 static const UniformBufferStruct* GetStruct() { return NULL; }
 };

 template<>
 class UniformBufferMemberType<BWVector4>
 {
	 enum { BaseType = UBMBT_FLOAT32 };
	 enum { NumRows = 1 };
	 enum { NumColums = 4 };
	 enum { NumElements = 0 };
	 enum { Alignment = 16 };
	 enum { IsResource = 0 };
	 static const UniformBufferStruct* GetStruct() { return NULL; }
 };

 template<>
 class UniformBufferMemberType<BWMatrix4>
 {
	 enum { BaseType = UBMBT_FLOAT32 };
	 enum { NumRows = 4 };
	 enum { NumColums = 4 };
	 enum { NumElements = 0 };
	 enum { Alignment = 16 };
	 enum { IsResource = 0 };
	 static const UniformBufferStruct* GetStruct() { return NULL; }
 };


#define  BEGIN_UNIFORM_BUFFER_STRUCT(StructTypeName) \
class StructTypeName \
{\
public:\
	StructTypeName(); \
	static UniformBufferStruct StaticStruct; \
	static std::string CreateShaderUniformBlock(){ return std::string("");} \
    static TBWUniformBufferObjectPtr<StructTypeName> CreateUniformBufferObject(const StructTypeName& InUniformBufferStruct) \
	{\
		// Use RenderSystem To CreateUniformBufferObject
	}
	