#pragma once
#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__
#include "cocos2d.h"
USING_NS_CC;

class MenuScene : public Scene  //����hellowordScene
{
public:
	static Scene* createScene() {
		return MenuScene::create();
	};
	virtual bool init();
	// a selector callback
	void menuCloseCallback(Ref* pSender);
	//ȥ�ؿ�ѡ�����
	void to_AdvantureScene(Ref* pSender);
	//ȥ���ý���
	void to_SetScene(Ref* pSender);
	//���ǹ�ս�����Ȳ�Ū
	void to_seeScene(Ref* pSender);
	//ȥboss
	void to_boss(Ref* pSender);
	void know_Callback(Ref* pSender);
	// implement the "static create()" method manually
	CREATE_FUNC(MenuScene);
};

#endif // __MENU_SCENE_H__

