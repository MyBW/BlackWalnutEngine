#include "GLVAO.h"
#include "GLHardwareVertexBuffer.h"
#include "GLHardwareIndexBuffer.h"
#include "GLPreDefine.h"
#include "GLSLGpuProgram.h"
GLVAO::GLVAO() :mID(0), mProgram(NULL)
{

}
GLVAO::~GLVAO()
{
	CHECK_GL_ERROR(glDeleteVertexArrays(1, &mID));
}
void GLVAO::init(GLSLGpuProgram *program , IndexDataPrt indexData , VertexDataPrt vertexData)
{
	if (program)
	{
		mProgram = program;
		if (mProgram->getLoadingState() != LOADSTATE_LOADED)
		{
			mProgram->Load();
		}
		NameLocation & nameLoaction = mProgram->mNameLoaction;

		CHECK_GL_ERROR(glGenVertexArrays(1, &mID));
		CHECK_GL_ERROR(glBindVertexArray(mID));


		VertexBufferBinding::VertexBufferBindMap VertexBindMap = vertexData->mVertexBufferBind->GetVertexBufferBindMap();
		for (VertexBufferBinding::VertexBufferBindMap::const_iterator BindMapItor  = VertexBindMap.begin() ; 
			BindMapItor != VertexBindMap.end() ;
			BindMapItor++)
		{
			GLHardwareVertexBuffer* vertexBuffer = dynamic_cast<GLHardwareVertexBuffer*>(BindMapItor->second.Get());
			CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->getGLHardwareBufferID()));
			VertexDeclearaion::VertexElementList elementList = vertexData->mVertexDeclaration->findElementBySource(BindMapItor->first);
			VertexDeclearaion::VertexElementList::iterator itor = elementList.begin();
			VertexDeclearaion::VertexElementList::iterator endItor = elementList.end();
			while (itor != endItor)
			{
				Log::GetInstance()->logMessage(mBufferElementToString[itor->getVertexSemantic()]);
				NameLocation::iterator namedLoactionItor = nameLoaction.find(mBufferElementToString[itor->getVertexSemantic()]);
				if (namedLoactionItor != nameLoaction.end())
				{
					GLint location = namedLoactionItor->second;
					if (location != -1)
					{
						CHECK_GL_ERROR(glEnableVertexAttribArray(location));
						CHECK_GL_ERROR(glVertexAttribPointer(location, itor->getTypeCount(itor->getType()), getGLType(itor->getType()),
							GL_FALSE, static_cast<GLsizei>(vertexBuffer->getVertexSize()), (void*)itor->getOffset()));
					}
				}
				itor++;
			}
		}

		/*for (int i = 0; i < vertexData->mVertexBufferBind->getBufferNum(); i++)
		{
			GLHardwareVertexBuffer* vertexBuffer = dynamic_cast<GLHardwareVertexBuffer*>(vertexData->mVertexBufferBind->getBuffer(i).Get());
		    CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER ,vertexBuffer->getGLHardwareBufferID()));
			VertexDeclearaion::VertexElementList elementList = vertexData->mVertexDeclaration->findElementBySource(i);
			VertexDeclearaion::VertexElementList::iterator itor = elementList.begin();
			VertexDeclearaion::VertexElementList::iterator endItor = elementList.end();
			while (itor != endItor)
			{
				Log::GetInstance()->logMessage(mBufferElementToString[itor->getVertexSemantic()]);
				NameLocation::iterator namedLoactionItor = nameLoaction.find(mBufferElementToString[itor->getVertexSemantic()]);
				if (namedLoactionItor != nameLoaction.end())
				{
					GLint location = namedLoactionItor->second;
					if (location != -1)
					{
						CHECK_GL_ERROR(glEnableVertexAttribArray(location));
						CHECK_GL_ERROR(glVertexAttribPointer(location, itor->getTypeCount(itor->getType()), getGLType(itor->getType()),
							GL_FALSE, static_cast<GLsizei>(vertexBuffer->getVertexSize()), (void*)itor->getOffset()));
					}
				}
			
				itor++;
			}
		}*/
		if (!indexData.IsNull())
		{
			GLHardwareIndexBuffer* indexBuffer = dynamic_cast<GLHardwareIndexBuffer*>(indexData->mIndexBuffer.Get());
			if (indexBuffer)
			{
				CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->getGLBufferID()));
			}
		}
		glBindVertexArray(0);
	}
}

GLint GLVAO::GetGLID()
{
	return mID;
}
void GLVAO::bind()
{
	CHECK_GL_ERROR(glBindVertexArray(mID));
}