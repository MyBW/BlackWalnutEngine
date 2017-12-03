#include "BWParser.h"
#include "BWStringConverter.h"
#include "BWStringOperation.h"
#include "BWPrimitive.h"
#include "BWMaterialManager.h"
#include "BWHighLevelGpuProgram.h"
#include "BWHighLevelGpuProgramManager.h"
#include "BWTextureUnitState.h"
#include "BWLog.h"
typedef std::map<std::string, unsigned int> ParserMap;
typedef std::vector<std::string> StringVec;

ParserMap mIds;

typedef std::vector<std::string> StringVec;
bool getBoolean(const std::string &node, bool *result)
{
	std::size_t pos = node.find("true");
	if (pos == std::string::npos)
	{
		pos = node.find("false");
		if (pos == std::string::npos)
		{

			std::size_t pos = node.find("off");
			if (pos == std::string::npos)
			{
				pos = node.find("on");
				if (pos == std::string::npos)
				{
					return false;
				}
				*result = true;
				return true;
			}
			*result = false;
			return true;
		}
		*result = false;
		return true;
	}
	*result = true;
	return true;
}
//-------------------------------------------------------------------------
bool getString(const std::string &node, std::string *result)
{
	*result = node;
	return true;
}
//-------------------------------------------------------------------------
bool getReal(const std::string &node, float *result)
{
	
#if OGRE_DOUBLE_PRECISION == 0
	int n = sscanf_s(node.c_str(), "%f", result);
#else
	int n = sscanf_s(node.c_str(), "%lf", result);
#endif

	if (n == 0 || n == EOF)
		return false; // Conversion failed
	return true;
}
//-------------------------------------------------------------------------
bool getFloat(const std::string &node, float *result)
{
	int n = sscanf_s(node.c_str(), "%f", result);
	if (n == 0 || n == EOF)
		return false; // Conversion failed

	return true;
}
//-------------------------------------------------------------------------
bool getInt(const std::string &node, int *result)
{
	
	int n = sscanf_s(node.c_str(), "%d", result);
	if (n == 0 || n == EOF)
		return false; // Conversion failed

	return true;
}
//-------------------------------------------------------------------------
bool getUInt(const std::string &node, unsigned int *result)
{
	
	int n = sscanf_s(node.c_str(), "%u", result);
	if (n == 0 || n == EOF)
		return false; // Conversion failed

	return true;
}
//-------------------------------------------------------------------------
bool getColour(StringVec::iterator i, StringVec::iterator end , ColourValue *result, int maxEntries)
{
	/*
	StringVec tmpString;
	std::size_t pos1 = 0 , pos2;
	while (1)
	{
		pos1 = color.find_first_not_of( " " , pos1);
		if (pos1 == std::string::npos)
		{
			break;
		}
		pos2 = color.find_first_of(" " , pos1);
		std::string substr = color.substr(pos1, pos2 - pos1);
		pos1 = pos2;
		tmpString.push_back(substr);
	}
	StringVec::iterator i = tmpString.begin();
	StringVec::iterator end = tmpString.end();*/

	int n = 0;
	while (i != end && n < maxEntries)
	{
		float v = 0;
		if (getFloat(*i, &v))
		{
			switch (n)
			{
			case 0:
				result->r = v;
				break;
			case 1:
				result->g = v;
				break;
			case 2:
				result->b = v;
				break;
			case 3:
				result->a = v;
				break;
			}
		}
		else
		{
			return false;
		}
		++n;
		++i;
	}
	// return error if we found less than rgb before end, unless constrained
	return (n >= 3 || n == maxEntries);
}
//-------------------------------------------------------------------------
bool getSceneBlendFactor(const std::string &node, SceneBlendFactor *sbf)
{
	
	switch (mIds[node])
	{
	case ID_ONE:
		*sbf = SBF_ONE;
		break;
	case ID_ZERO:
		*sbf = SBF_ZERO;
		break;
	case ID_DEST_COLOUR:
		*sbf = SBF_DEST_COLOUR;
		break;
	case ID_DEST_ALPHA:
		*sbf = SBF_DEST_ALPHA;
		break;
	case ID_SRC_ALPHA:
		*sbf = SBF_SOURCE_ALPHA;
		break;
	case ID_SRC_COLOUR:
		*sbf = SBF_SOURCE_COLOUR;
		break;
	case ID_ONE_MINUS_DEST_COLOUR:
		*sbf = SBF_ONE_MINUS_DEST_COLOUR;
		break;
	case ID_ONE_MINUS_SRC_COLOUR:
		*sbf = SBF_ONE_MINUS_SOURCE_COLOUR;
		break;
	case ID_ONE_MINUS_DEST_ALPHA:
		*sbf = SBF_ONE_MINUS_DEST_ALPHA;
		break;
	case ID_ONE_MINUS_SRC_ALPHA:
		*sbf = SBF_ONE_MINUS_SOURCE_ALPHA;
		break;
	default:
		return false;
	}
	return true;
}
//-------------------------------------------------------------------------
bool getCompareFunction(const std::string &node, CompareFunction *func)
{
	switch (mIds[node])
	{
	case ID_ALWAYS_FAIL:
		*func = CMPF_ALWAYS_FAIL;
		break;
	case ID_ALWAYS_PASS:
		*func = CMPF_ALWAYS_PASS;
		break;
	case ID_LESS:
		*func = CMPF_LESS;
		break;
	case ID_LESS_EQUAL:
		*func = CMPF_LESS_EQUAL;
		break;
	case ID_EQUAL:
		*func = CMPF_EQUAL;
		break;
	case ID_NOT_EQUAL:
		*func = CMPF_NOT_EQUAL;
		break;
	case ID_GREATER_EQUAL:
		*func = CMPF_GREATER_EQUAL;
		break;
	case ID_GREATER:
		*func = CMPF_GREATER;
		break;
	default:
		return false;
	}
	return true;
}
//-------------------------------------------------------------------------
bool getMatrix4(StringVec::iterator i, StringVec::iterator end, BWMatrix4 *m)
{
	/*typedef std::vector<std::string> StringVec;
	StringVec tmpString;
	std::size_t pos1 = 0, pos2;
	while (1)
	{
		pos1 = node.find_first_not_of(" ", pos1);
		if (pos1 == std::string::npos)
		{
			break;
		}
		pos2 = node.find_first_of(" ", pos1);
		std::string substr = node.substr(pos1, pos2 - pos1);
		pos1 = pos2;
		tmpString.push_back(substr);
	}
	int n = 0;
	StringVec::iterator i = tmpString.begin();
	StringVec::iterator end = tmpString.end();*/
	int n = 0;
	while (i != end && n < 16)
	{
		if (i != end)
		{
			float r = 0;
			if (getReal(*i, &r))
				(*m)[n / 4][n % 4] = r;
			else
				return false;
		}
		else
		{
			return false;
		}
		++i;
		++n;
	}
	return true;
}
//-------------------------------------------------------------------------
bool getInts(StringVec::iterator i, StringVec::iterator end , int *vals, int count)
{
	/*typedef std::vector<std::string> StringVec;
	StringVec tmpString;
	std::size_t pos1 = 0, pos2;
	while (1)
	{
	pos1 = node.find_first_not_of(" ", pos1);
	if (pos1 == std::string::npos)
	{
	break;
	}
	pos2 = node.find_first_of(" ", pos1);
	std::string substr = node.substr(pos1, pos2 - pos1);
	pos1 = pos2;
	tmpString.push_back(substr);
	}
	StringVec::iterator i = tmpString.begin();
	StringVec::iterator end = tmpString.end();*/
	bool success = true;
	int n = 0;
	while (n < count)
	{
		if (i != end)
		{
			int v = 0;
			if (getInt(*i, &v))
				vals[n] = v;
			else
				break;
			++i;
		}
		else
			vals[n] = 0;
		++n;
	}

	if (n < count)
		success = false;

	return success;
}
//----------------------------------------------------------------------------
bool getFloats(StringVec::iterator i, StringVec::iterator end ,float *vals, int count)
{
	/*typedef std::vector<std::string> StringVec;
	StringVec tmpString;
	std::size_t pos1 = 0, pos2;
	while (1)
	{
		pos1 = node.find_first_not_of(" ", pos1);
		if (pos1 == std::string::npos)
		{
			break;
		}
		pos2 = node.find_first_of(" ", pos1);
		std::string substr = node.substr(pos1, pos2 - pos1);
		pos1 = pos2;
		tmpString.push_back(substr);
	}
	int n = 0;
	StringVec::iterator i = tmpString.begin();
	StringVec::iterator i = tmpString.end();*/
	bool success = true;
	int n = 0;
	while (n < count)
	{
		if (i != end)
		{
			float v = 0;
			if (getFloat(*i, &v))
				vals[n] = v;
			else
				break;
			++i;
		}
		else
			vals[n] = 0;
		++n;
	}

	if (n < count)
		success = false;

	return success;
}
//-------------------------------------------------------------------------
bool getStencilOp(const   std::string &node, StencilOperation *op)
{
	switch (mIds[node])
	{
	case ID_KEEP:
		*op = SOP_KEEP;
		break;
	case ID_ZERO:
		*op = SOP_ZERO;
		break;
	case ID_REPLACE:
		*op = SOP_REPLACE;
		break;
	case ID_INCREMENT:
		*op = SOP_INCREMENT;
		break;
	case ID_DECREMENT:
		*op = SOP_DECREMENT;
		break;
	case ID_INCREMENT_WRAP:
		*op = SOP_INCREMENT_WRAP;
		break;
	case ID_DECREMENT_WRAP:
		*op = SOP_DECREMENT_WRAP;
		break;
	case ID_INVERT:
		*op = SOP_INVERT;
		break;
	default:
		return false;
	}
	return true;
}
std::size_t find_first_not_empty(const std::string & str)
{
	std::size_t pos1 = str.find_first_not_of(" ");
	std::size_t pos2 = str.find_first_not_of("\t");
	return pos2 > pos1 ? pos2 : pos1;
}
void DeleteChar(std::string& str, char ch)
{
	std::string final;
	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == ch)
		{
			continue; 
		}
		final += str[i];
	}
	str = final;
}
//---------------------------------------------------------------------
bool getConstantType(const std::string &val, GpuConstantType *op , int &num)
{
	if (val.find("float") != std::string::npos)
	{
		int count = 1;
		if (val.size() == 6)
			count = StringConverter::ParseInt(val.substr(5));
		else if (val.size() > 6)
			return false;

		if (count > 4 || count == 0)
			return false;

		*op = (GpuConstantType)(GCT_FLOAT1 + count - 1);
		num = count;
	}
	else if (val.find("int") != std::string::npos)
	{
		int count = 1;
		if (val.size() == 4)
			count = StringConverter::ParseInt(val.substr(3));
		else if (val.size() > 4)
			return false;

		if (count > 4 || count == 0)
			return false;

		*op = (GpuConstantType)(GCT_INT1 + count - 1);
		num = count;
	}
	else if (val.find("matrix") != std::string::npos)
	{
		int count1, count2;

		if (val.size() == 9)
		{
			count1 = StringConverter::ParseInt(val.substr(6, 1));
			count2 = StringConverter::ParseInt(val.substr(8, 1));
		}
		else
			return false;

		if (count1 > 4 || count1 < 2 || count2 > 4 || count2 < 2)
			return false;
		num = count2 * count1;
		switch (count1)
		{
		case 2:
			*op = (GpuConstantType)(GCT_MATRIX_2X2 + count2 - 2);
			break;
		case 3:
			*op = (GpuConstantType)(GCT_MATRIX_3X2 + count2 - 2);
			break;
		case 4:
			*op = (GpuConstantType)(GCT_MATRIX_4X2 + count2 - 2);
			break;
		}

	}

	return true;
}
void Parser::InitPaser()
{
	mIds["on"] = ID_ON;
	mIds["off"] = ID_OFF;
	mIds["true"] = ID_TRUE;
	mIds["false"] = ID_FALSE;
	mIds["yes"] = ID_YES;
	mIds["no"] = ID_NO;

	// Material ids
	mIds["material"] = ID_MATERIAL;
	mIds["vertex_program"] = ID_VERTEX_PROGRAM;
	mIds["geometry_program"] = ID_GEOMETRY_PROGRAM;
	mIds["fragment_program"] = ID_FRAGMENT_PROGRAM;
	mIds["technique"] = ID_TECHNIQUE;
	mIds["pass"] = ID_PASS;
	mIds["texture_unit"] = ID_TEXTURE_UNIT;
	mIds["vertex_program_ref"] = ID_VERTEX_PROGRAM_REF;
	mIds["geometry_program_ref"] = ID_GEOMETRY_PROGRAM_REF;
	mIds["fragment_program_ref"] = ID_FRAGMENT_PROGRAM_REF;
	mIds["shadow_caster_vertex_program_ref"] = ID_SHADOW_CASTER_VERTEX_PROGRAM_REF;
	mIds["shadow_caster_fragment_program_ref"] = ID_SHADOW_CASTER_FRAGMENT_PROGRAM_REF;
	mIds["shadow_receiver_vertex_program_ref"] = ID_SHADOW_RECEIVER_VERTEX_PROGRAM_REF;
	mIds["shadow_receiver_fragment_program_ref"] = ID_SHADOW_RECEIVER_FRAGMENT_PROGRAM_REF;
	mIds["gpu_program_ref"] = ID_GPU_PROGRAM_REF;
	mIds["vertex_shader_ref"] = ID_VERTEX_SHADER_REF; 
	mIds["fragment_shader_ref"] = ID_FRAGMENT_SHADER_REF;
	mIds["geometry_shader_ref"] = ID_GEOMETRY_SHADER_REF;
	mIds["compute_shader_ref"] = ID_COMPUTE_SHADER_REF;
	mIds["gpu_program"] = ID_GPU_PROGRAM;
	mIds["lod_values"] = ID_LOD_VALUES;
	mIds["lod_strategy"] = ID_LOD_STRATEGY;
	mIds["lod_distances"] = ID_LOD_DISTANCES;
	mIds["receive_shadows"] = ID_RECEIVE_SHADOWS;
	mIds["transparency_casts_shadows"] = ID_TRANSPARENCY_CASTS_SHADOWS;
	mIds["set_texture_alias"] = ID_SET_TEXTURE_ALIAS;

	mIds["source"] = ID_SOURCE;
	mIds["syntax"] = ID_SYNTAX;
	mIds["default_params"] = ID_DEFAULT_PARAMS;
	mIds["param_indexed"] = ID_PARAM_INDEXED;
	mIds["param_named"] = ID_PARAM_NAMED;
	mIds["param_named_uniform_block"]= ID_PARAM_NAMED_UNIFORM_BLOCK,
	mIds["param_indexed_auto"] = ID_PARAM_INDEXED_AUTO;
	mIds["param_named_auto"] = ID_PARAM_NAMED_AUTO;
	mIds["param_named_uniform_block_auto"] = ID_PARAM_NAMED_UNIFORM_BLOCK_AUTO;

	mIds["scheme"] = ID_SCHEME;
	mIds["lod_index"] = ID_LOD_INDEX;
	mIds["shadow_caster_material"] = ID_SHADOW_CASTER_MATERIAL;
	mIds["shadow_receiver_material"] = ID_SHADOW_RECEIVER_MATERIAL;
	mIds["gpu_vendor_rule"] = ID_GPU_VENDOR_RULE;
	mIds["gpu_device_rule"] = ID_GPU_DEVICE_RULE;
	mIds["include"] = ID_INCLUDE;
	mIds["exclude"] = ID_EXCLUDE;

	mIds["ambient"] = ID_AMBIENT;
	mIds["diffuse"] = ID_DIFFUSE;
	mIds["specular"] = ID_SPECULAR;
	mIds["emissive"] = ID_EMISSIVE;
	mIds["vertexcolour"] = ID_VERTEXCOLOUR;
	mIds["scene_blend"] = ID_SCENE_BLEND;
	mIds["colour_blend"] = ID_COLOUR_BLEND;
	mIds["one"] = ID_ONE;
	mIds["zero"] = ID_ZERO;
	mIds["dest_colour"] = ID_DEST_COLOUR;
	mIds["src_colour"] = ID_SRC_COLOUR;
	mIds["one_minus_src_colour"] = ID_ONE_MINUS_SRC_COLOUR;
	mIds["one_minus_dest_colour"] = ID_ONE_MINUS_DEST_COLOUR;
	mIds["dest_alpha"] = ID_DEST_ALPHA;
	mIds["src_alpha"] = ID_SRC_ALPHA;
	mIds["one_minus_dest_alpha"] = ID_ONE_MINUS_DEST_ALPHA;
	mIds["one_minus_src_alpha"] = ID_ONE_MINUS_SRC_ALPHA;
	mIds["separate_scene_blend"] = ID_SEPARATE_SCENE_BLEND;
	mIds["scene_blend_op"] = ID_SCENE_BLEND_OP;
	mIds["reverse_subtract"] = ID_REVERSE_SUBTRACT;
	mIds["min"] = ID_MIN;
	mIds["max"] = ID_MAX;
	mIds["separate_scene_blend_op"] = ID_SEPARATE_SCENE_BLEND_OP;
	mIds["depth_check"] = ID_DEPTH_CHECK;
	mIds["depth_write"] = ID_DEPTH_WRITE;
	mIds["depth_func"] = ID_DEPTH_FUNC;
	mIds["depth_bias"] = ID_DEPTH_BIAS;
	mIds["iteration_depth_bias"] = ID_ITERATION_DEPTH_BIAS;
	mIds["always_fail"] = ID_ALWAYS_FAIL;
	mIds["always_pass"] = ID_ALWAYS_PASS;
	mIds["less_equal"] = ID_LESS_EQUAL;
	mIds["less"] = ID_LESS;
	mIds["equal"] = ID_EQUAL;
	mIds["not_equal"] = ID_NOT_EQUAL;
	mIds["greater_equal"] = ID_GREATER_EQUAL;
	mIds["greater"] = ID_GREATER;
	mIds["alpha_rejection"] = ID_ALPHA_REJECTION;
	mIds["alpha_to_coverage"] = ID_ALPHA_TO_COVERAGE;
	mIds["light_scissor"] = ID_LIGHT_SCISSOR;
	mIds["light_clip_planes"] = ID_LIGHT_CLIP_PLANES;
	mIds["transparent_sorting"] = ID_TRANSPARENT_SORTING;
	mIds["illumination_stage"] = ID_ILLUMINATION_STAGE;
	mIds["decal"] = ID_DECAL;
	mIds["cull_hardware"] = ID_CULL_HARDWARE;
	mIds["clockwise"] = ID_CLOCKWISE;
	mIds["anticlockwise"] = ID_ANTICLOCKWISE;
	mIds["cull_software"] = ID_CULL_SOFTWARE;
	mIds["back"] = ID_BACK;
	mIds["front"] = ID_FRONT;
	mIds["normalise_normals"] = ID_NORMALISE_NORMALS;
	mIds["lighting"] = ID_LIGHTING;
	mIds["shading"] = ID_SHADING;
	mIds["flat"] = ID_FLAT;
	mIds["gouraud"] = ID_GOURAUD;
	mIds["phong"] = ID_PHONG;
	mIds["polygon_mode"] = ID_POLYGON_MODE;
	mIds["solid"] = ID_SOLID;
	mIds["wireframe"] = ID_WIREFRAME;
	mIds["points"] = ID_POINTS;
	mIds["polygon_mode_overrideable"] = ID_POLYGON_MODE_OVERRIDEABLE;
	mIds["fog_override"] = ID_FOG_OVERRIDE;
	mIds["none"] = ID_NONE;
	mIds["linear"] = ID_LINEAR;
	mIds["exp"] = ID_EXP;
	mIds["exp2"] = ID_EXP2;
	mIds["colour_write"] = ID_COLOUR_WRITE;
	mIds["max_lights"] = ID_MAX_LIGHTS;
	mIds["start_light"] = ID_START_LIGHT;
	mIds["iteration"] = ID_ITERATION;
	mIds["once"] = ID_ONCE;
	mIds["once_per_light"] = ID_ONCE_PER_LIGHT;
	mIds["per_n_lights"] = ID_PER_N_LIGHTS;
	mIds["per_light"] = ID_PER_LIGHT;
	mIds["point"] = ID_POINT;
	mIds["spot"] = ID_SPOT;
	mIds["directional"] = ID_DIRECTIONAL;
	mIds["light_mask"] = ID_LIGHT_MASK;
	mIds["point_size"] = ID_POINT_SIZE;
	mIds["point_sprites"] = ID_POINT_SPRITES;
	mIds["point_size_min"] = ID_POINT_SIZE_MIN;
	mIds["point_size_max"] = ID_POINT_SIZE_MAX;
	mIds["point_size_attenuation"] = ID_POINT_SIZE_ATTENUATION;

	mIds["texture_alias"] = ID_TEXTURE_ALIAS;
	mIds["texture"] = ID_TEXTURE;
	mIds["1d"] = ID_1D;
	mIds["2d"] = ID_2D;
	mIds["3d"] = ID_3D;
	mIds["cubic"] = ID_CUBIC;
	mIds["unlimited"] = ID_UNLIMITED;
	mIds["alpha"] = ID_ALPHA;
	mIds["gamma"] = ID_GAMMA;
	mIds["anim_texture"] = ID_ANIM_TEXTURE;
	mIds["cubic_texture"] = ID_CUBIC_TEXTURE;
	mIds["separateUV"] = ID_SEPARATE_UV;
	mIds["combinedUVW"] = ID_COMBINED_UVW;
	mIds["tex_coord_set"] = ID_TEX_COORD_SET;
	mIds["tex_address_mode"] = ID_TEX_ADDRESS_MODE;
	mIds["wrap"] = ID_WRAP;
	mIds["clamp"] = ID_CLAMP;
	mIds["mirror"] = ID_MIRROR;
	mIds["border"] = ID_BORDER;
	mIds["tex_border_colour"] = ID_TEX_BORDER_COLOUR;
	mIds["filtering"] = ID_FILTERING;
	mIds["bilinear"] = ID_BILINEAR;
	mIds["trilinear"] = ID_TRILINEAR;
	mIds["anisotropic"] = ID_ANISOTROPIC;
	mIds["max_anisotropy"] = ID_MAX_ANISOTROPY;
	mIds["mipmap_bias"] = ID_MIPMAP_BIAS;
	mIds["colour_op"] = ID_COLOUR_OP;
	mIds["replace"] = ID_REPLACE;
	mIds["add"] = ID_ADD;
	mIds["modulate"] = ID_MODULATE;
	mIds["alpha_blend"] = ID_ALPHA_BLEND;
	mIds["colour_op_ex"] = ID_COLOUR_OP_EX;
	mIds["source1"] = ID_SOURCE1;
	mIds["source2"] = ID_SOURCE2;
	mIds["modulate"] = ID_MODULATE;
	mIds["modulate_x2"] = ID_MODULATE_X2;
	mIds["modulate_x4"] = ID_MODULATE_X4;
	mIds["add"] = ID_ADD;
	mIds["add_signed"] = ID_ADD_SIGNED;
	mIds["add_smooth"] = ID_ADD_SMOOTH;
	mIds["subtract"] = ID_SUBTRACT;
	mIds["blend_diffuse_alpha"] = ID_BLEND_DIFFUSE_ALPHA;
	mIds["blend_texture_alpha"] = ID_BLEND_TEXTURE_ALPHA;
	mIds["blend_current_alpha"] = ID_BLEND_CURRENT_ALPHA;
	mIds["blend_manual"] = ID_BLEND_MANUAL;
	mIds["dotproduct"] = ID_DOT_PRODUCT;
	mIds["blend_diffuse_colour"] = ID_BLEND_DIFFUSE_COLOUR;
	mIds["src_current"] = ID_SRC_CURRENT;
	mIds["src_texture"] = ID_SRC_TEXTURE;
	mIds["src_diffuse"] = ID_SRC_DIFFUSE;
	mIds["src_specular"] = ID_SRC_SPECULAR;
	mIds["src_manual"] = ID_SRC_MANUAL;
	mIds["colour_op_multipass_fallback"] = ID_COLOUR_OP_MULTIPASS_FALLBACK;
	mIds["alpha_op_ex"] = ID_ALPHA_OP_EX;
	mIds["env_map"] = ID_ENV_MAP;
	mIds["spherical"] = ID_SPHERICAL;
	mIds["planar"] = ID_PLANAR;
	mIds["cubic_reflection"] = ID_CUBIC_REFLECTION;
	mIds["cubic_normal"] = ID_CUBIC_NORMAL;
	mIds["scroll"] = ID_SCROLL;
	mIds["scroll_anim"] = ID_SCROLL_ANIM;
	mIds["rotate"] = ID_ROTATE;
	mIds["rotate_anim"] = ID_ROTATE_ANIM;
	mIds["scale"] = ID_SCALE;
	mIds["wave_xform"] = ID_WAVE_XFORM;
	mIds["scroll_x"] = ID_SCROLL_X;
	mIds["scroll_y"] = ID_SCROLL_Y;
	mIds["scale_x"] = ID_SCALE_X;
	mIds["scale_y"] = ID_SCALE_Y;
	mIds["sine"] = ID_SINE;
	mIds["triangle"] = ID_TRIANGLE;
	mIds["sawtooth"] = ID_SAWTOOTH;
	mIds["square"] = ID_SQUARE;
	mIds["inverse_sawtooth"] = ID_INVERSE_SAWTOOTH;
	mIds["transform"] = ID_TRANSFORM;
	mIds["binding_type"] = ID_BINDING_TYPE;
	mIds["vertex"] = ID_VERTEX;
	mIds["fragment"] = ID_FRAGMENT;
	mIds["content_type"] = ID_CONTENT_TYPE;
	mIds["named"] = ID_NAMED;
	mIds["shadow"] = ID_SHADOW;
	mIds["texture_source"] = ID_TEXTURE_SOURCE;
	mIds["shared_params"] = ID_SHARED_PARAMS;
	mIds["shared_param_named"] = ID_SHARED_PARAM_NAMED;
	mIds["shared_params_ref"] = ID_SHARED_PARAMS_REF;

	// Particle system
	mIds["particle_system"] = ID_PARTICLE_SYSTEM;
	mIds["emitter"] = ID_EMITTER;
	mIds["affector"] = ID_AFFECTOR;

	// Compositor
	mIds["compositor"] = ID_COMPOSITOR;
	mIds["tarGet"] = ID_TARGET;
	mIds["tarGet_output"] = ID_TARGET_OUTPUT;

	mIds["input"] = ID_INPUT;
	mIds["none"] = ID_NONE;
	mIds["previous"] = ID_PREVIOUS;
	mIds["tarGet_width"] = ID_TARGET_WIDTH;
	mIds["tarGet_height"] = ID_TARGET_HEIGHT;
	mIds["tarGet_width_scaled"] = ID_TARGET_WIDTH_SCALED;
	mIds["tarGet_height_scaled"] = ID_TARGET_HEIGHT_SCALED;
	mIds["pooled"] = ID_POOLED;
	//mIds["gamma"] = ID_GAMMA; - already registered
	mIds["no_fsaa"] = ID_NO_FSAA;
	mIds["depth_pool"] = ID_DEPTH_POOL;

	mIds["texture_ref"] = ID_TEXTURE_REF;
	mIds["local_scope"] = ID_SCOPE_LOCAL;
	mIds["chain_scope"] = ID_SCOPE_CHAIN;
	mIds["global_scope"] = ID_SCOPE_GLOBAL;
	mIds["compositor_logic"] = ID_COMPOSITOR_LOGIC;

	mIds["only_initial"] = ID_ONLY_INITIAL;
	mIds["visibility_mask"] = ID_VISIBILITY_MASK;
	mIds["lod_bias"] = ID_LOD_BIAS;
	mIds["material_scheme"] = ID_MATERIAL_SCHEME;
	mIds["shadows"] = ID_SHADOWS_ENABLED;

	mIds["clear"] = ID_CLEAR;
	mIds["stencil"] = ID_STENCIL;
	mIds["render_scene"] = ID_RENDER_SCENE;
	mIds["render_quad"] = ID_RENDER_QUAD;
	mIds["identifier"] = ID_IDENTIFIER;
	mIds["first_render_queue"] = ID_FIRST_RENDER_QUEUE;
	mIds["last_render_queue"] = ID_LAST_RENDER_QUEUE;
	mIds["quad_normals"] = ID_QUAD_NORMALS;
	mIds["camera_far_corners_view_space"] = ID_CAMERA_FAR_CORNERS_VIEW_SPACE;
	mIds["camera_far_corners_world_space"] = ID_CAMERA_FAR_CORNERS_WORLD_SPACE;

	mIds["buffers"] = ID_BUFFERS;
	mIds["colour"] = ID_COLOUR;
	mIds["depth"] = ID_DEPTH;
	mIds["colour_value"] = ID_COLOUR_VALUE;
	mIds["depth_value"] = ID_DEPTH_VALUE;
	mIds["stencil_value"] = ID_STENCIL_VALUE;

	mIds["check"] = ID_CHECK;
	mIds["comp_func"] = ID_COMP_FUNC;
	mIds["ref_value"] = ID_REF_VALUE;
	mIds["mask"] = ID_MASK;
	mIds["fail_op"] = ID_FAIL_OP;
	mIds["keep"] = ID_KEEP;
	mIds["increment"] = ID_INCREMENT;
	mIds["decrement"] = ID_DECREMENT;
	mIds["increment_wrap"] = ID_INCREMENT_WRAP;
	mIds["decrement_wrap"] = ID_DECREMENT_WRAP;
	mIds["invert"] = ID_INVERT;
	mIds["depth_fail_op"] = ID_DEPTH_FAIL_OP;
	mIds["pass_op"] = ID_PASS_OP;
	mIds["two_sided"] = ID_TWO_SIDED;
#ifdef RTSHADER_SYSTEM_BUILD_CORE_SHADERS
	mIds["rtshader_system"] = ID_RT_SHADER_SYSTEM;
#endif
}

