#ifndef GPUPROGRAMPARAMS_H_
#define GPUPROGRAMPARAMS_H_

#include "BWDataStream.h"
#include "OgreAny.h"
#include "BWPrimitive.h"
#include "AllSmartPointRef.h"
#include <vector>
#include <map>
#include <limits>
#include "BWUniformBufferObject.h"
enum GpuConstantType
{
	GCT_FLOAT1 = 1,
	GCT_FLOAT2 = 2,
	GCT_FLOAT3 = 3,
	GCT_FLOAT4 = 4,
	GCT_SAMPLER1D = 5,
	GCT_SAMPLER2D = 6,
	GCT_SAMPLER3D = 7,
	GCT_SAMPLERCUBE = 8,
	GCT_SAMPLER1DSHADOW = 9,
	GCT_SAMPLER2DSHADOW = 10,
	GCT_SAMPLER2DARRAY = 11,
	GCT_MATRIX_2X2 = 12,
	GCT_MATRIX_2X3 = 13,
	GCT_MATRIX_2X4 = 14,
	GCT_MATRIX_3X2 = 15,
	GCT_MATRIX_3X3 = 16,
	GCT_MATRIX_3X4 = 17,
	GCT_MATRIX_4X2 = 18,
	GCT_MATRIX_4X3 = 19,
	GCT_MATRIX_4X4 = 20,
	GCT_INT1 = 21,
	GCT_INT2 = 22,
	GCT_INT3 = 23,
	GCT_INT4 = 24,
	GCT_UNKNOWN = 99
};
enum GpuParamVariability
{
	/// No variation except by manual setting - the default
	GPV_GLOBAL = 1,
	/// Varies per object (based on an auto param usually), but not per light setup
	GPV_PER_OBJECT = 2,
	/// Varies with light setup
	GPV_LIGHTS = 4,
	/// Varies with pass iteration number
	GPV_PASS_ITERATION_NUMBER = 8,


	/// Full mask (16-bit)
	GPV_ALL = 0xFFFF

};
struct  GpuConstantDefinition
{
	/// Data type
	GpuConstantType constType;
	/// Physical start index in buffer (either float or int buffer)
	size_t physicalIndex;
	/// Logical index - used to communicate this constant to the rendersystem
	int logicalIndex;
	/** Number of raw buffer slots per element
	(some programs pack each array element to float4, some do not) */
	size_t elementSize;
	/// Length of array
	size_t arraySize;
	/// How this parameter varies (bitwise combination of GpuProgramVariability)
	mutable unsigned int variability;

	bool isFloat() const
	{
		return isFloat(constType);
	}

	static bool isFloat(GpuConstantType c)
	{
		switch (c)
		{
		case GCT_INT1:
		case GCT_INT2:
		case GCT_INT3:
		case GCT_INT4:
		case GCT_SAMPLER1D:
		case GCT_SAMPLER2D:
		case GCT_SAMPLER2DARRAY:
		case GCT_SAMPLER3D:
		case GCT_SAMPLERCUBE:
		case GCT_SAMPLER1DSHADOW:
		case GCT_SAMPLER2DSHADOW:
			return false;
		default:
			return true;
		};

	}

	bool isSampler() const
	{
		return isSampler(constType);
	}

	static bool isSampler(GpuConstantType c)
	{
		switch (c)
		{
		case GCT_SAMPLER1D:
		case GCT_SAMPLER2D:
		case GCT_SAMPLER2DARRAY:
		case GCT_SAMPLER3D:
		case GCT_SAMPLERCUBE:
		case GCT_SAMPLER1DSHADOW:
		case GCT_SAMPLER2DSHADOW:
			return true;
		default:
			return false;
		};

	}


	/** Get the element size of a given type, including whether to pad the
	elements into multiples of 4 (e.g. SM1 and D3D does, GLSL doesn't)
	*/
	static size_t getElementSize(GpuConstantType ctype, bool padToMultiplesOf4)
	{
		if (padToMultiplesOf4)
		{
			switch (ctype)
			{
			case GCT_FLOAT1:
			case GCT_INT1:
			case GCT_SAMPLER1D:
			case GCT_SAMPLER2D:
			case GCT_SAMPLER2DARRAY:
			case GCT_SAMPLER3D:
			case GCT_SAMPLERCUBE:
			case GCT_SAMPLER1DSHADOW:
			case GCT_SAMPLER2DSHADOW:
			case GCT_FLOAT2:
			case GCT_INT2:
			case GCT_FLOAT3:
			case GCT_INT3:
			case GCT_FLOAT4:
			case GCT_INT4:
				return 4;
			case GCT_MATRIX_2X2:
			case GCT_MATRIX_2X3:
			case GCT_MATRIX_2X4:
				return 8; // 2 float4s
			case GCT_MATRIX_3X2:
			case GCT_MATRIX_3X3:
			case GCT_MATRIX_3X4:
				return 12; // 3 float4s
			case GCT_MATRIX_4X2:
			case GCT_MATRIX_4X3:
			case GCT_MATRIX_4X4:
				return 16; // 4 float4s
			default:
				return 4;
			};
		}
		else
		{
			switch (ctype)
			{
			case GCT_FLOAT1:
			case GCT_INT1:
			case GCT_SAMPLER1D:
			case GCT_SAMPLER2D:
			case GCT_SAMPLER2DARRAY:
			case GCT_SAMPLER3D:
			case GCT_SAMPLERCUBE:
			case GCT_SAMPLER1DSHADOW:
			case GCT_SAMPLER2DSHADOW:
				return 1;
			case GCT_FLOAT2:
			case GCT_INT2:
				return 2;
			case GCT_FLOAT3:
			case GCT_INT3:
				return 3;
			case GCT_FLOAT4:
			case GCT_INT4:
				return 4;
			case GCT_MATRIX_2X2:
				return 4;
			case GCT_MATRIX_2X3:
			case GCT_MATRIX_3X2:
				return 6;
			case GCT_MATRIX_2X4:
			case GCT_MATRIX_4X2:
				return 8;
			case GCT_MATRIX_3X3:
				return 9;
			case GCT_MATRIX_3X4:
			case GCT_MATRIX_4X3:
				return 12;
			case GCT_MATRIX_4X4:
				return 16;
			default:
				return 4;
			};

		}
	}

