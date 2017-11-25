#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include <list>
#include <map>
#include "BWAxisAlignedBox.h"
#include "BWRenderQueue.h"
#include"BWLight.h"
class BWSceneNode;
class BWCamera;
class BWAutoParamDataSource;
class BWRenderSystem;
class BWViewport;
class BWSceneManager;
class BWPointLight;
class BWDirectionalLight;
typedef std::map<std::string ,BWLight*> LightList;
typedef std::map<std::string, BWLight*> LightMap;
typedef std::map<std::string, BWPointLight*> PointLightMap;
typedef std::map<std::string, BWDirectionalLight*> DirectionalLightMap;
typedef std::map<std::string, BWRenderQueue*> ObjectsQueneWithInLightMap;
enum IlluminationRenderStage
{
	IRS_NONE,
	IRS_RENDER_TO_TEXTURE,
	IRT_RENDER_RECEIVER_PASS
};
struct VisibleObjectsBoundsInfo
{
	AxisAlignedBox aabb;
	AxisAlignedBox receiverAabb;
	float minDistance;
	float maxDistance;
	float minDistanceInFrustum;
	float maxDistanceInFrustum;
	void reset();
	//void merge(const AxisAlignedBox &boxBound , const )
	//void mergeNonRenderedButInFrustum(const AxisAlignedBox &boxBounds , )
};
class BWSceneManagerRenderableVisitor : public BWQueueRenderableVisitor
{
public:
	BWSceneManagerRenderableVisitor();
	~BWSceneManagerRenderableVisitor();
	bool visitor(const BWPass *pass);
	void visitor( BWRenderable *renderable);
	void visitor(RenderablePass *renderablePass);
	void setTargetSceneManager(BWSceneManager *seneManager){ mTargetSceneManager = seneManager; }
	bool autoLight;
	const LightList *manualLightList;
	bool scissoring;
private: 
	BWSceneManager *mTargetSceneManager;
	const BWPass *mUsePass;
};
class BWSceneManager
{
	friend class BWCamera;
public:
	typedef std::vector<BWSceneNode*> AutoTrackingSceneNodes;
	typedef std::map<const BWCamera*, VisibleObjectsBoundsInfo> CamVisibleObjectsMap;
	typedef std::map<std::string, BWCamera*> CameraMap;


	BWSceneManager();
	virtual ~BWSceneManager();

	bool isShadowTechniqueTextureBased() const;
	const VisibleObjectsBoundsInfo& getVisibleObjectsBoundsInfo(BWCamera * camera) const;
	const ColourValue & getShadowColour() const;
	void setViewport(BWViewport *viewport);
	void prepareRenderQueue();
	void setViewMatrix(const BWMatrix4 &matrix);
	void firepreFindVisibleObject(BWViewport* viewport);
	void firePostFindVisibleObject(BWViewport *viewport);
	BWSceneNode *createSceneNode();
	bool getShowBoundingBox();
	BWSceneNode* getRootSceneNode();
	BWRenderQueue* getRenderQueue();
	const BWPass* setPass(const BWPass *pass);
	virtual void firePreRenderQueue();
    virtual	void firePostRenderQueue();
    bool firePreRenderQueueGroupStarted(unsigned char renderGroupID);
	bool firePostRenderQueueGroupEnded(unsigned char renderGroupID);
	bool isRenderQueueGroupToBeProcessed(unsigned char goupID);
	void renderBasicQueueGroupObject(BWRenderQueueGroup* renderQueueGroup, BWQueueRenderableCollection::OrganisationMode organisationMode);
	void renderObject(BWQueueRenderableCollection& renderableCollection, BWQueueRenderableCollection::OrganisationMode OM, bool lightScissoringClipping, bool doLightItertion, const LightList* manualLightList = NULL);
	bool validatePassForRendering(const BWPass *pass);
	bool validateRenderableForRendering(const BWPass *pass ,const BWRenderable *renderable);
	virtual bool fireRenderSingleObject(const BWRenderable *renderable, const BWPass *pass, const BWAutoParamDataSource *autoParamDataSource,const LightList* manualLightList ,  bool suppressRenderStateChanges);
	void renderSingleObject( BWRenderable *renderable, const BWPass *pass, bool scissoring,bool autoLight, const LightList* manualLightList);
	void useRenderableViewProjMode(const BWRenderable *renderable, bool fixedFuntion);
	virtual void resetViewProjMode(bool fixedFunction);
	virtual BWCamera* createCamera(const std::string &name);
	BWAutoParamDataSource * getAutoParamDataSource() { return mAutoParamDataSource; }

