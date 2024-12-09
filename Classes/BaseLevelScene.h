#pragma once
#ifndef __BASE_H__
#define __BASE_H__
#include "cocos2d.h"
#include<string>
#include<map>
#include<vector>
#include<Monster.h>

USING_NS_CC;

class BaseLevelScene : public cocos2d::Scene {

private:
    cocos2d::TMXTiledMap* tileMap = nullptr;                // ��ͼ����
    cocos2d::Node* plantsLayer;                              //ֲ��ͼ��
    TMXLayer* plantableLayer = nullptr;                     // plantable ��
    static const std::vector<std::string> mapFiles;         // �洢��ͼ�ļ���·��
    cocos2d::Size tileSize;                                 //ÿ����Ƭ�Ĵ�С
    std::map<int, std::vector<cocos2d::Vec2>>pathsCache;     //�洢�Ѿ����ع��Ĺؿ�������·��
    std::map<int, std::vector<cocos2d::Vec2>>ScreenPaths;   //�洢�Ѿ����ع��Ĺؿ�����Ļ·��
    int levelId;                                            //�ؿ����
    Label* m_lable;            //��ʾ��ǰ��Ǯ
    int money = 400;
    bool isDoubleSpeed = false; // ��ʼ��Ϊfalse����ʾ��Ϸ��ʼ���Ƕ�����״̬
    bool isPaused = false; // ��ʼ��Ϊfalse����ʾ��Ϸ��ʼ������ͣ״̬
    Scheduler* scheduler = Director::getInstance()->getScheduler();
    std::vector<Sprite*>remove_table;//removeʱ����
    Sprite* curr_cell;//
    std::vector<std::string>buy_tower;//��������Ĺ���ͼ
    std::vector<int>index_table;//��¼�����õ������������
    int cell_flag;//
    Vec2 last_position;//��¼��һ�ε�λ��


    
    int _currNum = 0;                     // ��ǰ���ﲨ��
    int _monsterWave=15;                     // ���ﲨ��
    Label* _numberLabel;                  // ��ʾ���ﲨ��
    Label* _curNumberLabel;               // ��ʾ��ǰ���ﲨ��
    Sprite* Carrot;
    Sprite* HP;
    int carrot_HP = 1;       //Ѫ��,5
    std::vector<cocos2d::Vec2>dst1 = {Vec2(804,640-196),Vec2(816,640-284),Vec2(831,640-287)};
    std::vector<cocos2d::Vec2>dst2 = { Vec2(854,640 - 196),Vec2(866,640 - 284),Vec2(881,640 - 287) };

public:
    static cocos2d::Scene* createScene(int level);          // ��������ʱ����ؿ����
    virtual bool init();                                    // Ĭ�ϵ� init() ����
    bool initWithLevel(int level);                          //��д��init����
    CREATE_FUNC(BaseLevelScene);
    void loadMap();                                         // ���ص�ͼ�ĺ���
    void addMouseListener();                                // �����������
    void handleMouseDown(EventMouse* event);                //�����ʱ���ж��Ƿ���ֲ
    void handlePlant(const Vec2& position);                // ��ֲ�ж�
    Vec2 convertToTileCoord(const Vec2& screenPosition);    //���������ת����Ƭ����
    void drawGrid();                                        //��������
    void plantAt(const cocos2d::Vec2& tileCoord);           //��ֲ
    bool loadPathForLevel(int levelId, const std::string& filePath); // �� JSON �ļ��������ĳ���ؿ���·��
    void loadMonsterConfig();                                //���ع�������
    void moveMonsterAlongPath(Monster* monster, const std::vector<cocos2d::Vec2>& path);   //����·���ƶ�
    Vec2 gridToScreenCenter(const Vec2& gridPoint);         //����������ת����Ļ����Ĺ��ߺ���
    void  loadMonsters();                                    //���ع��ﾫ��֡
    void placeMonsters();                                    //���ݹؿ���ֹ����
    bool updatemoney(int add);           //����money��addΪ�仯��
    void doublespeed(Ref* pSender);
    void pause_all(Ref* pSender);
    void menu_all(Ref* pSender);



    void CountDown();             //����ʱ
    void gameover(bool is_win);   //������Ϸ
    void changeHP(int change);    //�ı�Ѫ��,change�����ɸ�
    void Jineng1(Ref* pSender);   //�ܲ�Ѫ����1
    void Jineng2(Ref* pSender);   //���������޼��٣���ʵ��

};
#endif