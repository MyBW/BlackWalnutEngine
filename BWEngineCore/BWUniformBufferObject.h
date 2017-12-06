#pragma once

#include "BWHardwareBuffer.h"
#include "BWPrimitive.h"
#include "BWTexture.h"
#include "BWRoot.h"
#include "BWRenderSystem.h"
class BWUniformBufferObject :public BWHardwareBuffer
{
public:
	BWUniformBufferObject(const std::string &Name) :BWHardwareBuffer(Name, BWHardwareBuffer::HBU_DYNAMIC, false, false)
	{
	}
	virtual ~BWUniformBufferObject()
	{

	}
	virtual void UploadData(char* Content, int Size){ }

};

template<typename T,unsigned int Alignment>
class TAlignmentdef;

#define IMPLEMENT_ALIGNED_TYPE(Alignment)\
template<typename T>\
class TAlignmentdef<T, Alignment>\
{\
public:\
	typedef __declspec(align(Alignment)) T TAlignedType ; \
};
IMPLEMENT_ALIGNED_TYPE(1)
IMPLEMENT_ALIGNED_TYPE(2)
IMPLEMENT_ALIGNED_TYPE(4)
IMPLEMENT_ALIGNED_TYPE(8)
IMPLEMENT_ALIGNED_TYPE(16)

 template<typename TUniformBufferStruct>
 class TBWUniformBufferObject
 {
 public:
	 TBWUniformBufferObject(const std::string &Name)
	 {
		 // Because Use The Template, The Size Is Constant
		 Content = NULL;
		 mSizeInBytes = sizeof(TUniformBufferStruct);
		 Content = new char[sizeof(TUniformBufferStruct)];
		 memset(Content, mSizeInBytes, 0);
		 UniformBufferObject = NULL;
		 StructName = Name;
	 }
	 void UploadData();
	 void SetContent(const TUniformBufferStruct& SourceContent);
	 BWUniformBufferObject* GetUniformBufferObject() { return UniformBufferObject; }
	 ~TBWUniformBufferObject()
	 {
		 if (Content) delete Content;
		 delete UniformBufferObject;
	 }
 protected:
	 char* Content;
	 int mSizeInBytes;
	 std::string StructName;
	 BWUniformBufferObject* UniformBufferObject;
 };

 
 template<typename T>
 class TBWUniformBufferObjectPtr : public SmartPointer<TBWUniformBufferObject<T>>
 {
 public:
	 TBWUniformBufferObjectPtr() :SmartPointer<TBWUniformBufferObject<T>>()
	 {
	 }
	 TBWUniformBufferObjectPtr(TBWUniformBufferObject<T>* TUniformBufferObject)
		 :SmartPointer<TBWUniformBufferObject<T>>(TUniformBufferObject)
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
		 const UniformBufferStructLayout* Layout;
		 Member(const std::string &InMemberName, const std::string &InMemberShaderType, EUniformBufferMemberBaseType InBaseType,int InOffset,int InNumRows,int InNumColums,int InNumElement,const UniformBufferStructLayout* InLayout):
			 MemberName(InMemberName), 
			 MemberShaderType(InMemberShaderType),
			 BaseType(InBaseType),
			 Offset(InOffset),
			 NumRows(InNumRows),
			 NumColums(InNumColums),
			 NumElement(InNumElement),
			 Layout(InLayout)
		 { }
	 };
	 UniformBufferStructLayout(const std::string& Name, int AllStructSize, const std::vector<Member>& Members):
		 StrucName(Name),
		 AllMembers(Members),
		 AllSize(AllStructSize)
	 {
		 
	 }
	 bool IsHaveMember(const std::string& Name)
	 {
		 for each (const Member& MemberEle in AllMembers)
		 {
			 if (MemberEle.MemberName == Name)
			 {
				 return true;
			 }
		 }
		 return false;
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
 struct UniformBufferMemberType<int>
 {
	 enum { BaseType = UBMBT_INT32};
	 enum { NumRows = 1 };
	 enum { NumColums = 1 };
	 enum { NumElements = 0 };
	 enum { Alignment = 4 };
	 enum { IsResource = 0 };
	 typedef int TAlignedType;
	 static const UniformBufferStructLayout* GetStruct() { return NULL; }
 };
 template<>
 struct UniformBufferMemberType<float>
 {
	 enum { BaseType = UBMBT_FLOAT32 };
	 enum { NumRows = 1 };
	 enum { NumColums = 1 };
	 enum { NumElements = 0 };
	 enum { Alignment = 4 };
	 enum { IsResource = 0 };
	 typedef float TAlignedType;
	 static const UniformBufferStructLayout* GetStruct() { return NULL; }
 };
 template<>
 struct UniformBufferMemberType<BWPoint2DD>
 {
	 enum { BaseType = UBMBT_FLOAT32 };
	 enum { NumRows = 1 };
	 enum { NumColums = 2 };
	 enum { NumElements = 0 };
	 enum { Alignment = 8 };
	 enum { IsResource = 0 };
	 typedef TAlignmentdef<BWPoint2DD, Alignment>::TAlignedType TAlignedType;
	 static const UniformBufferStructLayout* GetStruct() { return NULL; }
 };
 
 template<>
 struct UniformBufferMemberType<BWVector3>
 {
	 enum { BaseType = UBMBT_FLOAT32 };
	 enum { NumRows = 1 };
	 enum { NumColums = 3 };
	 enum { NumElements = 0 };
	 enum { Alignment = 16 };
	 enum { IsResource = 0 };
	 typedef TAlignmentdef<BWVector3, Alignment>::TAlignedType TAlignedType;
	 static const UniformBufferStructLayout* GetStruct() { return NULL; }
 };

 template<>
 struct UniformBufferMemberType<BWVector4>
 {
	 enum { BaseType = UBMBT_FLOAT32 };
	 enum { NumRows = 1 };
	 enum { NumColums = 4 };
	 enum { NumElements = 0 };
	 enum { Alignment = 16 };
	 enum { IsResource = 0 };
	 typedef TAlignmentdef<BWVector3, Alignment>::TAlignedType TAlignedType;
	 static const UniformBufferStructLayout* GetStruct() { return NULL; }
 };
 template<>
 struct UniformBufferMemberType<BWMatrix4>
 {
	 enum { BaseType = UBMBT_FLOAT32 };
	 enum { NumRows = 4 };
	 enum { NumColums = 4 };
	 enum { NumElements = 0 };
	 enum { Alignment = 16 };
	 enum { IsResource = 0 };
	 typedef TAlignmentdef<BWMatrix4, Alignment>::TAlignedType TAlignedType;
	 static const UniformBufferStructLayout* GetStruct() { return NULL; }
 };

#define IMPLEMEN_UNIFORM_BUFFER_STRUCT(StructTypeName)\
	UniformBufferStructLayout StructTypeName::StaticStruct(\
	std::string(#StructTypeName),\
    sizeof(StructTypeName),\
	StructTypeName::GetStructMember()\
);

#define BEGIN_UNIFORM_BUFFER_STRUCT(StructTypeName) \
class StructTypeName \
{\
public:\
	StructTypeName(){ } \
	static UniformBufferStructLayout StaticStruct; \
	static std::string CreateShaderUniformBlock(){ return std::string("");} \
    static TBWUniformBufferObjectPtr<StructTypeName> CreateUniformBufferObject() \
	{\
      return new TBWUniformBufferObject<StructTypeName>(#StructTypeName); \
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
	typedef UniformBufferMemberType<StructMemberType>::TAlignedType Aligned##StructMemberName;\
	Aligned##StructMemberName StructMemberName ;\
private:\
   struct NextMemberID##StructMemberName{ enum { IsResource = 0 }; }; \
	static std::vector<UniformBufferStructLayout::Member> GetBeforeStructMember(NextMemberID##StructMemberName)\
	{\
		std::vector<UniformBufferStructLayout::Member> Outer = GetBeforeStructMember(MemberID##StructMemberName());\
		Outer.push_back(UniformBufferStructLayout::Member(\
	#StructMemberName, \
	#StructMemberType, \
	EUniformBufferMemberBaseType(UniformBufferMemberType<StructMemberType>::BaseType), \
	(int)offsetof(zzThisStruct, StructMemberName), \
	(int)UniformBufferMemberType<StructMemberType>::NumRows, \
	(int)UniformBufferMemberType<StructMemberType>::NumColums, \
	(int)UniformBufferMemberType<StructMemberType>::NumElements, \
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