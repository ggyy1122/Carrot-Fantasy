#pragma once
#include "cocos2d.h"
#include"MonsterConfigs.h"
USING_NS_CC;
class Monster : public cocos2d::Sprite {
private:
    int   health;                   // 生命值
    float  speed;                  // 移动速度
    int   damage;                   // 攻击力
    int   reward;                   // 击杀奖励
    bool  isDead;                  //是否死亡
public:
    //静态创建方法，根据配置文件生成一个怪物对象
    static Monster* create(const std::string& monsterName,  const std::vector<Vec2>& path) {
        Monster* monster = new (std::nothrow) Monster();//使用new运算符分配内存，创建一个Monster对象
        if (monster && monster->initWithPath(monsterName,  path))//判断是否创建成功和初始化成功
        {
            monster->autorelease();
            return monster;
        }
        CC_SAFE_DELETE(monster);//如果创建或初始化失败，释放内存并返回nullptr
        return nullptr;
    }
    //用怪物名和关卡路径来初始化怪物的属性
    bool initWithPath(const std::string& monsterName,  const std::vector<Vec2>& path);
    //怪物的移动逻辑
    void moveAlongPath(const std::vector<Vec2>& path);
};