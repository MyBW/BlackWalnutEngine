#pragma once
#include <string>
class BWMesh;
class BWDemoMeshImport
{
public:
	static bool Load(std::string FileName, BWMesh* Mesh);
};