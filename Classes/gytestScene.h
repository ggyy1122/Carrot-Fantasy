#pragma once
/*
≤‚ ‘”√≥°æ∞
*/

#ifndef __TEST_H__
#define __TEST_H__

#include "cocos2d.h"
USING_NS_CC;

class gytestScene : public Layer {
public:
	static Scene* createScene();
	virtual bool init();

	void TotestScene(Ref* pSender);
	void adjustTileMapToFitScreen();
	// implement the "static create()" method manually
	CREATE_FUNC(gytestScene);
};

#endif __TEST_H__