	GpuConstantDefinition()
		: constType(GCT_UNKNOWN)
		, physicalIndex((std::numeric_limits<size_t>::max)())
		, logicalIndex(0)
		, elementSize(0)
		, arraySize(1)
		, variability(GPV_GLOBAL) {}
};


typedef std::map<std::string, GpuConstantDefinition> GpuConstantDefinitionMap;

/// Struct collecting together the information for named constants.
struct  GpuNamedConstants 
{
	/// Total size of the float buffer required
	size_t floatBufferSize;
	/// Total size of the int buffer required
	size_t intBufferSize;
	/// Map of parameter names to GpuConstantDefinition
	GpuConstantDefinitionMap map;

	GpuNamedConstants() : floatBufferSize(0), intBufferSize(0) {}

	void clearData()
	{
		floatBufferSize = 0;
		intBufferSize = 0;
		map.clear();
	}
	/** Generate additional constant entries for arrays based on a base definition.
	@remarks
	Array uniforms will be added just with their base name with no array
	suffix. This method will add named entries for array suffixes too
	so individual array entries can be addressed. Note that we only
	individually index array elements if the array size is up to 16
	entries in size. Anything larger than that only gets a [0] entry
	as well as the main entry, to save cluttering up the name map. After
	all, you can address the larger arrays in a bulk fashion much more
	easily anyway.
	*/
	void generateConstantDefinitionArrayEntries(const std::string& paramName,
		const GpuConstantDefinition& baseDef);

	/// Indicates whether all array entries will be generated and added to the definitions map
	static bool getGenerateAllConstantDefinitionArrayEntries();

	/** Sets whether all array entries will be generated and added to the definitions map.
	@remarks
	Usually, array entries can only be individually indexed if they're up to 16 entries long,
	to save memory - arrays larger than that can be set but only via the bulk setting
	methods. This option allows you to choose to individually index every array entry.
	*/
	static void setGenerateAllConstantDefinitionArrayEntries(bool generateAll);

	/** Saves constant definitions to a file, compatible with GpuProgram::setManualNamedConstantsFile.
	@see GpuProgram::setManualNamedConstantsFile
	*/
	void save(const std::string& filename) const;
	/** Loads constant definitions from a stream, compatible with GpuProgram::setManualNamedConstantsFile.
	@see GpuProgram::setManualNamedConstantsFile
	*/
	void load(BWDataStreamPrt& stream);

protected:
	/** Indicates whether all array entries will be generated and added to the definitions map
	@remarks
	Normally, the number of array entries added to the definitions map is capped at 16
	to save memory. Setting this value to <code>true</code> allows all of the entries
	to be generated and added to the map.
	*/
	static bool msGenerateAllConstantDefinitionArrayEntries;
};



/** Structure recording the use of a physical buffer by a logical parameter
index. Only used for low-level programs.
*/
struct  GpuLogicalIndexUse
{
	/// Physical buffer index
	size_t physicalIndex;
	/// Current physical size allocation
	size_t currentSize;
	/// How the contents of this slot vary
	mutable unsigned short variability;

	GpuLogicalIndexUse()
		: physicalIndex(99999), currentSize(0), variability(GPV_GLOBAL) {}
	GpuLogicalIndexUse(size_t bufIdx, size_t curSz, unsigned short v)
		: physicalIndex(bufIdx), currentSize(curSz), variability(v) {}
};
typedef std::map<size_t, GpuLogicalIndexUse> GpuLogicalIndexUseMap;
/// Container struct to allow params to safely & update shared list of logical buffer assignments
struct  GpuLogicalBufferStruct 
{
	/// Map from logical index to physical buffer location
	GpuLogicalIndexUseMap map;
	/// Shortcut to know the buffer size needs
	size_t bufferSize;
	GpuLogicalBufferStruct() : bufferSize(0) {}
};


/** Definition of container that holds the current float constants.
@note Not necessarily in direct index order to constant indexes, logical
to physical index map is derived from GpuProgram
*/
typedef std::vector<float> FloatConstantList;
/** Definition of container that holds the current float constants.
@note Not necessarily in direct index order to constant indexes, logical
to physical index map is derived from GpuProgram
*/
typedef std::vector<int> IntConstantList;


class BWGpuSharedParameters
{
public:
	BWGpuSharedParameters() :version(0)
	{
	}

	~BWGpuSharedParameters()
	{
	}
	GpuConstantDefinitionMap& GetConstantDefinitionMap(){ return namedConstant; }
	void AddConstantDefinition(const std::string&, GpuConstantType, int);
	void SetNamedConstant(const std::string &, int*, int);
	void SetNamedConstant(const std::string &, float*, int);
	float* GetFloatPointer(int physicalIndex){ return &flotConstantList[physicalIndex]; }
	int * GetIntPointer(int physicalIndex) { return &intConstantList[physicalIndex]; }
	const std::string GetName() { return name; };
	void SetName(const std::string &name) { this->name = name; }
	int GetVersion(){ return version; }
private:
	void MakeDirty();
	GpuConstantDefinitionMap namedConstant;
	FloatConstantList flotConstantList; 
	IntConstantList intConstantList;
	std::string  name;
	int version;
};


