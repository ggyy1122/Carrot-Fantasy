#pragma once
#ifndef __BASE_H__
#define __BASE_H__
#include "cocos2d.h"
#include<string>
#include<map>
#include<vector>
#include<Monster.h>
#include"Tower.h"

USING_NS_CC;

#define Y 9//y
#define X 15//x

struct Cell {//������ʾ��ͼ������Ϣ�Ľṹ��
    char flag;//
    int key;
};


class BaseLevelScene : public cocos2d::Scene {

private:
    cocos2d::Node* plantsLayer;                              //ֲ��ͼ��
    TMXLayer* plantableLayer = nullptr;                     // plantable ��
    static const std::vector<std::string> mapFiles;         // �洢��ͼ�ļ���·��
    int levelId;                                            //�ؿ����
    
    Label* money_lable;            //��ʾ��ǰ��Ǯ
    
    bool isDoubleSpeed = false; // ��ʼ��Ϊfalse����ʾ��Ϸ��ʼ���Ƕ�����״̬
    bool isPaused = false; // ��ʼ��Ϊfalse����ʾ��Ϸ��ʼ������ͣ״̬
    Scheduler* scheduler = Director::getInstance()->getScheduler();
    std::vector<Sprite*>remove_table;//removeʱ����
    Sprite* curr_cell;//
    std::vector<std::string>buy_tower[2];//��������Ĺ���ͼ
    std::vector<int>index_table;//��¼�����õ������������
    int cell_flag;//
    Vec2 last_position;//��¼��һ�ε�λ��

    std::map<int, Tower*> towers;//��¼���ϵ�������Ϣ
    Cell map_data[X][Y];//��ͼ����

public:
    int money = 1000;

    virtual void update(float deltaTime) override;
    cocos2d::TMXTiledMap* tileMap = nullptr;                // ��ͼ����
    cocos2d::Size tileSize;                                 //ÿ����Ƭ�Ĵ�С
    static cocos2d::Scene* createScene(int level);          // ��������ʱ����ؿ����
    virtual bool init();                                    // Ĭ�ϵ� init() ����
    void initUI();                                          //��ʼ��ui
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
    //void placeMonsters();                                    //���ݹؿ���ֹ����
    void updatemoney(int add);           //����money��addΪ�仯��
    void doublespeed(Ref* pSender);
    void pause_all(Ref* pSender);
    void menu_all(Ref* pSender);


    void CountDown(std::function<void()> onComplete);             //����ʱ
    void gameover(bool is_win);   //������Ϸ
    void Jineng1(Ref* pSender);   //�ܲ�Ѫ����1
    void Jineng2(Ref* pSender);   //���������޼��٣���ʵ��

    void PlantMenuAppear(Vec2 mapPos);//������ֲ�˵�
    void PlantMenuGone(Vec2 position);//��ֲ�˵���ʧ
    void InitMapData();//��ʼ����ͼ��Ϣ

    void UpMenuAppear(Vec2& position);//�����˵�����
    void UpMenuGone(Vec2& position);//�����˵���ʧ
};
#endif 