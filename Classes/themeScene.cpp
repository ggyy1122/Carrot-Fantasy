#include"advantureScene.h"
#include"themeScene.h"
#include"music.h"
#include "ui/CocosGUI.h"
#include"BaseLevelScene.h"
#include "json/document.h"
#include "json/rapidjson.h"
extern Music a;
USING_NS_CC;
using namespace ui;
// 假设这里有一个全局变量或者成员变量来存储页面相关内容，比如是否可操作等状态标识
    // 具体含义需根据项目实际情况确定，这里只是示例

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in themeScene.cpp\n");
}


// 函数用于清理与特定操作相关的按钮（如确认、取消、锁定按钮）
   // 避免在多处重复相同的清理逻辑代码
void themescene::clearRelatedButtons() {
    // 获取名为"confirmButton"的子节点
    auto confirm = this->getChildByName("confirmButton");
    // 获取名为"cancelButton"的子节点
    auto cancel = this->getChildByName("cancelButton");
    // 获取名为"lockedButton"的子节点
    auto locked = this->getChildByName("lockedButton");
    // 如果确认按钮和取消按钮存在，就从当前节点移除它们
    if (confirm && cancel) {
        this->removeChildByName("confirmButton");
        this->removeChildByName("cancelButton");
    }
    // 如果锁定按钮存在，就从当前节点移除它
    if (locked)
        this->removeChildByName("lockedButton");
}


