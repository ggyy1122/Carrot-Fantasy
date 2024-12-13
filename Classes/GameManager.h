#pragma once

#include "cocos2d.h"
#include "Monster.h"
#include<vector>
#include<map>
#include<string>
#include"BaseLevelScene.h"
#include<array>
#include"Carrot.h"
USING_NS_CC;

//���޲�����Ϣ
struct WaveConfig {
    int wave;
    std::string monsterName;
    int count;
    std::array<float, 2> spawnInterval={1,2};  // ���ɼ���ķ�Χ [0.0, 2.0] ֮��
};


class GameManager {
private:
    static GameManager* instance;            // ����ָ��
    BaseLevelScene* currentScene;            // ��ǰ�󶨵ĳ���
    GameManager() : currentScene(nullptr) {} // ˽�л����캯������ֹ�ⲿʵ����
    std::vector<Monster*> monsters;          // �����б�

    Carrot*carrot; // ���ܲ��������� Carrot ��


    int playerMoney;                         // ��ҽ�Ǯ
    int playerHealth;                        // �������ֵ
    float gameSpeed;                         // ��Ϸ�ٶ�
    std::map<int, std::vector<cocos2d::Vec2>>pathsCache;     //�洢�Ѿ����ع��Ĺؿ�������·��
    std::map<int, std::vector<cocos2d::Vec2>>ScreenPaths;   //�洢�Ѿ����ع��Ĺؿ�����Ļ·��
    std::vector<cocos2d::Vec2> path;               //��ǰ��Ļ����·��
    std::vector<cocos2d::Vec2> screenPath;         //��ǰ��Ļ·��
    GameManager(Scene* scene);               // ˽�й��캯��
    void cleanup();                          // ����������Դ
    int levelId;                             //�ؿ����
    std::vector<WaveConfig> waveConfigs;     //�洢���޲�
    int waveIndex = 0;
   int NumOfDeadMonster = 0;
public:
    void onMonsterPathComplete(cocos2d::EventCustom* event);
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
    void playSpawnEffect(const cocos2d::Vec2& spawnPosition);
    void spawnMonster(const Vec2& startPos, const Vec2& targetPos, float speed, int health);
    void update(float deltaTime);
    void produceMonsterWave(const WaveConfig& waveConfig);
    void startMonsterWaves();
    // ���ع�����Դ
    void loadMonsterResources();
    // ���ݹؿ����ù���
    void produceMonsters(const std::string monsterName,const int startIndex,int health=-1);
    void loadMonsterWaveConfig(const std::string& filename, const std::string& levelName);
    // �������
    void cleanupMonsters();
    int getCurrentWaveIndex() const{return waveIndex;}
    // ��Ϸ�ٶȿ���
    void setGameSpeed(float speed);
    float getGameSpeed() const;

    // ��Դ����
    void preloadResources();
    void loadGameData(const std::string& fileName);
    void saveMonstersDataToJson(const std::string& fileName);
    Vec2 gridToScreenCenter(const Vec2& gridPoint);
    bool loadPathForLevel(int levelId, const std::string& filePath);


    void initCarrot();             //ÿ�س�ʼ���ܲ�
    bool CheckLose();             //�����״̬
    bool CheckWin();               //���Ӯ״̬
    void ClearMonsters();         //������й����ڴ�
};