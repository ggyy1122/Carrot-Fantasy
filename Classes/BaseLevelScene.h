#pragma once
#ifndef __BASE_LEVEL_SCENE_H__
#define __BASE_LEVEL_SCENE_H__
#include "cocos2d.h"

USING_NS_CC;

//����һ���ؿ�����
//ע:����ֻ�ṩ�������ܣ����ฺ�𳡾��Ĵ����͹������Բ��ٻ������ṩ���������ķ���



//�ؿ�ui���ֵİ�ť����
// ���ذ�ť��Z��㼶
#define BACK_BUTTON_ZORDER 2


class BaseLevelScene : public Scene {
public:
  

    // ��ʼ����������д����ĳ�ʼ������
    virtual bool init() override;

    // ���������ĺ�
    CREATE_FUNC(BaseLevelScene);

    // ��ӷ��ذ�ť�ķ���
    void addBackButton();

private:
    // �����ذ�ť����¼�
    void onBackButtonClicked(Ref* pSender);
};
#endif // __BASE_LEVEL_SCENE_H__