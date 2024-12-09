#include"menuScene.h"
#include"setScene.h"
#include"advantureScene.h"
#include"seeScene.h"
#include"music.h"
using namespace ui;
extern Music a;
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in menuScene.cpp\n");
}

bool MenuScene::init() {
    if (!Scene::init())
        return false;
    auto visibleSize = Director::getInstance()->getVisibleSize();//�ֱ��ʴ�С
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size screenSize = Director::getInstance()->getWinSize();    //��ȡ��Ļ��С
	
	//�ű���
	auto backgroundImage = Sprite::create("CarrotGuardRes/UI/BasicBackground.png");
	float scaleX = screenSize.width / backgroundImage->getContentSize().width;     //������ű���
	float scaleY = screenSize.height / backgroundImage->getContentSize().height;
	if (backgroundImage == nullptr)
		problemLoading("'BasicBackground.png'");
	else
	{
		backgroundImage->setScaleX(scaleX);
		backgroundImage->setScaleY(scaleY);
		backgroundImage->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
		this->addChild(backgroundImage, 0);
	}

	//��ӱ�����Ҷ��ͼƬ
	auto leaf1 = Sprite::create("CarrotGuardRes/UI/LeafBackground_1.png");
	if (leaf1 == nullptr)
		problemLoading("'LeafBackground_1.png'");
	else
	{
		leaf1->setScale(1.35f * scaleX);
		leaf1->setPosition(Vec2(visibleSize.width / 2 + origin.x - visibleSize.width * 0.06f, visibleSize.height / 2 + origin.y + visibleSize.height * 0.18f));
		this->addChild(leaf1, 0);
	}

	//��ӱ����м�Ҷ��ͼƬ
	auto leaf2 = Sprite::create("CarrotGuardRes/UI/LeafBackground_2.png");
	if (leaf2 == nullptr)
	{
		problemLoading("'LeafBackground_2.png'");
	}
	else
	{
		leaf2->setScale(1.5f * scaleX);
		leaf2->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + visibleSize.height * 0.20f));
		this->addChild(leaf2, 0);
	}

	//��ӱ�����Ҷ��ͼƬ
	auto leaf3 = Sprite::create("CarrotGuardRes/UI/LeafBackground_3.png");
	if (leaf3 == nullptr)
	{
		problemLoading("'LeafBackground_3.png'");
	}
	else
	{
		leaf3->setScale(scaleX);
		leaf3->setPosition(Vec2(visibleSize.width / 2 + origin.x + visibleSize.width * 0.075f, visibleSize.height / 2 + origin.y + visibleSize.height * 0.18f));
		this->addChild(leaf3, 0);
	}

	//��ӱ����ܲ�ͼƬ
	auto carrot = Sprite::create("CarrotGuardRes/UI/CarrotBackground.png");
	if (carrot == nullptr)
		problemLoading("'CarrotBackground.png'");
	else
	{
		carrot->setScale(1.5f * scaleX);
		carrot->setPosition(Vec2(visibleSize.width / 2 + origin.x - visibleSize.width * 0.005f, visibleSize.height / 2 + origin.y + visibleSize.height * 0.06f));
		this->addChild(carrot, 0);
	}

	//��ӱ�������ͼƬ
	auto NameImage = Sprite::create("CarrotGuardRes/UI/NameBackground.png");
	if (NameImage == nullptr)
		problemLoading("'NameBackground.png'");
	else
	{
		NameImage->setScale(1.6f * scaleX);
		NameImage->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - visibleSize.height * 0.055f));
		this->addChild(NameImage, 0);
	}

	// create menu, it's an autorelease object,��Ӳ˵�
	auto menu = Menu::create(NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);



	//��ӹرհ�ť
	auto close = MenuItemImage::create("CarrotGuardRes/UI/CloseNormal.png", "CarrotGuardRes/UI/CloseSelected.png", CC_CALLBACK_1(MenuScene::menuCloseCallback, this));
	if (close == nullptr ||
		close->getContentSize().width <= 0 ||
		close->getContentSize().height <= 0)
	{
		problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
	}
	else
	{
		float x = origin.x + visibleSize.width - close->getContentSize().width / 2;
		float y = origin.y + close->getContentSize().height / 2;
		close->setScale(scaleX);
		close->setPosition(Vec2(x, y));
		menu->addChild(close);  //���뵽�˵���

	}


	//���ð��ģʽ��ť
	auto advanture = MenuItemImage::create("CarrotGuardRes/UI/AdvantureButtonNormal.png", "CarrotGuardRes/UI/AdvantureButtonSelected.png", CC_CALLBACK_1(MenuScene::to_AdvantureScene, this));
	if (advanture == nullptr ||
		advanture->getContentSize().width <= 0 ||
		advanture->getContentSize().height <= 0)
	{
		problemLoading("'AdvantureButtonNormal.png' and 'AdvantureButtonSelected.png'");
	}
	else
	{
		float x = visibleSize.width * 0.125;
		float y = visibleSize.height * 0.15;
		advanture->setScale(0.9*1.5f * scaleX);
		advanture->setPosition(Vec2(x, y));
		menu->addChild(advanture);
	}

	//���bossģʽ��ť
	auto boss = MenuItemImage::create("CarrotGuardRes/UI/BossButtonNormal.png", "CarrotGuardRes/UI/BossButtonSelected.png", CC_CALLBACK_1(MenuScene::to_boss, this));
	if (boss == nullptr ||
		boss->getContentSize().width <= 0 ||
		boss->getContentSize().height <= 0)
	{
		problemLoading("'CarrotGuardRes/UI/BossButtonNormal.png'and 'CarrotGuardRes/UI/BossButtonSelected.png'");
	}
	else
	{
		float x = visibleSize.width * 0.375;
		float y = visibleSize.height * 0.14;
		boss->setScale(0.9*scaleX);
		boss->setPosition(Vec2(x, y));
		menu->addChild(boss);
	}

	//��ӹ�սģʽ��ť
	auto see = MenuItemImage::create("CarrotGuardRes/UI/WitnessButtonNormal.png", "CarrotGuardRes/UI/WitnessButtonSelected.png", CC_CALLBACK_1(MenuScene::to_seeScene, this));
	if (see == nullptr ||
		see->getContentSize().width <= 0 ||
		see->getContentSize().height <= 0)
	{
		problemLoading("'CarrotGuardRes/UI/NetButtonNormal.png'and 'CarrotGuardRes/UI/NetButtonSelected.png'");
	}
	else
	{
		float x = visibleSize.width * 0.625;
		float y = visibleSize.height * 0.15;
		see->setScale(0.9*1.5 * scaleX);
		see->setPosition(Vec2(x, y));
		menu->addChild(see);
	}

	//��ӹ����ݰ�ť
	auto wu = MenuItemImage::create("CarrotGuardRes/UI/NetButtonNormal.png", "CarrotGuardRes/UI/NetButtonNormal.png", CC_CALLBACK_1(MenuScene::to_boss, this));
	if (wu == nullptr ||
		wu->getContentSize().width <= 0 ||
		wu->getContentSize().height <= 0)
	{
		problemLoading("'CarrotGuardRes/UI/WitnessButtonNormal.png'and 'CarrotGuardRes/UI/WitnessButtonNormal.png'");
	}
	else
	{
		float x = visibleSize.width * 0.875;
		float y = visibleSize.height * 0.15;
		wu->setScale(0.9*scaleX);
		wu->setPosition(Vec2(x, y));
		menu->addChild(wu);
	}




	//���ý��水ť
	auto set = MenuItemImage::create("CarrotGuardRes/UI/SettingNormal.png", "SettingSelected.png", CC_CALLBACK_1(MenuScene::to_SetScene, this));
	if (set == nullptr||
		set->getContentSize().width <= 0 ||
		set->getContentSize().height <= 0)
	{
		problemLoading("'SettingNormal.png' and 'SettingSelected.png'");
	}
	else
	{
		set->setPosition(Vec2(visibleSize.width / 2 + origin.x - visibleSize.width * 0.3f, visibleSize.height / 2 + origin.y - visibleSize.height * 0.135f));
		set->setScale(1.7f * scaleX);
		menu->addChild(set);
	}
	return true;
}