BWMaterial* Parser::ParseMaterial(BWDataStream &dataStream, const std::string &groupName)
{
	

	std::string line;
	std::string keyWord;
	std::size_t pos;
	BWMaterial* material = NULL;
	while (!dataStream.Eof())
	{
		line.clear();
		line = dataStream.GetLine();
		if (line.find("vertex_program") != std::string::npos || line.find("fragment_program") != std::string::npos)
		{
			std::string name;
			std::string language;
			std::string type;
			StringVec split = StringUtil::Split(line);
			if (split.size() > 3 || split.size() == 0)
			{
				//wrong
				return material;
			}
			if (split[0] != "vertex_program"  && split[0] != "fragment_program")
			{
				//wrong
				return material;
			}
			type = split[0];
			name = split[1];
			language = split[2];
			if (!ParseProgram(dataStream, name, language, mIds[type], groupName))
			{
				//wrong 
				return material;
			}
			continue;
		}
		keyWord = "material";
		pos = line.find(keyWord);
		if (pos!= std::string::npos)
		{
			std::string name = line.substr(pos + keyWord.length(), line.length() - pos - keyWord.length());
			name = name.substr(name.find_first_not_of(" "), name.find_first_of(" ", name.find_first_not_of(" ")));
			material  = dynamic_cast<BWMaterial*>( BWMaterialManager::GetInstance()->Create(name , groupName).Get());
			Log::GetInstance()->logMessage(std::string("Load material :" + name));
			
			while (!dataStream.Eof())
			{
				line.clear();
				line = dataStream.GetLine();
				DeleteChar(line, '\t');
				StringVec parem = StringUtil::Split(line);
				if (parem.size() == 0 || parem[0] == "{")
				{
					continue;
				}
				if (parem[0] == "}")
				{
					break;
				}
				if (parem[0] == "technique")//pos != std::string::npos)
				{
					BWTechnique* technique = material->CreateTechnique();
					if (parem.size() == 2)
					{
						technique->SetName(parem[1]);
					}
					if (!ParseTechnique(dataStream, technique))
					{
						// technique  有问题
						return NULL;
					}
				}
			}
		}
	}
	return material;
}
bool Parser::ParseProgram(BWDataStream &dataStream, const std::string &name, const std::string &language, unsigned int shaderType, const std::string &groupName)
{
	GpuProgramType  type;
	if (shaderType == ID_VERTEX_PROGRAM)
	{
		type = GPT_VERTEX_PROGRAM;
	}
	if (shaderType == ID_FRAGMENT_PROGRAM)
	{
		type = GPT_FRAGMENT_PROGRAM;
	}
	BWHighLevelGpuProgramPtr highLevelGpu = BWHighLevelGpuProgramManager::GetInstance()->CreateProgram(name, groupName, language, type);
	int brackets = 0;
	std::string line;
	while (1)
	{
		line.clear();
		line = dataStream.GetLine();
		StringVec tmp = StringUtil::Split(line);
		if (tmp.size() == 0)
		{
			continue;
		}
		if (tmp.size() > 2)
		{
			//wrong 
			return false;
		}
		if (tmp[0] != "{")
		{
			//wrong
			return false;
		}
		++brackets;
		break;
	}
	size_t pos1, pos2;
	std::string keyword;
	while (brackets != 0)
	{
		line.clear();
		line = dataStream.GetLine();
		DeleteChar(line, '\t');
		StringVec value = StringUtil::Split(line);
		if (value.size() == 0)
		{
			continue;
		}
		if (value.size() == 1)
		{
			if (value[0] == "{")
			{
				++brackets;
				continue;
			}
			if (value[0] == "}")
			{
				--brackets;
				continue;
			}
		}
		switch (mIds[value[0]])
		{
		case ID_SOURCE:
		{
			 highLevelGpu->SetSourceFile(value[1]);
             break;
		}
		case ID_DEFAULT_PARAMS:
		{
			  BWGpuProgramParametersPtr parameter = highLevelGpu->GetDefaultParameters();
			 if (!ParseParameter(dataStream, parameter))
			 {
			  assert(0);
			 }
			 break;
		}
		default:
			assert(0);
			break;
		}
	}
	return true;
}
bool Parser::ParseTechnique(BWDataStream &dataStream, BWTechnique* technique)
{
	std::string line;
	while (!dataStream.Eof())
	{
		line.clear();
		line = dataStream.GetLine();
		DeleteChar(line, '\t');
		StringVec parem = StringUtil::Split(line);
		if (parem.size() == 0 || parem[0] == "{")
		{
			continue;
		}
		if (parem[0] == "}")
		{
			return true;
		}
		if (parem[0] == "pass")
		{
			BWPass* pass = technique->CreatePass();
			if (parem.size() == 2)
			{
				pass->SetName(parem[1]);
			}
			if (!ParsePass(dataStream , pass))
			{
				assert(0);
				return false;
			}
			continue;
		}
	}
	assert(0); // 括号不匹配
	return false;
}
bool Parser::parseGPUProgram(BWDataStream &dataStream, const std::string goupName)
{
	std::string line;
	while (!dataStream.Eof())
	{
		line = dataStream.GetLine();
		StringUtil::DeleteChar(line, '\t');
		int i = 0;
		for ( ; i < line.length() && line[i] != ' '; i++);
		if (i == (line.length() - 1) || line.length() == 0)
		{
			continue;
		}
		StringVector vec = StringUtil::Split(line);
		if (vec.size() != 3 && mIds[vec[0]] != ID_GPU_PROGRAM)
		{
			assert(0);
		}
		BWHighLevelGpuProgramPtr gpuProgram = BWHighLevelGpuProgramManager::GetInstance()->CreateProgram(vec[1], goupName, vec[2], GPT_GPU_PROGRAM);
		Log::GetInstance()->logMessage(std::string("Load HightLevelGpu " + vec[1]));
		parseGPUProgram(dataStream, gpuProgram);
	}
	return true;
}
bool Parser::parseGPUProgram(BWDataStream &dataStream, BWHighLevelGpuProgramPtr gpuProgram)
{
	std::string line;
	BWShaderPtr shader;
	while (!dataStream.Eof())
	{
		line.clear();
		line = dataStream.GetLine();
		DeleteChar(line, '\t');
		StringVec parem = StringUtil::Split(line);
		if (parem.size() == 0 || parem[0] == "{")
		{
			continue;
		}
		if (parem[0] == "}")
		{
			return true;
		}
		if (parem.size() != 2)
		{
			assert(0);
		}
		shader = BWShaderManager::GetInstance()->CreateOrRetrieve(parem[1], gpuProgram->GetGroupName()).first;
		gpuProgram->addShader(shader);
		switch (mIds[parem[0]])
		{
		case ID_VERTEX_SHADER_REF:
			shader->setShaderType(ST_VERTEX_SHADER);
			break;
		case ID_FRAGMENT_SHADER_REF:
			shader->setShaderType(ST_FRAGMENT_SHADER);
			break;
		case ID_GEOMETRY_SHADER_REF:
			shader->setShaderType(ST_GEOMETRY_SHADER);
			break;
		case ID_COMPUTE_SHADER_REF:
			shader->setShaderType(ST_COMPUTE_SHADER);
			break;
		default:
			break;
		}
	}
	assert(0); // 括号不匹配
	return false;
}
bool Parser::ParseParameter(BWDataStream &dataStream, BWGpuProgramParametersPtr parameter)
{
	unsigned int animParametricsCount = 0;
	std::string line;
	while (1)
	{
		line = dataStream.GetLine();
		DeleteChar(line, '\t');
		StringVec parem = StringUtil::Split(line);
		if (parem.size() == 0)
		{
			continue; 
		}
		if (parem[0] == "}")
		{
			return true;
		}
		switch (mIds[parem[0]])
		{
		case ID_PARAM_NAMED:
		{
				GpuConstantType gpuConstantType;
				int num;
				if (!getConstantType(parem[2], &gpuConstantType, num))
				{
				  assert(0);
				}
				if ((parem.size() - 3) != num)
				{
				  assert(0);
				}
				StringVec::iterator begin, end;
				begin = parem.begin(); begin++; begin++; begin++;
				end = parem.end();
				if (parem[2].find("int") != std::string::npos)
				{
				  int *value = new int[num];
				  getInts(begin, end, value, num);
				  parameter->SetNamedConstant(parem[1], value, num, 1);
				  delete[] value;
				}
				else
				{
				  float *value = new float[num];
				  getFloats(begin, end, value, num);
				  parameter->SetNamedConstant(parem[1], value, num, 1);
				  delete[] value;
				}
				break;
		}
		case ID_PARAM_NAMED_AUTO:
		{           
						     if (parem.size() >= 3)
						     {
						     	size_t index = 0;
						     	// Look up the auto constant
						     	StringUtil::ToLowerCase(parem[2]);
						     	const BWGpuProgramParameters::AutoConstantDefinition *def =
						     		BWGpuProgramParameters::GetAutoConstantDefinition(parem[2]);
						     	if (def)
						     	{
						     		switch (def->dataType)
						     		{
									case BWGpuProgramParameters::ACDT_NONE:
									{
										// Set the auto constant
										parameter->SetNamedAutoConstant(parem[1], def->acType);
										break;
									}
						     		case BWGpuProgramParameters::ACDT_INT:
						     			if (def->acType == BWGpuProgramParameters::ACT_ANIMATION_PARAMETRIC)
						     			{
						     					parameter->SetNamedAutoConstant(parem[1], def->acType, animParametricsCount++);
						     			}
						     			else
						     			{
						     				// Only certain texture projection auto params will assume 0
						     				// Otherwise we will expect that 3rd parameter
						     				if (parem.size() == 3)
						     				{
						     					if (def->acType == BWGpuProgramParameters::ACT_TEXTURE_VIEWPROJ_MATRIX ||
						     						def->acType == BWGpuProgramParameters::ACT_TEXTURE_WORLDVIEWPROJ_MATRIX ||
						     						def->acType == BWGpuProgramParameters::ACT_SPOTLIGHT_VIEWPROJ_MATRIX ||
						     						def->acType == BWGpuProgramParameters::ACT_SPOTLIGHT_WORLDVIEWPROJ_MATRIX
						     						)
						     					{
						     								parameter->SetNamedAutoConstant(parem[1], def->acType, 0);
						     					}
						     					else
						     					{
													assert(0);
												}
						     				}
						     				else
						     				{
						     					bool success = false;
						     					unsigned int extraInfo = 0;
						     					if (parem.size() == 4)
						     					{ // Handle only one extra value
						     						if (getUInt(parem[3], &extraInfo))
						     						{
						     							success = true;
						     						}
						     					}
						     					else
						     					{ // Handle two extra values
						     						unsigned int extraInfo1 = 0, extraInfo2 = 0;
						     						if (getUInt(parem[3], &extraInfo1) && getUInt(parem[4], &extraInfo2))
						     						{
						     							extraInfo = extraInfo1 | (extraInfo2 << 16);
						     							success = true;
						     						}
						     					}
						     
						     					if (success)
						     					{
													parameter->SetNamedAutoConstant(parem[1], def->acType, extraInfo);
						     					}
						     					else
						     					{
													assert(0);
						     					/*	compiler->addError(ScriptCompiler::CE_INVALIDPARAMETERS, prop->file, prop->line,
						     							"invalid auto constant extra info parameter");
						     				*/	}
						     				}
						     			}
						     			break;
						     		case BWGpuProgramParameters::ACDT_REAL:
						     			if (def->acType == BWGpuProgramParameters::ACT_TIME ||
						     				def->acType == BWGpuProgramParameters::ACT_FRAME_TIME)
						     			{
						     				float f = 1.0f;
						     				if (parem.size() >= 4)
						     					getReal(parem[3], &f);
						     
											parameter->SetNamedAutoConstantReal(parem[1], def->acType, f);
						     			}
						     			else
						     			{
											if (parem.size() >= 4)
						     				{
						     					float extraInfo = 0.0f;
						     					if (getReal(parem[3], &extraInfo))
						     					{
													parameter->SetNamedAutoConstantReal(parem[1], def->acType, extraInfo);
						     					}
						     					else
						     					{
													assert(0);
						     						/*compiler->addError(ScriptCompiler::CE_INVALIDPARAMETERS, prop->file, prop->line,
						     							"incorrect float argument definition in extra parameters");*/
						     					}
						     				}
						     				else
						     				{
												assert(0);
						     					/*compiler->addError(ScriptCompiler::CE_NUMBEREXPECTED, prop->file, prop->line,
						     						"extra parameters required by constant definition " + atom1->value);*/
						     				}
						     			}
						     			break;
						     		}
						     	}
						     	else
						     	{
									assert(0);
						     	}
						     }
						 	break;
		}
		case ID_PARAM_NAMED_UNIFORM_BLOCK_AUTO:
		{
			if (BWGpuProgramParameters::GlobalViewportInformation.Name == parem[2])
			{
				parameter->SetGlobalViewportInformation(parem[1]);
			}
			break;
		}
		}
	}
}
bool Parser::ParsePass(BWDataStream &dataStream, BWPass *pass)
{
	std::string line;
	std::string keyword;
	std::string value;
	StringVec valueVec;
    while (!dataStream.Eof())
    {
		line.clear();
		valueVec.clear();
		value.clear();
		keyword.clear();
		line = dataStream.GetLine(); 
		if (line.find("{") != std::string::npos)
		{
			continue;
		}
		if (line.find("}") != std::string::npos)
		{
			//本pass结束
			return true;
		}
		std::size_t pos1, pos2;
		pos1 = find_first_not_empty(line);// line.find_first_not_of(" ");
		pos2 = line.find_first_of(" ", pos1);
		if (pos1 == pos2 || pos1 == std::string::npos)
		{
			continue;
		}
		keyword = line.substr(pos1, pos2 - pos1);
	    if (pos2 == std::string::npos)
	    {

	    }
		if (pos2 != std::string::npos)
		{
			value = line.substr(pos2, line.length() - pos2);
		}
		StringUtil::DeleteChar(value, '\t');
		valueVec = StringUtil::Split(value);
		switch (mIds[keyword])
		{
			 case ID_TEXTURE_UNIT:
			 {
									 BWTextureUnitState * textureUnitState = pass->CreateTextureUnitState();
									 if (!ParseTextureUnitState(dataStream , textureUnitState))
									 {
										 assert(0);
									 }
			 	 break;
			 }
			 case ID_VERTEX_PROGRAM_REF:
			 {
				if (valueVec.size() > 1)
				{
					assert(0);
				}
				pass->SetVertexProgram(valueVec[0]);
				BWGpuProgramParametersPtr ptr = pass->GetVertexProgramParameter();
				if (!ParseParameter(dataStream, ptr))
				{
					assert(0);
				}
			 	break;
			 }
			 case ID_GEOMETRY_PROGRAM_REF:
			 {
			 	break;
			 }
			 case ID_FRAGMENT_PROGRAM_REF:
			 {
			    if (valueVec.size() > 1)
			    {
					assert(0);
			    }
				pass->SetFragmentProgram(valueVec[0]);
				BWGpuProgramParametersPtr ptr = pass->GetFragmetProgramParameter();
				if (!ParseParameter(dataStream, ptr))
				{
					assert(0);
				}
			 	break;
			 }
			 case ID_GPU_PROGRAM_REF:
			 {
				if (valueVec.size() != 2)
				{
					assert(0);
				}
				pass->setGPUProgram(valueVec[0], valueVec[1]);
				if (!ParseParameter(dataStream , pass->getGPUProgramParameter()))
				{
					assert(0);
				}
				 // 这里匹配GPU程序的参数
			 }
			 case ID_SHADOW_CASTER_VERTEX_PROGRAM_REF:
			 {
			 											break;
			 }
			 case ID_SHADOW_CASTER_FRAGMENT_PROGRAM_REF:
			 {
			 											  break;
			 }
			 case ID_SHADOW_RECEIVER_VERTEX_PROGRAM_REF:
			 {
			 											  break;
			 }
			 case ID_SHADOW_RECEIVER_FRAGMENT_PROGRAM_REF:
			 {
			 												break;
			 }
			 case  ID_SHADOW_CASTER_MATERIAL:
			 {
			 								   break;
			 }
			 case  ID_SHADOW_RECEIVER_MATERIAL:
			 {
			 	   break;
			 }
			 case  ID_LOD_VALUES:
			 {
			 					   break;
			 }
			 case  ID_LOD_STRATEGY:
			 {
			 						 break;
			 }
			 case  ID_LOD_DISTANCES:
			 {
			 						  break;
			 }
			 case  ID_RECEIVE_SHADOWS:
			 {
			 							break;
			 }
			 case  ID_TRANSPARENCY_CASTS_SHADOWS:
			 {
			 									   break;
			 }
			 case  ID_SET_TEXTURE_ALIAS:
			 {
			 							  break;
			 }
			 
			 case  ID_SCHEME:
			 {
			 				   break;
			 }
			 case  ID_LOD_INDEX:
			 {
			 					  break;
			 }
			 case  ID_GPU_VENDOR_RULE:
			 {
			 							break;
			 }
			 case  ID_GPU_DEVICE_RULE:
			 {
			 							break;
			 }
			 case  ID_INCLUDE:
			 {
			 					break;
			 }
			 case  ID_EXCLUDE:
			 {
			 					break;
			 }
		      
			 case  ID_AMBIENT:
			 {
                if (value.find("vertexcolour") != std::string::npos)
                {
					pass->SetVertexColorTracking(pass->GetVertexColourTracking() | TVC_AMBIENT);
					break;
                }
				ColourValue val = ColourValue::White;
				if (getColour(valueVec.begin(), valueVec.end(),&val, 4))
				{
					pass->SetAmbient(val);
				}
				else
				{
					// 有问题
					break;
				}
			 	break;
			 }
			 case  ID_DIFFUSE:
			 {
				if (value.find("vertexcolour") != std::string::npos)
				{
					pass->SetVertexColorTracking(pass->GetVertexColourTracking() | TrackVertexColourEnum::TVC_DIFFUESE);
				}
				else
				{
				 ColourValue val = ColourValue::White;
				 if (getColour(valueVec.begin(), valueVec.end(), &val, 4))
					 pass->SetDiffuse(val);
				 else
					 break;
				}
				break;
			 }
			 case  ID_SPECULAR:
			 {
				if (value.find("vertexcolour") != std::string::npos)
				{
				 pass->SetVertexColorTracking(pass->GetVertexColourTracking() | TVC_SPECULAR);
				 if (valueVec.size() == 2)
				 {
					 float val;
					 if (getReal(valueVec.back() , &val))
					 {
						 pass->SetShininess(val);
					 }
					 else
					 {
						 // 有问题
						 break;
					 }
				 }
				 else
				 {
					 // 有问题
					  break;
				 }
				 break;
				}
				else
				{
					ColourValue val = ColourValue::White;
					float  shiness;
					if (getColour(valueVec.begin(), valueVec.end(), &val, 4) && getReal(valueVec.back(), &shiness))
					{
						pass->SetSpecular(val);
						pass->SetShininess(shiness);
					}
					else
					{
						// 有问题
						break;
					}
					break;
				}
				
			 }
			 case  ID_EMISSIVE:
			 {
					 if (value.find("vertexcolour") != std::string::npos)
					 {
					  pass->SetVertexColorTracking(pass->GetVertexColourTracking() | TrackVertexColourEnum::TVC_EMISSIVE);
					 }
					 else
					 {
					  ColourValue val = ColourValue::White;
					  if (getColour(valueVec.begin(), valueVec.end(), &val, 4))
						  pass->SetSelfIllumination(val);
					  else
						  break;
					 }
					 break;
			 }
			 case  ID_SCENE_BLEND:
			 {
					if (valueVec.empty())
					{
					 break;
					}
					else if (valueVec.size() > 2)
					{
					break;
					}
					else if (valueVec.size() == 1)
					{
						 switch (mIds[valueVec.front()])
						 {
						 case ID_ADD:
							 pass->SetSceneBlendType(SBT_ADD);
							 break;
						 case ID_MODULATE:
							 pass->SetSceneBlendType(SBT_MODULATE);
							 break;
						 case ID_COLOUR_BLEND:
							 pass->SetSceneBlendType(SBT_TRANSPARENT_COLOUR);
							 break;
						 case ID_ALPHA_BLEND:
							 pass->SetSceneBlendType(SBT_TRANSPARENT_ALPHA);
							 break;
						 default:
							 // wrong
							 break;
						   }
					}
					else
					{
					 SceneBlendFactor sbf0, sbf1;
					 if (getSceneBlendFactor(valueVec[0], &sbf0) && getSceneBlendFactor(valueVec[1], &sbf1))
					 {
						 pass->SetSceneBlending(sbf0, sbf1);
					 }
					 else
					 {
						 //wrong
						 assert(0);
					 }
					}
					break;
			 }
			 case  ID_SEPARATE_SCENE_BLEND:
			 {
					if (valueVec.empty())
					{
					 //worng
					 break;
					}
					else if (valueVec.size() == 3)
					{
					 //wrong
						break;
					}
					else if (valueVec.size() > 4)
					{
						//wrong
						break;
					}
					else if (valueVec.size() == 2)
					{
						 SceneBlendType sbt0, sbt1;
						  switch (mIds[valueVec[0]])
						  {
						  case ID_ADD:
							  sbt0 = SBT_ADD;
							  break;
						  case ID_MODULATE:
							  sbt0 = SBT_MODULATE;
							  break;
						  case ID_COLOUR_BLEND:
							  sbt0 = SBT_TRANSPARENT_COLOUR;
							  break;
						  case ID_ALPHA_BLEND:
							  sbt0 = SBT_TRANSPARENT_ALPHA;
							  break;
						  default:
							  //wrong
							  break;
						  }

						  switch (mIds[valueVec[1]])
						  {
						  case ID_ADD:
							  sbt1 = SBT_ADD;
							  break;
						  case ID_MODULATE:
							  sbt1 = SBT_MODULATE;
							  break;
						  case ID_COLOUR_BLEND:
							  sbt1 = SBT_TRANSPARENT_COLOUR;
							  break;
						  case ID_ALPHA_BLEND:
							  sbt1 = SBT_TRANSPARENT_ALPHA;
							  break;
						  default:
							  //wrong
							  break;
						  }

						  pass->SetSeparateSceneBlending(sbt0, sbt1);
					}
					else
					{
						  SceneBlendFactor sbf0, sbf1, sbf2, sbf3;
						  if (getSceneBlendFactor(valueVec[0], &sbf0) && getSceneBlendFactor(valueVec[1], &sbf1) && getSceneBlendFactor(valueVec[2], &sbf2) &&
							  getSceneBlendFactor(valueVec[3], &sbf3))
						  {
							  pass->SetSeparateSceneBlending(sbf0, sbf1, sbf2, sbf3);
						  }
						  else
						  {
							  //wrong
						  }
					}
					break;
			 }
			 case  ID_SCENE_BLEND_OP:
			{
				if (valueVec.empty())
				{
					assert(0);
					//wrong
					break;
				}
				else if (valueVec.size() > 1)
				{
					assert(0);
					//wrong
					break;
				}
				else
				{
						switch (mIds[valueVec[0]])
						{
						case ID_ADD:
							pass->SetSceneBlendingOperation(SBO_ADD);
							break;
						case ID_SUBTRACT:
							pass->SetSceneBlendingOperation(SBO_SUBTRACT);
							break;
						case ID_REVERSE_SUBTRACT:
							pass->SetSceneBlendingOperation(SBO_REVERSE_SUBTRACT);
							break;
						case ID_MIN:
							pass->SetSceneBlendingOperation(SBO_MIN);
							break;
						case ID_MAX:
							pass->SetSceneBlendingOperation(SBO_MAX);
							break;
						default:
							assert(0);
						}
				}
				break;
			 }

			 case  ID_SEPARATE_SCENE_BLEND_OP:
			 {
			    if (valueVec.empty())
			    {
			     assert(0);
			     //wrong
			     break;
			    }
			    else if (valueVec.size() != 2)
			    {
			     assert(0);
			     //wrong
			     break;
			    }
			    else
			    {
			       SceneBlendOperation op = SBO_ADD, alphaOp = SBO_ADD;
			       switch (mIds[valueVec[0]])
			       {
			       case ID_ADD:
			      	 op = SBO_ADD;
			       	 break;
			       case ID_SUBTRACT:
			    	 op = SBO_SUBTRACT;
			    	 break;
			       case ID_REVERSE_SUBTRACT:
			    	 op = SBO_REVERSE_SUBTRACT;
			    	 break;
			       case ID_MIN:
			    	 op = SBO_MIN;
			    	 break;
			       case ID_MAX:
			    	 op = SBO_MAX;
			    	 break;
			       default:
			    	 assert(0);
			     }
			    
			     switch (mIds[valueVec[1]])
			     {
			     case ID_ADD:
			    	 alphaOp = SBO_ADD;
			    	 break;
			     case ID_SUBTRACT:
			    	 alphaOp = SBO_SUBTRACT;
			    	 break;
			     case ID_REVERSE_SUBTRACT:
			    	 alphaOp = SBO_REVERSE_SUBTRACT;
			    	 break;
			     case ID_MIN:
			    	 alphaOp = SBO_MIN;
			    	 break;
			     case ID_MAX:
			    	 alphaOp = SBO_MAX;
			    	 break;
			     default:
			    	 assert(0);
			     }
			     pass->SetSeparateSceneBlendingOperation(op, alphaOp);
			    }
				break;
			 }
			 case  ID_DEPTH_CHECK:
			 {
					 if (valueVec.empty())
					 {
					  assert(0);
					  //wrong
					  break;
					 }
					 else if (valueVec.size() > 1)
					 {
					  assert(0);
					  //wrong
					  break;
					 }
					 else
					 {
						 bool val = true;
						 if (getBoolean(valueVec[0], &val))
							 pass->SetDepthCheckEnabled(val);
						 else
							 assert(0);
					 }
					 break;
			 }
			 case  ID_DEPTH_WRITE:
			 {
					 if (valueVec.empty())
					 {
						 assert(0);
						 //wrong
						 break;
					 }
					 else if (valueVec.size() > 1)
					 {
						 assert(0);
						 //wrong
						 break;
					 }
					 else
					 {
						 bool val = true;
						 if (getBoolean(valueVec[0], &val))
							 pass->SetDepthWriteEnabled(val);
						 else
							 assert(0);
					 }
					 break;
			 }
			 case  ID_DEPTH_FUNC:
			 {
				  if (valueVec.empty())
				  {
				  	assert(0);
				  	break;
				  }
				  else if (valueVec.size() > 1)
				  {
				  	assert(0);
				  	break;
				  }
				  else
				  {
				  	CompareFunction func;
				  	if (getCompareFunction(valueVec.front(), &func))
				  		pass->SetDepthFunction(func);
				  	else
				  		assert(0);
				  	break;
				  }
				  break;
			 }
			 case  ID_DEPTH_BIAS:
			 {
					if (valueVec.empty())
					{
						assert(0);
						break;
					}
					else if (valueVec.size() > 2)
					{
						assert(0);
						break;
					}
					else
					{
						float val0, val1 = 0.0f;
						if (getFloat(valueVec[0], &val0))
						{
							if ( 2 == valueVec.size())
								getFloat(valueVec[1], &val1);
							pass->SetDepthBias(val0, val1);
						}
						else
						{
							assert(0);
						}
					}
					break;
			 }
			 case  ID_ITERATION_DEPTH_BIAS:
			 {
				 if (valueVec.empty())
				 {
				  assert(0);
				  break;
				 }
				 else if (valueVec.size() > 1)
				 {
				  assert(0);
				  break;
				 }
				 else
				 {
				  float val = 0.0f;
				  if (getFloat(valueVec.front(), &val))
					  pass->SetIterationDepthBias(val);
				  else
					  assert(0);
				 }
				 break;
			 }
			 case  ID_ALPHA_REJECTION:
			 {
				  if (valueVec.empty())
				  {
				   assert(0);
				   break;
				  }
				  else if (valueVec.size() > 2)
				  {
				   assert(0);
				   break;
				  }
				  else
				  {
				   CompareFunction func;
				   if (getCompareFunction(valueVec[0], &func))
				   {
				  	 if (valueVec.size() == 2)
				  	 {
				  		 unsigned int val = 0;
				  		 if (getUInt(valueVec[1], &val))
				  			 pass->SetAlphaRejectSettings(func, static_cast<unsigned char>(val));
				  		 else
				  			 assert(0);
				  		 break;
				  	 }
				  	 else
				  		 pass->SetAlphaRejectFunction(func);
				   }
				   else
				   {
				  	 assert(0);
				  	 break;
				   }
				  	
				  }
				  break;
			 }
			 case  ID_ALPHA_TO_COVERAGE:
			 {
				if (valueVec.empty())
				{
				  assert(0);
				  break;
				}
				else if (valueVec.size() > 1)
				{
				  assert(0);
				  break;
				}
				else
				{
				  bool val = true;
				  if (getBoolean(valueVec.front(), &val))
				   pass->SetAlphaToCoverageEnabled(val);
				  else
				   assert(0);
				  
				}
				break;
			 }
			 case  ID_LIGHT_SCISSOR:
			 {
				if (valueVec.empty())
				{
				  assert(0);
				  break;
				}
				else if (valueVec.size() > 1)
				{
				  assert(0);
				  break;
				}
				else
				{
				  bool val = false;
				  if (getBoolean(valueVec.front(), &val))
				   pass->SetLightScissoringEnabled(val);
				  else
				   assert(0);
				  
				}
				break;
			 }
			 case  ID_LIGHT_CLIP_PLANES:
			 {
				if (valueVec.empty())
				{
				  assert(0);
				  break;
				}
				else if (valueVec.size() > 1)
				{
				  assert(0);
				  break;
				}
				else
				{
				  bool val = false;
				  if (getBoolean(valueVec.front(), &val))
				   pass->SetLightClipPlanesEnabled(val);
				  else
				   assert(0);
				  
				}
				break;
			 }
			 case  ID_TRANSPARENT_SORTING:
			 {
				 if (valueVec.empty())
				 {
					 assert(0);
					 break;
				 }
				 else if (valueVec.size() > 1)
				 {
					 assert(0);
					 break;
				 }
				 else
				 {
					 bool val = true;
					 if (getBoolean(valueVec.front(), &val))
					 {
						 pass->SetTransparentSortingEnabled(val);
						 pass->SetTransparentSortingForced(false);
					 }
					 else
					 {
						 std::string val2;
						 if (getString(valueVec.front(), &val2) && val2 == "force")
						 {
							 pass->SetTransparentSortingEnabled(true);
							 pass->SetTransparentSortingForced(true);
						 }
						 else
						 {
							 assert(0);
							 break;
						 }
					 }
				 }
				 break;
			 }
			 case  ID_ILLUMINATION_STAGE:
			 {
				if (valueVec.empty())
				{
					assert(0);
					break;
				}
				else if (valueVec.size() > 1)
				{
					assert(0);
					break;
				}
				else
				{
						switch (mIds[valueVec[0]])
						{
						case ID_AMBIENT:
							pass->SetIlluminationStage(IS_AMBIENT);
							break;
						case ID_PER_LIGHT:
							pass->SetIlluminationStage(IS_PER_LIGHT);
							break;
						case ID_DECAL:
							pass->SetIlluminationStage(IS_DECAL);
							break;
						default:
							assert(0);

						}
				}
				break;
			 }
			 case ID_CULL_SOFTWARE:
			 {
				  if (valueVec.empty())
				  {
				   assert(0);
				   break;
				  }
				  else if (valueVec.size() > 1)
				  {
				   assert(0);
				   break;
				  }
				  else
				  {
				   switch (mIds[valueVec[0]])
				   {
				   case  ID_FRONT:
					 pass->SetManualCullingMode(MANUAL_CULL_FRONT);
				 	  break;
				   case  ID_BACK:
					 pass->SetManualCullingMode(MANUAL_CULL_BACK);
				 	  break;
				   case ID_NONE:
					 pass->SetManualCullingMode(MANUAL_CULL_NONE);
				 	  break;
				   default:
				 	  assert(0);
				 	  break;
				   }
				  }
				  break;
			 }
			 case  ID_CULL_HARDWARE:
			 {
				if (valueVec.empty())
				{
				  assert(0);
				  break;
				}
				else if (valueVec.size() > 1)
				{
				  assert(0);
				  break;
				}
				else
				{
				   switch (mIds[valueVec[0]])
				   {
				   case ID_CLOCKWISE:
					   pass->SetCullingMode(CULL_CLOCKWISE);
					   break;
				   case ID_ANTICLOCKWISE:
					   pass->SetCullingMode(CULL_ANTICLOCKWISE);
					   break;
				   case ID_NONE:
					   pass->SetCullingMode(CULL_NONE);
					   break;
				   default:
					   assert(0);
					   break;
				   }
				  }
				break;
			 }
			 case  ID_CLOCKWISE:
			 {
				if (valueVec.empty())
				{
				  assert(0);
				  break;
				}
				else if (valueVec.size() > 1)
				{
				  assert(0);
				  break;
				}
				else
				{
				   switch (mIds[valueVec[0]])
				   {
				   case ID_FRONT:
					   pass->SetManualCullingMode(MANUAL_CULL_FRONT);
					   break;
				   case ID_BACK:
					   pass->SetManualCullingMode(MANUAL_CULL_BACK);
					   break;
				   case ID_NONE:
					   pass->SetManualCullingMode(MANUAL_CULL_NONE);
					   break;
				   default:
					   assert(0);
					 
				   }
				}
				break;
			 }
			 case  ID_NORMALISE_NORMALS:
			 {
				if (valueVec.empty())
				{
				  assert(0);
				  break;
				}
				else if (valueVec.size() > 1)
				{
				  assert(0);
				  break;
				}
				else
				{
				  bool val = false;
				  if (getBoolean(valueVec.front(), &val))
				   pass->SetNormaliseNormals(val);
				  else
				   assert(0);
				  
				}
				break;
			 }
			 case  ID_LIGHTING:
			 {
				  if (valueVec.empty())
				  {
				   assert(0);
				   break;
				  }
				  else if (valueVec.size() > 1)
				  {
				   assert(0);
				   break;
				  }
				  else
				  {
				   bool val = false;
				   if (getBoolean(valueVec.front(), &val))
				    pass->SetLightEnabled(val);
				   else
				    assert(0);
				   
				  }
				  break;
			 }
			 case  ID_SHADING:
			 {
				 if (valueVec.empty())
				 {
				  assert(0);
				  break;
				 }
				 else if (valueVec.size() > 1)
				 {
				  assert(0);
				  break;
				 }
				 else
				 {
				 	 switch (mIds[valueVec[0]])
				 	 {
				 	 case ID_FLAT:
				 		 pass->SetShadingMode(SO_FLAT);
				 		 break;
				 	 case ID_GOURAUD:
				 		 pass->SetShadingMode(SO_GOURAUD);
				 		 break;
				 	 case ID_PHONG:
				 		 pass->SetShadingMode(SO_PHONG);
				 		 break;
				 	 default:
				 		 assert(0);
				 		 break;
				 	 }
				  }
				 break;
			 }
			 case  ID_POLYGON_MODE:
			 {
				 if (valueVec.empty())
				 {
				  assert(0);
				  break;
				 }
				 else if (valueVec.size() > 1)
				 {
				  assert(0);
				  break;
				 }
				 else
				 {
					  switch (mIds[valueVec[0]])
					  {
					  case ID_SOLID:
						  pass->SetPolygonMode(PM_SOLID);
						  break;
					  case ID_POINTS:
						  pass->SetPolygonMode(PM_POINTS);
						  break;
					  case ID_WIREFRAME:
						  pass->SetPolygonMode(PM_WIREFRAME);
						  break;
					  default:
						  assert(0);
						  break;
					  }
				
				 }
				 break;
			 }
			 case  ID_POLYGON_MODE_OVERRIDEABLE:
			 {
					 if (valueVec.empty())
					 {
					  assert(0);
					  break;
					 }
					 else if (valueVec.size() > 1)
					 {
					  assert(0);
					  break;
					 }
					 else
					 {
					  bool val = false;
					  if (getBoolean(valueVec.front(), &val))
					   pass->SetPolygonModeOverrideable(val);
					  else
					   assert(0);
					  break;
					 }
					 break;
			 }
			 case  ID_FOG_OVERRIDE:
			 {
					 if (valueVec.empty())
					 {
					  assert(0);
					  break;
					 }
					 else if (valueVec.size() > 8)
					 {
					  assert(0);
					  break;
					 }
					 else
					 {
					  bool val = false;
					  if (getBoolean(valueVec.front(), &val))
					  {
						  FogMode mode = FOG_NONE;
						  ColourValue clr = ColourValue::White;
						  float dens = 0.001, start = 0.0f, end = 1.0f;

						  if (valueVec.size() >= 1)
						  {
								  switch (mIds[valueVec[0]])
								  {
								  case ID_NONE:
									  mode = FOG_NONE;
									  break;
								  case ID_LINEAR:
									  mode = FOG_LINEAR;
									  break;
								  case ID_EXP:
									  mode = FOG_EXP;
									  break;
								  case ID_EXP2:
									  mode = FOG_EXP2;
									  break;
								  default:
									  assert(0);
									  break;
								  }
						  }

						  if (valueVec.size() >= 2)
						  {
							  StringVec::iterator itor = valueVec.begin();
							  itor++; itor++;
							  if (!getColour(itor, valueVec.end(), &clr, 3))
							  {
								  assert(0);
								  break;
							  }

						  }

						  if (valueVec.size() >= 5)
						  {
							  if (!getReal(valueVec[5], &dens))
							  {
								  assert(0);
								  break;
							  }
							  
						  }

						  if (valueVec.size() >= 6)
						  {
							  if (!getReal(valueVec[6], &start))
							  {
								  assert(0);
								  return false;
							  }
							  
						  }

						  if (valueVec.size() >= 7)
						  {
							  if (!getReal(valueVec[7], &end))
							  {
								  assert(0);
							
								  return false;
							  }
						  }
						  pass->SetFog(val, mode, clr, dens, start, end);
					  }
					  else
						  assert(0);
					  
					 }
					 break;
			 }
		      case  ID_COLOUR_WRITE:
			  {
						  if (valueVec.empty())
						  {
						   assert(0);
						   break;
						  }
						  else if (valueVec.size() > 1)
						  {
						   assert(0);
						   break;
						  }
						  else
						  {
						   bool val = false;
						   if (getBoolean(valueVec.front(), &val))
							   pass->SetColorWriteEnable(val);
						   else
							   assert(0);
						   
						  }
						  break;
			  }
			  case  ID_MAX_LIGHTS:
			  {
						 if (valueVec.empty())
						 {
							 assert(0);
							 break;
						 }
						 else if (valueVec.size() > 1)
						 {
							 assert(0);
							 break;
						 }
						 else
						 {
							 unsigned int val = 0;
							 if (getUInt(valueVec.front(), &val))
								 pass->SetMaxSimulationLight(static_cast<unsigned short>(val));
							 else
								 assert(0);
							 
						 }
						 break;
			  }
			  case  ID_START_LIGHT:
			  {
					   if (valueVec.empty())
					   {
					    assert(0);
					    break;
					   }
					   else if (valueVec.size() > 1)
					   {
					    assert(0);
					    break;
					   }
					   else
					   {
					    unsigned int val = 0;
					    if (getUInt(valueVec.front(), &val))
					  	  pass->SetStartLight(static_cast<unsigned short>(val));
					    else
					  	  assert(0);
					    
					   }
					   break;
			  }
			  case ID_LIGHT_MASK:
			  {
					 if (valueVec.empty())
					 {
					 	assert(0);
					 	break;
					 }
					 else
					 {
					 	unsigned int val = 0;
					 	if (getUInt(valueVec.front(), &val))
					 		pass->SetLightMask(static_cast<unsigned short>(val));
					 	else
					 		assert(0);
					 	
					 }
					 break;
			  }
			  case  ID_ITERATION:
			  {
					 if (valueVec.empty())
					 {
					 	assert(0);
					 	break;
					 }
					 else
					 {
					 		if (mIds[valueVec[0]] == ID_ONCE)
					 		{
					 			pass->SetIteratePerLight(false);
					 		}
					 		else if (mIds[valueVec[0]] == ID_ONCE_PER_LIGHT)
					 		{
					 			if (1 < valueVec.size() )
					 			{
					 				switch (mIds[valueVec[1]])
					 				{
					 				case ID_POINT:
					 					pass->SetIteratePerLight(true);
					 					break;
					 				case ID_DIRECTIONAL:
					 					pass->SetIteratePerLight(true, true, BWLight::LT_DIRECTIONAL);
					 					break;
					 				case ID_SPOT:
					 					pass->SetIteratePerLight(true, true, BWLight::LT_SPOTLIGHT);
					 					break;
					 				default:
					 					assert(0);
					 					break;
					 				}
					 			}
					 			else
					 			{
					 				pass->SetIteratePerLight(true, false);
					 			}
					 
					 		}
					 		else if (StringConverter::IsNumber(valueVec[0]))
					 		{
					 			pass->SetPassIterationCount(StringConverter::ParseInt(valueVec[0]));
					 
					 			if (1<= valueVec.size())
					 			{
					 				if (mIds[valueVec[1]] == ID_PER_LIGHT)
					 				{
					 					if (2 <= valueVec.size())
					 					{
					 						switch (mIds[valueVec[2]])
					 						{
					 						case ID_POINT:
					 							pass->SetIteratePerLight(true);
					 							break;
					 						case ID_DIRECTIONAL:
					 							pass->SetIteratePerLight(true, true, BWLight::LT_DIRECTIONAL);
					 							break;
					 						case ID_SPOT:
					 							pass->SetIteratePerLight(true, true, BWLight::LT_SPOTLIGHT);
					 							break;
					 						default:
					 							assert(0);
					 							break;
					 						}
					 					}
					 					else
					 					{
					 						pass->SetIteratePerLight(true, false);
					 					}
					 				}
					 				else if (ID_PER_N_LIGHTS)
					 				{
					 					if (2 <= valueVec.size() )
					 					{
					 						if (StringConverter::IsNumber(valueVec[2]))
					 						{
					 							pass->SetLightCountPerIteration(
					 								static_cast<unsigned short>(StringConverter::ParseInt(valueVec[2])));
					 
					 							
					 							if (3 <= valueVec.size() )
					 							{
					 								switch (mIds[valueVec[3]])
					 								{
					 								case ID_POINT:
					 									pass->SetIteratePerLight(true);
					 									break;
					 								case ID_DIRECTIONAL:
					 									pass->SetIteratePerLight(true, true, BWLight::LT_DIRECTIONAL);
					 									break;
					 								case ID_SPOT:
					 									pass->SetIteratePerLight(true, true, BWLight::LT_SPOTLIGHT);
					 									break;
					 								default:
					 									assert(0);
					 									
					 								}
					 							}
					 							else
					 							{
					 								pass->SetIteratePerLight(true, false);
					 							}
					 						}
					 						else
					 						{
					 							assert(0);
					 							break;
					 						}
					 					}
					 					else
					 					{
					 						assert(0);
					 						break;
					 					}
					 				}
					 			}
					 		}
					 		else
					 		{
					 			assert(0);
					 			break;
					 		}
					 
					 }
					 break;
			  }
		      case  ID_POINT_SIZE:
			  {
					   if (valueVec.empty())
					   {
					    assert(0);
					    break;
					   }
					   else if (valueVec.size() > 1)
					   {
					    assert(0);
					    break;
					   }
					   else
					   {
					    float val = 0.0f;
					    if (getReal(valueVec.front(), &val))
					   	 pass->SetPointSize(val);
					    else
					   	 assert(0);
					    
					   }
					   break;
			  }
			  case  ID_POINT_SPRITES:
			  {
					 if (valueVec.empty())
					 {
					 	assert(0);
					 	break;
					 }
					 else if (valueVec.size() > 1)
					 {
					 	assert(0);
					 	break;
					 }
					 else
					 {
					 	bool val = false;
					 	if (getBoolean(valueVec.front(), &val))
					 		pass->SetPointSpriteEnable(val);
					 	else
					 		assert(0);
					 	
					 }
					 break;
			  }
			  case  ID_POINT_SIZE_ATTENUATION:
			  {
					 if (valueVec.empty())
					 {
						 assert(0);
						 break;
					 }
					 else if (valueVec.size() > 4)
					 {
						 assert(0);
						 break;
					 }
					 else
					 {
						 bool val = false;
						 if (getBoolean(valueVec.front(), &val))
						 {
							 if (val)
							 {
								 if (valueVec.size() > 1)
								 {

									 float constant = 0.0f, linear = 1.0f, quadratic = 0.0f;

									 if (1 <= valueVec.size() )
									 {
										 if (StringConverter::IsNumber(valueVec[1]))
											 constant = StringConverter::ParseReal(valueVec[1]);
										 else
											 assert(0);
										 
									 }
									 else
									 {
										 assert(0);
										 break;
									 }

									 if ( 2 <= valueVec.size() )
									 {
										 if (StringConverter::IsNumber(valueVec[2]))
											 linear = StringConverter::ParseReal(valueVec[2]);
										 else
											 assert(0);
										 
									 }
									 else
									 {
										 assert(0);
										 break;
									 }

									 if (valueVec.size() >= 3)
									 {
										 if (StringConverter::IsNumber(valueVec[3]))
											 quadratic = StringConverter::ParseReal(valueVec[3]);
										 else
											 assert(0);
										 
									 }
									 else
									 {
										 assert(0);
										 break;
									 }

									 pass->SetPointAttenuation(true, constant, linear, quadratic);
								 }
								 else
								 {
									 pass->SetPointAttenuation(true);
								 }
							 }
							 else
							 {
								 pass->SetPointAttenuation(false);
							 }
						 }
						 else
							 assert(0);
						 
					 }
					 break;
			  }
			  case  ID_POINT_SIZE_MIN:
			  {
					 if (valueVec.empty())
					 {
						 assert(0);
						 break;
					 }
					 else if (valueVec.size() > 1)
					 {
						 assert(0);
						 break;
					 }
					 else
					 {
						 float val = 0.0f;
						 if (getReal(valueVec.front(), &val))
							 pass->SetPointMinSize(val);
						 else
							 assert(0);

					 }
					 break;
			  }
			  case  ID_POINT_SIZE_MAX:
			  {
					 if (valueVec.empty())
					 {
						 assert(0);
						 break;
					 }
					 else if (valueVec.size() > 1)
					 {
						 assert(0);
						 break;
					 }
					 else
					 {
						 float val = 0.0f;
						 if (getReal(valueVec.front(), &val))
							 pass->SetPointMaxSize(val);
						 else
							 assert(0);
						     
					 }
					 break;
			  }
		      //case  ID_TEXTURE_ALIAS,
		      //case  ID_TEXTURE,
		      //case  ID_1D,
		      //case  ID_2D,
		      //case  ID_3D,
		      //case  ID_CUBIC,
		      //case  ID_UNLIMITED,
		      //case  ID_ALPHA,
		      //case  ID_GAMMA,
		      //case  ID_ANIM_TEXTURE,
		      //case  ID_CUBIC_TEXTURE,
		      //case  ID_SEPARATE_UV,
		      //case  ID_COMBINED_UVW,
		      //case  ID_TEX_COORD_SET,
		      //case  ID_TEX_ADDRESS_MODE,
		      //case  ID_WRAP,
		      //case  ID_CLAMP,
		      //case  ID_BORDER,
		      //case  ID_MIRROR,
		      //case  ID_TEX_BORDER_COLOUR,
		      //case  ID_FILTERING,
		      //case  ID_BILINEAR,
		      //case  ID_TRILINEAR,
		      //case  ID_ANISOTROPIC,
		      //case  ID_MAX_ANISOTROPY,
		      //case  ID_MIPMAP_BIAS,
		      //case  ID_COLOUR_OP,
		      //case  ID_REPLACE,
		      //case  ID_ADD,
		      //case  ID_MODULATE,
		      //case  ID_ALPHA_BLEND,
		      //case  ID_COLOUR_OP_EX,
		      //case  ID_SOURCE1,
		      //case  ID_SOURCE2,
		      //case  ID_MODULATE_X2,
		      //case  ID_MODULATE_X4,
		      //case  ID_ADD_SIGNED,
		      //case  ID_ADD_SMOOTH,
		      //case  ID_SUBTRACT,
		      //case  ID_BLEND_DIFFUSE_COLOUR,
		      //case  ID_BLEND_DIFFUSE_ALPHA,
		      //case  ID_BLEND_TEXTURE_ALPHA,
		      //case  ID_BLEND_CURRENT_ALPHA,
		      //case  ID_BLEND_MANUAL,
		      //case  ID_DOT_PRODUCT,
		      //case  ID_SRC_CURRENT,
		      //case  ID_SRC_TEXTURE,
		      //case  ID_SRC_DIFFUSE,
		      //case  ID_SRC_SPECULAR,
		      //case  ID_SRC_MANUAL,
		      //case  ID_COLOUR_OP_MULTIPASS_FALLBACK,
		      //case  ID_ALPHA_OP_EX,
		      //case  ID_ENV_MAP,
		      //case  ID_SPHERICAL,
		      //case  ID_PLANAR,
		      //case  ID_CUBIC_REFLECTION,
		      //case  ID_CUBIC_NORMAL,
		      //case  ID_SCROLL,
		      //case  ID_SCROLL_ANIM,
		      //case  ID_ROTATE,
		      //case  ID_ROTATE_ANIM,
		      //case  ID_SCALE,
		      //case  ID_WAVE_XFORM,
		      //case  ID_SCROLL_X,
		      //case  ID_SCROLL_Y,
		      //case  ID_SCALE_X,
		      //case  ID_SCALE_Y,
		      //case  ID_SINE,
		      //case  ID_TRIANGLE,
		      //case  ID_SQUARE,
		      //case  ID_SAWTOOTH,
		      //case  ID_INVERSE_SAWTOOTH,
		      //case  ID_TRANSFORM,
		      //case  ID_BINDING_TYPE,
		      //case  ID_VERTEX,
		      //case  ID_FRAGMENT,
		      //case  ID_CONTENT_TYPE,
		      //case  ID_NAMED,
		      //case  ID_SHADOW,
		      //case  ID_TEXTURE_SOURCE,
		      //case  ID_SHARED_PARAMS,
		      //case  ID_SHARED_PARAM_NAMED,
		      //case  ID_SHARED_PARAMS_REF,
		      //case  ID_PARTICLE_SYSTEM,
		      //case  ID_EMITTER,
		      //case  ID_AFFECTOR,
		      //case  ID_COMPOSITOR,
		      //case  ID_TARGET,
		      //case  ID_TARGET_OUTPUT,
		      //case  ID_INPUT,
		      //case  ID_PREVIOUS,
		      //case  ID_TARGET_WIDTH,
		      //case  ID_TARGET_HEIGHT,
		      //case  ID_TARGET_WIDTH_SCALED,
		      //case  ID_TARGET_HEIGHT_SCALED,
		      //case  ID_COMPOSITOR_LOGIC,
		      //case  ID_TEXTURE_REF,
		      //case  ID_SCOPE_LOCAL,
		      //case  ID_SCOPE_CHAIN,
		      //case  ID_SCOPE_GLOBAL,
		      //case  ID_POOLED,
		      //case  //ID_GAMMA, - already registered for ma
		      //case  ID_NO_FSAA,
		      //case  ID_DEPTH_POOL,
		      //case  ID_ONLY_INITIAL,
		      //case  ID_VISIBILITY_MASK,
		      //case  ID_LOD_BIAS,
		      //case  ID_MATERIAL_SCHEME,
		      //case  ID_SHADOWS_ENABLED,
		      //case  ID_CLEAR,
		      //case  ID_STENCIL,
		      //case  ID_RENDER_SCENE,
		      //case  ID_RENDER_QUAD,
		      //case  ID_IDENTIFIER,
		      //case  ID_FIRST_RENDER_QUEUE,
		      //case  ID_LAST_RENDER_QUEUE,
		      //case  ID_QUAD_NORMALS,
		      //case  ID_CAMERA_FAR_CORNERS_VIEW_SPACE,
		      //case  ID_CAMERA_FAR_CORNERS_WORLD_SPACE,
		      //case  ID_BUFFERS,
		      //case  ID_COLOUR,
		      //case  ID_DEPTH,
		      //case  ID_COLOUR_VALUE,
		      //case  ID_DEPTH_VALUE,
		      //case  ID_STENCIL_VALUE,
		      //case  ID_CHECK,
		      //case  ID_COMP_FUNC,
		      //case  ID_REF_VALUE,
		      //case  ID_MASK,
		      //case  ID_FAIL_OP,
		      //case  ID_KEEP,
		      //case  ID_INCREMENT,
		      //case  ID_DECREMENT,
		      //case  ID_INCREMENT_WRAP,
		      //case  ID_DECREMENT_WRAP,
		      //case  ID_INVERT,
		      //case  ID_DEPTH_FAIL_OP,
		      //case  ID_PASS_OP :
		      //case  ID_TWO_SIDED :
		}
		
    }
}

