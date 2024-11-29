
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
	//调用父类的init
	if (!Layer::init())
		return false;
	//获取屏幕大小
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	//添加一个背景
    auto background = Sprite::create("map/map1.png");
    background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(background, -1); // 背景 zOrder=-1

    // 添加一个测试标签
    auto label = Label::createWithSystemFont("Test Scene", "Arial", 36);
    label->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height - 50));
    this->addChild(label, 1); // 标签 zOrder=1

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