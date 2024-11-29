#include "gytestScene.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

// 创建场景
Scene* gytestScene::createScene()
{
    auto scene = Scene::create();  // 创建场景
    auto layer = gytestScene::create();  // 创建 gytestScene 层
    scene->addChild(layer);  // 将层添加到场景中
    return scene;
}

// 调整 TileMap 适配屏幕
void gytestScene::adjustTileMapToFitScreen() {
    // 获取屏幕大小
    auto screenSize = Director::getInstance()->getVisibleSize();
    // 加载地图
    auto tileMap = TMXTiledMap::create("map/map1.tmx");
    if (tileMap) {
        // 获取瓦片地图的原始尺寸
        auto tileMapSize = tileMap->getContentSize();

        // 缩放瓦片地图，使其填满整个屏幕
        tileMap->setScaleX(screenSize.width / tileMapSize.width);
        tileMap->setScaleY(screenSize.height / tileMapSize.height);

        // 设置瓦片地图的锚点为 (0, 0)，使地图左下角与屏幕左下角对齐
        tileMap->setAnchorPoint(Vec2::ZERO);
        tileMap->setPosition(Vec2::ZERO);  // 将地图放置在屏幕的左下角

        // 将瓦片地图添加到当前场景中
        this->addChild(tileMap, 1);
    }
    else {
        CCLOG("Failed to load map1.tmx.");
    }
}

// 初始化方法
bool gytestScene::init()
{
    // 调用父类的 init()
    if (!BaseLevelScene::init())  // 修改为调用基类的 init
    {
        return false;
    }

    // 调整地图大小
    adjustTileMapToFitScreen();

    // 获取屏幕大小
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // 这里可以直接调用父类中的返回按钮函数
    addBackButton();

    return true;
}