class BWGpuProgramParameters;
class BWGpuSharedParametersUsage
{
protected:
	struct CopyDataEntry
	{
		const GpuConstantDefinition *srcDefinition;
		const GpuConstantDefinition *dstDefinition;
	};
	typedef std::vector<CopyDataEntry> CopyDataList;
	BWGpuSharedParametersPtr sharedParameter;
	BWGpuProgramParameters* parent;
	CopyDataList  copyDataList;
	mutable Any renderSystemData;
	unsigned long CopyDataVersion;
	void InitCopyData();
	int version;
public:
	BWGpuSharedParametersUsage(BWGpuSharedParametersPtr sharedParam, BWGpuProgramParameters *param);
	void CopySharedParamsToTargetParams();
	const std::string &GetName() const { return sharedParameter->GetName(); }
	BWGpuSharedParametersPtr GetSharedParam() const { return sharedParameter; }
	BWGpuProgramParameters* GetGpuProgramParameter() const { return parent; }
	void SetRenderSystemData(const Any& data) const { renderSystemData = data; }
	const Any& GetRenderSystemData() const { return renderSystemData; }
};
typedef std::vector<BWGpuSharedParametersUsage> BWGpuSharedParameterUsageList;


//Global Uniform Buffer Object
BEGIN_UNIFORM_BUFFER_STRUCT(ViewportInforUniformBufferStruct)
UNIFORM_BUFFER_STRUCT_MEMBER(BWMatrix4, ViewMatrix)
UNIFORM_BUFFER_STRUCT_MEMBER(BWMatrix4, ProjectMatrix)
UNIFORM_BUFFER_STRUCT_MEMBER(BWMatrix4, PreViewMatrix)
UNIFORM_BUFFER_STRUCT_MEMBER(BWMatrix4, PreProjectMatrix)
UNIFORM_BUFFER_STRUCT_MEMBER(BWMatrix4, ViewInversMatrix)
UNIFORM_BUFFER_STRUCT_MEMBER(BWVector3, ViewPositionWorldSpace)
UNIFORM_BUFFER_STRUCT_MEMBER(float, FoV)
UNIFORM_BUFFER_STRUCT_MEMBER(float, PrjPlaneWInverseH)
UNIFORM_BUFFER_STRUCT_MEMBER(BWPoint2DD, NearFar)
UNIFORM_BUFFER_STRUCT_MEMBER(BWPoint2DD, ScreenWH)
END_UNIFORM_BUFFER_STRUCT(ViewportInforUniformBufferStruct)


class BWGpuProgramParameters 
{
public: 

	enum ElementType
	{
		ET_REAL,
		ET_INT
	};
	enum AutoConstantType
	{
		/// The current world matrix
		ACT_WORLD_MATRIX,
		ACT_PRE_WORLD_MATRIX,
		/// The current world matrix, inverted
		ACT_INVERSE_WORLD_MATRIX,
		/** Provides transpose of world matrix.
		Equivalent to RenderMonkey's "WorldTranspose".
		*/
		ACT_TRANSPOSE_WORLD_MATRIX,
		/// The current world matrix, inverted & transposed
		ACT_INVERSE_TRANSPOSE_WORLD_MATRIX,

		/// The current array of world matrices, as a 3x4 matrix, used for blending
		ACT_WORLD_MATRIX_ARRAY_3x4,
		/// The current array of world matrices, used for blending
		ACT_WORLD_MATRIX_ARRAY,
		/// The current array of world matrices transformed to an array of dual quaternions, represented as a 2x4 matrix
		ACT_WORLD_DUALQUATERNION_ARRAY_2x4,
		/// The scale and shear components of the current array of world matrices
		ACT_WORLD_SCALE_SHEAR_MATRIX_ARRAY_3x4,

		/// The current view matrix
		ACT_VIEW_MATRIX,

		ACT_PRE_VIEW_MATIX,
		/// The current view matrix, inverted
		ACT_INVERSE_VIEW_MATRIX,
		/** Provides transpose of view matrix.
		Equivalent to RenderMonkey's "ViewTranspose".
		*/
		ACT_TRANSPOSE_VIEW_MATRIX,
		/** Provides inverse transpose of view matrix.
		Equivalent to RenderMonkey's "ViewInverseTranspose".
		*/
		ACT_INVERSE_TRANSPOSE_VIEW_MATRIX,


		/// The current projection matrix
		ACT_PROJECTION_MATRIX,
		/** Provides inverse of projection matrix.
		Equivalent to RenderMonkey's "ProjectionInverse".
		*/
		ACT_INVERSE_PROJECTION_MATRIX,

		ACT_PRE_PROJECTION_MATRIX,
		
		/** Provides transpose of projection matrix.
		Equivalent to RenderMonkey's "ProjectionTranspose".
		*/
		ACT_TRANSPOSE_PROJECTION_MATRIX,
		/** Provides inverse transpose of projection matrix.
		Equivalent to RenderMonkey's "ProjectionInverseTranspose".
		*/
		ACT_INVERSE_TRANSPOSE_PROJECTION_MATRIX,


		/// The current view & projection matrices concatenated
		ACT_VIEWPROJ_MATRIX,
		/** Provides inverse of concatenated view and projection matrices.
		Equivalent to RenderMonkey's "ViewProjectionInverse".
		*/
		ACT_INVERSE_VIEWPROJ_MATRIX,
		/** Provides transpose of concatenated view and projection matrices.
		Equivalent to RenderMonkey's "ViewProjectionTranspose".
		*/
		ACT_TRANSPOSE_VIEWPROJ_MATRIX,
		/** Provides inverse transpose of concatenated view and projection matrices.
		Equivalent to RenderMonkey's "ViewProjectionInverseTranspose".
		*/
		ACT_INVERSE_TRANSPOSE_VIEWPROJ_MATRIX,