bool Parser::ParseTextureUnitState(BWDataStream &dataStream, BWTextureUnitState *textureUnitState)
{
	std::string line;
	while (!dataStream.Eof())
	{
		line = dataStream.GetLine();
		DeleteChar(line, '\t');
		StringVec parem = StringUtil::Split(line);
		if (parem.size() == 0 || parem[0] == "{")
		{
			continue;
		}
		if (parem[0] == "}")
		{
			return true;
		}
		switch (mIds[parem[0]])
		{
		case ID_TEXTURE_ALIAS:
		{
		        if (parem.size() != 2)
		        {
					assert(0);
		        }
				textureUnitState->setTextureNameAlias(parem[1]);
				break;
		}
		case ID_TEXTURE:
		{
						   if (parem.size() > 5)
						   {
							   assert(0);
						   }
						   TextureType texType = TEX_TYPE_2D;
						   int MipmapNum = MIP_DEFAULT;
						   bool isAlpha = false;
						   bool rgbRead = false;
						   bool IsEnableHardwareGamma = false;
						   PixelFormat format = PF_UNKNOWN;
						   StringVec::iterator itor = parem.begin();
						   //itor++; itor++;
						   while (itor != parem.end())
						   {
							   switch (mIds[(*itor)])
							   {
							   case ID_1D:
							   {
											 texType = TEX_TYPE_1D;
											 break;
							   }
							   case ID_2D:
							   {
											 texType = TEX_TYPE_2D;
											 break;
							   }
							   case  ID_3D:
							   {
											  texType = TEX_TYPE_3D;
											  break;
							   }
							   case  ID_CUBIC:
							   {
											  texType = TEX_TYPE_CUBE_MAP;
											  break;
							   }
							   case ID_UNLIMITED:
							   {
													MipmapNum = MIP_UNLIMITED;
													break;
							   }
							   case  ID_ALPHA:
							   {
												 isAlpha = true;
												 break;
							   }
							   case ID_GAMMA:
							   {
								   IsEnableHardwareGamma = true;
												break;
							   }
							   default:
								   if (StringConverter::IsNumber(*itor))
								   {
									   MipmapNum = StringConverter::ParseInt(*itor);
								   }
								   else
								   {
									  /* std::string tmp = *itor;
									   for (int i = 0; i < PF_COUNT; i++)
									   {
										   PixelFormat pf = static_cast<PixelFormat>(i);
										   if ()
										   {
										   }
									   }
									   format = PixelUtil::getFormatFromName(*itor, true);*/
								   }
								   break;
							   }
							   itor++;
						   }
						   textureUnitState->setNumMipmaps(MipmapNum);
						   textureUnitState->setDesiredFormat(format);
						   textureUnitState->setIsAlpha(isAlpha);
						   textureUnitState->setHardwareGammaEnable(rgbRead);
						   textureUnitState->setTextureName(parem[parem.size() - 1], texType);
						   textureUnitState->setHardwareGammaEnable(IsEnableHardwareGamma);
						   break;
		}
		case ID_ANIM_TEXTURE:
		{
		                   if (StringConverter::IsNumber(parem[parem.size()- 2]))
		                   {
							   textureUnitState->setAnimatedTextureName(parem[1], StringConverter::ParseInt(parem[2]), StringConverter::ParseReal(parem[3]));
		                   }
						   else
						   {
							   std::string *stringVec = new std::string[parem.size() - 2];
							   for (int  i = 1; i < parem.size() - 1; i++)
							   {
								   stringVec[i] = parem[i];
							   }
							   textureUnitState->setAnimatedTextureName(stringVec, parem.size(), StringConverter::ParseReal(parem[parem.size() - 1]));
						   }
								break;
		}
		case ID_CUBIC_TEXTURE:
		{
								
							if (parem.size() > 6)
							{
								std::string *stringVec = new std::string[parem.size() - 2];
								for (int i = 1; i < parem.size() - 1; i++)
								{
									stringVec[i] = parem[i];
								}
								textureUnitState->setCubicTextureName(stringVec, false);
							}
							else
							{
								bool forUVW;
								if (parem[parem.size() - 1] == "separateUV")
								{
									forUVW = false;
								}
								else
								{
									forUVW = true;
								}
								textureUnitState->setCubicTextureName(parem[1], forUVW);
							}
							break;
		}
		case ID_TEX_COORD_SET:
		{
								 if (parem.size() != 2)
								 {
									 assert(0);
								 }
								 textureUnitState->setTextureCoordSet(StringConverter::ParseInt(parem[1]));
								 break;
		}
		case ID_TEX_ADDRESS_MODE:
		{
									BWTextureUnitState::UVWAddressingMode addressingMode;

									if (parem.size() == 2)
									{
										switch (mIds[parem[1]])
										{
										case ID_WRAP:
										{
														addressingMode.u = BWTextureUnitState::TAM_WRAP;
														addressingMode.v = BWTextureUnitState::TAM_WRAP;
														addressingMode.w = BWTextureUnitState::TAM_WRAP;
														break;
										}
										case ID_MIRROR:
										{
											
												         addressingMode.u = BWTextureUnitState::TAM_MIRROR;
												         addressingMode.v = BWTextureUnitState::TAM_MIRROR;
												         addressingMode.w = BWTextureUnitState::TAM_MIRROR;
												         break;
											
										}
										case  ID_CLAMP:
										{
														  addressingMode.u = BWTextureUnitState::TAM_CLAMP;
														  addressingMode.v = BWTextureUnitState::TAM_CLAMP;
														  addressingMode.w = BWTextureUnitState::TAM_CLAMP;
														  break;
										}
										case  ID_BORDER:
										{
														   addressingMode.u = BWTextureUnitState::TAM_BORDER;
														   addressingMode.v = BWTextureUnitState::TAM_BORDER;
														   addressingMode.w = BWTextureUnitState::TAM_BORDER;
														   break;
										}
										default:
											break;
										}
									}
									else
									{
										BWTextureUnitState::TextureAddressingMode mode[3];
										for (int i = 0; i < 3;i++)
										{
											switch (mIds[parem[1+ i]])
											{
											case ID_WRAP:
											{
															mode[i] = BWTextureUnitState::TAM_WRAP;
															break;
											}
											case ID_MIRROR:
											{

															  mode[i] = BWTextureUnitState::TAM_MIRROR;
															  break;
											}
											case  ID_CLAMP:
											{
															  mode[i] = BWTextureUnitState::TAM_CLAMP;
															  break;
											}
											case  ID_BORDER:
											{
															   mode[i] = BWTextureUnitState::TAM_BORDER;
															   break;
											}
											default:
												break;
											}
										}
										addressingMode.u = mode[0];
										addressingMode.v = mode[1];
										addressingMode.w = mode[2];
									}
									textureUnitState->setTextureAddressingMode(addressingMode);
									break;
		}
		case ID_TEX_BORDER_COLOUR:
		{
									 float value[4]; value[3] = 1.0;
									 for (int i = 1; i < parem.size() ; i++)
									 {
										 value[i - 1] = StringConverter::ParseReal(parem[i]);
									 }
									 ColourValue colourValue(value[0],value[1], value[2], value[3]);
									 textureUnitState->setTextureBorderColour(colourValue);
									 break;
		}
		case  ID_FILTERING:
		{
							  if (parem.size() == 2)
							  {
								  TextureFilterOptions options;
								  switch (mIds[parem[1]])
								  {
								  case  ID_NONE:
									  options = TFO_NONE;
									  break;
								  case  ID_BILINEAR :
									  options = TFO_BILINEAR;
									  break;
								  case  ID_TRILINEAR:
									  options = TFO_TRILINEAR;
									  break;
								  case ID_ANISOTROPIC:
									  options = TFO_ANISOTROPIC;
									  break;
								  default:
									  break;
								  }
								  textureUnitState->setTextureFiltering(options);
							  }
							  else
							  {
								  FilterOptions opetion[3];
								  for (int i = 0; i < 3;i++)
								  {
									  switch (mIds[parem[1+i]])
									  {
									  case  ID_NONE:
										  opetion[i] = FO_NONE;
										  break;
									  case  ID_BILINEAR:
										  opetion[i] = FO_BILINEAR;
										  break;
									  case  ID_TRILINEAR:
										  opetion[i] = FO_TRILINEAR;
										  break;
									  case ID_ANISOTROPIC:
										  opetion[i] = FO_ANISOTROPIC;
										  break;
									  default:
										  break;
									  }
								  }
								  textureUnitState->setTextureFiltering(opetion[0], opetion[1], opetion[2]);
							  }
							  break;
		}
		case  ID_MAX_ANISOTROPY:
		{
								   if (parem.size() != 2)
								   {
									   assert(0);
								   }
								   textureUnitState->setTextureAnisotropy(StringConverter::ParseInt(parem[1]));
								   break;
		}
		case  ID_MIPMAP_BIAS:
		{
								if (parem.size() != 2)
								{
									assert(0);
								}
								textureUnitState->setTextureMipmapBias(StringConverter::ParseReal(parem[1]));
								break;
		}
		case  ID_COLOUR_OP:
		{
							  if (parem.size() != 2)
							  {
								  assert(0);
							  }
							  switch (mIds[parem[1]])
							  {
							  case ID_REPLACE:
								  textureUnitState->setColourOperation(LBO_REPLACE);
								  break;
							  case ID_ADD:
								  textureUnitState->setColourOperation(LBO_ADD);
								  break;
							  case ID_ALPHA_BLEND:
								  textureUnitState->setColourOperation(LBO_ALPHA_BLEND);
								  break;
							  default:
								  textureUnitState->setColourOperation(LBO_MODULATE);
								  break;
							  }
							  break;
		}
		case ID_COLOUR_OP_EX:
		{
								if (parem.size() < 4)
								{
									assert(0);
								}
								LayerBlendOperationEx op = LBX_ADD;
								LayerBlendSource src1, src2;
								src1 = LBS_CURRENT; src2 = LBS_TEXTURE;
								ColourValue arg1 = ColourValue::White;
								ColourValue arg2 = ColourValue::White;
								float manualBlend = 0.0f;
								switch (mIds[parem[1]])
								{
								case ID_SOURCE1 : op = LBX_SOURCE1;  break; 
								case ID_SOURCE2 : op = LBX_SOURCE2;  break;
								case ID_MODULATE: op = LBX_MODULATE; break;
								case ID_MODULATE_X2: op = LBX_MODULATE_X2; break;
								case  ID_MODULATE_X4:op = LBX_MODULATE_X4; break;
								case  ID_ADD: op = LBX_ADD; break;
								case ID_ADD_SIGNED: op = LBX_ADD_SIGNED; break;
								case ID_ADD_SMOOTH:op = LBX_ADD_SMOOTH; break;
								case ID_SUBTRACT: op = LBX_SUBTRACT; break;
								case  ID_BLEND_DIFFUSE_ALPHA: op = LBX_BLEND_DIFFUSE_ALPHA; break; 
								case ID_BLEND_TEXTURE_ALPHA: op = LBX_BLEND_TEXTURE_ALPHA; break;
								case  ID_BLEND_DIFFUSE_COLOUR: op = LBX_BLEND_DIFFUSE_COLOUR; break;
								case  ID_BLEND_MANUAL: op = LBX_BLEND_MANUAL; break;
								case ID_BLEND_CURRENT_ALPHA: op = LBX_BLEND_CURRENT_ALPHA; break; 
								case  ID_DOT_PRODUCT: op = LBX_DOTPRODUCT; break;
								}
								switch (mIds[parem[2]])
								{
								case ID_SRC_CURRENT: src1 = LBS_CURRENT; break;
								case ID_SRC_TEXTURE: src1 = LBS_TEXTURE; break;
								case  ID_SRC_DIFFUSE:src1 = LBS_DIFFUSE; break;
								case ID_SRC_MANUAL: src1 = LBS_MANUAL; break;
								case ID_SPECULAR: src1 = LBS_SPECULAR; break;
								}
								switch (mIds[parem[3]])
								{
								case ID_SRC_CURRENT: src2 = LBS_CURRENT; break;
								case ID_SRC_TEXTURE: src2 = LBS_TEXTURE; break;
								case  ID_SRC_DIFFUSE:src2 = LBS_DIFFUSE; break;
								case ID_SRC_MANUAL: src2 = LBS_MANUAL; break;
								case ID_SPECULAR: src2 = LBS_SPECULAR; break;
								}
								int offset = 0;
								if (op == LBX_BLEND_MANUAL)
								{
									manualBlend = StringConverter::ParseReal(parem[4] , -1.0f);
									if (manualBlend == -1.0f)
									{
										assert(0);
									}
									offset++;
								}
								if (src1 == LBS_MANUAL)
								{
									float value[4]; value[3] = 1.0;
									for (int i = 3 + offset ; i < 3 + offset + 4; i++)
									{
										value[i - 3 - offset] = StringConverter::ParseReal(parem[i]);
									}
									arg1.r = value[0];
									arg1.g = value[1];
									arg1.b = value[2];
									arg1.a = value[3];
									offset += 4;
								}
								if (src2 == LBS_MANUAL)
								{
									float value[4]; value[3] = 1.0;
									for (int i = 3 + offset; i < 3 + offset + 4; i++)
									{
										value[i - 3 - offset] = StringConverter::ParseReal(parem[i]);
									}
									arg2.r = value[0];
									arg2.g = value[1];
									arg2.b = value[2];
									arg2.a = value[3];
								}
								textureUnitState->setColourOperationEx(op, src1, src2, arg1, arg2 , manualBlend);
								break;
		}
		case ID_COLOUR_OP_MULTIPASS_FALLBACK:
		{
								if (parem.size() != 3)
								{
									assert(0);
								}
								if (parem.size() == 2)
								{
									switch (mIds[parem[1]])
									{
									case ID_ADD:
										textureUnitState->setColourOpMultipassFallback(SBF_ONE, SBF_ONE);
										break;
									case ID_MODULATE:
										textureUnitState->setColourOpMultipassFallback(SBF_DEST_COLOUR, SBF_ZERO);
										break;
									case  ID_COLOUR_BLEND:
										textureUnitState->setColourOpMultipassFallback(SBF_SOURCE_COLOUR, SBF_ONE_MINUS_SOURCE_COLOUR);
										break;
									case ID_ALPHA_BLEND:
										textureUnitState->setColourOpMultipassFallback(SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
										break;
									case  ID_REPLACE:
										textureUnitState->setColourOpMultipassFallback(SBF_ONE, SBF_ZERO);
										break;
									default:
										assert(0);
									}
								}
								else
								{
									SceneBlendFactor factor[2];
									getSceneBlendFactor(parem[1], &factor[0]);
									getSceneBlendFactor(parem[1], &factor[1]);
									textureUnitState->setColourOpMultipassFallback(factor[0], factor[1]);
								}
								break;
		}
		case ID_ALPHA_OP_EX:
		{
							   if (parem.size() < 4)
							   {
								   assert(0);
							   }
							   LayerBlendOperationEx op = LBX_ADD;
							   LayerBlendSource src1, src2;
							   src1 = LBS_CURRENT; src2 = LBS_TEXTURE;
							   ColourValue arg1 = ColourValue::White;
							   ColourValue arg2 = ColourValue::White;
							   float manualBlend = 0.0f;
							   switch (mIds[parem[1]])
							   {
							   case ID_SOURCE1: op = LBX_SOURCE1;  break;
							   case ID_SOURCE2: op = LBX_SOURCE2;  break;
							   case ID_MODULATE: op = LBX_MODULATE; break;
							   case ID_MODULATE_X2: op = LBX_MODULATE_X2; break;
							   case  ID_MODULATE_X4:op = LBX_MODULATE_X4; break;
							   case  ID_ADD: op = LBX_ADD; break;
							   case ID_ADD_SIGNED: op = LBX_ADD_SIGNED; break;
							   case ID_ADD_SMOOTH:op = LBX_ADD_SMOOTH; break;
							   case ID_SUBTRACT: op = LBX_SUBTRACT; break;
							   case  ID_BLEND_DIFFUSE_ALPHA: op = LBX_BLEND_DIFFUSE_ALPHA; break;
							   case ID_BLEND_TEXTURE_ALPHA: op = LBX_BLEND_TEXTURE_ALPHA; break;
							   case  ID_BLEND_DIFFUSE_COLOUR: op = LBX_BLEND_DIFFUSE_COLOUR; break;
							   case  ID_BLEND_MANUAL: op = LBX_BLEND_MANUAL; break;
							   case ID_BLEND_CURRENT_ALPHA: op = LBX_BLEND_CURRENT_ALPHA; break;
							   case  ID_DOT_PRODUCT: op = LBX_DOTPRODUCT; break;
							   }
							   switch (mIds[parem[2]])
							   {
							   case ID_SRC_CURRENT: src1 = LBS_CURRENT; break;
							   case ID_SRC_TEXTURE: src1 = LBS_TEXTURE; break;
							   case  ID_SRC_DIFFUSE:src1 = LBS_DIFFUSE; break;
							   case ID_SRC_MANUAL: src1 = LBS_MANUAL; break;
							   case ID_SPECULAR: src1 = LBS_SPECULAR; break;
							   }
							   switch (mIds[parem[3]])
							   {
							   case ID_SRC_CURRENT: src2 = LBS_CURRENT; break;
							   case ID_SRC_TEXTURE: src2 = LBS_TEXTURE; break;
							   case  ID_SRC_DIFFUSE:src2 = LBS_DIFFUSE; break;
							   case ID_SRC_MANUAL: src2 = LBS_MANUAL; break;
							   case ID_SPECULAR: src2 = LBS_SPECULAR; break;
							   }
							   int offset = 0;
							   if (op = LBX_BLEND_MANUAL)
							   {
								   manualBlend = StringConverter::ParseReal(parem[4], -1.0f);
								   if (manualBlend == -1.0f)
								   {
									   assert(0);
								   }
								   offset++;
							   }
							   if (src1 == LBS_MANUAL)
							   {
								   float value[4]; value[3] = 1.0;
								   for (int i = 3 + offset; i < 3 + offset + 4; i++)
								   {
									   value[i - 3 - offset] = StringConverter::ParseReal(parem[i]);
								   }
								   arg1.r = value[0];
								   arg1.g = value[1];
								   arg1.b = value[2];
								   arg1.a = value[3];
								   offset += 4;
							   }
							   if (src2 == LBS_MANUAL)
							   {
								   float value[4]; value[3] = 1.0;
								   for (int i = 3 + offset; i < 3 + offset + 4; i++)
								   {
									   value[i - 3 - offset] = StringConverter::ParseReal(parem[i]);
								   }
								   arg2.r = value[0];
								   arg2.g = value[1];
								   arg2.b = value[2];
								   arg2.a = value[3];
							   }
							   textureUnitState->setAlphaOperation(op, src1, src2, arg1, arg2, manualBlend);
							   break;

		}
		case ID_ENV_MAP:
		{
						   if (parem.size() != 2)
						   {
							   assert(0);
						   }
						   switch (mIds[parem[1]])
						   {
						   case  ID_SPHERICAL :
							   textureUnitState->setEnvironmentMap(true, BWTextureUnitState::ENV_CURVED);
							   break;
						   case ID_OFF:
							   textureUnitState->setEnvironmentMap(false);
							   break;
						   case ID_PLANAR:
							   textureUnitState->setEnvironmentMap(true, BWTextureUnitState::ENV_PLANAR);
							   break;
						   case ID_CUBIC_REFLECTION:
							   textureUnitState->setEnvironmentMap(true, BWTextureUnitState::ENV_REFLECTION);
							   break;
						   case ID_CUBIC_NORMAL:
							   textureUnitState->setEnvironmentMap(true, BWTextureUnitState::ENV_NORMAL);
							   break;
						   default:
							   break;
						   }
						   break;
		}
		case ID_SCROLL:
		{
						  if (parem.size() != 3)
						  {
							  assert(0);
						  }
						  float x = StringConverter::ParseReal(parem[1]);
						  float y = StringConverter::ParseReal(parem[2]);
						  textureUnitState->setTextureScroll(x, y);
						  break;
		}
		case ID_SCROLL_ANIM:
		{
							   if (parem.size() != 3)
							   {
								   assert(0);
							   }
							   float x = StringConverter::ParseReal(parem[1]);
							   float y = StringConverter::ParseReal(parem[2]);
							   textureUnitState->setScrollAnimation(x, y);
							   break;
		}
		case ID_ROTATE:
		{
						  if (parem.size() != 2)
						  {
							  assert(0);
						  }
						  float rotate = StringConverter::ParseReal(parem[1]);
						  textureUnitState->setTextureRotate(rotate);
						  break;
		}
		case ID_ROTATE_ANIM:
		{
							   if (parem.size() != 2)
							   {
								   assert(0);
							   }
							   float rotate = StringConverter::ParseReal(parem[1]);
							   textureUnitState->setRotateAnimation(rotate);
							   break;
		}
		case ID_SCALE:
		{
						 if (parem.size() != 3)
						 {
							 assert(0);
						 }
						 float x = StringConverter::ParseReal(parem[1]);
						 float y = StringConverter::ParseReal(parem[2]);
						 textureUnitState->setTextureScale(x, y);
						 break;
		}
		case ID_TRANSFORM:
		{
							 if (parem.size() != 17)
							 {
								 assert(0);
							 }
							 float matrix[16];
							 for (int i = 0; i < 16; i++)
							 {
								 matrix[i] = StringConverter::ParseReal(parem[i + 1]);
							 }
							 textureUnitState->setTextureTransform(matrix);
							 break;
		}
		default:
			break;
		}
	}
	assert(0);//括号不匹配
}