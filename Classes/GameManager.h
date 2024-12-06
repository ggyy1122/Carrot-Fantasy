#pragma once

#include "cocos2d.h"
#include "Monster.h"
#include<vector>
#include<map>
#include"BaseLevelScene.h"
USING_NS_CC;

class GameManager {
private:
    static GameManager* instance;            // 单例指针
    BaseLevelScene* currentScene;            // 当前绑定的场景
    GameManager() : currentScene(nullptr) {} // 私有化构造函数，防止外部实例化
    std::vector<Monster*> monsters;          // 怪物列表
    int playerMoney;                         // 玩家金钱
    int playerHealth;                        // 玩家生命值
    float gameSpeed;                         // 游戏速度
    std::map<int, std::vector<cocos2d::Vec2>>pathsCache;     //存储已经加载过的关卡的网格路径
    std::map<int, std::vector<cocos2d::Vec2>>ScreenPaths;   //存储已经加载过的关卡的屏幕路径
    std::vector<cocos2d::Vec2> path;               //当前屏幕路径
    std::vector<cocos2d::Vec2> screenPath;               //当前屏幕路径
    GameManager(Scene* scene);               // 私有构造函数
    void cleanup();                          // 清理所有资源
    int levelId;                             //关卡编号

public:
    // 禁用拷贝和赋值
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    // 单例获取函数
    static GameManager* getInstance(BaseLevelScene* scene = nullptr);
    void setScene(BaseLevelScene* scene);    //设置当前场景
    // 获取当前场景
    BaseLevelScene* getScene() const;
    static void destroyInstance();

    void initLevel(int level);               // 初始化关卡数据
    void initPath();              // 初始化路径
    int getCurrentLevel() const;             // 获取当前关卡编号

    // 游戏状态管理
    void setPlayerMoney(int money);
    int getPlayerMoney() const;
    void modifyPlayerMoney(int amount); // 修改金钱

    void setPlayerHealth(int health);
    int getPlayerHealth() const;
    void modifyPlayerHealth(int amount); // 修改生命值

    // 怪物管理
    void updateMonsterPosition(Monster* monster, float deltaTime);
    void spawnMonster(const Vec2& startPos, const Vec2& targetPos, float speed, int health);
    void update(float deltaTime);
    // 加载怪物资源
    void loadMonsterResources();
    // 根据关卡放置怪物
    void placeMonsters(int levelId);

    // 怪物更新
    void updateMonsters(float deltaTime);
    // 清理怪物
    void cleanupMonsters();

    // 游戏速度控制
    void setGameSpeed(float speed);
    float getGameSpeed() const;

    // 资源管理
    void preloadResources();

    Vec2 gridToScreenCenter(const Vec2& gridPoint);
    bool loadPathForLevel(int levelId, const std::string& filePath);
};