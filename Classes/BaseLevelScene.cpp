#include "BaseLevelScene.h"
#include "ui/CocosGUI.h"  // UI组件的头文件

using namespace ui;  // 使用UI命名空间

//BaseLevelScene的初始化
bool BaseLevelScene::init()  {
    if (!Scene::init()) {
        return false;
    }
    // 调用添加返回按钮的方法
    this->addBackButton();
    return true;
}

//通用函数:防止返回按钮
void BaseLevelScene::addBackButton() {
    // 获取屏幕大小
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建返回按钮
    auto button = MenuItemLabel::create(
        Label::createWithSystemFont("Back", "Arial", 24),
        CC_CALLBACK_1(BaseLevelScene::onBackButtonClicked, this)
    );
    button->setPosition(Vec2(visibleSize.width / 2, 50));  // 设置按钮位置

    // 创建菜单并添加返回按钮
    auto menu = Menu::create(button, nullptr);
    menu->setPosition(Vec2::ZERO);  // 菜单的锚点设为 (0,0)
    this->addChild(menu, BACK_BUTTON_ZORDER);  // 添加菜单到当前场景，设置Z轴层级
}

// 处理返回按钮的点击事件
void BaseLevelScene::onBackButtonClicked(Ref* pSender) {
    Director::getInstance()->popScene();  // 返回上一场景
}