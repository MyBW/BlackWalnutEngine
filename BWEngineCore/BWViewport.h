#ifndef VIEWPORT_H_
#define VIEWPORT_H_

#include "BWRenderTarget.h"
#include "BWCamera.h"
#include "BWColorValue.h"
class BWViewport
{
	friend class BWRenderTarget;
	friend class BWRenderSystem;
public:
	BWViewport();
	BWViewport(BWCamera *ownor, BWRenderTarget *target, float left, float top, float width, float height, float zOrder);
	~BWViewport();
	bool isAutoUpdated();
	void update();
	int getActualWidth() const;
	int getActualHeight() const;
	int getActualLeft() const;
	int getActualTop() const;
	int getHeight() const;
	void _clearUpdateFlag();
	BWRenderTarget *getRenderTarget();
	bool getClearEveryFrame();
	unsigned int getClearFrameBuffer();
	const ColourValue &getBackGroundColour();
	void setMaterialScheme(const std::string &schemeName);
	const std::string& getMaterialScheme();

	bool _isUpdated() const;
	void _updateDimensions();
	BWCamera* getCamare() const { return mCamera; }
private:
	unsigned int _getNumRenderedFace();
	unsigned int _getNumRenderedBatches();
	int Zorder;
	bool mShowOverlays;
	BWRenderTarget* mRenderTarget;
	BWCamera* mCamera;
	float mRelLeft;
	float mRelTop;
	float mRelWidth;
	float mRelHeight;
	float mZOrder;
	std::string mMaterialSchemeName;
	int mActLeft, mActTop, mActWidth, mActHeight;
	bool mUpdate;
};


#endif