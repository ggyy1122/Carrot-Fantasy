#pragma once
/*
≤‚ ‘”√≥°æ∞
*/

#ifndef __TEST_H__
#define __TEST_H__

#include "cocos2d.h"
USING_NS_CC;

class testScene : public Layer {
public:
	static Scene* createScene();
	virtual bool init();

	void TotestScene(Ref* pSender);
	// implement the "static create()" method manually
	CREATE_FUNC(testScene);
};

#endif __TEST_H__