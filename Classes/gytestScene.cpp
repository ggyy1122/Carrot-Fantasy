
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
    // ��ȡ��Ļ��С
    auto screenSize = Director::getInstance()->getVisibleSize();
    // ���ص�ͼ
    auto tileMap = TMXTiledMap::create("map/map3.tmx");
    if (tileMap) {
        // ��ȡ��Ƭ��ͼ��ԭʼ�ߴ�
        auto tileMapSize = tileMap->getContentSize();

        // ������Ƭ��ͼ��ʹ������������Ļ
       tileMap->setScaleX(screenSize.width / tileMapSize.width);
        tileMap->setScaleY(screenSize.height / tileMapSize.height);

        // ������Ƭ��ͼ��ê��Ϊ (0, 0)��ʹ��ͼ���½�����Ļ���½Ƕ���
        tileMap->setAnchorPoint(Vec2::ZERO);
        tileMap->setPosition(Vec2::ZERO);  // ����ͼ��������Ļ�����½�

        // ����Ƭ��ͼ��ӵ���ǰ������
        this->addChild(tileMap, 1);
    }
    else {
        CCLOG("Failed to load map2.tmx.");
    }
}
// ��ʼ������
bool gytestScene::init()
{
    // ���ø���� init()
    if (!Layer::init())
    {
        return false;
    }
    gytestScene::adjustTileMapToFitScreen();


    // ��ȡ��Ļ��С
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // ���һ�����԰�ť
    auto button = MenuItemLabel::create(
        Label::createWithSystemFont("Back", "Arial", 24),
        [](Ref* sender) {
            Director::getInstance()->popScene(); // ������һ����
        });
    button->setPosition(Vec2(visibleSize.width / 2, 50));

    auto menu = Menu::create(button, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 2); // �˵� zOrder=2

    return true;
}