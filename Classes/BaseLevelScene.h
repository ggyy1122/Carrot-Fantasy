#pragma once
#include "cocos2d.h"
#include<string>
#include<map>
#include<vector>
#include<Monster.h>
USING_NS_CC;

class BaseLevelScene : public cocos2d::Scene {

private:
    cocos2d::TMXTiledMap* tileMap = nullptr;                // ��ͼ����
    cocos2d::Node*plantsLayer;                              //ֲ��ͼ��
    TMXLayer* plantableLayer = nullptr;                     // plantable ��
    static const std::vector<std::string> mapFiles;         // �洢��ͼ�ļ���·��
    cocos2d::Size tileSize;                                 //ÿ����Ƭ�Ĵ�С
    std::map<int,std::vector<cocos2d::Vec2>>pathsCache;          //�洢�Ѿ����ع��Ĺؿ���·��
    int levelId;                                            //�ؿ����
public:
    static cocos2d::Scene* createScene(int level);          // ��������ʱ����ؿ����
    virtual bool init();                                    // Ĭ�ϵ� init() ����
    bool initWithLevel(int level);                          //��д��init����
    CREATE_FUNC(BaseLevelScene);
    void loadMap();                                         // ���ص�ͼ�ĺ���
    void addMouseListener();                                // �����������
    void handleMouseDown(EventMouse* event);                //�����ʱ���ж��Ƿ���ֲ
    void handlePlant(const Vec2& position) ;                // ��ֲ�ж�
    Vec2 convertToTileCoord(const Vec2& screenPosition);    //���������ת����Ƭ����
    void drawGrid();                                        //��������
    void plantAt(const cocos2d::Vec2& tileCoord);           //��ֲ
    bool loadPathForLevel(int levelId, const std::string& filePath); // �� JSON �ļ��������ĳ���ؿ���·��
    void loadMonsterConfig();                                //���ع�������
    void moveMonsterAlongPath(Monster* monster, const std::vector<cocos2d::Vec2>& path);   //����·���ƶ�
    Vec2 gridToScreenCenter(const Vec2& gridPoint);         //����������ת����Ļ����Ĺ��ߺ���

};