		/// The current world & view matrices concatenated
		ACT_WORLDVIEW_MATRIX,
		/// The current world & view matrices concatenated, then inverted
		ACT_INVERSE_WORLDVIEW_MATRIX,
		/** Provides transpose of concatenated world and view matrices.
		Equivalent to RenderMonkey's "WorldViewTranspose".
		*/
		ACT_TRANSPOSE_WORLDVIEW_MATRIX,
		/// The current world & view matrices concatenated, then inverted & transposed
		ACT_INVERSE_TRANSPOSE_WORLDVIEW_MATRIX,
		/// view matrices.


		/// The current world, view & projection matrices concatenated
		ACT_WORLDVIEWPROJ_MATRIX,
		/** Provides inverse of concatenated world, view and projection matrices.
		Equivalent to RenderMonkey's "WorldViewProjectionInverse".
		*/
		ACT_INVERSE_WORLDVIEWPROJ_MATRIX,
		/** Provides transpose of concatenated world, view and projection matrices.
		Equivalent to RenderMonkey's "WorldViewProjectionTranspose".
		*/
		ACT_TRANSPOSE_WORLDVIEWPROJ_MATRIX,
		/** Provides inverse transpose of concatenated world, view and projection
		matrices. Equivalent to RenderMonkey's "WorldViewProjectionInverseTranspose".
		*/
		ACT_INVERSE_TRANSPOSE_WORLDVIEWPROJ_MATRIX,
		

		/// render target related values
		/** -1 if requires texture flipping, +1 otherwise. It's useful when you bypassed
		projection matrix transform, still able use this value to adjust transformed y position.
		*/
		ACT_RENDER_TARGET_FLIPPING,

		/** -1 if the winding has been inverted (e.g. for reflections), +1 otherwise.
		*/
		ACT_VERTEX_WINDING,

		/// Fog colour
		ACT_FOG_COLOUR,
		/// Fog params: density, linear start, linear end, 1/(end-start)
		ACT_FOG_PARAMS,


		/// Surface ambient colour, as set in Pass::setAmbient
		ACT_SURFACE_AMBIENT_COLOUR,
		/// Surface diffuse colour, as set in Pass::setDiffuse
		ACT_SURFACE_DIFFUSE_COLOUR,
		/// Surface specular colour, as set in Pass::setSpecular
		ACT_SURFACE_SPECULAR_COLOUR,
		/// Surface emissive colour, as set in Pass::setSelfIllumination
		ACT_SURFACE_EMISSIVE_COLOUR,
		/// Surface shininess, as set in Pass::setShininess
		ACT_SURFACE_SHININESS,


		/// The number of active light sources (better than gl_MaxLights)
		ACT_LIGHT_COUNT,


		/// The ambient light colour set in the scene
		ACT_AMBIENT_LIGHT_COLOUR,

		/// Light diffuse colour (index determined by setAutoConstant call)
		ACT_LIGHT_DIFFUSE_COLOUR,
		/// Light specular colour (index determined by setAutoConstant call)
		ACT_LIGHT_SPECULAR_COLOUR,
		/// Light attenuation parameters, Vector4(range, constant, linear, quadric)
		ACT_LIGHT_ATTENUATION,
		/** Spotlight parameters, Vector4(innerFactor, outerFactor, falloff, isSpot)
		innerFactor and outerFactor are cos(angle/2)
		The isSpot parameter is 0.0f for non-spotlights, 1.0f for spotlights.
		Also for non-spotlights the inner and outer factors are 1 and nearly 1 respectively
		*/
		ACT_SPOTLIGHT_PARAMS,
		/// A light position in world space (index determined by setAutoConstant call)
		ACT_LIGHT_POSITION,
		/// A light position in object space (index determined by setAutoConstant call)
		ACT_LIGHT_POSITION_OBJECT_SPACE,
		/// A light position in view space (index determined by setAutoConstant call)
		ACT_LIGHT_POSITION_VIEW_SPACE,
		/// A light direction in world space (index determined by setAutoConstant call)
		ACT_LIGHT_DIRECTION,
		/// A light direction in object space (index determined by setAutoConstant call)
		ACT_LIGHT_DIRECTION_OBJECT_SPACE,
		/// A light direction in view space (index determined by setAutoConstant call)
		ACT_LIGHT_DIRECTION_VIEW_SPACE,
		/** The distance of the light from the center of the object
		a useful approximation as an alternative to per-vertex distance
		calculations.
		*/
		ACT_LIGHT_DISTANCE_OBJECT_SPACE,
		/** Light power level, a single scalar as set in Light::setPowerScale  (index determined by setAutoConstant call) */
		ACT_LIGHT_POWER_SCALE,
		/// Light diffuse colour pre-scaled by Light::setPowerScale (index determined by setAutoConstant call)
		ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED,
		/// Light specular colour pre-scaled by Light::setPowerScale (index determined by setAutoConstant call)
		ACT_LIGHT_SPECULAR_COLOUR_POWER_SCALED,
		/// Array of light diffuse colours (count set by extra param)
		ACT_LIGHT_DIFFUSE_COLOUR_ARRAY,
		/// Array of light specular colours (count set by extra param)
		ACT_LIGHT_SPECULAR_COLOUR_ARRAY,
		/// Array of light diffuse colours scaled by light power (count set by extra param)
		ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED_ARRAY,
		/// Array of light specular colours scaled by light power (count set by extra param)
		ACT_LIGHT_SPECULAR_COLOUR_POWER_SCALED_ARRAY,
		/// Array of light attenuation parameters, Vector4(range, constant, linear, quadric) (count set by extra param)
		ACT_LIGHT_ATTENUATION_ARRAY,
		/// Array of light positions in world space (count set by extra param)
		ACT_LIGHT_POSITION_ARRAY,
		/// Array of light positions in object space (count set by extra param)
		ACT_LIGHT_POSITION_OBJECT_SPACE_ARRAY,
		/// Array of light positions in view space (count set by extra param)
		ACT_LIGHT_POSITION_VIEW_SPACE_ARRAY,
		/// Array of light directions in world space (count set by extra param)
		ACT_LIGHT_DIRECTION_ARRAY,
		/// Array of light directions in object space (count set by extra param)
		ACT_LIGHT_DIRECTION_OBJECT_SPACE_ARRAY,
		/// Array of light directions in view space (count set by extra param)
		ACT_LIGHT_DIRECTION_VIEW_SPACE_ARRAY,
		/** Array of distances of the lights from the center of the object
		a useful approximation as an alternative to per-vertex distance
		calculations. (count set by extra param)
		*/
		ACT_LIGHT_DISTANCE_OBJECT_SPACE_ARRAY,
		/** Array of light power levels, a single scalar as set in Light::setPowerScale
		(count set by extra param)
		*/
		ACT_LIGHT_POWER_SCALE_ARRAY,
		/** Spotlight parameters array of Vector4(innerFactor, outerFactor, falloff, isSpot)
		innerFactor and outerFactor are cos(angle/2)
		The isSpot parameter is 0.0f for non-spotlights, 1.0f for spotlights.
		Also for non-spotlights the inner and outer factors are 1 and nearly 1 respectively.
		(count set by extra param)
		*/
		ACT_SPOTLIGHT_PARAMS_ARRAY,

