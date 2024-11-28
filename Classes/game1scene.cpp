#include"game1scene.h"
#include"music.h"
USING_NS_CC;
using namespace ui;

Scene* game1scene::createScene() {
   

	auto scene = Scene::create();
	auto layer = game1scene::create();
	scene->addChild(layer);
	return scene;
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in game1scene.cpp\n");
}

bool game1scene::init() {
	if (!Layer::init())
		return false;
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    Size screenSize = Director::getInstance()->getWinSize();//获取屏幕大小


    // 加地图1
    auto sprite = Sprite::create("map/map1.png");
    float scaleX = screenSize.width / sprite->getContentSize().width;
    float scaleY = screenSize.height / sprite->getContentSize().height;
    if (sprite == nullptr)
    {
        problemLoading("'map/map1.png'");
    }
    else
    {
        sprite->setScaleX(scaleX);
        sprite->setScaleY(scaleY);
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }




























}
//关闭按钮
void game1scene::menuCloseCallback(Ref* pSender) {
    Music a;
    a.button_music();
    Director::getInstance()->end();
}

