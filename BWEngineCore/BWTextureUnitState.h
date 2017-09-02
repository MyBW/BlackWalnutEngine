#ifndef TEXTUREUNITSTATE_H_
#define TEXTUREUNITSTATE_H_
#include "BWCommon.h"
#include "BWTexture.h"
#include "OgreBlendMode.h"
class BWFrustum;
class BWPass;
class BWTextureUnitState
{
public:
	enum TextureEffectType
	{
		ET_ENVIRONMENT_MAP ,
		ET_PROJECTIVE_TEXTURE,
		ET_UVSCROLL,
		ET_USCROLL ,
		ET_VSCROLL,
		ET_ROTATE ,
		ET_TRANSFORM
	};
	enum EnvMapType
	{
		ENV_PLANAR ,
		ENV_CURVED,
		ENV_REFLECTION,
		ENV_NORMAL
	};
	enum TextureTransformType
	{
		TT_TRANSLATE_U ,
		TT_TRANSLATE_V ,
		TT_SCALE_U,
		TT_SCALE_V ,
		TT_ROTATE
	};
	enum TextureAddressingMode
	{
		TAM_WRAP ,
		TAM_MIRROR ,
		TAM_CLAMP ,
		TAM_BORDER
	};
	struct UVWAddressingMode 
	{
		TextureAddressingMode u, v, w;
	};
	enum TextureCubeFace
	{
		CUBE_FRONT = 0 ,
		CUBE_BACK = 1,
		CUBE_LEFT = 2 ,
		CUBE_RIGHT = 3 , 
		CUBE_UP = 4 ,
		CUBE_DOWN = 5
	};
	struct TextureEffect 
	{
		TextureEffectType type;
		int subtype;
		float arg1, arg2;
		WaveformType waveType;
		float base;
		float frequency;
		float phase;
		float amplitude;
		// controller;
		BWFrustum *frustum ;
	};
	typedef std::multimap<TextureEffectType, TextureEffect> EffectMap;
	BWTextureUnitState(BWPass *parent);
	BWTextureUnitState(BWPass *parent, const BWTextureUnitState &oth);
	BWTextureUnitState & operator=(const BWTextureUnitState &oth);
	virtual ~BWTextureUnitState();
	BWTextureUnitState(BWPass *parent, const std::string &textureName, unsigned int texCoordSet = 0);

	const std::string & getTextureName();
	void setTextureName(const std::string &name, TextureType type = TEX_TYPE_2D);
	void setCubicTextureName(const std::string &name, bool forUVW = false);
	void setCubicTextureName(const std::string *name, bool forUVW = false);
	void setAnimatedTextureName(const std::string &name, unsigned int numFrame, float duration = 0.f);
	void setAnimatedTextureName(const std::string *name, unsigned int numFrame, float duration = 0.0f);
	std::pair<size_t, size_t> getTextureDimensions(unsigned int frame = 0) const;
	void setCurrentFrame(unsigned int frameNumber);
	unsigned int getCurrentFrame() const;
	const std::string & getFramTextureName(unsigned int frameNumber) const;
	void setFrameTextureNmae(const std::string &name, unsigned int frameNum);
	void addFrameTextureName(const std::string &name);
	void deleteFrameTextureName(const size_t frameNum);
	unsigned int getNumFrames() const;
	BWMatrix4 getTextureTransform() const ;
	enum BindingType
	{
		BT_FRAGMENT = 0 ,
		BT_VERTEX = 1
	};
	enum ContentType
	{
		CONTENT_NAMED = 0 ,
		CONTENT_SHADOW = 1,
		CONTENT_COMPOSITOR =2
	};
	void setBindingType(BindingType bindingType);
	BindingType getBindingType();
	void setContentType(ContentType contentType);
	ContentType getContentType();
	bool isCubic();
	bool is3D();
	TextureType GetTextureType();
	void setDesiredFormat(PixelFormat desiredFormat);
	PixelFormat getDesiredFormat();
	void setNumMipmaps(int num){ mTextureSrcMipmap = num; }
	int getNumMipmaps();
	void setIsAlpha(bool isAlpha) ;
	void setHardwareGammaEnable(bool enable);
	bool isHardwareGammaEnbale();
	void setTextureCoordSet(unsigned int set);
	void setTextureTransform(const float *matrix);
	void setTextureScroll(float u, float v);

	void setTextureUScroll(float value);
	float getTextureUScroll();
	void setTextureVScroll(float value);
	float getTextureVScroll();
	void setTextureUScale(float value);
	float getTextureUSacle();
	void setTextureVScale(float value);
	float getTextureVScale();
	void setTextureScale(float u, float v);

	void setTextureRotate(float angle);
	float getTextureRotate();
	const UVWAddressingMode & getTextureAddressionMode() const;
	void setTextureAddressingMode(TextureAddressingMode mode);
	void setTextureAddressingMode(UVWAddressingMode mode);