		/** The derived ambient light colour, with 'r', 'g', 'b' components filled with
		product of surface ambient colour and ambient light colour, respectively,
		and 'a' component filled with surface ambient alpha component.
		*/
		ACT_DERIVED_AMBIENT_LIGHT_COLOUR,
		/** The derived scene colour, with 'r', 'g' and 'b' components filled with sum
		of derived ambient light colour and surface emissive colour, respectively,
		and 'a' component filled with surface diffuse alpha component.
		*/
		ACT_DERIVED_SCENE_COLOUR,

		/** The derived light diffuse colour (index determined by setAutoConstant call),
		with 'r', 'g' and 'b' components filled with product of surface diffuse colour,
		light power scale and light diffuse colour, respectively, and 'a' component filled with surface
		diffuse alpha component.
		*/
		ACT_DERIVED_LIGHT_DIFFUSE_COLOUR,
		/** The derived light specular colour (index determined by setAutoConstant call),
		with 'r', 'g' and 'b' components filled with product of surface specular colour
		and light specular colour, respectively, and 'a' component filled with surface
		specular alpha component.
		*/
		ACT_DERIVED_LIGHT_SPECULAR_COLOUR,

		/// Array of derived light diffuse colours (count set by extra param)
		ACT_DERIVED_LIGHT_DIFFUSE_COLOUR_ARRAY,
		/// Array of derived light specular colours (count set by extra param)
		ACT_DERIVED_LIGHT_SPECULAR_COLOUR_ARRAY,
		/** The absolute light number of a local light index. Each pass may have
		a number of lights passed to it, and each of these lights will have
		an index in the overall light list, which will differ from the local
		light index due to factors like setStartLight and setIteratePerLight.
		This binding provides the global light index for a local index.
		*/
		ACT_LIGHT_NUMBER,
		/// Returns (int) 1 if the  given light casts shadows, 0 otherwise (index set in extra param)
		ACT_LIGHT_CASTS_SHADOWS,


