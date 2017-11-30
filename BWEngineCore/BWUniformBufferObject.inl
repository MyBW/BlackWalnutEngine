
template<typename TUniformBufferStruct>
void TBWUniformBufferObject<TUniformBufferStruct>::SetContent(const TUniformBufferStruct& SourceContent)
{
	if (Content) delete[] Content;
	Content = new char[sizeof(TUniformBufferStruct)];
	memcpy(Content, &SourceContent, sizeof(TUniformBufferStruct));
}
