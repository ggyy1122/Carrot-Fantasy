#include "BaseLevelScene.h"
#include "ui/CocosGUI.h"  // UI�����ͷ�ļ�

using namespace ui;  // ʹ��UI�����ռ�

//BaseLevelScene�ĳ�ʼ��
bool BaseLevelScene::init()  {
    if (!Scene::init()) {
        return false;
    }
    // ������ӷ��ذ�ť�ķ���
    this->addBackButton();
    return true;
}

//ͨ�ú���:��ֹ���ذ�ť
void BaseLevelScene::addBackButton() {
    // ��ȡ��Ļ��С
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // �������ذ�ť
    auto button = MenuItemLabel::create(
        Label::createWithSystemFont("Back", "Arial", 24),
        CC_CALLBACK_1(BaseLevelScene::onBackButtonClicked, this)
    );
    button->setPosition(Vec2(visibleSize.width / 2, 50));  // ���ð�ťλ��

    // �����˵�����ӷ��ذ�ť
    auto menu = Menu::create(button, nullptr);
    menu->setPosition(Vec2::ZERO);  // �˵���ê����Ϊ (0,0)
    this->addChild(menu, BACK_BUTTON_ZORDER);  // ��Ӳ˵�����ǰ����������Z��㼶
}

// �����ذ�ť�ĵ���¼�
void BaseLevelScene::onBackButtonClicked(Ref* pSender) {
    Director::getInstance()->popScene();  // ������һ����
}