#pragma once

#include "cocos2d.h"
#include"MonsterConfigs.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace cocos2d::ui;
class Monster : public cocos2d::Sprite {
private:
   
    float  speed;                  // 移动速度
    int   damage;                   // 攻击力
  
    
    Vec2  endPos;                   //终止位置
    int startPosIndex;              //起始位置
    std::string name;               //怪兽名
    int PathIndex=0;                //路径索引
    LoadingBar* _HP;       // 进度条效果表示血条
    int maxHp;
   
public:
    bool pause;             //暂停
    int   reward;                   // 击杀奖励
    int   health;                   // 生命值
    static int DeadCount;
    bool  isDead = false;             //是否死亡
    //静态创建方法，根据配置文件生成一个怪物对象
    static Monster* create(const std::string& monsterName,  const std::vector<Vec2>& path, int startIndex,bool pause) {
        Monster* monster = new (std::nothrow) Monster();//使用new运算符分配内存，创建一个Monster对象
        if (monster && monster->initWithPath(monsterName,  path, startIndex,pause))//判断是否创建成功和初始化成功
        {
            monster->autorelease();
            //添加血条
            monster->_HP = cocos2d::ui::LoadingBar::create("Monsters/HPbar.png");
            monster->_HP->setPercent(100); // 初始血量为满血
            monster->_HP->setPosition(Vec2(monster->getContentSize().width / 2, monster->getContentSize().height * 1.3f));
            monster->addChild(monster->_HP, 10);
            //添加血条背景
            auto HPBackground = Sprite::create("Monsters/HPBackground.png");
            HPBackground->setPosition(Vec2(monster->getContentSize().width / 2, monster->getContentSize().height * 1.3f));
            monster->addChild(HPBackground, 9);
            return monster;
        }
        CC_SAFE_DELETE(monster);//如果创建或初始化失败，释放内存并返回nullptr
        return nullptr;
    }
    //用怪物名和关卡路径来初始化怪物的属性
    bool initWithPath(const std::string& monsterName,  const std::vector<Vec2>& path, int startIndex,bool pause);
    //怪物的移动逻辑
    void moveAlongPath(const std::vector<Vec2>& path);
    //让怪兽死亡
    void toDie();
    //检查怪兽是否死亡
    bool checkLive()const{return (!isDead);};
    //获取怪兽生命值
    int getHealth()const{return health;};
    //获取怪兽的名字
    std::string getMonsterName()const{return name;};
    //获取怪物当前的路径索引
    int getPathIndex()const{return PathIndex;};
    //设置怪物血量
    void setHealth(int health){this->health=health;};
    //清理内存
    void cleanUp(){this->removeFromParentAndCleanup(this);}
    void updateHealthBar() {
        float percentage = (static_cast<float>(health) / maxHp) * 100;
        if (percentage < 0) percentage = 0;
        if (percentage > 100) percentage = 100;
        _HP->setPercent(percentage); // 更新血条百分比
    }
};

