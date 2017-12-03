#pragma once
#include "BWSmartPointer.h"
class BWGpuProgramParameters;;
class BWGpuProgram;
class BWHighLevelGpuProgram;
class BWGpuProgramUsage;
struct  GpuNamedConstants;
struct GpuLogicalBufferStruct;
struct BWGpuSharedParameters;
class BWResource;
class BWTexture;
class BWImageTextureBuffer;
typedef SmartPointer<BWGpuProgramParameters> BWGpuProgramParametersPtr;
typedef SmartPointer<BWGpuProgram>  BWGpuProgramPtr;
typedef SmartPointer<GpuNamedConstants> GpuNamedConstantsPtr;
typedef SmartPointer<GpuLogicalBufferStruct> GpuLogicalBufferStructPtr;
typedef SmartPointer<BWGpuSharedParameters> BWGpuSharedParametersPtr;
typedef SmartPointer<BWHighLevelGpuProgram> BWHighLevelGpuProgramPtr;
typedef SmartPointer<BWGpuProgramUsage> BWGpuProgramUsagePtr;
typedef SmartPointer<BWResource> BWResourcePtr;
typedef SmartPointer<BWTexture> BWTexturePtr;
typedef SmartPointer<BWImageTextureBuffer> BWImageTexturebufferPtr;
