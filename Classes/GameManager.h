#pragma once

#include "cocos2d.h"
#include "Monster.h"
#include<vector>
#include<map>
#include"BaseLevelScene.h"
USING_NS_CC;

class GameManager {
private:
    static GameManager* instance;            // ����ָ��
    BaseLevelScene* currentScene;            // ��ǰ�󶨵ĳ���
    GameManager() : currentScene(nullptr) {} // ˽�л����캯������ֹ�ⲿʵ����
    std::vector<Monster*> monsters;          // �����б�
    int playerMoney;                         // ��ҽ�Ǯ
    int playerHealth;                        // �������ֵ
    float gameSpeed;                         // ��Ϸ�ٶ�
    std::map<int, std::vector<cocos2d::Vec2>>pathsCache;     //�洢�Ѿ����ع��Ĺؿ�������·��
    std::map<int, std::vector<cocos2d::Vec2>>ScreenPaths;   //�洢�Ѿ����ع��Ĺؿ�����Ļ·��
    std::vector<cocos2d::Vec2> path;               //��ǰ��Ļ·��
    std::vector<cocos2d::Vec2> screenPath;               //��ǰ��Ļ·��
    GameManager(Scene* scene);               // ˽�й��캯��
    void cleanup();                          // ����������Դ
    int levelId;                             //�ؿ����

public:
    // ���ÿ����͸�ֵ
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    // ������ȡ����
    static GameManager* getInstance(BaseLevelScene* scene = nullptr);
    void setScene(BaseLevelScene* scene);    //���õ�ǰ����
    // ��ȡ��ǰ����
    BaseLevelScene* getScene() const;
    static void destroyInstance();

    void initLevel(int level);               // ��ʼ���ؿ�����
    void initPath();              // ��ʼ��·��
    int getCurrentLevel() const;             // ��ȡ��ǰ�ؿ����

    // ��Ϸ״̬����
    void setPlayerMoney(int money);
    int getPlayerMoney() const;
    void modifyPlayerMoney(int amount); // �޸Ľ�Ǯ

    void setPlayerHealth(int health);
    int getPlayerHealth() const;
    void modifyPlayerHealth(int amount); // �޸�����ֵ

    // �������
    void updateMonsterPosition(Monster* monster, float deltaTime);
    void spawnMonster(const Vec2& startPos, const Vec2& targetPos, float speed, int health);
    void update(float deltaTime);
    // ���ع�����Դ
    void loadMonsterResources();
    // ���ݹؿ����ù���
    void placeMonsters(int levelId);

    // �������
    void updateMonsters(float deltaTime);
    // �������
    void cleanupMonsters();

    // ��Ϸ�ٶȿ���
    void setGameSpeed(float speed);
    float getGameSpeed() const;

    // ��Դ����
    void preloadResources();

    Vec2 gridToScreenCenter(const Vec2& gridPoint);
    bool loadPathForLevel(int levelId, const std::string& filePath);
};