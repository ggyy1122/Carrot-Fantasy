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
    //���������
    static GameManager* instance;                           // ����ָ��
    BaseLevelScene* currentScene;                           // ��ǰ�󶨵ĳ���
    GameManager() : currentScene(nullptr), monsters(100) {} // ˽�л����캯������ֹ�ⲿʵ����                                    // ˽�л����캯������ֹ�ⲿʵ����
    GameManager(Scene* scene);                              // ˽�й��캯��
    //�ܲ����
    Carrot*carrot;                                          // Carrot����
    void initCarrot();                                      //ÿ�س�ʼ���ܲ�
    //����
    int AllMonsterNum=0;                                    // ��������
    std::vector<WaveConfig> waveConfigs;                    //�洢���޲�
    int waveIndex = 0;                                      //��ǰ����
    int AllWaveNum=0;
    //·��
    std::map<int, std::vector<cocos2d::Vec2>>pathsCache;     //�洢�Ѿ����ع��Ĺؿ�������·��
    std::map<int, std::vector<cocos2d::Vec2>>ScreenPaths;    //�洢�Ѿ����ع��Ĺؿ�����Ļ·��
    std::vector<cocos2d::Vec2> path;                         //��ǰ��Ļ����·��
    std::vector<cocos2d::Vec2> screenPath;                   //��ǰ��Ļ·��
    //�ؿ�
    int levelId;                                             //�ؿ����
    //�¼�������
    cocos2d::EventListenerCustom* _listener;                 //���ڼ����޵����յ���¼�
public:

    std::vector<Monster*> monsters;                          // �����б�
    void stopAllSchedulers();                                //ֹͣmanager�����е�����
    GameManager(const GameManager&) = delete;               
    GameManager& operator=(const GameManager&) = delete;     // ���ÿ����͸�ֵ
    static GameManager* getInstance(BaseLevelScene* scene = nullptr);// ������ȡ����
    void setScene(BaseLevelScene* scene);                    //���õ�ǰ����
    BaseLevelScene* getScene() const;                        // ��ȡ��ǰ����
    static void destroyInstance();                           //�ͷ�ʵ��
    //����
    void update(float deltaTime);
    bool CheckLose();                                        //�����״̬
    bool CheckWin();                                         //���Ӯ״̬
    //�ؿ�
    void initLevel(int level, bool ReadMode);                               // ��ʼ���ؿ�����
    void initPath();                                         // ��ʼ��·��
    int getCurrentLevel() const;                             // ��ȡ��ǰ�ؿ����
    // �������
    void playSpawnEffect(const cocos2d::Vec2& spawnPosition);//���޳�����Ч
    void spawnMonster(const Vec2& startPos, const Vec2& targetPos, float speed, int health);
    void produceMonsterWave(const WaveConfig& waveConfig);   //�������޲�
    void startMonsterWaves();                                //��ʼ���޲�
    void loadMonsterResources();                             //���ع�����Դ
    void produceMonsters(const std::string monsterName,const int startIndex,int health=-1,bool pause=false);
                                                             //���ݹؿ����ù���
    void loadMonsterWaveConfig
  (const std::string& filename, const std::string& levelName);//���ع��޲�����
    void ClearMonsters();                                     //������й����ڴ�
    int getCurrentWaveIndex() const{return waveIndex;}        //��ȡ��ǰ���޲����
    int getAllWaveNum()const { return AllWaveNum; }     //��ȡ�ܲ���
    int getCurrentWaveNum()const { return waveIndex; }          //��ȡ�ֲ���

  //�����浵���
    bool loadGameData(const std::string& fileName);          //���س�ʼ��Ϸ����
    bool loadPathForLevel(int levelId, const std::string& filePath);
                                                             //���ص�ͼ����
    void saveMonstersDataToJson(const std::string& fileName);//�洢��������
    Vec2 gridToScreenCenter(const Vec2& gridPoint);
    void registerListener();                                 // ע���¼�������
    void removeListener() ;                                  // �Ƴ��¼�������
    void onMonsterPathComplete(cocos2d::EventCustom* event); // �¼��ص�
    void Jineng1();

};