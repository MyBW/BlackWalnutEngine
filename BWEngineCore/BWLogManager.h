#ifndef LOGMANAGER_H_
#define LOGMANAGER_H_
#include "BWSingleton.h"

class BWLogManager : public BWSingleton<BWLogManager>
{
public:
	BWLogManager* GetInstance();

protected:
private:
};

#endif