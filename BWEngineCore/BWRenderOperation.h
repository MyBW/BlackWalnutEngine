#ifndef RENDEROPERATION_H_
#define RENDEROPERATION_H_
#include "BWHardwareVertexBuffer.h"
#include "BWVertexIndexData.h"
class BWRenderable;
class BWRenderOperation 
{

public:
	enum OperatinType
	{
		OT_POINT_LIST = 1,
		OT_LINE_LIST = 2,
		OT_LINE_STRIP = 3, 
		OT_TRIANGLE_LIST = 4 ,
		OT_TRIANGLE_STRIP = 5 ,
		OT_TRIGANLE_FAN = 6
	};
	OperatinType operationType;
	bool useIndexes;

	BWRenderable *sourceRenderable;
	VertexDataPrt  vertexData;
	IndexDataPrt indexData;
	BWRenderOperation() :vertexData(NULL), operationType(OperatinType::OT_TRIANGLE_LIST), useIndexes(true),
		indexData(NULL), sourceRenderable(NULL)
	{

	}
protected:
private:
};
#endif