#include "BWTextureUnitState.h"
#include "BWPass.h"
#include "BWTextureManager.h"
#include "BWStringConverter.h"
#include "BWMaterialManager.h"
BWTextureUnitState::BWTextureUnitState(BWPass *pass) :
 mParent(pass)
 , mCurrentFrame(0)
 , mAnimationDuration(0)
 , mCubic(false)
 , mTextureType(TEX_TYPE_2D)
 , mDesireFormat(PF_UNKNOWN)
 , mTextureSrcMipmap(0)
 , mTextureCoordSetIndex(0)
 , mIsAlpha(false)
 , mRecalcTexMatrix(false)
 , mUMod(1.0f)
 , mVMod(1.0f)
 , mUScale(1.0f)
 , mVScale(1.0f)
 , mRotate(0.0f)
 , mMinFilter(FO_LINEAR)
 , mMagFilter(FO_LINEAR)
 , mMipFilter(FO_POINT)
 , mMaxAniso(BWMaterialManager::GetInstance()->GetDefaultAnisotropy())
 , mMipmapBias(0.0f)
 , mIsDefaultAniso(true)
 , mIsDefaultFiltering(true)
 , mBindingType(BT_FRAGMENT)
 , mContentType(CONTENT_NAMED)
 , mTextureLoadFailed(false)
 , mBorderColour(ColourValue::Black)
{
	mColourBlendMode.blendType = LBT_COLOUR;
	mAlphaBlendMode.operation = LBX_MODULATE;
	mAlphaBlendMode.blendType = LBT_ALPHA;
	mAlphaBlendMode.source1 = LBS_TEXTURE;
	mAlphaBlendMode.source2 = LBS_CURRENT;
	mAlphaBlendMode.alphaArg2 = 0.0; 
	mAlphaBlendMode.alphaArg1 = 0.0;
	setColourOperation(LBO_MODULATE);
	setTextureAddressingMode(TAM_WRAP);
}
BWTextureUnitState::~BWTextureUnitState()
{

}
void BWTextureUnitState::setName(const std::string &name)
{
	mName = name;
}
const std::string & BWTextureUnitState::getName()
{
	return mName;
}
bool BWTextureUnitState::isLoad()
{
	return mParent->IsLoaded(); // 这里 主要解决的状况是  如果material是load状态  那么在向他添加新的资源的时候 也要让资源在这个状态
}
BWPass *BWTextureUnitState::getParent()
{
	return mParent;
}

void BWTextureUnitState::SetIndex(int Index)
{
	mIndex = Index;
	for (int i = 0; i < mFramePtrs.size(); i++)
	{
		if (mFramePtrs[i].Get())
		{
			mFramePtrs[i]->SetIndex(Index);
		}
	}
}