		/** The distance a shadow volume should be extruded when using
		finite extrusion programs.
		*/
		ACT_SHADOW_EXTRUSION_DISTANCE,
		/// The current camera's position in world space
		ACT_CAMERA_POSITION,
		/// The current camera's position in object space 
		ACT_CAMERA_POSITION_OBJECT_SPACE,
		/// The view/projection matrix of the assigned texture projection frustum
		ACT_TEXTURE_VIEWPROJ_MATRIX,
		/// Array of view/projection matrices of the first n texture projection frustums
		ACT_TEXTURE_VIEWPROJ_MATRIX_ARRAY,
		/** The view/projection matrix of the assigned texture projection frustum,
		combined with the current world matrix
		*/
		ACT_TEXTURE_WORLDVIEWPROJ_MATRIX,
		/// Array of world/view/projection matrices of the first n texture projection frustums
		ACT_TEXTURE_WORLDVIEWPROJ_MATRIX_ARRAY,
		/// The view/projection matrix of a given spotlight
		ACT_SPOTLIGHT_VIEWPROJ_MATRIX,
		/// Array of view/projection matrix of a given spotlight
		ACT_SPOTLIGHT_VIEWPROJ_MATRIX_ARRAY,
		/** The view/projection matrix of a given spotlight projection frustum,
		combined with the current world matrix
		*/
		ACT_SPOTLIGHT_WORLDVIEWPROJ_MATRIX,
		/// A custom parameter which will come from the renderable, using 'data' as the identifier
		ACT_CUSTOM,
		/** provides current elapsed time
		*/
		ACT_TIME,
		/** Single float value, which repeats itself based on given as
		parameter "cycle time". Equivalent to RenderMonkey's "Time0_X".
		*/
		ACT_TIME_0_X,
		/// Cosine of "Time0_X". Equivalent to RenderMonkey's "CosTime0_X".
		ACT_COSTIME_0_X,
		/// Sine of "Time0_X". Equivalent to RenderMonkey's "SinTime0_X".
		ACT_SINTIME_0_X,
		/// Tangent of "Time0_X". Equivalent to RenderMonkey's "TanTime0_X".
		ACT_TANTIME_0_X,
		/** Vector of "Time0_X", "SinTime0_X", "CosTime0_X",
		"TanTime0_X". Equivalent to RenderMonkey's "Time0_X_Packed".
		*/
		ACT_TIME_0_X_PACKED,
		/** Single float value, which represents scaled time value [0..1],
		which repeats itself based on given as parameter "cycle time".
		Equivalent to RenderMonkey's "Time0_1".
		*/
		ACT_TIME_0_1,
		/// Cosine of "Time0_1". Equivalent to RenderMonkey's "CosTime0_1".
		ACT_COSTIME_0_1,
		/// Sine of "Time0_1". Equivalent to RenderMonkey's "SinTime0_1".
		ACT_SINTIME_0_1,
		/// Tangent of "Time0_1". Equivalent to RenderMonkey's "TanTime0_1".
		ACT_TANTIME_0_1,
		/** Vector of "Time0_1", "SinTime0_1", "CosTime0_1",
		"TanTime0_1". Equivalent to RenderMonkey's "Time0_1_Packed".
		*/
		ACT_TIME_0_1_PACKED,
		/**	Single float value, which represents scaled time value [0..2*Pi],
		which repeats itself based on given as parameter "cycle time".
		Equivalent to RenderMonkey's "Time0_2PI".
		*/
		ACT_TIME_0_2PI,
		/// Cosine of "Time0_2PI". Equivalent to RenderMonkey's "CosTime0_2PI".
		ACT_COSTIME_0_2PI,
		/// Sine of "Time0_2PI". Equivalent to RenderMonkey's "SinTime0_2PI".
		ACT_SINTIME_0_2PI,
		/// Tangent of "Time0_2PI". Equivalent to RenderMonkey's "TanTime0_2PI".
		ACT_TANTIME_0_2PI,
		/** Vector of "Time0_2PI", "SinTime0_2PI", "CosTime0_2PI",
		"TanTime0_2PI". Equivalent to RenderMonkey's "Time0_2PI_Packed".
		*/
		ACT_TIME_0_2PI_PACKED,
		/// provides the scaled frame time, returned as a floating point value.
		ACT_FRAME_TIME,
		/// provides the calculated frames per second, returned as a floating point value.
		ACT_FPS,
		/// viewport-related values
		/** Current viewport width (in pixels) as floating point value.
		Equivalent to RenderMonkey's "ViewportWidth".
		*/
		ACT_VIEWPORT_WIDTH,
		/** Current viewport height (in pixels) as floating point value.
		Equivalent to RenderMonkey's "ViewportHeight".
		*/
		ACT_VIEWPORT_HEIGHT,
		ACT_VIEWPORT_WIDTH_AND_HEIGHT,

		/** This variable represents 1.0/ViewportWidth.
		Equivalent to RenderMonkey's "ViewportWidthInverse".
		*/
		ACT_INVERSE_VIEWPORT_WIDTH,
		/** This variable represents 1.0/ViewportHeight.
		Equivalent to RenderMonkey's "ViewportHeightInverse".
		*/
		ACT_INVERSE_VIEWPORT_HEIGHT,
		/** Packed of "ViewportWidth", "ViewportHeight", "ViewportWidthInverse",
		"ViewportHeightInverse".
		*/
		ACT_VIEWPORT_SIZE,

		/// view parameters
		/** This variable provides the view direction vector (world space).
		Equivalent to RenderMonkey's "ViewDirection".
		*/
		ACT_VIEW_DIRECTION,
		/** This variable provides the view side vector (world space).
		Equivalent to RenderMonkey's "ViewSideVector".
		*/
		ACT_VIEW_SIDE_VECTOR,
		/** This variable provides the view up vector (world space).
		Equivalent to RenderMonkey's "ViewUpVector".
		*/
		ACT_VIEW_UP_VECTOR,
		/** This variable provides the field of view as a floating point value.
		Equivalent to RenderMonkey's "FOV".
		*/
		ACT_FOV,
		/**	This variable provides the near clip distance as a floating point value.
		Equivalent to RenderMonkey's "NearClipPlane".
		*/
		ACT_NEAR_CLIP_DISTANCE,
		/**	This variable provides the far clip distance as a floating point value.
		Equivalent to RenderMonkey's "FarClipPlane".
		*/
		ACT_FAR_CLIP_DISTANCE,

	    ACT_FAR_AND_NEAR_CLIP_DISTANCE,
		ACT_WIDTH_INVERSE_HIGHT_CLIP,
		/** provides the pass index number within the technique
		of the active materil.
		*/
		ACT_PASS_NUMBER,

		/** provides the current iteration number of the pass. The iteration
		number is the number of times the current render operation has
		been drawn for the active pass.
		*/
		ACT_PASS_ITERATION_NUMBER,


		/** Provides a parametric animation value [0..1], only available
		where the renderable specifically implements it.
		*/
		ACT_ANIMATION_PARAMETRIC,

		/** Provides the texel offsets required by this rendersystem to map
		texels to pixels. Packed as
		float4(absoluteHorizontalOffset, absoluteVerticalOffset,
		horizontalOffset / viewportWidth, verticalOffset / viewportHeight)
		*/
		ACT_TEXEL_OFFSETS,

		/** Provides information about the depth range of the scene as viewed
		from the current camera.
		Passed as float4(minDepth, maxDepth, depthRange, 1 / depthRange)
		*/
		ACT_SCENE_DEPTH_RANGE,

