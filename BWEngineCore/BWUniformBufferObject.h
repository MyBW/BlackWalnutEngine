#pragma once

#include "BWHardwareBuffer.h"
#include "BWPrimitive.h"

 template<typename TUniformBufferStruct>
 class TBWUniformBufferObject : public BWHardwareBuffer
 {
 public:
	 TBWUniformBufferObject(const std::string &Name):BWHardwareBuffer(Name, BWHardwareBuffer::HBU_DYNAMIC, false, false)
	 {
		 Content = NULL;
	 }
	 void SetContent(const TUniformBufferStruct& SourceContent);
 private:
	 char* Content;
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

 class UniformBufferStructLayout
 {
 public:
	 struct Member
	 {
		 std::string MemberName;
		 std::string MemberShaderType;
		 EUniformBufferMemberBaseType BaseType;
		 int Offset;
		 int NumRows;
		 int NumColums;
		 int NumElement;
	 };


	 UniformBufferStructLayout(const std::string& Name, int AllStructSize, const std::vector<Member>& Members):
		 StrucName(Name),
		 AllMembers(Members),
		 AllSize(AllStructSize)
	 {
		 
	 }
	 std::vector<Member> AllMembers;
	 int AllSize;
	 std::string StrucName;
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
	 static const UniformBufferStructLayout* GetStruct() { return NULL; }
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
	 static const UniformBufferStructLayout* GetStruct() { return NULL; }
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
	 static const UniformBufferStructLayout* GetStruct() { return NULL; }
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
	 static const UniformBufferStructLayout* GetStruct() { return NULL; }
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
	 static const UniformBufferStructLayout* GetStruct() { return NULL; }
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
	 static const UniformBufferStructLayout* GetStruct() { return NULL; }
 };

#define IMPLEMEN_UNIFORM_BUFFER_STRUCT(StructTypeName)\
	UniformBufferStructLayout StructTypeName::StaticSruct(\
	std::string(##StructTypeName),\
    sizeof(StructTypeName),\
	StructTypeName::GetStructMember()\
);

#define BEGIN_UNIFORM_BUFFER_STRUCT(StructTypeName) \
class StructTypeName \
{\
public:\
	StructTypeName(); \
	static UniformBufferStructLayout StaticStruct; \
	static std::string CreateShaderUniformBlock(){ return std::string("");} \
    static TBWUniformBufferObjectPtr<StructTypeName> CreateUniformBufferObject(const StructTypeName& InUniformBufferStruct) \
	{\
      return TBWUniformBufferObjectPtr<StructTypeName>(); \
    }\
private:\
	 typedef StructTypeName zzThisStruct; \
 struct FirstMemberID { enum { IsResource = 0 }; }; \
	 static std::vector<UniformBufferStructLayout::Member> GetBeforeStructMember(FirstMemberID)\
 {\
	 return std::vector<UniformBufferStructLayout::Member>(); \
 }\
	 typedef FirstMemberID

#define UNIFORM_BUFFER_STRUCT_MEMBER(StructMemberType, StructMemberName)\
	MemberID##StructMemberName;\
public:\
	StructMemberType StructMemberName;\
private:\
   struct NextMemberID##StructMemberName{ enum { IsResource = 0 }; }; \
	static std::vector<UniformBufferStructLayout::Member> GetBeforeStructMember(NextMemberID##StructMemberName)\
	{\
		std::vector<UniformBufferStructLayout::Member> Outer = GetBeforeStructMember(MemberID##StructMemberName());\
		Outer.push_back(UniformBufferStructLayout::Member(\
	#StructMemberName, \
	#StructMemberType, \
	EUniformBufferMemberBaseType(\
		UniformBufferMemberType<StructMemberType>::BaseType), \
	offsetof(zzThisStruct, StructMemberName), \
	UniformBufferMemberType<StructMemberType>::NumRows, \
	UniformBufferMemberType<StructMemberType>::NumColums, \
	UniformBufferMemberType<StructMemberType>::NumElements, \
	UniformBufferMemberType<StructMemberType>::GetStruct()\
	)); \
	 return Outer;\
   }\
	 typedef NextMemberID##StructMemberName

#define END_UNIFORM_BUFFER_STRUCT(StructTypeName)\
	FinalMemberID##StructTypeName;\
public : \
	static std::vector<UniformBufferStructLayout::Member> GetStructMember()\
	{\
		return GetBeforeStructMember(FinalMemberID##StructTypeName());\
	}\
};\
template<>\
class UniformBufferMemberType<StructTypeName>\
{\
 public:\
	 enum { BaseType = UBMBT_STRUCT };\
	 enum { NumRows = 1 };\
	 enum { NumColums = 1 };\
	 enum { NumElements = 0 };\
	 enum { Alignment = 4 };\
	 enum { IsResource = 0 };\
	 static const UniformBufferStructLayout* GetStruct() { return &StructTypeName::StaticStruct; }\
 };


#include "BWUniformBufferObject.inl"