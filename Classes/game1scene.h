#pragma once
#ifndef __GAME1_SCENE_H__
#define __GAME1_SCENE_H__
#include "cocos2d.h"
USING_NS_CC;
using namespace ui;
class game1scene : public Layer  //¿‡À∆hellowordScene
{
public:
	static Scene* createScene();
	virtual bool init();
	// a selector callback
	void menuCloseCallback(Ref* pSender);





	// implement the "static create()" method manually
	CREATE_FUNC(game1scene);
};

#endif // __GAME1_SCENE_H__

