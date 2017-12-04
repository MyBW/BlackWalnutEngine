#include "BWGpuProgramParams.h"
#include "BWGpuProgramManager.h"
#include "BWStringOperation.h"
#include "BWStringConverter.h"
#include <cassert>
#include <memory>

BWGpuProgramParameters::AutoConstantDefinition BWGpuProgramParameters::autoConstantDictionary[] = {
	AutoConstantDefinition(ACT_WORLD_MATRIX, "world_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_PRE_WORLD_MATRIX, "pre_world_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_INVERSE_WORLD_MATRIX, "inverse_world_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_TRANSPOSE_WORLD_MATRIX, "transpose_world_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_INVERSE_TRANSPOSE_WORLD_MATRIX, "inverse_transpose_world_matrix", 16, ET_REAL, ACDT_NONE),

	AutoConstantDefinition(ACT_WORLD_MATRIX_ARRAY_3x4, "world_matrix_array_3x4", 12, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_WORLD_MATRIX_ARRAY, "world_matrix_array", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_WORLD_DUALQUATERNION_ARRAY_2x4, "world_dualquaternion_array_2x4", 8, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_WORLD_SCALE_SHEAR_MATRIX_ARRAY_3x4, "world_scale_shear_matrix_array_3x4", 9, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_VIEW_MATRIX, "view_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_PRE_VIEW_MATIX, "pre_view_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_INVERSE_VIEW_MATRIX, "inverse_view_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_TRANSPOSE_VIEW_MATRIX, "transpose_view_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_INVERSE_TRANSPOSE_VIEW_MATRIX, "inverse_transpose_view_matrix", 16, ET_REAL, ACDT_NONE),

	AutoConstantDefinition(ACT_PROJECTION_MATRIX, "projection_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_PRE_PROJECTION_MATRIX,"pre_projection_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_INVERSE_PROJECTION_MATRIX, "inverse_projection_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_TRANSPOSE_PROJECTION_MATRIX, "transpose_projection_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_INVERSE_TRANSPOSE_PROJECTION_MATRIX, "inverse_transpose_projection_matrix", 16, ET_REAL, ACDT_NONE),

	AutoConstantDefinition(ACT_VIEWPROJ_MATRIX, "viewproj_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_INVERSE_VIEWPROJ_MATRIX, "inverse_viewproj_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_TRANSPOSE_VIEWPROJ_MATRIX, "transpose_viewproj_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_INVERSE_TRANSPOSE_VIEWPROJ_MATRIX, "inverse_transpose_viewproj_matrix", 16, ET_REAL, ACDT_NONE),

	AutoConstantDefinition(ACT_WORLDVIEW_MATRIX, "worldview_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_INVERSE_WORLDVIEW_MATRIX, "inverse_worldview_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_TRANSPOSE_WORLDVIEW_MATRIX, "transpose_worldview_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_INVERSE_TRANSPOSE_WORLDVIEW_MATRIX, "inverse_transpose_worldview_matrix", 16, ET_REAL, ACDT_NONE),

	AutoConstantDefinition(ACT_WORLDVIEWPROJ_MATRIX, "worldviewproj_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_INVERSE_WORLDVIEWPROJ_MATRIX, "inverse_worldviewproj_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_TRANSPOSE_WORLDVIEWPROJ_MATRIX, "transpose_worldviewproj_matrix", 16, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_INVERSE_TRANSPOSE_WORLDVIEWPROJ_MATRIX, "inverse_transpose_worldviewproj_matrix", 16, ET_REAL, ACDT_NONE),

	AutoConstantDefinition(ACT_RENDER_TARGET_FLIPPING, "render_target_flipping", 1, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_VERTEX_WINDING, "vertex_winding", 1, ET_REAL, ACDT_NONE),

	AutoConstantDefinition(ACT_FOG_COLOUR, "fog_colour", 4, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_FOG_PARAMS, "fog_params", 4, ET_REAL, ACDT_NONE),

	AutoConstantDefinition(ACT_SURFACE_AMBIENT_COLOUR, "surface_ambient_colour", 4, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_SURFACE_DIFFUSE_COLOUR, "surface_diffuse_colour", 4, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_SURFACE_SPECULAR_COLOUR, "surface_specular_colour", 4, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_SURFACE_EMISSIVE_COLOUR, "surface_emissive_colour", 4, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_SURFACE_SHININESS, "surface_shininess", 1, ET_REAL, ACDT_NONE),

	AutoConstantDefinition(ACT_LIGHT_COUNT, "light_count", 1, ET_REAL, ACDT_NONE),

	AutoConstantDefinition(ACT_AMBIENT_LIGHT_COLOUR, "ambient_light_colour", 4, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_LIGHT_DIFFUSE_COLOUR, "light_diffuse_colour", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_SPECULAR_COLOUR, "light_specular_colour", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_ATTENUATION, "light_attenuation", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_SPOTLIGHT_PARAMS, "spotlight_params", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_POSITION, "light_position", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_POSITION_OBJECT_SPACE, "light_position_object_space", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_POSITION_VIEW_SPACE, "light_position_view_space", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_DIRECTION, "light_direction", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_DIRECTION_OBJECT_SPACE, "light_direction_object_space", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_DIRECTION_VIEW_SPACE, "light_direction_view_space", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_DISTANCE_OBJECT_SPACE, "light_distance_object_space", 1, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_POWER_SCALE, "light_power", 1, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED, "light_diffuse_colour_power_scaled", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_SPECULAR_COLOUR_POWER_SCALED, "light_specular_colour_power_scaled", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_DIFFUSE_COLOUR_ARRAY, "light_diffuse_colour_array", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_SPECULAR_COLOUR_ARRAY, "light_specular_colour_array", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED_ARRAY, "light_diffuse_colour_power_scaled_array", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_SPECULAR_COLOUR_POWER_SCALED_ARRAY, "light_specular_colour_power_scaled_array", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_ATTENUATION_ARRAY, "light_attenuation_array", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_POSITION_ARRAY, "light_position_array", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_POSITION_OBJECT_SPACE_ARRAY, "light_position_object_space_array", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_POSITION_VIEW_SPACE_ARRAY, "light_position_view_space_array", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_DIRECTION_ARRAY, "light_direction_array", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_DIRECTION_OBJECT_SPACE_ARRAY, "light_direction_object_space_array", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_DIRECTION_VIEW_SPACE_ARRAY, "light_direction_view_space_array", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_DISTANCE_OBJECT_SPACE_ARRAY, "light_distance_object_space_array", 1, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_POWER_SCALE_ARRAY, "light_power_array", 1, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_SPOTLIGHT_PARAMS_ARRAY, "spotlight_params_array", 4, ET_REAL, ACDT_INT),

	AutoConstantDefinition(ACT_DERIVED_AMBIENT_LIGHT_COLOUR, "derived_ambient_light_colour", 4, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_DERIVED_SCENE_COLOUR, "derived_scene_colour", 4, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_DERIVED_LIGHT_DIFFUSE_COLOUR, "derived_light_diffuse_colour", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_DERIVED_LIGHT_SPECULAR_COLOUR, "derived_light_specular_colour", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_DERIVED_LIGHT_DIFFUSE_COLOUR_ARRAY, "derived_light_diffuse_colour_array", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_DERIVED_LIGHT_SPECULAR_COLOUR_ARRAY, "derived_light_specular_colour_array", 4, ET_REAL, ACDT_INT),

	AutoConstantDefinition(ACT_LIGHT_NUMBER, "light_number", 1, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LIGHT_CASTS_SHADOWS, "light_casts_shadows", 1, ET_REAL, ACDT_INT),

	AutoConstantDefinition(ACT_SHADOW_EXTRUSION_DISTANCE, "shadow_extrusion_distance", 1, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_CAMERA_POSITION, "camera_position", 3, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_CAMERA_POSITION_OBJECT_SPACE, "camera_position_object_space", 3, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_TEXTURE_VIEWPROJ_MATRIX, "texture_viewproj_matrix", 16, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_TEXTURE_VIEWPROJ_MATRIX_ARRAY, "texture_viewproj_matrix_array", 16, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_TEXTURE_WORLDVIEWPROJ_MATRIX, "texture_worldviewproj_matrix", 16, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_TEXTURE_WORLDVIEWPROJ_MATRIX_ARRAY, "texture_worldviewproj_matrix_array", 16, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_SPOTLIGHT_VIEWPROJ_MATRIX, "spotlight_viewproj_matrix", 16, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_SPOTLIGHT_VIEWPROJ_MATRIX_ARRAY, "spotlight_viewproj_matrix_array", 16, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_SPOTLIGHT_WORLDVIEWPROJ_MATRIX, "spotlight_worldviewproj_matrix", 16, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_CUSTOM, "custom", 4, ET_REAL, ACDT_INT),  // *** needs to be tested
	AutoConstantDefinition(ACT_TIME, "time", 1, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_TIME_0_X, "time_0_x", 4, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_COSTIME_0_X, "costime_0_x", 4, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_SINTIME_0_X, "sintime_0_x", 4, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_TANTIME_0_X, "tantime_0_x", 4, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_TIME_0_X_PACKED, "time_0_x_packed", 4, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_TIME_0_1, "time_0_1", 4, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_COSTIME_0_1, "costime_0_1", 4, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_SINTIME_0_1, "sintime_0_1", 4, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_TANTIME_0_1, "tantime_0_1", 4, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_TIME_0_1_PACKED, "time_0_1_packed", 4, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_TIME_0_2PI, "time_0_2pi", 4, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_COSTIME_0_2PI, "costime_0_2pi", 4, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_SINTIME_0_2PI, "sintime_0_2pi", 4, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_TANTIME_0_2PI, "tantime_0_2pi", 4, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_TIME_0_2PI_PACKED, "time_0_2pi_packed", 4, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_FRAME_TIME, "frame_time", 1, ET_REAL, ACDT_REAL),
	AutoConstantDefinition(ACT_FPS, "fps", 1, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_VIEWPORT_WIDTH, "viewport_width", 1, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_VIEWPORT_HEIGHT, "viewport_height", 1, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_VIEWPORT_WIDTH_AND_HEIGHT, "viewport_width_and_height", 2, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_INVERSE_VIEWPORT_WIDTH, "inverse_viewport_width", 1, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_INVERSE_VIEWPORT_HEIGHT, "inverse_viewport_height", 1, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_VIEWPORT_SIZE, "viewport_size", 4, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_VIEW_DIRECTION, "view_direction", 3, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_VIEW_SIDE_VECTOR, "view_side_vector", 3, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_VIEW_UP_VECTOR, "view_up_vector", 3, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_FOV, "fov", 1, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_NEAR_CLIP_DISTANCE, "near_clip_distance", 1, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_FAR_CLIP_DISTANCE, "far_clip_distance", 1, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_FAR_AND_NEAR_CLIP_DISTANCE, "far_and_near_clip_distance", 2, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_WIDTH_INVERSE_HIGHT_CLIP, "width_inverse_hight_clip", 1, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_PASS_NUMBER, "pass_number", 1, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_PASS_ITERATION_NUMBER, "pass_iteration_number", 1, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_ANIMATION_PARAMETRIC, "animation_parametric", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_TEXEL_OFFSETS, "texel_offsets", 4, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_SCENE_DEPTH_RANGE, "scene_depth_range", 4, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_SHADOW_SCENE_DEPTH_RANGE, "shadow_scene_depth_range", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_SHADOW_COLOUR, "shadow_colour", 4, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_TEXTURE_SIZE, "texture_size", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_INVERSE_TEXTURE_SIZE, "inverse_texture_size", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_PACKED_TEXTURE_SIZE, "packed_texture_size", 4, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_TEXTURE_MATRIX, "texture_matrix", 16, ET_REAL, ACDT_INT),
	AutoConstantDefinition(ACT_LOD_CAMERA_POSITION, "lod_camera_position", 3, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_LOD_CAMERA_POSITION_OBJECT_SPACE, "lod_camera_position_object_space", 3, ET_REAL, ACDT_NONE),
	AutoConstantDefinition(ACT_LIGHT_CUSTOM, "light_custom", 4, ET_REAL, ACDT_INT),
};

IMPLEMEN_UNIFORM_BUFFER_STRUCT(ViewportInforUniformBufferStruct)

BWGpuProgramParameters::AutoUniformBufferObject<ViewportInforUniformBufferStruct> BWGpuProgramParameters::GlobalViewportInformation =
BWGpuProgramParameters::AutoUniformBufferObject<ViewportInforUniformBufferStruct>(
	AUOT_VIEWPORT_INFORMATION ,
	"viewport_information", 
	{
		ACT_VIEW_MATRIX,
		ACT_PROJECTION_MATRIX,
		ACT_PRE_VIEW_MATIX,
		ACT_PRE_PROJECTION_MATRIX,
		ACT_INVERSE_VIEW_MATRIX,
		ACT_CAMERA_POSITION,
		ACT_FOV,
		ACT_WIDTH_INVERSE_HIGHT_CLIP,
		ACT_FAR_AND_NEAR_CLIP_DISTANCE,
		ACT_VIEWPORT_WIDTH_AND_HEIGHT
	}
);

void BWGpuProgramParameters::UpdateViewportInformationBuffer(ViewportInforUniformBufferStruct& InViewportInformation)
{
	auto TansToGLMatrix = [](BWMatrix4& ToTrans)
	{
		float *tmpData = (float*)(ToTrans.M);
		for (int i = 0; i < 4; i++)
		{
			for (int j = i; j < 4; j++)
			{
				float a = tmpData[i * 4 + j];
				tmpData[i * 4 + j] = tmpData[j * 4 + i];
				tmpData[j * 4 + i] = a;
			}
		}
	};
	TansToGLMatrix(InViewportInformation.PreProjectMatrix);
	TansToGLMatrix(InViewportInformation.PreViewMatrix);
	TansToGLMatrix(InViewportInformation.ViewMatrix);
	TansToGLMatrix(InViewportInformation.ProjectMatrix);
	TansToGLMatrix(InViewportInformation.ViewInversMatrix);
	BWGpuProgramParameters::GlobalViewportInformation.UniforBufferObjectPtr->SetContent(InViewportInformation);
	BWGpuProgramParameters::GlobalViewportInformation.UniforBufferObjectPtr->UploadData();
}

bool BWGpuProgramParameters::IsHaveGlobalUniformBufferObject() const
{
	return GlobalViewportInformationPtr != nullptr;
}

bool BWGpuProgramParameters::IsGlobalUniformBufferHaveTheMember(const std::string &Name) const
{
	return ViewportInforUniformBufferStruct::StaticStruct.IsHaveMember(Name);
}

void BWGpuProgramParameters::SetGlobalViewportInformation(const std::string &InShaderViewportInformationStructName)
{
	GlobalViewportInformationPtr = &GlobalViewportInformation; 
	ShaderViewportInformationStructName = InShaderViewportInformationStructName;
}

const std::string& BWGpuProgramParameters::GetGlobalViewportInformatioinStructName()
{
	return ShaderViewportInformationStructName;
}

bool GpuNamedConstants::msGenerateAllConstantDefinitionArrayEntries = false;

//---------------------------------------------------------------------
void GpuNamedConstants::generateConstantDefinitionArrayEntries(
	const std::string& paramName, const GpuConstantDefinition& baseDef)
{
	// Copy definition for use with arrays
	GpuConstantDefinition arrayDef = baseDef;
	arrayDef.arraySize = 1;
	std::string arrayName;

	// Add parameters for array accessors
	// [0] will refer to the same location, [1+] will increment
	// only populate others individually up to 16 array slots so as not to get out of hand,
	// unless the system has been explicitly configured to allow all the parameters to be added

	// paramName[0] version will always exist 
	size_t maxArrayIndex = 1;
	if (baseDef.arraySize <= 16 || msGenerateAllConstantDefinitionArrayEntries)
		maxArrayIndex = baseDef.arraySize;

	for (size_t i = 0; i < maxArrayIndex; i++)
	{
		arrayName = paramName + "[" + StringConverter::ToString(int(i)) + "]";
		map.insert(GpuConstantDefinitionMap::value_type(arrayName, arrayDef));
		// increment location
		arrayDef.physicalIndex += arrayDef.elementSize;
	}
	// note no increment of buffer sizes since this is shared with main array def

}

//---------------------------------------------------------------------
bool GpuNamedConstants::getGenerateAllConstantDefinitionArrayEntries()
{
	return msGenerateAllConstantDefinitionArrayEntries;
}

//---------------------------------------------------------------------
void GpuNamedConstants::setGenerateAllConstantDefinitionArrayEntries(bool generateAll)
{
	msGenerateAllConstantDefinitionArrayEntries = generateAll;
}
//---------------------------------------------------------------------
//  GpuNamedConstants methods
//---------------------------------------------------------------------
void GpuNamedConstants::save(const std::string& filename) const
{
	/*GpuNamedConstantsSerializer ser;
	ser.exportNamedConstants(this, filename);*/
}
//---------------------------------------------------------------------
void GpuNamedConstants::load(BWDataStreamPrt& stream)
{
	return;
}

void BWGpuSharedParameters::AddConstantDefinition(const std::string& name , GpuConstantType type, int arraysize)
{
	GpuConstantDefinition def;
	def.arraySize = arraysize;
	def.constType = type;
	def.elementSize = GpuConstantDefinition::getElementSize(type, false);
	def.logicalIndex = 0;
	def.variability = GPV_GLOBAL;

    if (def.isFloat())
    {
		def.physicalIndex = flotConstantList.size() - 1;
		flotConstantList.resize(flotConstantList.size() + def.arraySize * def.elementSize);
    }
	else
	{
		def.physicalIndex = intConstantList.size() - 1;
		intConstantList.resize(intConstantList.size() + def.arraySize * def.elementSize);
	}
	namedConstant[name] = def;
	++version;
}
void BWGpuSharedParameters::SetNamedConstant(const std::string & name, float* val, int size)
{
	GpuConstantDefinitionMap::iterator itor = namedConstant.find(name);
	if (itor != namedConstant.end())
	{
		const GpuConstantDefinition & def = itor->second;
		memcpy(&flotConstantList[def.physicalIndex], val, std::min(size,int( def.elementSize * def.arraySize)));
	}
	MakeDirty();
}
void BWGpuSharedParameters::SetNamedConstant(const std::string & name, int* val, int size)
{
	GpuConstantDefinitionMap::iterator itor = namedConstant.find(name);
	if (itor != namedConstant.end())
	{
		const GpuConstantDefinition &def = itor->second;
		memcpy(&intConstantList[def.physicalIndex], val, std::min(size,int (def.arraySize * def.elementSize)));
	}
	MakeDirty();
}
void BWGpuSharedParameters::MakeDirty()
{
      // 因为数据发生了改变 所以 通知渲染系统。。。。。为啥要这样做？？？？？
}
BWGpuSharedParametersUsage::BWGpuSharedParametersUsage(BWGpuSharedParametersPtr sharedParam, BWGpuProgramParameters *param) :sharedParameter(sharedParam), parent(param)
{
}
void BWGpuSharedParametersUsage::InitCopyData()
{
	copyDataList.clear();

	const GpuConstantDefinitionMap& sharedmap = sharedParameter->GetConstantDefinitionMap();
	for (GpuConstantDefinitionMap::const_iterator i = sharedmap.begin(); i != sharedmap.end(); ++i)
	{
		const std::string & pName = i->first;
		const GpuConstantDefinition& shareddef = i->second;

		const GpuConstantDefinition* instdef = parent->FindNamedConstantDefinition(pName, false);
		if (instdef)
		{
			// Check that the definitions are the same 
			if (instdef->constType == shareddef.constType &&
				instdef->arraySize == shareddef.arraySize)
			{
				CopyDataEntry e;
				e.srcDefinition = &shareddef;
				e.dstDefinition = instdef;	
				copyDataList.push_back(e);
			}
		}

	}

}
void BWGpuSharedParametersUsage::CopySharedParamsToTargetParams()
{
	if (version != sharedParameter->GetVersion())
		InitCopyData();

	for (CopyDataList::iterator i = copyDataList.begin(); i != copyDataList.end(); ++i)
	{
		CopyDataEntry& e = *i;

		if (e.dstDefinition->isFloat())
		{
			const float* pSrc = sharedParameter->GetFloatPointer(e.srcDefinition->physicalIndex);
			float* pDst = parent->GetFloatPointer(e.dstDefinition->physicalIndex);

			// Deal with matrix transposition here!!!
			// transposition is specific to the dest param set, shared params don't do it
			if (e.dstDefinition->constType == GCT_MATRIX_4X4)
			{
				for (int row = 0; row < 4; ++row)
				for (int col = 0; col < 4; ++col)
					pDst[row * 4 + col] = pSrc[col * 4 + row];
			}
			else
			{
				if (e.dstDefinition->elementSize == e.srcDefinition->elementSize)
				{
					// simple copy
					memcpy(pDst, pSrc, sizeof(float)* e.dstDefinition->elementSize * e.dstDefinition->arraySize);
				}
				else
				{
					// target params may be padded to 4 elements, shared params are packed
					assert(e.dstDefinition->elementSize % 4 == 0);
					size_t iterations = e.dstDefinition->elementSize / 4
						* e.dstDefinition->arraySize;
					assert(iterations > 0);
					size_t valsPerIteration = e.srcDefinition->elementSize / iterations;
					for (size_t l = 0; l < iterations; ++l)
					{
						memcpy(pDst, pSrc, sizeof(float)* valsPerIteration);
						pSrc += valsPerIteration;
						pDst += 4;
					}
				}
			}
		}
		else
		{
			const int* pSrc = sharedParameter->GetIntPointer(e.srcDefinition->physicalIndex);
			int* pDst = parent->GetIntPointer(e.dstDefinition->physicalIndex);

			if (e.dstDefinition->elementSize == e.srcDefinition->elementSize)
			{
				// simple copy
				memcpy(pDst, pSrc, sizeof(int)* e.dstDefinition->elementSize * e.dstDefinition->arraySize);
			}
			else
			{
				// target params may be padded to 4 elements, shared params are packed
				assert(e.dstDefinition->elementSize % 4 == 0);
				size_t iterations = (e.dstDefinition->elementSize / 4)
					* e.dstDefinition->arraySize;
				assert(iterations > 0);
				size_t valsPerIteration = e.srcDefinition->elementSize / iterations;
				for (size_t l = 0; l < iterations; ++l)
				{
					memcpy(pDst, pSrc, sizeof(int)* valsPerIteration);
					pSrc += valsPerIteration;
					pDst += 4;
				}
			}
		}
	}
}

GpuLogicalIndexUse*  BWGpuProgramParameters::GetIntConstantLogicalIndexUse(int logicalIndex, int requestedSize, unsigned short variablility)
{
	if (intLogicalToPhysical.IsNull())
	{
		//异常
		return 0;
	}

	GpuLogicalIndexUse* indexUse = 0;

		GpuLogicalIndexUseMap::iterator logi = intLogicalToPhysical->map.find(logicalIndex);
	if (logi == intLogicalToPhysical->map.end())
	{
		if (requestedSize)
		{
			int physicalIndex = intConstants.size();

			// Expand at buffer end
			intConstants.insert(intConstants.end(), requestedSize, 0);

			// Record extended size for future GPU params re-using this information
			intLogicalToPhysical->bufferSize = intConstants.size();

			// low-level programs will not know about mapping ahead of time, so 
			// populate it. Other params objects will be able to just use this
			// accepted mapping since the constant structure will be the same

			// Set up a mapping for all items in the count
			int currPhys = physicalIndex;
			int count = requestedSize / 4;
			GpuLogicalIndexUseMap::iterator insertedIterator;
			for (int logicalNum = 0; logicalNum < count; ++logicalNum)
			{
				GpuLogicalIndexUseMap::iterator it =
					intLogicalToPhysical->map.insert(
					GpuLogicalIndexUseMap::value_type(
					logicalIndex + logicalNum,
					GpuLogicalIndexUse(currPhys, requestedSize, variablility))).first;
				if (logicalNum == 0)
					insertedIterator = it;
				currPhys += 4;
			}
			indexUse = &(insertedIterator->second);

		}
		else
		{
			// no match
			return 0;
		}

	}
	else
	{
		int physicalIndex = logi->second.physicalIndex;
		indexUse = &(logi->second);

		// check size
		if (logi->second.currentSize < requestedSize)
		{
			// init buffer entry wasn't big enough; could be a mistake on the part
			// of the original use, or perhaps a variable length we can't predict
			// until first actual runtime use e.g. world matrix array
			int insertCount = requestedSize - logi->second.currentSize;
			IntList::iterator insertPos = intConstants.begin();
			std::advance(insertPos, physicalIndex);
			intConstants.insert(insertPos, insertCount, 0);
			// shift all physical positions after this one
			for (GpuLogicalIndexUseMap::iterator i = intLogicalToPhysical->map.begin();
				i != intLogicalToPhysical->map.end(); ++i)
			{
				if (i->second.physicalIndex > physicalIndex)
					i->second.physicalIndex += insertCount;
			}
			intLogicalToPhysical->bufferSize += insertCount;
			for (AutoConstantList::iterator i = autoConstantSet.begin();
				i != autoConstantSet.end(); ++i)
			{
				if (i->physicalIndex > physicalIndex &&
					GetAutoConstantDefinition(i->paramType)->eleType == ET_INT)
				{
					i->physicalIndex += insertCount;
				}
			}
			if (!namedConstantSet.IsNull())
			{
				for (GpuConstantDefinitionMap::iterator i = namedConstantSet->map.begin();
					i != namedConstantSet->map.end(); ++i)
				{
					if (!i->second.isFloat() && i->second.physicalIndex > physicalIndex)
						i->second.physicalIndex += insertCount;
				}
				namedConstantSet->intBufferSize += insertCount;
			}

			logi->second.currentSize += insertCount;
		}
	}

	if (indexUse)
		indexUse->variability = variablility;

	return indexUse;
}
GpuLogicalIndexUse* BWGpuProgramParameters::GetFloatConstantLogicalIndexUse(int logicalIndex, int requestedSize, unsigned short variablility)
{
	if (floatLogicalToPhysical.IsNull())
		return 0;

	GpuLogicalIndexUse* indexUse = 0;

	GpuLogicalIndexUseMap::iterator logi = floatLogicalToPhysical->map.find(logicalIndex);
	if (logi == floatLogicalToPhysical->map.end())
	{
		if (requestedSize)
		{
			int physicalIndex = floatConstants.size();

			// Expand at buffer end
			floatConstants.insert(floatConstants.end(), requestedSize, 0.0f);

			// Record extended size for future GPU params re-using this information
			floatLogicalToPhysical->bufferSize = floatConstants.size();

			// low-level programs will not know about mapping ahead of time, so 
			// populate it. Other params objects will be able to just use this
			// accepted mapping since the constant structure will be the same

			// Set up a mapping for all items in the count
			int currPhys = physicalIndex;
			int count = requestedSize / 4;
			GpuLogicalIndexUseMap::iterator insertedIterator;

			for (int logicalNum = 0; logicalNum < count; ++logicalNum)
			{
				GpuLogicalIndexUseMap::iterator it =
					floatLogicalToPhysical->map.insert(
					GpuLogicalIndexUseMap::value_type(
					logicalIndex + logicalNum,
					GpuLogicalIndexUse(currPhys, requestedSize, variablility))).first;
				currPhys += 4;

				if (logicalNum == 0)
					insertedIterator = it;
			}

			indexUse = &(insertedIterator->second);
		}
		else
		{
			// no match & ignore
			return 0;
		}

	}
	else
	{
		int physicalIndex = logi->second.physicalIndex;
		indexUse = &(logi->second);
		// check size
		if (logi->second.currentSize < requestedSize)
		{
			// init buffer entry wasn't big enough; could be a mistake on the part
			// of the original use, or perhaps a variable length we can't predict
			// until first actual runtime use e.g. world matrix array
			int insertCount = requestedSize - logi->second.currentSize;
			FloatList::iterator insertPos = floatConstants.begin();
			std::advance(insertPos, physicalIndex);
			floatConstants.insert(insertPos, insertCount, 0.0f);
			// shift all physical positions after this one
			for (GpuLogicalIndexUseMap::iterator i = floatLogicalToPhysical->map.begin();
				i != floatLogicalToPhysical->map.end(); ++i)
			{
				if (i->second.physicalIndex > physicalIndex)
					i->second.physicalIndex += insertCount;
			}
			floatLogicalToPhysical->bufferSize += insertCount;
			for (AutoConstantList::iterator i = autoConstantSet.begin();
				i != autoConstantSet.end(); ++i)
			{
				if (i->physicalIndex > physicalIndex &&
					GetAutoConstantDefinition(i->paramType)->eleType == ET_REAL)
				{
					i->physicalIndex += insertCount;
				}
			}
			if (!namedConstantSet.IsNull())
			{
				for (GpuConstantDefinitionMap::iterator i = namedConstantSet->map.begin();
					i != namedConstantSet->map.end(); ++i)
				{
					if (i->second.isFloat() && i->second.physicalIndex > physicalIndex)
						i->second.physicalIndex += insertCount;
				}
				namedConstantSet->floatBufferSize += insertCount;
			}

			logi->second.currentSize += insertCount;
		}
	}

	if (indexUse)
		indexUse->variability = variablility;

	return indexUse;
}
int BWGpuProgramParameters::GetNumAutoConstantDefinition()
{
	return sizeof(autoConstantDictionary) / sizeof(AutoConstantDefinition);
}

int BWGpuProgramParameters::GetIntConstantPhysicalIndex(int logicalIndex, int requestedSize, unsigned short variablility)
{
	GpuLogicalIndexUse* indexUse = GetIntConstantLogicalIndexUse(logicalIndex, requestedSize, variablility);
	return indexUse ? indexUse->physicalIndex : 0;
}
bool BWGpuProgramParameters::IsUsingSharedParameters(const std::string &name)
{
	BWGpuSharedParameterUsageList::iterator itor = gpuSharedParametersSet.begin();
	while (itor != gpuSharedParametersSet.end())
	{
		if (itor->GetName() == name)
		{
			return true;
		}
		itor++;
	}
	return false;
}
int BWGpuProgramParameters::GetFloatConstantPhysicalIndex(int logicalIndex, int requestedSize, unsigned short variablility)
{
	GpuLogicalIndexUse * tmp = GetFloatConstantLogicalIndexUse(logicalIndex, requestedSize, variablility);
	return tmp->physicalIndex;
}
void BWGpuProgramParameters::AddSharedParameters(const std::string& shaderdParam)
{
	AddSharedParameters(BWGpuProgramManager::GetInstance()->GetSharedParameters(shaderdParam));
}
void BWGpuProgramParameters::AddSharedParameters(BWGpuSharedParametersPtr sharedParam)
{
	if (!IsUsingSharedParameters(sharedParam->GetName()))
	{
		gpuSharedParametersSet.push_back(BWGpuSharedParametersUsage(sharedParam , this));
	}
}
void BWGpuProgramParameters::SetNamedConstant(const std::string& name, const int *data, int count, int multiple)
{
	int rawCount = count * multiple;
	const GpuConstantDefinition* def = FindNamedConstantDefinition(name, !ignoreMissingParams);
	if (def)
	{
		WriteRawConstants(def->physicalIndex, data, rawCount);
	}
}
void BWGpuProgramParameters::SetConstant(int logicalindex, const int* val, int count)
{
	count *= 4;
	assert(!intLogicalToPhysical.IsNull());
	int  physicalIndex = GetIntConstantPhysicalIndex(logicalindex, count, GPV_GLOBAL);
	WriteRawConstants(physicalIndex, val, count);
}

void BWGpuProgramParameters::SetConstant(int logical,const float *val, int count)
{
	int size = count * 4;
	assert(!floatLogicalToPhysical.IsNull());
	int physicalIndex = GetFloatConstantPhysicalIndex(logical, size, GPV_GLOBAL);
	WriteRawConstants(physicalIndex, val, size);
}
void BWGpuProgramParameters::WriteRawConstants(const int physicalIndex, const int *value, int count)
{
	//assert(0);// 下面的赋值方式不对
	assert(physicalIndex + count <= intConstants.size());
	memcpy(&intConstants[physicalIndex], value, sizeof(int)* count);
}
void BWGpuProgramParameters::WriteRawConstants(const int physicalIndex, const float *value, int count)
{
	//assert(0);// 下面的赋值方式不对  floatConstants 是一个Vector的容器 这样的使用memcpy是不合理的 但是使用下面的数组循环赋值又不对
	assert(physicalIndex + count <= floatConstants.size());
	memcpy(&floatConstants[physicalIndex], value, sizeof(float)* count);
	/*for (int i = physicalIndex , j = 0; i < count; i++ , j++)
	{
		floatConstants[i] = value[j];
	}*/
}

void BWGpuProgramParameters::SetNamedConstant(const std::string &name, const float *value, int size, int multiple)
{
	int rawCount = size * multiple;
	const GpuConstantDefinition* def = FindNamedConstantDefinition(name, !ignoreMissingParams);
	if (def)
	{
		WriteRawConstants(def->physicalIndex, value, rawCount);
	}
}
void BWGpuProgramParameters::SetNamedConstants(const GpuNamedConstantsPtr &constantPtr)
{
	namedConstantSet = constantPtr;
	if (constantPtr->floatBufferSize > floatConstants.size())
	{
		floatConstants.insert(floatConstants.end(), constantPtr->floatBufferSize - floatConstants.size(), 0.0f);
	}
	if (constantPtr->intBufferSize > intConstants.size())
	{
		intConstants.insert(intConstants.end(), constantPtr->intBufferSize - intConstants.size(), 0);
	}
}
void BWGpuProgramParameters::SetNamedConstant(const std::string &name, BWMatrix4& matrix)
{
	const GpuConstantDefinition *def = FindNamedConstantDefinition(name, !ignoreMissingParams);
 	WriteRawConstants(def->physicalIndex, matrix.M, 16);
}
void BWGpuProgramParameters::SetConstant(int index, BWMatrix4& matrix)
{
	BWGpuProgramParameters::SetConstant(index, matrix.M, 4);
}
const GpuConstantDefinition* BWGpuProgramParameters::FindNamedConstantDefinition(const std::string & name, bool throwExceptionIfNotFound)
{
	if (namedConstantSet.IsNull())
	{
		//抛出异常
		return NULL;
	}
	GpuConstantDefinitionMap::iterator itor =  namedConstantSet->map.find(name);
	if (itor == namedConstantSet->map.end())
	{
		//抛出异常
		return NULL;
	}
	return &(itor->second);
}
const GpuNamedConstantsPtr BWGpuProgramParameters::getNamedConstants() const 
{
	return namedConstantSet;
}
unsigned short BWGpuProgramParameters::deriveVariability(AutoConstantType act)
{
	switch (act)
	{
	case ACT_VIEW_MATRIX:
	case ACT_INVERSE_VIEW_MATRIX:
	case ACT_TRANSPOSE_VIEW_MATRIX:
	case ACT_INVERSE_TRANSPOSE_VIEW_MATRIX:
	case ACT_PROJECTION_MATRIX:
	case ACT_INVERSE_PROJECTION_MATRIX:
	case ACT_TRANSPOSE_PROJECTION_MATRIX:
	case ACT_INVERSE_TRANSPOSE_PROJECTION_MATRIX:
	case ACT_VIEWPROJ_MATRIX:
	case ACT_INVERSE_VIEWPROJ_MATRIX:
	case ACT_TRANSPOSE_VIEWPROJ_MATRIX:
	case ACT_INVERSE_TRANSPOSE_VIEWPROJ_MATRIX:
	case ACT_RENDER_TARGET_FLIPPING:
	case ACT_VERTEX_WINDING:
	case ACT_AMBIENT_LIGHT_COLOUR:
	case ACT_DERIVED_AMBIENT_LIGHT_COLOUR:
	case ACT_DERIVED_SCENE_COLOUR:
	case ACT_FOG_COLOUR:
	case ACT_FOG_PARAMS:
	case ACT_SURFACE_AMBIENT_COLOUR:
	case ACT_SURFACE_DIFFUSE_COLOUR:
	case ACT_SURFACE_SPECULAR_COLOUR:
	case ACT_SURFACE_EMISSIVE_COLOUR:
	case ACT_SURFACE_SHININESS:
	case ACT_CAMERA_POSITION:
	case ACT_TIME:
	case ACT_TIME_0_X:
	case ACT_COSTIME_0_X:
	case ACT_SINTIME_0_X:
	case ACT_TANTIME_0_X:
	case ACT_TIME_0_X_PACKED:
	case ACT_TIME_0_1:
	case ACT_COSTIME_0_1:
	case ACT_SINTIME_0_1:
	case ACT_TANTIME_0_1:
	case ACT_TIME_0_1_PACKED:
	case ACT_TIME_0_2PI:
	case ACT_COSTIME_0_2PI:
	case ACT_SINTIME_0_2PI:
	case ACT_TANTIME_0_2PI:
	case ACT_TIME_0_2PI_PACKED:
	case ACT_FRAME_TIME:
	case ACT_FPS:
	case ACT_VIEWPORT_WIDTH:
	case ACT_VIEWPORT_HEIGHT:
	case ACT_VIEWPORT_WIDTH_AND_HEIGHT:
	case ACT_INVERSE_VIEWPORT_WIDTH:
	case ACT_INVERSE_VIEWPORT_HEIGHT:
	case ACT_VIEWPORT_SIZE:
	case ACT_TEXEL_OFFSETS:
	case ACT_TEXTURE_SIZE:
	case ACT_INVERSE_TEXTURE_SIZE:
	case ACT_PACKED_TEXTURE_SIZE:
	case ACT_SCENE_DEPTH_RANGE:
	case ACT_VIEW_DIRECTION:
	case ACT_VIEW_SIDE_VECTOR:
	case ACT_VIEW_UP_VECTOR:
	case ACT_FOV:
	case ACT_NEAR_CLIP_DISTANCE:
	case ACT_FAR_CLIP_DISTANCE:
	case ACT_FAR_AND_NEAR_CLIP_DISTANCE:
	case ACT_WIDTH_INVERSE_HIGHT_CLIP:
	case ACT_PASS_NUMBER:
	case ACT_TEXTURE_MATRIX:
	case ACT_LOD_CAMERA_POSITION:

		return (unsigned short)GPV_GLOBAL;

	case ACT_WORLD_MATRIX:
	case ACT_PRE_WORLD_MATRIX:
	case ACT_INVERSE_WORLD_MATRIX:
	case ACT_TRANSPOSE_WORLD_MATRIX:
	case ACT_INVERSE_TRANSPOSE_WORLD_MATRIX:
	case ACT_WORLD_MATRIX_ARRAY_3x4:
	case ACT_WORLD_MATRIX_ARRAY:
	case ACT_WORLD_DUALQUATERNION_ARRAY_2x4:
	case ACT_WORLD_SCALE_SHEAR_MATRIX_ARRAY_3x4:
	case ACT_WORLDVIEW_MATRIX:
	case ACT_INVERSE_WORLDVIEW_MATRIX:
	case ACT_TRANSPOSE_WORLDVIEW_MATRIX:
	case ACT_INVERSE_TRANSPOSE_WORLDVIEW_MATRIX:
	case ACT_WORLDVIEWPROJ_MATRIX:
	case ACT_INVERSE_WORLDVIEWPROJ_MATRIX:
	case ACT_TRANSPOSE_WORLDVIEWPROJ_MATRIX:
	case ACT_INVERSE_TRANSPOSE_WORLDVIEWPROJ_MATRIX:
	case ACT_CAMERA_POSITION_OBJECT_SPACE:
	case ACT_LOD_CAMERA_POSITION_OBJECT_SPACE:
	case ACT_CUSTOM:
	case ACT_ANIMATION_PARAMETRIC:

		return (unsigned short)GPV_PER_OBJECT;

	case ACT_LIGHT_POSITION_OBJECT_SPACE:
	case ACT_LIGHT_DIRECTION_OBJECT_SPACE:
	case ACT_LIGHT_DISTANCE_OBJECT_SPACE:
	case ACT_LIGHT_POSITION_OBJECT_SPACE_ARRAY:
	case ACT_LIGHT_DIRECTION_OBJECT_SPACE_ARRAY:
	case ACT_LIGHT_DISTANCE_OBJECT_SPACE_ARRAY:
	case ACT_TEXTURE_WORLDVIEWPROJ_MATRIX:
	case ACT_TEXTURE_WORLDVIEWPROJ_MATRIX_ARRAY:
	case ACT_SPOTLIGHT_WORLDVIEWPROJ_MATRIX:

		// These depend on BOTH lights and objects
		return ((unsigned short)GPV_PER_OBJECT) | ((unsigned short)GPV_LIGHTS);

	case ACT_LIGHT_COUNT:
	case ACT_LIGHT_DIFFUSE_COLOUR:
	case ACT_LIGHT_SPECULAR_COLOUR:
	case ACT_LIGHT_POSITION:
	case ACT_LIGHT_DIRECTION:
	case ACT_LIGHT_POSITION_VIEW_SPACE:
	case ACT_LIGHT_DIRECTION_VIEW_SPACE:
	case ACT_SHADOW_EXTRUSION_DISTANCE:
	case ACT_SHADOW_SCENE_DEPTH_RANGE:
	case ACT_SHADOW_COLOUR:
	case ACT_LIGHT_POWER_SCALE:
	case ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED:
	case ACT_LIGHT_SPECULAR_COLOUR_POWER_SCALED:
	case ACT_LIGHT_NUMBER:
	case ACT_LIGHT_CASTS_SHADOWS:
	case ACT_LIGHT_ATTENUATION:
	case ACT_SPOTLIGHT_PARAMS:
	case ACT_LIGHT_DIFFUSE_COLOUR_ARRAY:
	case ACT_LIGHT_SPECULAR_COLOUR_ARRAY:
	case ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED_ARRAY:
	case ACT_LIGHT_SPECULAR_COLOUR_POWER_SCALED_ARRAY:
	case ACT_LIGHT_POSITION_ARRAY:
	case ACT_LIGHT_DIRECTION_ARRAY:
	case ACT_LIGHT_POSITION_VIEW_SPACE_ARRAY:
	case ACT_LIGHT_DIRECTION_VIEW_SPACE_ARRAY:
	case ACT_LIGHT_POWER_SCALE_ARRAY:
	case ACT_LIGHT_ATTENUATION_ARRAY:
	case ACT_SPOTLIGHT_PARAMS_ARRAY:
	case ACT_TEXTURE_VIEWPROJ_MATRIX:
	case ACT_TEXTURE_VIEWPROJ_MATRIX_ARRAY:
	case ACT_SPOTLIGHT_VIEWPROJ_MATRIX:
	case ACT_SPOTLIGHT_VIEWPROJ_MATRIX_ARRAY:
	case ACT_LIGHT_CUSTOM:

		return (unsigned short)GPV_LIGHTS;

	case ACT_DERIVED_LIGHT_DIFFUSE_COLOUR:
	case ACT_DERIVED_LIGHT_SPECULAR_COLOUR:
	case ACT_DERIVED_LIGHT_DIFFUSE_COLOUR_ARRAY:
	case ACT_DERIVED_LIGHT_SPECULAR_COLOUR_ARRAY:

		return ((unsigned short)GPV_GLOBAL | (unsigned short)GPV_LIGHTS);

	case ACT_PASS_ITERATION_NUMBER:

		return (unsigned short)GPV_PASS_ITERATION_NUMBER;

	default:
		return (unsigned short)GPV_GLOBAL;
	};

}
void BWGpuProgramParameters::SetNamedAutoConstant(const std::string &name, AutoConstantType type , unsigned int extraInfor)
{
	const GpuConstantDefinition* tmp = FindNamedConstantDefinition(name, !ignoreMissingParams);
	if (tmp)
	{
		tmp->variability = deriveVariability(type);
		GpuLogicalIndexUse* indexUse = GetFloatConstantLogicalIndexUse(tmp->logicalIndex, tmp->elementSize * tmp->arraySize, tmp->variability);
		if (indexUse)
		{
			indexUse->variability - tmp->variability;
		}
		SetRawAutoConstant(tmp->physicalIndex, type, extraInfor, tmp->variability, tmp->elementSize);
	}
}
void BWGpuProgramParameters::SetAutoConstant(int logicalIndex, AutoConstantType type, unsigned int extraInfor )
{
	const AutoConstantDefinition* autoDef = GetAutoConstantDefinition(type);
	int size = autoDef->elementCount;
	if (size % 4 > 0)
	{
		size += 4 - (size & 4);
	}
	GpuLogicalIndexUse* indexUse = GetFloatConstantLogicalIndexUse(logicalIndex, size, deriveVariability(type));
	SetRawAutoConstant(indexUse->physicalIndex, type, extraInfor, indexUse->variability, size);
}
void BWGpuProgramParameters::SetNamedAutoConstantReal(const std::string& name, AutoConstantType type, float data)
{
	const GpuConstantDefinition* def = FindNamedConstantDefinition(name, !ignoreMissingParams);
	if (def)
	{
		def->variability = deriveVariability(type);
		GpuLogicalIndexUse *indexUse = GetFloatConstantLogicalIndexUse(def->logicalIndex, def->arraySize * def->elementSize, def->variability);
		if (indexUse)
		{
			indexUse->variability = def->variability;
		}
		SetRawAutoConstant(def->physicalIndex, type, data, def->variability, def->elementSize);
	}
}
void BWGpuProgramParameters::SetAutoConstantReal(int logicalIndex, AutoConstantType type, float data)
{
	const AutoConstantDefinition* autoDef = GetAutoConstantDefinition(type);
	int size = autoDef->elementCount;
	if (size % 4 > 0)
	{
		size += 4 - (size & 4);
	}
	GpuLogicalIndexUse* indexUse = GetFloatConstantLogicalIndexUse(logicalIndex, size, deriveVariability(type));
	SetRawAutoConstant(indexUse->physicalIndex, type, data, indexUse->variability, size);
}
void BWGpuProgramParameters::SetRawAutoConstant(int physicalIndex, AutoConstantType type, int extraInfor, unsigned short ability, int elementsize)
{
	bool found = false;
	AutoConstantList::iterator itor = autoConstantSet.begin();
	while (itor != autoConstantSet.end())
	{
		if (itor->physicalIndex == physicalIndex)
		{
			itor->paramType = type;
			itor->data = extraInfor;
			itor->elementCount = ability;
			found = true;
			break;
		}
		itor++;
	}
	if (!found)
	{
		autoConstantSet.push_back(AutoConstantEntry(type, physicalIndex, extraInfor, ability, elementsize));
	}
	combinVariability |= ability;
}
void BWGpuProgramParameters::ClearNamedAutoConstant(const std::string& name)
{
	const GpuConstantDefinition* def = FindNamedConstantDefinition(name , !ignoreMissingParams);
	if (def)
	{
		def->variability = GPV_GLOBAL;
		if (def->isFloat())
		{
			for (AutoConstantList::iterator itor = autoConstantSet.begin(); itor != autoConstantSet.end() ; itor++)
			{
				if (itor->physicalIndex == def->physicalIndex)
				{
					autoConstantSet.erase(itor);
					break;
				}
			}
		}
	}
}

//void BWGpuProgramParameters::AddAutoUniformBufferObject(const AutoUniformBufferObject* InUniformBufferObject)
//{
//	for each (const AutoUniformBufferObject *AutoUniformBufferObject in AutoUniformBufferObjectParam)
//	{
//		if (AutoUniformBufferObject == InUniformBufferObject) return;
//	}
//	AutoUniformBufferObjectParam.push_back(InUniformBufferObject);
//}

void BWGpuProgramParameters::SetLogicalIndexes(const GpuLogicalBufferStructPtr &intLogicalToPhysical, const GpuLogicalBufferStructPtr &floatLogicalToPhysical)
{
	//为什么要用这种方式来更改其中的内容呢  
	this->intLogicalToPhysical = intLogicalToPhysical;
	this->floatLogicalToPhysical = floatLogicalToPhysical;
	if (!intLogicalToPhysical.IsNull() && intLogicalToPhysical->bufferSize > intConstants.size())
	{
		intConstants.insert(intConstants.end(),
			intLogicalToPhysical->bufferSize - intConstants.size(), 0.0f);
	}
	if (!floatLogicalToPhysical.IsNull() && floatLogicalToPhysical->bufferSize > floatConstants.size())
	{
		floatConstants.insert(floatConstants.end(),
			floatLogicalToPhysical->bufferSize - floatConstants.size(), 0);
	}
}

void BWGpuProgramParameters::CopyConstantsFrom( const BWGpuProgramParameters& params)
{
	floatConstants = params.GetFlotConstantList();
	intConstants = params.GetIntConstantList();
	autoConstantSet = params.GetAutoConstantList();
	combinVariability = params.combinVariability;
	CopySharedParamSetUsage(params.gpuSharedParametersSet);
}
void BWGpuProgramParameters::CopySharedParamSetUsage(const BWGpuSharedParameterUsageList& shared)
{
	gpuSharedParametersSet.clear();
	BWGpuSharedParameterUsageList::const_iterator itor = shared.begin();
	while (itor != shared.end())
	{
		gpuSharedParametersSet.push_back(BWGpuSharedParametersUsage(itor->GetSharedParam(), this));
		itor++;
	}
}
void BWGpuProgramParameters::ClearAutoConstant(int index)
{
	GpuLogicalIndexUse* indexUse = GetFloatConstantLogicalIndexUse(index, 0, GPV_GLOBAL);
	if (indexUse)
	{
		indexUse->variability = GPV_GLOBAL;
		int physicalIndex = indexUse->physicalIndex;
		for (AutoConstantList::iterator itor = autoConstantSet.begin(); itor != autoConstantSet.end() ; itor++)
		{
			if (itor->physicalIndex == physicalIndex)
			{
				autoConstantSet.erase(itor);
				break;
			}
		}
	}
}
const BWGpuProgramParameters::AutoConstantDefinition* BWGpuProgramParameters::GetAutoConstantDefinition(const std::string& name)
{
	int num = GetNumAutoConstantDefinition();
	for (int i = 0; i < num; i++)
	{
		if (autoConstantDictionary[i].name == name)
		{
			return &autoConstantDictionary[i];
		}
	}
	return NULL;
}
const BWGpuProgramParameters::AutoConstantDefinition* BWGpuProgramParameters::GetAutoConstantDefinition(const int idx)
{
	if (idx < GetNumAutoConstantDefinition())
	{
		assert(idx == static_cast<int>(autoConstantDictionary[idx].acType));
		return &autoConstantDictionary[idx];
	}
	return NULL;
}

const BWGpuProgramParameters::AutoConstantDefinition* BWGpuProgramParameters::GetAutoConstantDefinition(AutoConstantType InACT)
{
	for each (AutoConstantDefinition &ConstantDefinition in autoConstantDictionary)
	{
		if (ConstantDefinition.acType == InACT)  return &ConstantDefinition;
	}
	return nullptr;
}

//const BWGpuProgramParameters::AutoUniformBufferObject* BWGpuProgramParameters::GetAutoUniformObject(const std::string &InName)
//{
//	for each (AutoUniformBufferObject &UniformObject in AutoUniformBufferObjects )
//	{
//		if (UniformObject.Name == InName) return &UniformObject;
//	}
//	return nullptr;
//}

void BWGpuProgramParameters::CopyMatchingNamedConstantForm(const BWGpuProgramParameters &source)
{

	if (!namedConstantSet.IsNull() && !source.namedConstantSet.IsNull())
	{
		std::map<int, std::string> srcToDestNamedMap;
		for (GpuConstantDefinitionMap::const_iterator i = source.namedConstantSet->map.begin();
			i != source.namedConstantSet->map.end(); ++i)
		{
			const std::string& paramName = i->first;
			const GpuConstantDefinition& olddef = i->second;
			const GpuConstantDefinition* newdef = FindNamedConstantDefinition(paramName, false);
			if (newdef)
			{
				// Copy data across, based on smallest common definition size
				int srcsz = olddef.elementSize * olddef.arraySize;
				int destsz = newdef->elementSize * newdef->arraySize;
				int sz = std::min(srcsz, destsz);
				if (newdef->isFloat())
				{

					memcpy(GetFloatPointer(newdef->physicalIndex),
						source.GetFloatPointer(olddef.physicalIndex),
						sz * sizeof(float));
				}
				else
				{
					memcpy(GetIntPointer(newdef->physicalIndex),
						source.GetIntPointer(olddef.physicalIndex),
						sz * sizeof(int));
				}
				// we'll use this map to resolve autos later
				// ignore the [0] aliases
				if (!StringUtil::EndsWith(paramName, "[0]"))
					srcToDestNamedMap[olddef.physicalIndex] = paramName;
			}
		}

		for (AutoConstantList::const_iterator i = source.autoConstantSet.begin();
			i != source.autoConstantSet.end(); ++i)
		{
			const BWGpuProgramParameters::AutoConstantEntry& autoEntry = *i;
			// find dest physical index
			std::map<int, std::string>::iterator mi = srcToDestNamedMap.find(autoEntry.physicalIndex);
			if (mi != srcToDestNamedMap.end())
			{
				if (autoEntry.fData)
				{
					SetNamedAutoConstantReal(mi->second, autoEntry.paramType, autoEntry.fData);
				}
				else
				{
					SetNamedAutoConstantReal(mi->second, autoEntry.paramType, autoEntry.data);
				}
			}

		}

		// Copy shared param sets
		for (BWGpuSharedParameterUsageList::const_iterator i = source.gpuSharedParametersSet.begin();
			i != source.gpuSharedParametersSet.end(); ++i)
		{
			const BWGpuSharedParametersUsage& usage = *i;
			if (!IsUsingSharedParameters(usage.GetName()))
			{
				AddSharedParameters(usage.GetSharedParam());
			}
		}
	}
}