		/** Provides information about the depth range of the scene as viewed
		from a given shadow camera. Requires an index parameter which maps
		to a light index relative to the current light list.
		Passed as float4(minDepth, maxDepth, depthRange, 1 / depthRange)
		*/
		ACT_SHADOW_SCENE_DEPTH_RANGE,

		/** Provides the fixed shadow colour as configured via SceneManager::setShadowColour;
		useful for integrated modulative shadows.
		*/
		ACT_SHADOW_COLOUR,
		/** Provides texture size of the texture unit (index determined by setAutoConstant
		call). Packed as float4(width, height, depth, 1)
		*/
		ACT_TEXTURE_SIZE,
		/** Provides inverse texture size of the texture unit (index determined by setAutoConstant
		call). Packed as float4(1 / width, 1 / height, 1 / depth, 1)
		*/
		ACT_INVERSE_TEXTURE_SIZE,
		/** Provides packed texture size of the texture unit (index determined by setAutoConstant
		call). Packed as float4(width, height, 1 / width, 1 / height)
		*/
		ACT_PACKED_TEXTURE_SIZE,

		/** Provides the current transform matrix of the texture unit (index determined by setAutoConstant
		call), as seen by the fixed-function pipeline.
		*/
		ACT_TEXTURE_MATRIX,

		/** Provides the position of the LOD camera in world space, allowing you
		to perform separate LOD calculations in shaders independent of the rendering
		camera. If there is no separate LOD camera then this is the real camera
		position. See Camera::setLodCamera.
		*/
		ACT_LOD_CAMERA_POSITION,
		/** Provides the position of the LOD camera in object space, allowing you
		to perform separate LOD calculations in shaders independent of the rendering
		camera. If there is no separate LOD camera then this is the real camera
		position. See Camera::setLodCamera.
		*/
		ACT_LOD_CAMERA_POSITION_OBJECT_SPACE,
		/** Binds custom per-light constants to the shaders. */
		ACT_LIGHT_CUSTOM,

		ACT_VIEW_PORT_INFORMATION
	};
	enum ACDataType {
		/// no data is required
		ACDT_NONE,
		/// the auto constant requires data of type int
		ACDT_INT,
		/// the auto constant requires data of type real
		ACDT_REAL
	};
	/** Structure recording the use of an automatic parameter. */
	class AutoConstantEntry
	{
	public:
		/// The type of parameter
		AutoConstantType paramType;
		/// The target (physical) constant index
		int physicalIndex;
		/** The number of elements per individual entry in this constant
		Used in case people used packed elements smaller than 4 (e.g. GLSL)
		and bind an auto which is 4-element packed to it */
		int elementCount;
		/// Additional information to go with the parameter
		union{
			int data;
			float fData;
		};
		/// The variability of this parameter (see GpuParamVariability)
		unsigned short variability;

		AutoConstantEntry(AutoConstantType theType, int theIndex, int theData,
			unsigned short theVariability, int theElemCount = 4)
			: paramType(theType), physicalIndex(theIndex), elementCount(theElemCount),
			data(theData), variability(theVariability) {}

		AutoConstantEntry(AutoConstantType theType, int theIndex, float theData,
			unsigned short theVariability, int theElemCount = 4)
			: paramType(theType), physicalIndex(theIndex), elementCount(theElemCount),
			fData(theData), variability(theVariability) {}

	};
	// Auto parameter storage
	typedef std::vector<AutoConstantEntry> AutoConstantList;
	enum AutoUniformObjectType
	{
		AUOT_VIEWPORT_INFORMATION,
		AUOT_LIHGT_INFORMATION
	}; 
	template<typename UniformBufferStruct>
	struct AutoUniformBufferObject
	{
		AutoUniformObjectType UniformObjectType;
		std::string Name;
		std::vector<AutoConstantType> AutoConstantTypeList;
		typedef UniformBufferStruct UniformBufferStructType;
		TBWUniformBufferObjectPtr<UniformBufferStructType> UniforBufferObjectPtr;
		AutoUniformBufferObject(AutoUniformObjectType InUniformObjectType, const std::string& InName, const std::vector<AutoConstantType>& InAutoConstantTypeList)
		{
			UniformObjectType = InUniformObjectType;
			Name = InName;
			AutoConstantTypeList = InAutoConstantTypeList;
			UniforBufferObjectPtr = UniformBufferStructType::CreateUniformBufferObject();
		}

	};

	struct AutoConstantDefinition
	{
		AutoConstantType acType;
		std::string name;
		int elementCount;
		ElementType eleType;
		ACDataType dataType;
		AutoConstantDefinition(AutoConstantType _acType, const std::string& _name,
			int elemtCount, ElementType _elementType,
			ACDataType _dataType)
			:acType(_acType), name(_name), elementCount(elemtCount),
			eleType(_elementType), dataType(_dataType)
		{

		}
	};
public:
	typedef std::vector<int> IntList;
	typedef std::vector<float> FloatList;

	void AddSharedParameters(const std::string&);
	void AddSharedParameters(BWGpuSharedParametersPtr sharedParam);
	//SetNamedConstant 主要是用来设置各种 命名常量的值 其中包括自动命名常量
	void SetNamedConstant(const std::string&, BWMatrix4&);
	void SetConstant(int, BWMatrix4&);
	void SetNamedConstant(const std::string&, const int *, int, int multiple = 4);
	void SetConstant(int,const int*, int multiple = 4);
	void SetNamedConstant(const std::string&,const float *, int, int mutiple = 4);
	void SetConstant(int,const float*, int);
	void SetNamedConstants(const GpuNamedConstantsPtr &);
	void WriteRawConstants(const int physicalIndex, const int *value, int count);
	void WriteRawConstants(const int physicalIndex, const float *value, int count);
	//SetNamedAutoConstant 主要是用来添加自动命名常量 
	void SetNamedAutoConstant(const std::string &name, AutoConstantType type, unsigned int extraInfor = 0);
	void SetAutoConstant(int, AutoConstantType, unsigned int extraInfor = 0);
	void SetNamedAutoConstantReal(const std::string&, AutoConstantType, float);
	void SetAutoConstantReal(int, AutoConstantType, float);
	void SetRawAutoConstant(int physicalIndex, AutoConstantType type, int extraInfor, unsigned short ability, int elementsize);
	void ClearNamedAutoConstant(const std::string&);