	void setAlphaOperation(LayerBlendOperationEx op, LayerBlendSource src1, LayerBlendSource src2, const ColourValue& arg1, const ColourValue& arg2, float factor);
	void setTextureBorderColour(const ColourValue &colour);
	const ColourValue& getTextureBorderColour() const;
	void setColourOperation(const LayerBlendOperation op);
	void setColourOperationEx(LayerBlendOperationEx op, LayerBlendSource src1, LayerBlendSource src2, const ColourValue& color1 = ColourValue::White, const ColourValue& color2 = ColourValue::White, float manualBlend = 0.0);
	void setColourOpMultipassFallback(const SceneBlendFactor sourceFactor, const SceneBlendFactor destFactor);
	const LayerBlendModeEx & getColorBlendMode() const;
	const LayerBlendModeEx & getAlphaBlendMode() const;
	SceneBlendFactor getColourBlendFallbackSrc() const;
	SceneBlendFactor getColourBlendFallbackDes() const;
	void addEffect(TextureEffect &effect);
	void setEnvironmentMap(bool enable, EnvMapType envMapType = ENV_CURVED);
	void setScrollAnimation(float uSpeed, float vSpeed);
	void setRotateAnimation(float speed);
	void setTransformAnimation(const TextureTransformType type,
		const WaveformType waveType, float base = 0, float frequency = 1, float phase = 0, float amplitude = 1);
	//void setProjectiveTexturing(bool enable, const Frustum * progectionSetting = 0);
	void removeAllEffects();
	void removeEffect(const TextureEffectType effectType);
	void isBlank() const;
	void setBlank() const;
	bool isTextureLoadFailing() const;
	void retryTextureLoad();
	const EffectMap& getEffects() const;
	void setTextureFiltering(TextureFilterOptions filterType);
	void setTextureFiltering(FilterType filterType, FilterOptions opts);
	void setTextureFiltering(FilterOptions minfilter, FilterOptions magfilter, FilterOptions mipfilter);
	FilterOptions getTextureFiltering(FilterType type);
	void setTextureAnisotropy(unsigned int maxAnio);
	unsigned int getTextureAnisotropy() const;
	void setTextureMipmapBias(float bias);
	float getTextureMipmapBias();

	void  setCompositionReference(const std::string &compositionName, const std::string & textureName, size_t mrtIndex = 0);
	const std::string &getReferencedCompositorName() const;
	const std::string & getReferencedTextureName() const;
	size_t getReferencedMRTIndex() const;
	BWPass * getParent();
	FORCEINLINE int GetIndex() const { return mIndex; }
	FORCEINLINE void SetIndex(int Index) { mIndex = Index; }

	void _prepare();
	void _unprepare();
	void _load();
	void _unload();
	bool hasViewRelativeTextureCoordinateGeneration() const;
	bool isLoad();
	void setName(const std::string &name);
	const std::string &getName();

	void setTextureNameAlias(const std::string &name);
	const std::string & getTextureAlias();
	bool applyTextureAliases(const AliasTextureNamePairList& aliasList, const bool apply = true);
	const BWTexturePtr & _getTexturePtr();
	const BWTexturePtr & _getTexturePtr(size_t frame) const;
	void _setTexturePtr(const BWTexturePtr &texturePtr);
	void _setTexturePtr(const BWTexturePtr &texturePtr, size_t frame);
	unsigned int getTextureCoordSet() const;
protected:
	unsigned int mCurrentFrame;
	float mAnimationDuration;
	bool mCubic;
	TextureType mTextureType;
	PixelFormat mDesireFormat;
	int mTextureSrcMipmap;
	unsigned int mTextureCoordSetIndex;
	UVWAddressingMode mAddressMode;
	ColourValue mBorderColour;
	bool mIsAlpha;
	LayerBlendModeEx mColourBlendMode;
	SceneBlendFactor mColourBlendFallbackDest; // 这两者是当不支持多重纹理的时候 使用多次渲染pass的方式来模拟 这时就是场景混合了
	SceneBlendFactor mColourBlendFallbackSrc;

	LayerBlendModeEx mAlphaBlendMode;
	mutable bool mRecalcTexMatrix;
	float mUMod, mVMod;
	float mUScale, mVScale;
	float mRotate;
	mutable float mMatrix[16];

	FilterOptions mMinFilter;
	FilterOptions mMagFilter;
	FilterOptions mMipFilter;
	unsigned int mMaxAniso;
	float mMipmapBias;

	bool mIsDefaultAniso;
	bool mIsDefaultFiltering;
	BindingType mBindingType;
	ContentType mContentType;
	size_t mCompositorRefMrtIndex;

	std::vector<std::string> mFrame;
	mutable std::vector<BWTexturePtr> mFramePtrs;
	std::string mName;
	std::string mTextureNameAlias;
	bool mTextureLoadFailed;

	EffectMap mEffects;
	std::string mCompositorRefName;
	std::string mCompositorRefTextureName;
	bool mHardwareGama;
	BWPass *mParent;
	int mIndex; // 在Pass中的Index
	void recalcTextureMatrix();
	void createAnimContorller();
	void createEffectContorller(TextureEffect &effect);
	void ensurePrepare(size_t frame) const;
	void ensureLoaded(size_t frame) const;
private:
};

#endif