// a selector callback
void MenuScene::menuCloseCallback(Ref* pSender) {
	a.button_music();
	Director::getInstance()->end();
}

//ȥ�ؿ�ѡ�����
void MenuScene::to_AdvantureScene(Ref* pSender) {
	a.button_music();
	auto advantureScene = advanturescene::createScene();
	Director::getInstance()->pushScene(advantureScene);
}

//ȥ���ý���
void MenuScene::to_SetScene(Ref* pSender) {
	a.button_music();
	auto settingScene = setscene::createScene();
	Director::getInstance()->pushScene(settingScene);
}

//���ǹ�ս�����Ȳ�Ū
void MenuScene::to_seeScene(Ref* pSender) {
	a.button_music();
	auto witnessScene = seescene::createScene();
	Director::getInstance()->pushScene(witnessScene);
}
void MenuScene::to_boss(Ref* pSender) {
	a.button_music();
	auto visibleSize = Director::getInstance()->getVisibleSize();//�ֱ��ʴ�С
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	//���boss����
	auto boss1 = Sprite::create("CarrotGuardRes/UI/un_select1.png");
	boss1->setTag(100);
	if (boss1 == nullptr)
		problemLoading("'un_select1.png'");
	else
	{
		boss1->setScale(0.8);
		boss1->setPosition(Vec2(visibleSize.width / 2 , visibleSize.height / 2 ));
		this->addChild(boss1,2);
	}
	auto menu1 = Menu::create(NULL);
	menu1->setPosition(Vec2::ZERO);
	this->addChild(menu1, 3);
	auto know = MenuItemImage::create("CarrotGuardRes/UI/know.png", "CarrotGuardRes/UI/know.png", CC_CALLBACK_1(MenuScene::know_Callback, this));
	if (know == nullptr ||
		know->getContentSize().width <= 0 ||
		know->getContentSize().height <= 0)
	{
		problemLoading("'know.png'");
	}
	else
	{
		float x = origin.x + visibleSize.width/1.8 ;
		float y = origin.y + visibleSize.width / 3.8;
		know->setScale(1);
		know->setPosition(Vec2(x, y));
		menu1->addChild(know);  //���뵽�˵���
	}
}

void MenuScene::know_Callback(Ref* pSender) {
	auto sprite = this->getChildByTag(100);
	// ����ҵ��˾��飬�ʹ������ڸ��ڵ����Ƴ���
	if (sprite)
		this->removeChild(sprite);
	auto menu = ((MenuItem*)pSender)->getParent();
	// �Ӳ˵����Ƴ���ť
	menu->removeChild((MenuItem*)pSender);
}