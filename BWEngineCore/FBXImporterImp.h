#pragma once
#include "fbxsdk.h"
#include <functional>
#include <vector>
#include "BWBaseTemplate.h"
#include <map>

class FBXAttributeBaseImport;
class FBXImporterImp
{
public:
	typedef std::map<FbxNodeAttribute::EType, std::function<FBXAttributeBaseImport*()>> AllCreateFunctionType;
	static AllCreateFunctionType& GetAllCreateFunctionType();
public:
	void InitFBX(std::function<void(std::string, std::string)> PrintLogFunc);
	bool LoadFBX(const std::string &FileName);
	const std::vector<FBXAttributeBaseImport*>* GetAttributeDataImport(FbxNodeAttribute::EType Type);
	void ReleseData();
private:
	void LoadNode(FbxNode* Node);
	std::function<void(std::string, std::string)> PrintLog;
	FbxManager* mSdkManager;
	FbxScene* mScene;
	FbxImporter* mImporter;
	typedef std::map<FbxNodeAttribute::EType, std::vector<FBXAttributeBaseImport*>> AllAttributeType;
	AllAttributeType AllAttribute;

};
