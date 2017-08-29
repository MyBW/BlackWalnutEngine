#include "BWMeshSerializer.h"
#include"BWLog.h"
BWMeshSerializer::BWMeshSerializer()
{
	BWMeshSerializerImp *tmp = new BWMeshSerializerImp();
	tmp->setVersion("[MeshSerializer_v1.41]");
	MeshVersionData *versionData = new MeshVersionData(MESH_VERSION_1_4 , tmp->getVersion() , tmp);
	mMeshVersionDataVector.push_back(versionData);
}
BWMeshSerializer::~BWMeshSerializer()
{
	MeshVersionDataVector::iterator itor = mMeshVersionDataVector.begin();
	while (itor != mMeshVersionDataVector.end())
	{
		delete (*itor);
		itor++;
	}
}
void BWMeshSerializer::importMesh(BWDataStreamPrt &dataStream,  BWMesh *pMesh)
{
	determineEndianness(dataStream);
	unsigned short  headerID;

	readShorts(dataStream, &headerID);

	if (headerID != HEAD_STREAM_ID)
	{
		assert(0);
	}
	std::string version = readString(dataStream);
	Log::GetInstance()->logMessage(version);
	dataStream->Seek(0);
	BWMeshSerializerImp *Imp = NULL;
	for (MeshVersionDataVector::iterator i = mMeshVersionDataVector.begin(); i != mMeshVersionDataVector.end() ; i++)
	{
		if ((*i)->mVersionString == version)
		{
			Imp = (*i)->mMeshSerializerImp;
			break; 
		}
	}
	if (Imp == NULL)
	{
		assert(0);
	}
	Imp->importMesh(dataStream, pMesh);
}