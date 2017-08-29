#include "FBXAttributeTypeData.h"
#include <iostream>

REGISTER_FBXIMPORT_IMP(FBXMeshAttributeImport);
FBXMeshAttributeImport::~FBXMeshAttributeImport()
{
	for each (SubMesh* var in mSubMeshes)
	{
		delete var;
	}
}
void FBXMeshAttributeImport::ImportNodeAttribute(FbxNode* Node)
{
	FbxMesh* Mesh = Node->GetMesh();
	if (!Mesh->GetNode())
	{
		PrintLog(std::string("FBXImport::ImportMesh"), std::string("Can Not Find Node Of Mesh"));
		return;
	}
	const int lPolygonCount = Mesh->GetPolygonCount();

	// Count the polygon count of each material
	FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;
	FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;
	if (Mesh->GetElementMaterial())
	{
		lMaterialIndice = &Mesh->GetElementMaterial()->GetIndexArray();
		lMaterialMappingMode = Mesh->GetElementMaterial()->GetMappingMode();
		if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
		{
			FBX_ASSERT(lMaterialIndice->GetCount() == lPolygonCount);
			if (lMaterialIndice->GetCount() == lPolygonCount)
			{
				// Count the faces of each material
				for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
				{
					const int lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
					if (mSubMeshes.size() < lMaterialIndex + 1)
					{
						mSubMeshes.resize(lMaterialIndex + 1);
					}
					if (mSubMeshes[lMaterialIndex] == NULL)
					{
						mSubMeshes[lMaterialIndex] = new SubMesh;
					}
					mSubMeshes[lMaterialIndex]->TriangleCount += 1;
				}

				// Make sure we have no "holes" (NULL) in the mSubMeshes table. This can happen
				// if, in the loop above, we resized the mSubMeshes by more than one slot.
				for (int i = 0; i < mSubMeshes.size(); i++)
				{
					if (mSubMeshes[i] == NULL)
						mSubMeshes[i] = new SubMesh;
				}

				// Record the offset (how many vertex)
				const int lMaterialCount = mSubMeshes.size();
				int lOffset = 0;
				for (int lIndex = 0; lIndex < lMaterialCount; ++lIndex)
				{
					mSubMeshes[lIndex]->IndexOffset = lOffset;
					lOffset += mSubMeshes[lIndex]->TriangleCount * 3;
					// This will be used as counter in the following procedures, reset to zero
					mSubMeshes[lIndex]->TriangleCount = 0;
				}
				FBX_ASSERT(lOffset == lPolygonCount * 3);
			}
		}
	}

	// All faces will use the same material.
	if (mSubMeshes.size() == 0)
	{
		mSubMeshes.resize(1);
		mSubMeshes[0] = new SubMesh();
	}

	// Congregate all the data of a mesh to be cached in VBOs.
	// If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex.
	bool IsHasNormal = Mesh->GetElementNormalCount() > 0;
	bool IsHaveUV = Mesh->GetElementUVCount() > 0;
	bool IsAllByControlPoint = false;
	FbxGeometryElement::EMappingMode lNormalMappingMode = FbxGeometryElement::eNone;
	FbxGeometryElement::EMappingMode lUVMappingMode = FbxGeometryElement::eNone;
	if (IsHasNormal)
	{
		lNormalMappingMode = Mesh->GetElementNormal(0)->GetMappingMode();
		if (lNormalMappingMode == FbxGeometryElement::eNone)
		{
			IsHasNormal = false;
		}
		if (IsHasNormal && lNormalMappingMode != FbxGeometryElement::eByControlPoint)
		{
			IsAllByControlPoint = false;
		}
	}
	if (IsHaveUV)
	{
		lUVMappingMode = Mesh->GetElementUV(0)->GetMappingMode();
		if (lUVMappingMode == FbxGeometryElement::eNone)
		{
			IsHaveUV = false;
		}
		if (IsHaveUV && lUVMappingMode != FbxGeometryElement::eByControlPoint)
		{
			IsAllByControlPoint = false;
		}
	}

	// Allocate the array memory, by control point or by polygon vertex.
	int lPolygonVertexCount = Mesh->GetControlPointsCount();
	if (!IsAllByControlPoint)
	{
		lPolygonVertexCount = lPolygonCount * TRIANGLE_VERTEX_COUNT;
	}
	lVertices.resize(lPolygonVertexCount * VERTEX_STRIDE);
	//float * lVertices = new float[lPolygonVertexCount * VERTEX_STRIDE];

	lIndices.resize(lPolygonCount * TRIANGLE_VERTEX_COUNT);
	//unsigned int * lIndices = new unsigned int[lPolygonCount * TRIANGLE_VERTEX_COUNT];

	//float * lNormals = NULL;
	if (IsHasNormal)
	{
		lNormals.resize(lPolygonVertexCount * NORMAL_STRIDE);
		//lNormals = new float[lPolygonVertexCount * NORMAL_STRIDE];
	}
	//float * lUVs = NULL;
	FbxStringList lUVNames;
	Mesh->GetUVSetNames(lUVNames);
	const char * lUVName = NULL;
	if (IsHaveUV && lUVNames.GetCount())
	{
		lUVs.resize(lPolygonVertexCount * UV_STRIDE);
		//lUVs = new float[lPolygonVertexCount * UV_STRIDE];
		lUVName = lUVNames[0];
	}

	// Populate the array with vertex attribute, if by control point.
	const FbxVector4 * lControlPoints = Mesh->GetControlPoints();
	FbxVector4 lCurrentVertex;
	FbxVector4 lCurrentNormal;
	FbxVector2 lCurrentUV;
	if (IsAllByControlPoint)
	{
		const FbxGeometryElementNormal * lNormalElement = NULL;
		const FbxGeometryElementUV * lUVElement = NULL;
		if (IsHasNormal)
		{
			lNormalElement = Mesh->GetElementNormal(0);
		}
		if (IsHaveUV)
		{
			lUVElement = Mesh->GetElementUV(0);
		}
		for (int lIndex = 0; lIndex < lPolygonVertexCount; ++lIndex)
		{
			// Save the vertex position.
			lCurrentVertex = lControlPoints[lIndex];
			lVertices[lIndex * VERTEX_STRIDE] = static_cast<float>(lCurrentVertex[0]);
			lVertices[lIndex * VERTEX_STRIDE + 1] = static_cast<float>(lCurrentVertex[1]);
			lVertices[lIndex * VERTEX_STRIDE + 2] = static_cast<float>(lCurrentVertex[2]);
			lVertices[lIndex * VERTEX_STRIDE + 3] = 1;

			// Save the normal.
			if (IsHasNormal)
			{
				int lNormalIndex = lIndex;
				if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
				{
					lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndex);
				}
				lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
				lNormals[lIndex * NORMAL_STRIDE] = static_cast<float>(lCurrentNormal[0]);
				lNormals[lIndex * NORMAL_STRIDE + 1] = static_cast<float>(lCurrentNormal[1]);
				lNormals[lIndex * NORMAL_STRIDE + 2] = static_cast<float>(lCurrentNormal[2]);
			}

			// Save the UV.
			if (IsHaveUV)
			{
				int lUVIndex = lIndex;
				if (lUVElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
				{
					lUVIndex = lUVElement->GetIndexArray().GetAt(lIndex);
				}
				lCurrentUV = lUVElement->GetDirectArray().GetAt(lUVIndex);
				lUVs[lIndex * UV_STRIDE] = static_cast<float>(lCurrentUV[0]);
				lUVs[lIndex * UV_STRIDE + 1] = static_cast<float>(lCurrentUV[1]);
			}
		}

	}
	
	if (Mesh->GetElementTangentCount() <= 0)
	{
		Mesh->GenerateTangentsData(0);
	}
	lTangents.resize(lVertices.size());
	FbxGeometryElementTangent* Tangent = Mesh->GetElementTangent(0);


	bool ShowInfor = false;


	int lVertexCount = 0;
	for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
	{
		// The material for current face.
		int lMaterialIndex = 0;
		if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
		{
			lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
		}

		// Where should I save the vertex attribute index, according to the material
		const int lIndexOffset = mSubMeshes[lMaterialIndex]->IndexOffset +
			mSubMeshes[lMaterialIndex]->TriangleCount * 3;

		for (int lVerticeIndex = 0; lVerticeIndex < TRIANGLE_VERTEX_COUNT; ++lVerticeIndex)
		{
			const int lControlPointIndex = Mesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
			if (ShowInfor)
			{
				std::cout << "vertex " << lVertexCount << "have :" << std::endl;
			}

			if (IsAllByControlPoint)
			{
				lIndices[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lControlPointIndex);
			}
			// Populate the array with vertex attribute, if by polygon vertex.
			else
			{
				lIndices[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lVertexCount);

				lCurrentVertex = lControlPoints[lControlPointIndex];
				lVertices[lVertexCount * VERTEX_STRIDE] = static_cast<float>(lCurrentVertex[0]);
				lVertices[lVertexCount * VERTEX_STRIDE + 1] = static_cast<float>(lCurrentVertex[1]);
				lVertices[lVertexCount * VERTEX_STRIDE + 2] = static_cast<float>(lCurrentVertex[2]);
				lVertices[lVertexCount * VERTEX_STRIDE + 3] = 1;

				if (ShowInfor)
				{
					std::cout << "Position 0		" << lCurrentVertex[0] << std::endl;
					std::cout << "Position 1		" << lCurrentVertex[1] << std::endl;
					std::cout << "Position 2		" << lCurrentVertex[2] << std::endl;
					std::cout << "Position 3		" << 1 << std::endl;
				}
				

				if (IsHasNormal)
				{
					Mesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);
					lNormals[lVertexCount * NORMAL_STRIDE] = static_cast<float>(lCurrentNormal[0]);
					lNormals[lVertexCount * NORMAL_STRIDE + 1] = static_cast<float>(lCurrentNormal[1]);
					lNormals[lVertexCount * NORMAL_STRIDE + 2] = static_cast<float>(lCurrentNormal[2]);
					if (ShowInfor)
					{
						std::cout << "Normal 0		" << lCurrentNormal[0] << std::endl;
						std::cout << "Normal 1		" << lCurrentNormal[1] << std::endl;
						std::cout << "Normal 2		" << lCurrentNormal[2] << std::endl;
						std::cout << "Normal 3		" << 1 << std::endl;
					}
					

				}

				if (IsHaveUV)
				{
					bool lUnmappedUV;
					Mesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName, lCurrentUV, lUnmappedUV);
					lUVs[lVertexCount * UV_STRIDE] =  static_cast<float>(lCurrentUV[0]);
					lUVs[lVertexCount * UV_STRIDE + 1] =1 - static_cast<float>(lCurrentUV[1]);
					if (ShowInfor)
					{
						std::cout << "VU 0		" << lCurrentUV[0] << std::endl;
						std::cout << "VU 1		" << lCurrentUV[1] << std::endl;
					}
					
				}
			}

			if (Tangent)
			{
				switch (Tangent->GetMappingMode())
				{
				case  FbxGeometryElement::eByControlPoint:
				{
					switch (Tangent->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						PrintLog("FbxGeometryElement::eByControlPoint", "FbxGeometryElement::eDirect");
						break;
					}
					case FbxGeometryElement::eIndexToDirect:
					{
						PrintLog("FbxGeometryElement::eByControlPoint", "FbxGeometryElement::eIndexToDirect");
						break;
					}
					}
				}
				case FbxGeometryElement::eByPolygonVertex:
				{
					switch (Tangent->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						lTangents[lVertexCount * TANGENT_STRIDE] = Tangent->GetDirectArray().GetAt(lVertexCount)[0];
						lTangents[lVertexCount * TANGENT_STRIDE + 1] = Tangent->GetDirectArray().GetAt(lVertexCount)[1];
						lTangents[lVertexCount * TANGENT_STRIDE + 2] = Tangent->GetDirectArray().GetAt(lVertexCount)[2];
						lTangents[lVertexCount * TANGENT_STRIDE + 3] = Tangent->GetDirectArray().GetAt(lVertexCount)[3];

						if (ShowInfor)
						{
							std::cout << "Tangent 0		" << lTangents[lVertexCount * TANGENT_STRIDE] << std::endl;
							std::cout << "Tangent 1		" << lTangents[lVertexCount * TANGENT_STRIDE + 1] << std::endl;
							std::cout << "Tangent 2		" << lTangents[lVertexCount * TANGENT_STRIDE + 2] << std::endl;
							std::cout << "Tangent 3		" << lTangents[lVertexCount * TANGENT_STRIDE + 3] << std::endl;

						}
						
						break;
					}
					case FbxGeometryElement::eIndexToDirect:
					{
						
						PrintLog("FbxGeometryElement::eByPolygonVertex", "FbxGeometryElement::eIndexToDirect");
						break;
					}
					}
				}
				}
			}
			++lVertexCount;
		}
		mSubMeshes[lMaterialIndex]->TriangleCount += 1;
	}
}
