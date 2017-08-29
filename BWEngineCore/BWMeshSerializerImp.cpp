#include "BWMeshSerializerImp.h"
#include "BWHardwareIndexBuffer.h"
#include "BWHardwareBufferManager.h"
#include "BWMeshFileFormat.h"
#include "BWColorValue.h"
#include "BWMesh.h"
void BWMeshSerializerImp::importMesh(BWDataStreamPrt &dataStream, BWMesh *mesh)
{ 
	determineEndianness(dataStream);
	readFileHeader(dataStream);
	unsigned int streamID;
	while (!dataStream->Eof())
	{
		streamID = readChunk(dataStream);
		switch (streamID)
		{
		case  M_MESH:
			readMesh(dataStream, mesh);
		default:
			break;
		}
	}
}

void BWMeshSerializerImp::readTextureLayer(BWDataStreamPrt &dataStream, BWMesh *mesh, BWMaterialPtr &material)
{

}
void BWMeshSerializerImp::readMeshLodInfo(BWDataStreamPrt& stream, BWMesh* pMesh)
{
	unsigned short streamID, i;

	// Read the strategy to be used for this mesh
	std::string strategyName = readString(stream);
	/*LodStrategy *strategy = LodStrategyManager::getSingleton().getStrategy(strategyName);
	pMesh->setLodStrategy(strategy);*/

	// unsigned short numLevels;
	  readShorts(stream, &(pMesh->mNumLods), 1);
	// bool manual;  (true for manual alternate meshes, false for generated)
	readBool(stream, &(pMesh->mIsLodManual), 1);
	

	// Preallocate submesh lod face data if not manual
	if (!pMesh->mIsLodManual)
	{
		unsigned short numsubs = pMesh->getNumSubMeshes();
		for (i = 0; i < numsubs; ++i)
		{
			BWSubMesh* sm = pMesh->getSubMesh(i);
			sm->mLodFaceList.resize(pMesh->mNumLods - 1);
		}
	}

	// Loop from 1 rather than 0 (full detail index is not in file)
	for (i = 1; i < pMesh->mNumLods; ++i)
	{
		streamID = readChunk(stream);
		if (streamID != M_MESH_LOD_USAGE)
		{
			/*OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
				"Missing M_MESH_LOD_USAGE stream in " + pMesh->getName(),
				"MeshSerializerImpl::readMeshLodInfo");*/
			assert(0);
		}
		// Read depth
		/*
		MeshLodUsage usage;
		readFloats(stream, &(usage.userValue), 1);
*/
		float userValue;
		readFloats(stream, &userValue, 1);
		//if (pMesh->isLodManual())
		//{
		//	readMeshLodUsageManual(stream, pMesh, i, usage);
		//}
		//else //(!pMesh->isLodManual)
		//{
		//	readMeshLodUsageGenerated(stream, pMesh, i, usage);
		//}
		//usage.edgeData = NULL;

		//// Save usage
		//pMesh->mMeshLodUsageList.push_back(usage);

		if (pMesh->isLodManual())
		{
			readMeshLodUsageManual(stream, pMesh, i);
		}
		else
		{
			readMeshLodUsageGenerated(stream, pMesh, i);
		}
	}


}
void BWMeshSerializerImp::readMeshLodUsageManual(BWDataStreamPrt& stream,
	BWMesh* pMesh, unsigned short lodNum)
{
	unsigned long streamID;
	// Read detail stream
	streamID = readChunk(stream);
	if (streamID != M_MESH_LOD_MANUAL)
	{
		/*OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
			"Missing M_MESH_LOD_MANUAL stream in " + pMesh->getName(),
			"MeshSerializerImpl::readMeshLodUsageManual");*/
		assert(0);
	}
	std::string manualName = readString(stream);
	//usage.manualName = readString(stream);
	//usage.manualMesh.setNull(); // will trigger load later
}
//---------------------------------------------------------------------
void BWMeshSerializerImp::readMeshLodUsageGenerated(BWDataStreamPrt& stream,
	BWMesh* pMesh, unsigned short lodNum)
{
	//usage.manualName = "";
	//usage.manualMesh.setNull();

	// Get one set of detail per SubMesh
	unsigned short numSubs, i;
	unsigned long streamID;
	numSubs = pMesh->getNumSubMeshes();
	for (i = 0; i < numSubs; ++i)
	{
		streamID = readChunk(stream);
		if (streamID != M_MESH_LOD_GENERATED)
		{
			/*OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
				"Missing M_MESH_LOD_GENERATED stream in " + pMesh->getName(),
				"MeshSerializerImpl::readMeshLodUsageGenerated");*/
		}

		BWSubMesh* sm = pMesh->getSubMesh(i);
		// lodNum - 1 because SubMesh doesn't store full detail LOD
		sm->mLodFaceList[lodNum - 1] = new IndexData();
		IndexData* indexData = sm->mLodFaceList[lodNum - 1];
		// unsigned int numIndexes
		unsigned int numIndexes;
		readInts(stream, &numIndexes, 1);
		indexData->mIndexCount = static_cast<size_t>(numIndexes);
		// bool indexes32Bit
		bool idx32Bit;
		readBool(stream, &idx32Bit, 1);
		// unsigned short*/int* faceIndexes;  ((v1, v2, v3) * numFaces)
		if (idx32Bit)
		{
			indexData->mIndexBuffer = BWHardwareBufferManager::GetInstance()->
				createIndexBuffer(BWHardwareIndexBuffer::IT_32BIT, indexData->mIndexCount,
				pMesh->mIndexBufferUsage, pMesh->mIndexBufferShadowed);
			unsigned int* pIdx = static_cast<unsigned int*>(
				indexData->mIndexBuffer->lock(
				0,
				indexData->mIndexBuffer->getSizeInBytes(),
				BWHardwareBuffer::HBL_DISCARD));

			readInts(stream, pIdx, indexData->mIndexCount);
			indexData->mIndexBuffer->unlock();

		}
		else
		{
			indexData->mIndexBuffer = BWHardwareBufferManager::GetInstance()->
				createIndexBuffer(BWHardwareIndexBuffer::IT_16BIT, indexData->mIndexCount,
				pMesh->mIndexBufferUsage, pMesh->mIndexBufferShadowed);
			unsigned short* pIdx = static_cast<unsigned short*>(
				indexData->mIndexBuffer->lock(
				0,
				indexData->mIndexBuffer->getSizeInBytes(),
				BWHardwareBuffer::HBL_DISCARD));
			readShorts(stream, pIdx, indexData->mIndexCount);
			indexData->mIndexBuffer->unlock();

		}

	}
}
void BWMeshSerializerImp::readBoundsInfo(BWDataStreamPrt& stream, BWMesh* pMesh)
{
	//Vector3 min, max;
	float minx, miny, minz;
	readFloats(stream, &minx, 1);
	readFloats(stream, &miny, 1);
	readFloats(stream, &minz, 1);
	float maxx, maxy, maxz;
	readFloats(stream, &maxx, 1);
	readFloats(stream, &maxy, 1);
	readFloats(stream, &maxz, 1);
	//AxisAlignedBox box(min, max);
	//pMesh->_setBounds(box, true);
	// float radius
	float radius;
	readFloats(stream, &radius, 1);
	//pMesh->_setBoundingSphereRadius(radius);
}
void BWMeshSerializerImp::readSubMeshNameTable(BWDataStreamPrt& stream, BWMesh* pMesh)
{
	// The map for
	std::map<unsigned short, std::string> subMeshNames;
	unsigned short streamID, subMeshIndex;

	// Need something to store the index, and the objects name
	// This table is a method that imported meshes can retain their naming
	// so that the names established in the modelling software can be used
	// to get the sub-meshes by name. The exporter must support exporting
	// the optional stream M_SUBMESH_NAME_TABLE.

	// Read in all the sub-streams. Each sub-stream should contain an index and Ogre::String for the name.
	if (!stream->Eof())
	{
		streamID = readChunk(stream);
		while (!stream->Eof() && (streamID == M_SUBMESH_NAME_TABLE_ELEMENT))
		{
			// Read in the index of the submesh.
			readShorts(stream, &subMeshIndex, 1);
			// Read in the String and map it to its index.
			subMeshNames[subMeshIndex] = readString(stream);

			// If we're not end of file get the next stream ID
			if (!stream->Eof())
				streamID = readChunk(stream);
		}
		if (!stream->Eof())
		{
			// Backpedal back to start of stream
			stream->Skip(-STREAM_OVERHEAD_SIZE);
		}
	}

	// Set all the submeshes names
	// ?

	// Loop through and save out the index and names.
	std::map<unsigned short, std::string>::const_iterator it = subMeshNames.begin();

	while (it != subMeshNames.end())
	{
		// Name this submesh to the stored name.
		pMesh->nameSubMesh(it->second, it->first);
		++it;
	}
}
void BWMeshSerializerImp::readEdgeList(BWDataStreamPrt& stream, BWMesh* pMesh)
{
	unsigned short streamID;

	if (!stream->Eof())
	{
		streamID = readChunk(stream);
		while (!stream->Eof() &&
			streamID == M_EDGE_LIST_LOD)
		{
			// Process single LOD

			// unsigned short lodIndex
			unsigned short lodIndex;
			readShorts(stream, &lodIndex, 1);

			// bool isManual			// If manual, no edge data here, loaded from manual mesh
			bool isManual;
			readBool(stream, &isManual, 1);
			// Only load in non-manual levels; others will be connected up by Mesh on demand
			if (!isManual)
			{
				/*MeshLodUsage& usage = const_cast<MeshLodUsage&>(pMesh->getLodLevel(lodIndex));

				usage.edgeData = OGRE_NEW EdgeData();
*/
				// Read detail information of the edge list
				readEdgeListLodInfo(stream/*, usage.edgeData*/);

				// Postprocessing edge groups
				/*EdgeData::EdgeGroupList::iterator egi, egend;
				egend = usage.edgeData->edgeGroups.end();*/
				//for (egi = usage.edgeData->edgeGroups.begin(); egi != egend; ++egi)
				//{
				//	EdgeData::EdgeGroup& edgeGroup = *egi;
				//	// Populate edgeGroup.vertexData pointers
				//	// If there is shared vertex data, vertexSet 0 is that,
				//	// otherwise 0 is first dedicated
				//	if (pMesh->sharedVertexData)
				//	{
				//		if (edgeGroup.vertexSet == 0)
				//		{
				//			edgeGroup.vertexData = pMesh->sharedVertexData;
				//		}
				//		else
				//		{
				//			edgeGroup.vertexData = pMesh->getSubMesh(
				//				(unsigned short)edgeGroup.vertexSet - 1)->vertexData;
				//		}
				//	}
				//	else
				//	{
				//		edgeGroup.vertexData = pMesh->getSubMesh(
				//			(unsigned short)edgeGroup.vertexSet)->vertexData;
				//	}
				//}
			}

			if (!stream->Eof())
			{
				streamID = readChunk(stream);
			}

		}
		if (!stream->Eof())
		{
			// Backpedal back to start of stream
			stream->Skip(-STREAM_OVERHEAD_SIZE);
		}
	}

	//pMesh->mEdgeListsBuilt = true;
}
void BWMeshSerializerImp::readEdgeListLodInfo(BWDataStreamPrt& stream
	/*EdgeData* edgeData*/)
{
	bool isClosed;
	readBool(stream, &isClosed, 1);
	// unsigned long numTriangles
	unsigned int numTriangles;
	readInts(stream, &numTriangles, 1);
	// Allocate correct amount of memory
	/*edgeData->triangles.resize(numTriangles);
	edgeData->triangleFaceNormals.resize(numTriangles);
	edgeData->triangleLightFacings.resize(numTriangles);*/
	// unsigned long numEdgeGroups
	unsigned int numEdgeGroups;
	readInts(stream, &numEdgeGroups, 1);
	// Allocate correct amount of memory
	/*edgeData->edgeGroups.resize(numEdgeGroups);*/
	// Triangle* triangleList
	unsigned int tmp[3];
	for (size_t t = 0; t < numTriangles; ++t)
	{
		//EdgeData::Triangle& tri = edgeData->triangles[t];
		// unsigned long indexSet
		readInts(stream, tmp, 1);
		//tri.indexSet = tmp[0];
		// unsigned long vertexSet
		readInts(stream, tmp, 1);
		//tri.vertexSet = tmp[0];
		// unsigned long vertIndex[3]
		readInts(stream, tmp, 3);
		/*tri.vertIndex[0] = tmp[0];
		tri.vertIndex[1] = tmp[1];
		tri.vertIndex[2] = tmp[2];
		*/// unsigned long sharedVertIndex[3]
		readInts(stream, tmp, 3);
		/*tri.sharedVertIndex[0] = tmp[0];
		tri.sharedVertIndex[1] = tmp[1];
		tri.sharedVertIndex[2] = tmp[2];*/
		float normal[4];
		readFloats(stream, normal, 4);

	}

	for (unsigned int eg = 0; eg < numEdgeGroups; ++eg)
	{
		unsigned short streamID = readChunk(stream);
		if (streamID != M_EDGE_GROUP)
		{
			/*OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR,
				"Missing M_EDGE_GROUP stream",
				"MeshSerializerImpl::readEdgeListLodInfo");*/
		}
		//EdgeData::EdgeGroup& edgeGroup = edgeData->edgeGroups[eg];

		// unsigned long vertexSet
		readInts(stream, tmp, 1);
		//edgeGroup.vertexSet = tmp[0];
		// unsigned long triStart
		readInts(stream, tmp, 1);
		//edgeGroup.triStart = tmp[0];
		// unsigned long triCount
		readInts(stream, tmp, 1);
		//edgeGroup.triCount = tmp[0];
		// unsigned long numEdges
		unsigned int numEdges;
		readInts(stream, &numEdges, 1);
		//edgeGroup.edges.resize(numEdges);
		// Edge* edgeList
		for (unsigned int e = 0; e < numEdges; ++e)
		{
			//EdgeData::Edge& edge = edgeGroup.edges[e];
			// unsigned long  triIndex[2]
			readInts(stream, tmp, 2);
			//edge.triIndex[0] = tmp[0];
			//edge.triIndex[1] = tmp[1];
			// unsigned long  vertIndex[2]
			readInts(stream, tmp, 2);
			//edge.vertIndex[0] = tmp[0];
			//edge.vertIndex[1] = tmp[1];
			// unsigned long  sharedVertIndex[2]
			readInts(stream, tmp, 2);
			//edge.sharedVertIndex[0] = tmp[0];
			//edge.sharedVertIndex[1] = tmp[1];
			bool degenerate;
			readBool(stream, &degenerate, 1);
		}
	}
}
void BWMeshSerializerImp::readMesh(BWDataStreamPrt &dataStream, BWMesh *pMesh)
{
	// Never automatically build edge lists for this version
	// expect them in the file or not at all
	pMesh->mAutoBuildEdgeLists = false;

	// bool skeletallyAnimated
	bool skeletallyAnimated;
	readBool(dataStream, &skeletallyAnimated, 1);

	// Find all substreams
	if (!dataStream->Eof())
	{
		unsigned short streamID = readChunk(dataStream);
		while (!dataStream->Eof() &&
			(streamID == M_GEOMETRY ||
			streamID == M_SUBMESH ||
			streamID == M_MESH_SKELETON_LINK ||
			streamID == M_MESH_BONE_ASSIGNMENT ||
			streamID == M_MESH_LOD ||
			streamID == M_MESH_BOUNDS ||
			streamID == M_SUBMESH_NAME_TABLE ||
			streamID == M_EDGE_LISTS ||
			streamID == M_POSES ||
			streamID == M_ANIMATIONS ||
			streamID == M_TABLE_EXTREMES))
		{
			switch (streamID)
			{
			case M_GEOMETRY:
				pMesh->sharedVertexData = new VertexData();
				readGeometry(dataStream, pMesh, pMesh->sharedVertexData.Get());
				break;
			case M_SUBMESH:
				readSubMesh(dataStream, pMesh);
				break;
			case M_MESH_SKELETON_LINK:
				//readSkeletonLink(dataStream, pMesh, listener);
				assert(0);
				break;
			case M_MESH_BONE_ASSIGNMENT:
				//readMeshBoneAssignment(dataStream, pMesh);
				assert(0);
				break;
			case M_MESH_LOD:
				readMeshLodInfo(dataStream, pMesh);
				break;
			case M_MESH_BOUNDS: // 包围盒
				readBoundsInfo(dataStream, pMesh);
				break;
			case M_SUBMESH_NAME_TABLE:
				readSubMeshNameTable(dataStream, pMesh);
				break;
			case M_EDGE_LISTS:
				readEdgeList(dataStream, pMesh);
				break;
			case M_POSES:
				//readPoses(dataStream, pMesh);
				assert(0);
				break;
			case M_ANIMATIONS:
				//readAnimations(dataStream, pMesh);
				assert(0);
				break;
			case M_TABLE_EXTREMES:
				//readExtremes(dataStream, pMesh);
				assert(0);
				break;
			}

			if (!dataStream->Eof())
			{
				streamID = readChunk(dataStream);
			}

		}
		if (!dataStream->Eof())
		{
			dataStream->Skip(-STREAM_OVERHEAD_SIZE);
		}
	}

}
void BWMeshSerializerImp::readSubMesh(BWDataStreamPrt &dataStream, BWMesh *mesh)
{
	unsigned short streamID;
	BWSubMesh *subMesh = mesh->createSubMesh();
	std::string materialName = readString(dataStream);
	subMesh->setMaterialName(materialName, mesh->GetGroupName());
	readBool(dataStream, &subMesh->mIsUseSharedVertex, 1);
	subMesh->mIndexData->mIndexStart = 0;
	unsigned int indexCount = 0;
	readInts(dataStream, &indexCount, 1);
	subMesh->mIndexData->mIndexCount = indexCount;

	BWHardwareIndexBufferPtr ibuf;
	bool idx32bit;
	readBool(dataStream, &idx32bit, 1);
	if (indexCount >0)
	{
		if (idx32bit)
		{
			ibuf = BWHardwareBufferManager::GetInstance()->createIndexBuffer(BWHardwareIndexBuffer::IT_32BIT,
				subMesh->mIndexData->mIndexCount,
				mesh->mIndexBufferUsage,
				mesh->mIndexBufferShadowed);
			unsigned int *pIdx = static_cast<unsigned int*>(ibuf->lock(BWHardwareBuffer::HBL_DISCARD));
			readInts(dataStream, pIdx, subMesh->mIndexData->mIndexCount);
			ibuf->unlock();
		}
		else
		{
			ibuf = BWHardwareBufferManager::GetInstance()->createIndexBuffer(BWHardwareIndexBuffer::IT_16BIT,
				subMesh->mIndexData->mIndexCount,
				mesh->mIndexBufferUsage,
				mesh->mIndexBufferShadowed);
			unsigned short *pIdx = static_cast<unsigned short*>(ibuf->lock(BWHardwareBuffer::HBL_DISCARD));
			readShorts(dataStream, pIdx, subMesh->mIndexData->mIndexCount);
			ibuf->unlock();
		}
	}
	subMesh->mIndexData->mIndexBuffer = ibuf;
    if (!subMesh->mIsUseSharedVertex)
    {
		streamID = readChunk(dataStream);
		if (streamID != M_GEOMETRY)
		{
			assert(0);
		}
		subMesh->mVertexData = new VertexData();
		readGeometry(dataStream, mesh, subMesh->mVertexData.Get());
    }
	if (!dataStream->Eof())
	{
		streamID = readChunk(dataStream);
		while (!dataStream->Eof() &&
			( streamID == M_SUBMESH_BONE_ASSIGNMENT ||
			streamID == M_SUBMESH_OPERATION ||
			streamID == M_SUBMESH_TEXTURE_ALIAS
			)
			)
		{
			switch (streamID)
			{
			case M_SUBMESH_BONE_ASSIGNMENT:
				readSubMeshBoneAssignment(dataStream, mesh, subMesh);
				break; 
			case M_SUBMESH_OPERATION:
				readSubMeshOperation(dataStream, mesh, subMesh);
				break;
			case M_SUBMESH_TEXTURE_ALIAS:
				readSubMeshTextureAlias(dataStream, mesh, subMesh);
				break;
			default:
				break;
			}
			if (!dataStream->Eof())
			{
				streamID = readChunk(dataStream);
			}
		}
	}
	if (!dataStream->Eof())
	{
		dataStream->Skip(-STREAM_OVERHEAD_SIZE);
	}
}
void BWMeshSerializerImp::readSubMeshBoneAssignment(BWDataStreamPrt &dataStream, BWMesh *mesh, BWSubMesh *subMesh)
{
	unsigned int tmpInt;
	unsigned short  tmpShort;
	float tmpFloat;
	readInts(dataStream, &tmpInt, 1);
	readShorts(dataStream, &tmpShort);
	readFloats(dataStream, &tmpFloat);
}
void BWMeshSerializerImp::readSubMeshOperation(BWDataStreamPrt &dataStream, BWMesh *mesh, BWSubMesh *subMesh)
{
	unsigned short opType;
	readShorts(dataStream, &opType);
	subMesh->mOperationType = static_cast<BWRenderOperation::OperatinType>(opType);
}
void BWMeshSerializerImp::readSubMeshTextureAlias(BWDataStreamPrt &dataStream, BWMesh *mesh, BWSubMesh *subMesh)
{
	std::string aliasName = readString(dataStream);
	std::string textureName = readString(dataStream);
	subMesh->addTextureAlias(aliasName, textureName);
}
void BWMeshSerializerImp::readGeometry(BWDataStreamPrt &dataStream, BWMesh *mesh, VertexData *vertexData)
{
	vertexData->mVertexStart = 0;
	unsigned int vertexCounter = 0;	
	readInts(dataStream, &vertexCounter, 1);  // 总的顶点个数
	vertexData->mVertexCount = vertexCounter;
	if (!dataStream->Eof())
	{
		unsigned short streamID = readChunk(dataStream);
		while (!dataStream->Eof() && (streamID == M_GEOMETRY_VERTEX_DECLARATION || streamID == M_GEOMETRY_VERTEX_BUFFER))
		{
			switch (streamID)
			{
			case  M_GEOMETRY_VERTEX_DECLARATION:
				readGeometryVertexDeclaration(dataStream, mesh, vertexData); // 这里存放的是各种数据块（例如 颜色 位置  法线，成为element）的声明  声明中包含了 数据起始的位置 类型（float vector color）等等  用来解读vertexbuffer
				break;
			case M_GEOMETRY_VERTEX_BUFFER:
				readGeometryVertexBuffer(dataStream, mesh, vertexData);// 这里就存放了整块的数据
				break;
			}
			if (!dataStream->Eof())
			{
				streamID = readChunk(dataStream);
			}
		}
		if (!dataStream->Eof())
		{
			dataStream->Skip(-STREAM_OVERHEAD_SIZE);
		}
	}
}
void BWMeshSerializerImp::readGeometryVertexDeclaration(BWDataStreamPrt &dataStream, BWMesh *mesh, VertexData *vertexData)
{
	if(!dataStream->Eof())
	{
		unsigned short streamID = readChunk(dataStream);
		while (!dataStream->Eof() && 
			(streamID == M_GEOMETRY_VERTEX_ELEMENT))
		{
			switch (streamID)
			{
			case M_GEOMETRY_VERTEX_ELEMENT:
				readGeometryVertexelement(dataStream, mesh, vertexData);
			default:
				break;
			}
			if (!dataStream->Eof())
			{
				streamID = readChunk(dataStream);
			}
		}
		if (!dataStream->Eof())
		{
			dataStream->Skip(-STREAM_OVERHEAD_SIZE);
		}
	}
}
void BWMeshSerializerImp::readGeometryVertexelement(BWDataStreamPrt &dataStream, BWMesh *mesh, VertexData *vertexData)
{
	unsigned short source, offset, index, tmp;
	VertexElementType vertexElemetType;
	VertexElementSemantic vertexElemtSemantic;
	readShorts(dataStream, &source);
	readShorts(dataStream, &tmp);
	vertexElemetType = static_cast<VertexElementType>(tmp);
	readShorts(dataStream, &tmp);
	vertexElemtSemantic = static_cast<VertexElementSemantic>(tmp);
	if (vertexElemtSemantic == VES_NORMAL)
	{
		// 这里是读取的法线
		//assert(0);
	}
	readShorts(dataStream, &offset);
	readShorts(dataStream, &index);
	vertexData->mVertexDeclaration->addElement(source, offset, vertexElemetType, vertexElemtSemantic, index);
}
void BWMeshSerializerImp::readGeometryVertexBuffer(BWDataStreamPrt &dataStream, BWMesh *mesh, VertexData *vertexData)
{
	unsigned short bindIndex, vertexSize;
	readShorts(dataStream, &bindIndex);  // 这个buffer是和哪个declaration 绑定的
	readShorts(dataStream, &vertexSize); // buffer中元素的大小

	unsigned short headID;
	headID = readChunk(dataStream);
	if (headID != M_GEOMETRY_VERTEX_BUFFER_DATA)
	{
		assert(0);
	}
    if (vertexData->mVertexDeclaration->getVertexSize(bindIndex) != vertexSize)
    {
		assert(0);
    }
	BWHardwareVertexBufferPtr vbuf;
	vbuf = BWHardwareBufferManager::GetInstance()->createVertexBuffer(
		vertexSize, vertexData->mVertexCount, mesh->mVertextBufferUsage, mesh->mVertexBufferShadowed);
	void * buf = vbuf->lock(BWHardwareBuffer::HBL_DISCARD);
	dataStream->Read(buf, vertexData->mVertexCount * vertexSize);
	flipFromLittleEndian(buf,
		vertexData->mVertexCount, 
		vertexSize, 
		vertexData->mVertexDeclaration->findElementBySource(bindIndex));
	vbuf->unlock();
	vertexData->mVertexBufferBind->setBinding(bindIndex, vbuf);
}
void BWMeshSerializerImp::flipFromLittleEndian(void *data, size_t vertexCounter, size_t vertexSize, const VertexDeclearaion::VertexElementList & elementList)
{
	if (mFlipEndian)
	{
		flipEndian(data, vertexCounter, vertexSize, elementList);
	}
}
void BWMeshSerializerImp::flipEndian(void *data, size_t vertexCount, size_t vertexSize, const VertexDeclearaion::VertexElementList &elem)
{
	void *pBase = data;
	for (size_t v = 0; v < vertexCount; v++)
	{
		VertexDeclearaion::VertexElementList::const_iterator ei, eiend;
		eiend = elem.end();
		for (ei = elem.begin(); ei != eiend; ei++)
		{
			void *pElem;
			(*ei).baseVertexPointerToElement(pBase, &pElem);
			size_t typeSize;
			switch (VertexElement::getBaseType((*ei).getType()))
			{
			case VET_FLOAT1:
				typeSize = sizeof(float);
				break;
			case VET_SHORT1:
				typeSize = sizeof(short);
				break;
			case VET_COLOR:
			case VET_COLOUR_ABGR:
			case VET_COLOUR_ARGB:
				typeSize = sizeof(RGBA);
				break;
			case VET_UBYTE4:
				typeSize = 0; 
				break;
			default:
				assert(0);
				break;
			}
			BWSerializer::flipEndian(pElem, typeSize, VertexElement::getTypeCount((*ei).getType()));
		}
		pBase = static_cast<void *>(static_cast<unsigned char*>(pBase)+vertexSize);
	}
}