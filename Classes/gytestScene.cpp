
#include"gytestScene.h"
#include "ui/CocosGUI.h"
#include<cocos2d.h>
using namespace ui;



static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
}
Scene* gytestScene::createScene()
{
	auto scene = Scene::create();
	auto layer = gytestScene::create();
	scene->addChild(layer);
	return scene;
}

void gytestScene::adjustTileMapToFitScreen() {
    // 获取屏幕大小
    auto screenSize = Director::getInstance()->getVisibleSize();
    // 加载地图
    auto tileMap = TMXTiledMap::create("map/map3.tmx");
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
        CCLOG("Failed to load map2.tmx.");
    }
}
// 初始化方法
bool gytestScene::init()
{
    // 调用父类的 init()
    if (!Layer::init())
    {
        return false;
    }
    gytestScene::adjustTileMapToFitScreen();


    // 获取屏幕大小
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // 添加一个测试按钮
    auto button = MenuItemLabel::create(
        Label::createWithSystemFont("Back", "Arial", 24),
        [](Ref* sender) {
            Director::getInstance()->popScene(); // 返回上一场景
        });
    button->setPosition(Vec2(visibleSize.width / 2, 50));

    auto menu = Menu::create(button, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 2); // 菜单 zOrder=2

    return true;
}