void BWTextureUnitState::_prepare()
{
	for (unsigned int i = 0; i < mFrame.size(); i++)
	{
		ensurePrepare(i);
	}
}
void BWTextureUnitState::ensurePrepare(size_t frame) const 
{
	assert(0);
}
void BWTextureUnitState::_load()
{
	for (unsigned int i = 0; i < mFrame.size(); i++)
	{
		ensureLoaded(i);
	}
	if (mAnimationDuration != 0)
	{
		createAnimContorller();
	}
	for (EffectMap::iterator itor = mEffects.begin(); itor != mEffects.end(); itor++)
	{
		createEffectContorller(itor->second);
	}
}
void BWTextureUnitState::ensureLoaded(size_t frame) const 
{
	if (!mFrame[frame].empty() && !mTextureLoadFailed)
	{
		if (mFramePtrs[frame].IsNull())
		{
			mFramePtrs[frame] = BWTextureManager::GetInstance()->load(mFrame[frame], mParent->GetGroupName(), 
				TU_DYNAMIC ,mTextureType, mTextureSrcMipmap, 1.0, mIsAlpha, mDesireFormat , mHardwareGama);
		}
	}
	else
	{
		mFramePtrs[frame]->Load();
	}
}
void BWTextureUnitState::setTextureName(const std::string &name, TextureType type /* = TEX_TYPE_2D */)
{
	setContentType(CONTENT_NAMED);
	mTextureLoadFailed = false;

	if (type == TEX_TYPE_CUBE_MAP)
	{
		setCubicTextureName(name, true);
	}
	else
	{
		mFramePtrs.resize(1);
		mFrame.resize(1);
		mFrame[0] = name;
		mFramePtrs[0].SetNull();
		mCurrentFrame = 0;
		mCubic = false;
		mTextureType = type;
		if (name.empty())
		{
			return;
		}
		if (isLoad())
		{
			_load();
		}
	}
}
void BWTextureUnitState::setColourOperation(const LayerBlendOperation op)
{
	switch (op)
	{
	case LBO_REPLACE:
		setColourOperationEx(LBX_SOURCE1, LBS_TEXTURE, LBS_CURRENT);
		setColourOpMultipassFallback(SBF_ONE, SBF_ZERO);
		break;
	case LBO_ADD:
		setColourOperationEx(LBX_ADD, LBS_TEXTURE, LBS_CURRENT);
		setColourOpMultipassFallback(SBF_ONE, SBF_ONE);
		break;
	case LBO_MODULATE:
		setColourOperationEx(LBX_MODULATE, LBS_TEXTURE, LBS_CURRENT);
		setColourOpMultipassFallback(SBF_DEST_COLOUR, SBF_ZERO);
		break;
	case LBO_ALPHA_BLEND:
		setColourOperationEx(LBX_BLEND_TEXTURE_ALPHA, LBS_TEXTURE, LBS_CURRENT);
		setColourOpMultipassFallback(SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
		break;
	}
}
void BWTextureUnitState::setAlphaOperation(LayerBlendOperationEx op, LayerBlendSource src1, LayerBlendSource src2, const ColourValue& arg1, const ColourValue& arg2, float factor)
{
	mAlphaBlendMode.operation = op;
	mAlphaBlendMode.source1 = src1;
	mAlphaBlendMode.source2 = src2;
	mAlphaBlendMode.colourArg1 = arg1;
	mAlphaBlendMode.colourArg2 = arg2;
	mAlphaBlendMode.factor =  factor;
}
void BWTextureUnitState::setEnvironmentMap(bool enable, EnvMapType envMapType /* = ENV_CURVED */)
{
	if (enable)
	{
		TextureEffect eff;
		eff.type = ET_ENVIRONMENT_MAP;
		eff.subtype = envMapType;
		addEffect(eff);
	}
	else
	{
		removeEffect(ET_ENVIRONMENT_MAP);
	}
}
void BWTextureUnitState::setRotateAnimation(float speed)
{
	removeEffect(ET_ROTATE);
	if (speed == 0.0)
	{
		return;
	}
	TextureEffect eff;
	eff.type = ET_ROTATE;
	eff.arg1 = speed;
	addEffect(eff);
}
void BWTextureUnitState::removeEffect(const TextureEffectType effectType)
{
	std::pair<EffectMap::iterator, EffectMap::iterator> remPar = mEffects.equal_range(effectType);
	for (EffectMap::iterator itor = remPar.first ; itor != remPar.second ; itor++)
	{
		//销毁controller
	}
	mEffects.erase(remPar.first, remPar.second);
}
void BWTextureUnitState::removeAllEffects()
{
	EffectMap::iterator itor = mEffects.begin();
	while (itor != mEffects.end())
	{
		//销毁controller
		itor++;
	}
	mEffects.clear();
}
void BWTextureUnitState::setTextureFiltering(FilterOptions minfilter, FilterOptions magfilter, FilterOptions mipfilter)
{
	mMinFilter = minfilter;
	mMagFilter = magfilter;
	mMipFilter = mipfilter;
}
void BWTextureUnitState::setTextureFiltering(TextureFilterOptions filterType)
{
	switch (filterType)
	{
	case TFO_NONE:
		setTextureFiltering(FO_POINT, FO_POINT, FO_NONE);
		break;
	case TFO_BILINEAR:
		setTextureFiltering(FO_LINEAR, FO_LINEAR, FO_POINT);
		break;
	case TFO_TRILINEAR:
		setTextureFiltering(FO_LINEAR, FO_LINEAR, FO_LINEAR);
		break;
	case TFO_ANISOTROPIC:
		setTextureFiltering(FO_ANISOTROPIC, FO_ANISOTROPIC, FO_LINEAR);
		break;
	}
	mIsDefaultFiltering = false;
}
void BWTextureUnitState::setTextureScroll(float u, float v)
{
	mUMod = u;
	mVMod = v;
	mRecalcTexMatrix = true;
}
void BWTextureUnitState::setTextureTransform(const float *matrix)
{
	memcpy(mMatrix, matrix, 16 * sizeof(float));
	mRecalcTexMatrix = true;
}
void BWTextureUnitState::setTextureRotate(float angle)
{
	mRotate = angle;
	mRecalcTexMatrix = true;
}
void BWTextureUnitState::setTextureScale(float u, float v)
{
	mUScale = u;
	mVScale = v;
	mRecalcTexMatrix = true;
}
void BWTextureUnitState::setTextureMipmapBias(float bias)
{
	mMipmapBias = bias;
}
void BWTextureUnitState::setScrollAnimation(float uSpeed, float vSpeed)
{
	removeEffect(ET_USCROLL);
	removeEffect(ET_VSCROLL);
	removeEffect(ET_UVSCROLL);
	if (uSpeed == 0.0f && vSpeed == 0.0f)
	{
		return;
	}
	TextureEffect eff;
	if (uSpeed == vSpeed)
	{
		eff.type = ET_UVSCROLL;
		eff.arg1 = uSpeed;
	}
	else
	{
		if (uSpeed)
		{
			eff.type = ET_USCROLL;
			eff.arg1 = uSpeed;
			addEffect(eff);
		}
		if (vSpeed)
		{
			eff.type = ET_VSCROLL;
			eff.arg1 = vSpeed;
			addEffect(eff);
		}
	}
}
void BWTextureUnitState::setTextureAddressingMode(UVWAddressingMode mode)
{
	mAddressMode = mode;
}
void BWTextureUnitState::setTextureAddressingMode(TextureAddressingMode mode)
{
	mAddressMode.u = mode;
	mAddressMode.v = mode;
	mAddressMode.w = mode;
}
void BWTextureUnitState::setTextureCoordSet(unsigned int set)
{
	mTextureCoordSetIndex = set;
}
void BWTextureUnitState::setTextureBorderColour(const ColourValue &colour)
{
	mBorderColour = colour;
}
void BWTextureUnitState::setTextureAnisotropy(unsigned int maxAnio)
{
	mMaxAniso = maxAnio;
}
void BWTextureUnitState::addEffect(TextureEffect &effect)
{
	if (effect.type == ET_ENVIRONMENT_MAP
        || effect.type == ET_UVSCROLL
		|| effect.type == ET_USCROLL
		|| effect.type == ET_VSCROLL
		|| effect.type == ET_ROTATE
		|| effect.type == ET_PROJECTIVE_TEXTURE)
	{
		EffectMap::iterator itor = mEffects.find(effect.type);
		if (itor != mEffects.end())
		{
			/*if (itor->second->controll)
			{
			}*/
			mEffects.erase(itor);
		}
	}
	if (isLoad())
	{
		createEffectContorller(effect);
	}
	mEffects.insert(EffectMap::value_type(effect.type, effect));
}
void BWTextureUnitState::setIsAlpha(bool isAlpha) 
{
	mIsAlpha = isAlpha;
}

void BWTextureUnitState::setHardwareGammaEnable(bool enable)
{
	mHardwareGama = enable;
}
bool BWTextureUnitState::isHardwareGammaEnbale()
{
	return mHardwareGama;
}
void BWTextureUnitState::setDesiredFormat(PixelFormat desiredFormat)
{
	mDesireFormat = desiredFormat;
}

void BWTextureUnitState::setContentType(ContentType contentType)
{
	mContentType = contentType;
}
void BWTextureUnitState::setColourOpMultipassFallback(const SceneBlendFactor sourceFactor, const SceneBlendFactor destFactor)
{
	mColourBlendFallbackSrc = sourceFactor;
	mColourBlendFallbackDest = destFactor;
}
void BWTextureUnitState::setColourOperationEx(LayerBlendOperationEx op, LayerBlendSource src1, LayerBlendSource src2, const ColourValue& color1 /* = ColourValue::White */, const ColourValue& color2 /* = ColourValue::White */, float manualBlend /* = 0.0 */)
{
	mColourBlendMode.operation = op;
	mColourBlendMode.source1 = src1;
	mColourBlendMode.source2 = src2;
	mColourBlendMode.colourArg1 = color1;
	mColourBlendMode.colourArg2 = color2;
	mColourBlendMode.factor = manualBlend;
}
void BWTextureUnitState::setAnimatedTextureName(const std::string &name, unsigned int numFrame, float duration /* = 0.f */)
{
	
	std::string * imageSet = new std::string[numFrame];
	std::string image = name.substr(0 ,name.find('.'));
	std::string subfix = name.substr(name.find('.'), name.length() - name.find('.'));
	for (int i = 0; i < numFrame; i++)
	{
		imageSet[i] = image + "_" + StringConverter::ToString(i) + subfix;
	}
	setAnimatedTextureName(imageSet, numFrame, duration);
}
void BWTextureUnitState::setAnimatedTextureName(const std::string *name, unsigned int numFrame, float duration /* = 0.0f */)
{
	setContentType(CONTENT_NAMED);
	mTextureLoadFailed = false;
	mAnimationDuration = duration;
	mFramePtrs.resize(numFrame);
	mFrame.resize(numFrame);
	mCurrentFrame = 0;
	mCubic = false;

	for (int i = 0; i < numFrame; i++)
	{
		mFrame.push_back(name[i]);
		mFramePtrs[i].SetNull();
	}
	mAnimationDuration = duration;
	if (isLoad())
	{
		_load();
	}
}
void BWTextureUnitState::setCubicTextureName(const std::string &name, bool forUVW /* = false */)
{
	std::string * imageSet = new std::string[6];
	std::string image = name.substr(0, name.find('.'));
	std::string subfix = name.substr(name.find('.'), name.length() - name.find('.'));
	if (!forUVW)
	{
		imageSet[0] = image + "_fr" + subfix;
		imageSet[1] = image + "_bk" + subfix;
		imageSet[2] = image + "_up" + subfix;
		imageSet[3] = image + "_dn" + subfix;
		imageSet[4] = image + "_lf" + subfix;
		imageSet[5] = image + "_rt" + subfix;
	}
	else
	{
		for (int i = 0; i < 6; i++)
		{
			imageSet[i] = name;
		}
	}

	setCubicTextureName(imageSet, forUVW);
	delete[] imageSet;
}
void BWTextureUnitState::setCubicTextureName(const std::string *name, bool forUVW /* = false */)
{
	setContentType(CONTENT_NAMED);
	mTextureLoadFailed = false;
	mAnimationDuration = 0;
	mFramePtrs.resize(forUVW ? 1: 6);
	mFrame.resize(forUVW ? 1 : 6);
	mTextureType = forUVW ? TEX_TYPE_CUBE_MAP :TEX_TYPE_CUBE_MAP;
	mCurrentFrame = 0;
	mCubic = true;
	for (int i = 0; i < mFrame.size(); i++)
	{
		mFrame[i] = name[i];
		mFramePtrs[i].SetNull();
	}
	if (isLoad())
	{
		_load();
	}
}
void BWTextureUnitState::setTextureNameAlias(const std::string &name)
{
	mTextureNameAlias = name;
}
void BWTextureUnitState::createAnimContorller()
{

}
void BWTextureUnitState::createEffectContorller(TextureEffect &effect)
{

}

const BWTexturePtr&  BWTextureUnitState::_getTexturePtr() 
{
	return mFramePtrs[0];
}

BWMatrix4 BWTextureUnitState::getTextureTransform() const
{
	BWMatrix4 tmp = BWMatrix4::IDENTITY;
	return tmp;
}
unsigned int BWTextureUnitState::getTextureCoordSet() const
{
	return mTextureCoordSetIndex;
}
const BWTextureUnitState::EffectMap& BWTextureUnitState::getEffects() const
{
	return mEffects;
}
bool BWTextureUnitState::hasViewRelativeTextureCoordinateGeneration() const
{
	return false;
}
const ColourValue& BWTextureUnitState::getTextureBorderColour() const
{
	return mBorderColour;
}
const LayerBlendModeEx &  BWTextureUnitState::getAlphaBlendMode() const
{

	return mAlphaBlendMode;
}
const LayerBlendModeEx&  BWTextureUnitState::getColorBlendMode() const
{
	return mColourBlendMode;
}
BWTextureUnitState::BindingType BWTextureUnitState::getBindingType() 
{
	return mBindingType;
}
BWTextureUnitState::ContentType BWTextureUnitState::getContentType()
{
	return mContentType;
}
FilterOptions BWTextureUnitState::getTextureFiltering(FilterType type)
{
	switch (type)
	{
	case FT_MIN:
		return mIsDefaultFiltering ?
		BWMaterialManager::GetInstance()->GetDefaultTextureFiltering(FT_MIN) : mMinFilter;
	case FT_MAG:
		return mIsDefaultFiltering ?
			BWMaterialManager::GetInstance()->GetDefaultTextureFiltering(FT_MIN) : mMagFilter;
	case FT_MIP:
		return mIsDefaultFiltering ?
			BWMaterialManager::GetInstance()->GetDefaultTextureFiltering(FT_MIN) : mMipFilter;
	}
	return mMinFilter;
}
float BWTextureUnitState::getTextureMipmapBias()
{
	return mMipmapBias;
}

const BWTextureUnitState::UVWAddressingMode & BWTextureUnitState::getTextureAddressionMode() const 
{
	return mAddressMode;
}
unsigned int  BWTextureUnitState::getTextureAnisotropy() const
{
	return mIsDefaultAniso ? BWMaterialManager::GetInstance()->GetDefaultAnisotropy() : mMaxAniso;
}