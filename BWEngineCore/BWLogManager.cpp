#include "BWLogManager.h"

BWLogManager * BWLogManager::GetInstance()
{
	assert(instance);
	return instance;
}

