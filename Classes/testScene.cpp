
#include"testScene.h"
#include "ui/CocosGUI.h"
using namespace ui;

static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
}
Scene* testScene::createScene()
{
	auto scene = Scene::create();
	auto layer = testScene::create();
	scene->addChild(layer);
	return scene;
}
bool testScene::init()
{
	//���ø����init
	if (!Layer::init())
		return false;
	//��ȡ��Ļ��С
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	//���һ������
    auto background = Sprite::create("map/map1.png");
    background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(background, -1); // ���� zOrder=-1

    // ���һ�����Ա�ǩ
    auto label = Label::createWithSystemFont("Test Scene", "Arial", 36);
    label->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height - 50));
    this->addChild(label, 1); // ��ǩ zOrder=1

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