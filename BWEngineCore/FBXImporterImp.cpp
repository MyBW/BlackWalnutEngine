#include "FBXImporterImp.h"
#include "FBXAttributeTypeData.h"
FBXImporterImp::AllCreateFunctionType& FBXImporterImp::GetAllCreateFunctionType()
{
	static AllCreateFunctionType AttributeImportCreateFunction;
	return AttributeImportCreateFunction;
}

void FBXImporterImp::InitFBX(std::function<void(std::string, std::string)> PrintLogFunc)
{
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	mSdkManager = FbxManager::Create();
	if (!mSdkManager)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		exit(1);
	}
	else FBXSDK_printf("Autodesk FBX SDK version %s\n", mSdkManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(mSdkManager, IOSROOT);
	mSdkManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	mSdkManager->LoadPluginsDirectory(lPath.Buffer());

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	mScene = FbxScene::Create(mSdkManager, "My Scene");
	if (!mScene)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}
	PrintLog = PrintLogFunc;
	AllCreateFunctionType::iterator CreateFunctionItor = GetAllCreateFunctionType().begin();
	for( ;CreateFunctionItor != GetAllCreateFunctionType().end() ; CreateFunctionItor++ )
	{
		FbxNodeAttribute::EType AttributeType = CreateFunctionItor->first;
		if (AllAttribute.find(AttributeType) == AllAttribute.end())
		{
			std::vector<FBXAttributeBaseImport*> ImportList;
			AllAttribute.insert(AllAttributeType::value_type(AttributeType, ImportList));
		}
	}
}

bool FBXImporterImp::LoadFBX(const std::string &FileName)
{
	if (mSdkManager)
	{
		int lFileFormat = -1;
		mImporter = FbxImporter::Create(mSdkManager, "");
		if (!mSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(FileName.c_str(), lFileFormat))
		{
			// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
			lFileFormat = mSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
		}
		// Initialize the importer by providing a filename.
		if (mImporter->Initialize(FileName.c_str(), lFileFormat) == false)
		{
			std::string ErrorString = mImporter->GetStatus().GetErrorString();
			PrintLog(std::string("FBXImport::LoadFBX"), ErrorString);
			return false;
		}
		mImporter->Import(mScene);

		FbxAxisSystem SceneAxisSystem = mScene->GetGlobalSettings().GetAxisSystem();
		FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
		if (SceneAxisSystem != OurAxisSystem)
		{
			OurAxisSystem.ConvertScene(mScene);
		}

		// Convert Unit System to what is used in this example, if needed
		FbxSystemUnit SceneSystemUnit = mScene->GetGlobalSettings().GetSystemUnit();
		if (SceneSystemUnit.GetScaleFactor() != 1.0)
		{
			//The unit in this example is centimeter.
			FbxSystemUnit::cm.ConvertScene(mScene);
		}
		FbxNode* SceneRootNode = mScene->GetRootNode();
		LoadNode(SceneRootNode);
		return  true;
	}
	return false;
}

const std::vector<FBXAttributeBaseImport*>* FBXImporterImp::GetAttributeDataImport(FbxNodeAttribute::EType Type)
{
	if (AllAttribute.find(Type) != AllAttribute.end())
	{
		return  &(AllAttribute.find(Type)->second);
	}
	return nullptr;
}

void FBXImporterImp::ReleseData()
{
	mScene->Destroy();
	mImporter->Destroy();
	mSdkManager->Destroy();
	for each (std::pair<FbxNodeAttribute::EType, std::vector<FBXAttributeBaseImport*>> var in AllAttribute)
	{
		for each (FBXAttributeBaseImport* AttributeBaseImport in var.second)
		{
			delete AttributeBaseImport;
		}
	}
}

void FBXImporterImp::LoadNode(FbxNode* Node)
{
	// µü´útypelist
	/*FbxNodeAttribute* NodeAttribute = Node->GetNodeAttribute();
	 for (int i = 0 ; i < length<AttributeTypeList>::value ; i++)
	 {
		  auto Data = get<1>(AttributeList);
		  if (!Data)
		  {
			  Data = new at<2, AttributeTypeList>::type();
		  }
		  if (Data->IsType(NodeAttribute->GetAttributeType()))
		  {
			  Data->LoadNode(Node);
		  }
	 }*/
	FbxNodeAttribute* NodeAttribute = Node->GetNodeAttribute();
	if (NodeAttribute)
	{
		FbxNodeAttribute::EType AttributeType = NodeAttribute->GetAttributeType();
		AllCreateFunctionType::iterator TypeFunction = GetAllCreateFunctionType().find(AttributeType);
		if (TypeFunction != GetAllCreateFunctionType().end())
		{
			std::function<FBXAttributeBaseImport*()> CreateFunction = TypeFunction->second;
			FBXAttributeBaseImport* Import = CreateFunction();
			Import->PrintLog = PrintLog;
			AllAttribute.find(AttributeType)->second.push_back(Import);
			Import->ImportNodeAttribute(Node);
		}
	}
	
	int ChildrenNum = Node->GetChildCount();
	for (int i = 0; i < ChildrenNum; i++)
	{
		LoadNode(Node->GetChild(i));
	}
}

