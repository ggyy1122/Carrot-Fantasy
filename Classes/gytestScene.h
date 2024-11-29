#pragma once

#include "BaseLevelScene.h"  // 包含基类头文件
#include "cocos2d.h"
class gytestScene : public BaseLevelScene {  // 修改为继承自 BaseLevelScene
public:
    static Scene*createScene();            //创建场景的静态方法
    virtual bool init() override;          // 初始化方法，重写自 BaseLevelScene
    void TotestScene(Ref* pSender);        // 场景跳转的函数
    void adjustTileMapToFitScreen();       // 调整 TileMap 适配屏幕的函数

    CREATE_FUNC(gytestScene);  // 自动生成 create 方法的宏
};

