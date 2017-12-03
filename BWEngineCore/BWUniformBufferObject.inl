
template<typename TUniformBufferStruct>
void TBWUniformBufferObject<TUniformBufferStruct>::SetContent(const TUniformBufferStruct& SourceContent)
{
	memcpy(Content, &SourceContent, sizeof(TUniformBufferStruct));
}


template<typename TUniformBufferStruct>
void TBWUniformBufferObject<TUniformBufferStruct>::UploadData()
{
	if (UniformBufferObject == NULL)
	{
		UniformBufferObject = BWRoot::GetInstance()->getRenderSystem()->CreateUniformBufferObject(StructName);
	}
	UniformBufferObject->UploadData(Content, mSizeInBytes);
}
