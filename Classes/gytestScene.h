#pragma once

#include "BaseLevelScene.h"  // ��������ͷ�ļ�
#include "cocos2d.h"
class gytestScene : public BaseLevelScene {  // �޸�Ϊ�̳��� BaseLevelScene
public:
    static Scene*createScene();            //���������ľ�̬����
    virtual bool init() override;          // ��ʼ����������д�� BaseLevelScene
    void TotestScene(Ref* pSender);        // ������ת�ĺ���
    void adjustTileMapToFitScreen();       // ���� TileMap ������Ļ�ĺ���

    CREATE_FUNC(gytestScene);  // �Զ����� create �����ĺ�
};

