#include"menuScene.h"
#include"advantureScene.h"
#include"themeScene.h"
#include"music.h"
#include "ui/CocosGUI.h"
bool level_is_win[3] = { false };
bool is_newgame[3] = { false };
extern Music a;
USING_NS_CC;
using namespace ui;

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in seeScene.cpp\n");
}
void advanturescene::clearRelatedButtons() {
	auto sprite = this->getChildByTag(100);
	// ����ҵ��˾��飬�ʹ������ڸ��ڵ����Ƴ���
	if (sprite)
		this->removeChild(sprite);
	auto know1 = this->getChildByName("know");
	if (know1)
		this->removeChildByName("know");
}

bool advanturescene::init() {
	if (!Layer::init())
		return false;

	auto visibleSize = Director::getInstance()->getVisibleSize();//�ֱ��ʴ�С
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Size screenSize = Director::getInstance()->getWinSize();    //��ȡ��Ļ��С

	//�����ʾ��
	auto topLabel = Label::createWithSystemFont(StringUtils::format("Fuck you"), "Arial", 32);
	topLabel->setColor(Color3B::WHITE);
	topLabel->setPosition(screenSize.width * 0.49, screenSize.height * 0.95);
	this->addChild(topLabel, 2);
	//��ӱ���
	auto backgroundImage = Sprite::create("CarrotGuardRes/UI/AdvantureBackground.png");
	float scaleX = screenSize.width / backgroundImage->getContentSize().width;     //������ű���
	float scaleY = screenSize.height / backgroundImage->getContentSize().height;
	if (backgroundImage == nullptr)
		problemLoading("'AdvantureBackground.png'");
	else
	{
		backgroundImage->setScaleX(scaleX);
		backgroundImage->setScaleY(scaleY);
		backgroundImage->setPosition(Vec2(screenSize.width / 2 + origin.x, screenSize.height / 2 + origin.y));
		this->addChild(backgroundImage, 0);
	}
	
	//��ӷ��ذ�ť,��menu
	auto menu = Menu::create();
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	auto returnButton = MenuItemImage::create("CarrotGuardRes/UI/SettingReturnNormal.png", "CarrotGuardRes/UI/SettingReturnSelected.png", CC_CALLBACK_1(advanturescene::to_menuScene, this));
	if (returnButton == nullptr ||
		returnButton->getContentSize().width <= 0 ||
		returnButton->getContentSize().height <= 0)
	{
		problemLoading("'SettingReturnNormal.png' and 'SettingReturnSelected.png'");
	}
	else
	{
		float x = origin.x + returnButton->getContentSize().width / 2 + visibleSize.width * 0.05f;
		float y = origin.y + visibleSize.height - returnButton->getContentSize().width / 2 - visibleSize.height * 0.04f;
		returnButton->setScale(1.5);
		returnButton->setPosition(Vec2(x, y));
		menu->addChild(returnButton);
	}

	//������ҷ�ҳ��ť
   // ����PageView
	auto pageView = PageView::create();
	pageView->setContentSize(Size(screenSize.width, screenSize.height));
	pageView->setPosition(Vec2::ZERO);
	this->addChild(pageView);

	// �����ͼͼƬ����
	std::vector<std::string> mapImages = {
		"CarrotGuardRes/UI/advanture1.png",
		"CarrotGuardRes/UI/advanture2.png",
		 "CarrotGuardRes/UI/advanture3.png",
		 "CarrotGuardRes/UI/advanture4.png",
		 "CarrotGuardRes/UI/advanture5.png"
	};


	// ���ҳ��
	for (int i = 0; i < mapImages.size(); ++i) {
		Layout* layout = Layout::create();
		layout->setContentSize(Size(screenSize.width, screenSize.height));
		ImageView* imageView = ImageView::create(mapImages[i]);
		imageView->setContentSize(Size(screenSize.width, screenSize.height));
		imageView->setPosition(Vec2(layout->getContentSize().width / 2, layout->getContentSize().height / 2));
		imageView->setScale(0.6);  //����
		layout->addChild(imageView, 1);
		pageView->addPage(layout);
	}


	// ��Ӵ����¼�������
	pageView->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			a.button_music();
			PageView* pageView = dynamic_cast<PageView*>(pSender);
			int currentIndex = pageView->getCurrentPageIndex();
			Scene* themeScene;
			Size visibleSize;
			Vec2 origin;
			Sprite* boss1;
			Menu* menu1;
			MenuItemImage* know;
			// ����ҳ���л���ɺ���߼�
			switch (currentIndex) {
			case 0:
				themeScene=themescene::createScene();
				Director::getInstance()->pushScene(themeScene);
				break;
			case 1:
			case 2:
			case 3:
			case 4:
				visibleSize = Director::getInstance()->getVisibleSize();//�ֱ��ʴ�С
				origin = Director::getInstance()->getVisibleOrigin();
				//�����������
				boss1 = Sprite::create("CarrotGuardRes/UI/un_select.png");
				boss1->setTag(100);
				if (boss1 == nullptr)
					problemLoading("'un_select.png'");
				else
				{
					boss1->setScale(0.55);
					boss1->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
					this->addChild(boss1, 2);
				}

				menu1 = Menu::create(NULL);
				menu1->setPosition(Vec2::ZERO);
				this->addChild(menu1, 3);
				//��ӹرհ�ť
				know = MenuItemImage::create("CarrotGuardRes/UI/know.png", "CarrotGuardRes/UI/know.png", CC_CALLBACK_1(advanturescene::know_Callback, this));
				if (know == nullptr ||
					know->getContentSize().width <= 0 ||
					know->getContentSize().height <= 0)
				{
					problemLoading("'know.png'");
				}
				else
				{
					float x = origin.x + visibleSize.width / 1.8;
					float y = origin.y + visibleSize.width / 3.8;
					know->setScale(1);
					know->setPosition(Vec2(x, y));
					menu1->addChild(know);  //���뵽�˵���
				}
				break;
			default:
				break;
			}
		}
		});

	// ������ҷ�ҳ��ť
	auto leftButton = Button::create("CarrotGuardRes/UI/leftButtonNormal.png", "CarrotGuardRes/UI/leftButtonSelected.png");
	leftButton->setPosition(Vec2(screenSize.width * 0.1, screenSize.height / 2));
	leftButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			a.button_music();
			int currentIndex = pageView->getCurrentPageIndex();
			CCLOG("currentIndex:  %d", currentIndex);
			if (currentIndex > 0) 
				pageView->scrollToPage(currentIndex - 1);
			clearRelatedButtons();
		}
		});
	this->addChild(leftButton, 2);

	auto rightButton = Button::create("CarrotGuardRes/UI/rightButtonNormal.png", "CarrotGuardRes/UI/rightButtonSelected.png");
	rightButton->setPosition(Vec2(screenSize.width * 0.9, screenSize.height / 2));
	rightButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			a.button_music();
			int currentIndex = pageView->getCurrentPageIndex();
			if (currentIndex < 0)
				currentIndex = 0;  
			CCLOG("currentIndex:  %d", currentIndex);
			if (currentIndex < mapImages.size() - 1) 
				pageView->scrollToPage(currentIndex + 1);
			clearRelatedButtons();
		}
		});
	this->addChild(rightButton, 2);
	return true;
}

void advanturescene::to_menuScene(Ref* pSender) {
	a.button_music();
	Director::getInstance()->popScene();
}

void advanturescene::know_Callback(Ref* pSender) {
	auto sprite = this->getChildByTag(100);
	// ����ҵ��˾��飬�ʹ������ڸ��ڵ����Ƴ���
	if (sprite)
		this->removeChild(sprite);
	// ��ȡ��ť���ڵĲ˵������谴ť�ĸ��ڵ��ǲ˵�������ʵ���������������ͬ��
	auto menu = ((MenuItem*)pSender)->getParent();
	// �Ӳ˵����Ƴ���ť
	menu->removeChild((MenuItem*)pSender);
}