bool themescene::init() {
    if (!Layer::init())
        return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();//分辨率大小
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size screenSize = Director::getInstance()->getWinSize();    //获取屏幕大小

	//添加提示词
	auto topLabel = Label::createWithSystemFont(StringUtils::format("Fuck you"), "Arial", 32);
	topLabel->setColor(Color3B::WHITE);
	topLabel->setPosition(screenSize.width * 0.49, screenSize.height * 0.95);
	this->addChild(topLabel, 2);
	//添加背景
	auto backgroundImage = Sprite::create("CarrotGuardRes/UI/AdvantureBackground.png");
	float scaleX = screenSize.width / backgroundImage->getContentSize().width;     //获得缩放倍数
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
	
	auto menu = Menu::create();
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	//添加返回按钮
	auto returnButton = MenuItemImage::create("CarrotGuardRes/UI/ReturnButtonNormal.png", "CarrotGuardRes/UI/ReturnButtonSelected.png", CC_CALLBACK_1(themescene::to_advantureScene, this));
	if (returnButton == nullptr ||
		returnButton->getContentSize().width <= 0 ||
		returnButton->getContentSize().height <= 0)
	{
		problemLoading("'ReturnButtonNormal.png' and 'ReturnButtonSelected.png'");
	}
	else
	{
		float x = origin.x + returnButton->getContentSize().width / 2 + screenSize.width * 0.05f;
		float y = origin.y + screenSize.height - returnButton->getContentSize().width / 2 - screenSize.height * 0.04f;
		returnButton->setPosition(Vec2(x, y));
		returnButton->setScale(3.0f * scaleX);
		menu->addChild(returnButton);
	}

	//添加左右翻页按钮
	// 创建PageView
	auto pageView = PageView::create();
	pageView->setContentSize(Size(screenSize.width, screenSize.height));
	pageView->setPosition(Vec2::ZERO);
	this->addChild(pageView);

	// 定义地图图片数组
	std::vector<std::string> mapImages = {
		"CarrotGuardRes/UI/SkyLineLevel_1.png",
		"CarrotGuardRes/UI/SkyLineLevel_2.png",
		"CarrotGuardRes/UI/SkyLineLevel_3.png",
		"CarrotGuardRes/UI/SkyLineLevel_4.png"
	};

	// 添加页面
	for (int i = 0; i < mapImages.size(); ++i) {
		Layout* layout = Layout::create();
		layout->setContentSize(Size(screenSize.width, screenSize.height));
		ImageView* imageView = ImageView::create(mapImages[i]);
		imageView->setContentSize(Size(screenSize.width, screenSize.height));
		imageView->setPosition(Vec2(layout->getContentSize().width / 2, layout->getContentSize().height / 2));
		imageView->setScale(2.0f);
		layout->addChild(imageView, 1);
		pageView->addPage(layout);
	}

	// 添加触摸事件监听器
	pageView->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::BEGAN)
			clearRelatedButtons();
		if (type == Widget::TouchEventType::ENDED) {
			a.button_music();
			PageView* pageView = dynamic_cast<PageView*>(pSender);
			clearRelatedButtons();
			int currentIndex = pageView->getCurrentPageIndex();
			if (currentIndex < 3) {
				// 创建存档按钮
				auto confirmButton = Button::create("CarrotGuardRes/UI/fileNormal.png", "CarrotGuardRes/UI/fileSelected.png");  // 替换成你的按钮图片
				confirmButton->setName("confirmButton");
				confirmButton->setPosition(Vec2(screenSize.width * 0.6, screenSize.height * 0.13));
				confirmButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
					if (type == Widget::TouchEventType::ENDED) {
						a.button_music();
						// 载入存档，tmp=1
						//int tmp = 1;
						//auto gameScene = GameScene::createSceneWithLevel(currentIndex + 1, tmp);
						//Director::getInstance()->replaceScene(gameScene);
					}
					});
				this->addChild(confirmButton);

				// 创建开始按钮
				auto cancelButton = Button::create("CarrotGuardRes/UI/startNormal.png", "CarrotGuardRes/UI/startSelected.png");  // 替换成你的按钮图片
				cancelButton->setName("cancelButton");
				cancelButton->setPosition(Vec2(screenSize.width * 0.4, screenSize.height * 0.13));  // 调整位置
				cancelButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
					if (type == Widget::TouchEventType::ENDED) {
						a.button_music();
					

						// 创建并切换到第一关
						auto scene = BaseLevelScene::createScene(currentIndex+1);  
						Director::getInstance()->replaceScene(scene);
						// 不载入存档，tmp=0
						//int tmp = 0;
						//auto gameScene = GameScene::createSceneWithLevel(currentIndex + 1, tmp);
						//Director::getInstance()->replaceScene(gameScene);
					}
					});
				this->addChild(cancelButton);
			}
			else {
				auto lockedButton = Button::create("CarrotGuardRes/UI/locked.png");  // 替换成你的按钮图片
				lockedButton->setName("lockedButton");
				lockedButton->setPosition(Vec2(screenSize.width * 0.5, screenSize.height * 0.13));  // 调整位置
				this->addChild(lockedButton);


				auto visibleSize = Director::getInstance()->getVisibleSize();//分辨率大小
				Vec2 origin = Director::getInstance()->getVisibleOrigin();
				//添加boss界面
				auto boss1 = Sprite::create("CarrotGuardRes/UI/un_select2.png");
				boss1->setTag(100);
				if (boss1 == nullptr)
					problemLoading("'un_select2.png'");
				else
				{
					boss1->setScale(0.5);
					boss1->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
					this->addChild(boss1, 2);
				}
				auto menu1 = Menu::create(NULL);
				menu1->setPosition(Vec2::ZERO);
				this->addChild(menu1, 3);
				auto know = MenuItemImage::create("CarrotGuardRes/UI/know.png", "CarrotGuardRes/UI/know.png", CC_CALLBACK_1(themescene::know_Callback, this));

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
					menu1->addChild(know);  //加入到菜单中
				}
			}
		}
		});

	// 添加左右翻页按钮
	auto leftButton = Button::create("CarrotGuardRes/UI/leftButtonNormal.png", "CarrotGuardRes/UI/leftButtonSelected.png");
	leftButton->setPosition(Vec2(screenSize.width * 0.1, screenSize.height / 2));
	leftButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			a.button_music();
			int currentIndex = pageView->getCurrentPageIndex();
			if (currentIndex > 0) {
				pageView->scrollToPage(currentIndex - 1);

				//消除上一页地图的选择按钮
				clearRelatedButtons();
			}
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
				currentIndex = 0;  //  未知原因导致开始的时候index是-1，暂时像这样处理。
			if (currentIndex < mapImages.size() - 1) {
				pageView->scrollToPage(currentIndex + 1);
				clearRelatedButtons();
			}
		}
		});
	this->addChild(rightButton, 2);
	return true;
}

void themescene::to_advantureScene(Ref* pSender) {
	a.button_music();
	Director::getInstance()->popScene();
}

void themescene::know_Callback(Ref* pSender) {
	auto sprite = this->getChildByTag(100);
	// 如果找到了精灵，就从其所在父节点中移除它
	if (sprite)
		this->removeChild(sprite);
	auto menu = ((MenuItem*)pSender)->getParent();
	// 从菜单中移除按钮
	menu->removeChild((MenuItem*)pSender);
}