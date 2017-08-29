#include "BWMaterial.h"
#include "BWTechnique.h"

BWMaterial::~BWMaterial()
{
	RemoveAllTechniques();
}

void BWMaterial::RemoveAllTechniques()
{
	Techniques::iterator itor = techniques.begin();
	while (itor != techniques.end())
	{
		delete (*itor);
		itor++;
	}
	techniques.clear();
}

BWTechnique* BWMaterial::CreateTechnique()
{
	BWTechnique* technique = new BWTechnique();
	technique->SetParent(this);
	techniques.push_back(technique);
	return technique;
}

void BWMaterial::loadImpl()
{
	Techniques::iterator itor = techniques.begin();
	Techniques::iterator end = techniques.end();
	while (itor != end)
	{
		(*itor)->load();
		itor++;
	}
}
BWTechnique* BWMaterial::getTechnique(int i)
{
	return techniques[i];
}
