#pragma once
#ifndef __BASE_LEVEL_SCENE_H__
#define __BASE_LEVEL_SCENE_H__
#include "cocos2d.h"

USING_NS_CC;

//创建一个关卡基类
//注:基类只提供基础功能，子类负责场景的创建和管理，所以不再基类中提供场景创建的方法



//关卡ui部分的按钮参数
// 返回按钮的Z轴层级
#define BACK_BUTTON_ZORDER 2


class BaseLevelScene : public Scene {
public:
  

    // 初始化函数，重写父类的初始化方法
    virtual bool init() override;

    // 创建场景的宏
    CREATE_FUNC(BaseLevelScene);

    // 添加返回按钮的方法
    void addBackButton();

private:
    // 处理返回按钮点击事件
    void onBackButtonClicked(Ref* pSender);
};
#endif // __BASE_LEVEL_SCENE_H__