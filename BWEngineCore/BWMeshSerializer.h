#ifndef MESHSERIALIZER_H_
#define MESHSERIALIZER_H_
#include "BWSerializer.h"
#include "BWMesh.h"
#include <vector>
#include "BWMeshSerializerImp.h"
enum MeshVersion
{
	MESH_VERSION_LATEST,
	MESH_VERSION_1_8,
	MESH_VERSION_1_7,
	MESH_VERSION_1_4,
	MESH_VERSION_1_0,
	MESH_VERSION_LEGACY
};

class BWMeshSerializer : public BWSerializer
{
public:
	BWMeshSerializer();
	~BWMeshSerializer();
	/*void exportMesh(const BWMesh *pMesh, const std::string &fileName, Endian endian = BWSerializer::ENDIAN_NATIVE);
	void exportMesh(const BWMesh *pMehs, const std::string &fileName, MeshVersion version, Endian endian = BWSerializer::ENDIAN_NATIVE);
	void exportMesh(const BWMesh *pMesh, BWDataStreamPrt & dataStream, Endian endian = BWSerializer::ENDIAN_NATIVE);
	void exportMesh(const BWMesh *pMesh, BWDataStreamPrt &dataStream, MeshVersion version, Endian endian = BWSerializer::ENDIAN_NATIVE);*/
	void importMesh( BWDataStreamPrt &dataStream,  BWMesh *pMesh);
	class MeshVersionData 
	{
	public:
		MeshVersion mMeshVersion;
		std::string mVersionString;
		BWMeshSerializerImp *mMeshSerializerImp;
		MeshVersionData(MeshVersion meshVersion, std::string versionString, BWMeshSerializerImp *meshserializerImp) :mMeshVersion(meshVersion), mVersionString(versionString), mMeshSerializerImp(meshserializerImp)
		{

		}
		~MeshVersionData(){ delete mMeshSerializerImp; }

	protected:
	private:
	};
	typedef std::vector<MeshVersionData*> MeshVersionDataVector;
	MeshVersionDataVector mMeshVersionDataVector;
private:

};

#endif