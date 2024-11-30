#pragma once
#include "cocos2d.h"
USING_NS_CC;

class BaseLevelScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene(int level);  // ��������ʱ����ؿ����
    virtual bool init();                            // Ĭ�ϵ� init() ����

    CREATE_FUNC(BaseLevelScene);

protected:
    void loadMap(int level);                                // ���ص�ͼ�ĺ���
    void addMouseListener();                                // �����������
    void handleMouseDown(EventMouse* event);                //�����ʱ���ж��Ƿ���ֲ
    void handlePlant(const Vec2& position) ;               // ��ֲ����
    TMXTiledMap* tileMap = nullptr;                         // ��ͼ����
    TMXLayer* plantableLayer = nullptr;                     // plantable ��
    Vec2 convertToTileCoord(const Vec2& screenPosition);    //���������ת����Ƭ����
    void drawGrid();                                        //��������
    cocos2d::Size tileSize;                                 //ÿ����Ƭ�Ĵ�С

private:
    static const std::vector<std::string> mapFiles;  // �洢��ͼ�ļ���·��
};