	//void AddAutoUniformBufferObject(const AutoUniformBufferObject* InUniformBufferObject);
	void SetLogicalIndexes(const GpuLogicalBufferStructPtr& intLogicalToPhysical, const GpuLogicalBufferStructPtr &floatLogicalToPhysical);
    const FloatConstantList& GetFlotConstantList() const { return floatConstants; }
	const IntConstantList& GetIntConstantList() const { return intConstants; }
	const AutoConstantList& GetAutoConstantList() const { return autoConstantSet; }
	const float* GetFloatPointer(int index) const { return &floatConstants[index]; }
	const int *  GetIntPointer(int index) const { return &intConstants[index]; }
	 float* GetFloatPointer(int index)  { return &floatConstants[index]; }
	 int *  GetIntPointer(int index)  { return &intConstants[index]; }

	void CopySharedParamSetUsage(const BWGpuSharedParameterUsageList& sharedList);
	void CopyConstantsFrom(const BWGpuProgramParameters&);
	void CopyMatchingNamedConstantForm(const BWGpuProgramParameters&);
	void ClearAutoConstant(int);
	int GetIntConstantPhysicalIndex(int logicalIndex, int requestedSize, unsigned short variablility);
	int GetFloatConstantPhysicalIndex(int logicalIndex, int requestedSize, unsigned short variablility);
	//使用逻辑索引更新物理索引，数据长度等数据 
	GpuLogicalIndexUse* GetIntConstantLogicalIndexUse(int logicalIndex, int requestedSize, unsigned short variablility);
	GpuLogicalIndexUse* GetFloatConstantLogicalIndexUse(int logicalIndex, int requestedSize, unsigned short variablility);
	bool IsUsingSharedParameters(const std::string &name);
	const GpuConstantDefinition* FindNamedConstantDefinition(const std::string & name, bool throwExceptionIfNotFound);
    static const AutoConstantDefinition* GetAutoConstantDefinition(const std::string&);
	static const AutoConstantDefinition* GetAutoConstantDefinition(const int idx);
	static const AutoConstantDefinition* GetAutoConstantDefinition(AutoConstantType InACT);
	//static const AutoUniformBufferObject* GetAutoUniformObject(const std::string &InName);
	static int  GetNumAutoConstantDefinition();
	unsigned short deriveVariability(AutoConstantType act);
	const GpuNamedConstantsPtr getNamedConstants() const;

	//If There Have A Lot Of Global Uniform Buffer , We Can Use TempleteList To Manager All Of Them
	static AutoUniformBufferObject<ViewportInforUniformBufferStruct> GlobalViewportInformation;
	static void UpdateViewportInformationBuffer(ViewportInforUniformBufferStruct& InViewportInformation);
	bool IsHaveGlobalUniformBufferObject() const;
	bool IsGlobalUniformBufferHaveTheMember(const std::string &Name) const;
	void SetGlobalViewportInformation(const std::string &ShaderViewportInformationStructName);
	const std::string& GetGlobalViewportInformatioinStructName();
	const AutoUniformBufferObject<ViewportInforUniformBufferStruct>* GetGlobalViewportInformation() const { return GlobalViewportInformationPtr; }
private:
	//logicalIndex指的是 GPU程序中的数据的索引  比如 layout(loaction = 8) vec4 position, 其中8 就是逻辑索引  而物理索引指的是传入
	//GPU程序中参数的数据存放的地址 也就是下面各种intConstants , intConstants等中的位置   
	 
	 static AutoConstantDefinition autoConstantDictionary[];// 所有自动常量的信息
	 

	 AutoUniformBufferObject<ViewportInforUniformBufferStruct>* GlobalViewportInformationPtr{NULL};
	 std::string ShaderViewportInformationStructName;
	 unsigned short combinVariability;
	 bool ignoreMissingParams;
	 BWGpuSharedParameterUsageList gpuSharedParametersSet; //共享参数的信息

	 GpuNamedConstantsPtr namedConstantSet;              //负责将所有命名常量(包括自动常量)的名字和其物理索引 ，逻辑索引等等定义联系起来 这是一个指针 指向的是GPUprogram中的namedconstant
	 
	 AutoConstantList autoConstantSet;                   //负责记录使用了那些自动常量 主要包含其物理索引 其逻辑索引放在namedConstantSet
	 GpuLogicalBufferStructPtr intLogicalToPhysical;     //负责 将int类型的数据 从逻辑索引转换到物理索引
	 GpuLogicalBufferStructPtr floatLogicalToPhysical;   //负责 将float类型的数据 从逻辑索引转换到物理索引
	 //这两个列表存放了所有要闯入gpu参数中的数据 在本类中 logicalIndex指的是 GPU程序中的数据的索引  比如 layout(loaction = 8) vec4 position, 其中8 就是逻辑索引  而物理索引指的是传入
	 //GPU程序中参数的数据存放的地址 也就是下面各种list中的位置   
	 IntList intConstants;
	 FloatList floatConstants;
};
#endif