	void AddDirectLight(BWDirectionalLight* DirectionalLight);
	void AddPointLight(BWPointLight *PointLight);
	LightList& getActiveLightList() { return mActiveLightList; }
	DirectionalLightMap& GetActiveDirectionalLights()  { return ActiveDirectionalLightMap; }
	PointLightMap&  GetActivePointLights() { return ActivePointLihgtMap; }
	

    BWLight*  CreateLight(const std::string& name , BWLight::LightType LightType = BWLight::LightType::LT_DIRECTIONAL);
	BWDirectionalLight* CreateDirectionalLight(const std::string &name);
	BWPointLight* CreatePointLight(const std::string &name); 

	BWRenderQueue* GetObjectsQueueWithinLight(const std::string & LightName);
	bool CreateObjectsQueueWithinLight(const std::string &name);
	ObjectsQueneWithInLightMap& GetObjectQueueWithinLightMap() { return ObjectsQuenesWithInLight; }
private:
	void _renderScene(BWCamera *camera, BWViewport *viewpotr, bool isShowOverlay);
	void _updateSceneGraph(BWCamera *camera);
	void SetupUniformBuffer();
	void _findVisibleObjects(BWFrustum *camera, VisibleObjectsBoundsInfo *visibelBoundInfor, bool isrenderToTexture);
	void _renderVisibleObjec();
	void _renderQueueGroupObject(BWRenderQueueGroup* renderQueueGroup, BWQueueRenderableCollection::OrganisationMode organisationMode);
	virtual BWSceneNode* createSceneNodeImp(const std::string &name);
	virtual BWSceneNode* createSceneNodeImp();
	virtual void renderVisibleObjectsDefaultsSequence();

	IlluminationRenderStage mIlluminationStage;
	CameraMap mCameraMap;
	BWCamera *mActiveCamera;
	BWViewport *mCurrentViewport;
	BWRenderSystem *mActiveRenderSystem;
	AutoTrackingSceneNodes mAutoTrackingSceneNodes;
	BWAutoParamDataSource *mAutoParamDataSource;
	//真的渲染队列 最终渲染到GBuffer上数据
	BWRenderQueue* renderQueue;
	//在光源内的对象的集合 每一个光源有一个
	ObjectsQueneWithInLightMap ObjectsQuenesWithInLight;

	bool mCameraRelativeRendering;
	float mShadowDirLightExtrudeDist;
	ColourValue mAmbientLight;
	bool mFindVisibleObject;
	CamVisibleObjectsMap mCamVisibleObjectMap;
	// 该矩阵中保存了每一帧渲染时的矩阵  因为在渲染每个物体的时候 viewMatrix可能会变 所以这里缓存一下
	BWMatrix4 mCachedViewMatrix;
	// 是否启用相对于摄像机的渲染  这个是做什么用的??
	BWVector3 mCameraRelativePosition;
	BWSceneManagerRenderableVisitor *mAcitiveSceneManagerRenderableVisitor;

	unsigned short mGpuParamsDirty;
	//是否禁止渲染状态改变
	bool mSuppressRenderStateChanges;
	//当缩放值为负值时  裁剪模式（顺、逆时针） 是否跟着改变
	bool mFlipCullingOnNegativeScale;
	//当存在缩放时 要法线是否归一
	bool mNormaliseNormalsOnScale;
	CullingMode mPassCullingMode;
	BWMatrix4 mTempXform[256];
	bool mResetIdentityView; // 是否将视图矩阵重置为单位矩阵
	bool mResetIdentityProj; // 是否将投影矩阵重置为单位矩阵

	BWSceneNode *mRootSceneNode;
	typedef std::list<BWSceneNode*> SceneNodeList;
	SceneNodeList mSceneNodeList; // 这个只是用于内存管理的  mRootSceneNode 才是管理场景的

	LightList mAllLight; // 当前场景中所有的灯光
	LightList mActiveLightList;// 当前用于渲染的灯光 
	PointLightMap ActivePointLihgtMap;
	DirectionalLightMap ActiveDirectionalLightMap;
	



	




};




#endif