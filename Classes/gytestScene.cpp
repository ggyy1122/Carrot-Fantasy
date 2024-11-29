#include "gytestScene.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

// ��������
Scene* gytestScene::createScene()
{
    auto scene = Scene::create();  // ��������
    auto layer = gytestScene::create();  // ���� gytestScene ��
    scene->addChild(layer);  // ������ӵ�������
    return scene;
}

// ���� TileMap ������Ļ
void gytestScene::adjustTileMapToFitScreen() {
    // ��ȡ��Ļ��С
    auto screenSize = Director::getInstance()->getVisibleSize();
    // ���ص�ͼ
    auto tileMap = TMXTiledMap::create("map/map1.tmx");
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
        CCLOG("Failed to load map1.tmx.");
    }
}

// ��ʼ������
bool gytestScene::init()
{
    // ���ø���� init()
    if (!BaseLevelScene::init())  // �޸�Ϊ���û���� init
    {
        return false;
    }

    // ������ͼ��С
    adjustTileMapToFitScreen();

    // ��ȡ��Ļ��С
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // �������ֱ�ӵ��ø����еķ��ذ�ť����
    addBackButton();

    return true;
}