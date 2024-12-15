#pragma once

#include "cocos2d.h"
#include"MonsterConfigs.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace cocos2d::ui;
class Monster : public cocos2d::Sprite {
private:
   
    float  speed;                  // �ƶ��ٶ�
    int   damage;                   // ������
  
    
    Vec2  endPos;                   //��ֹλ��
    int startPosIndex;              //��ʼλ��
    std::string name;               //������
    int PathIndex=0;                //·������
    LoadingBar* _HP;       // ������Ч����ʾѪ��
    int maxHp;
   
public:
    bool pause;             //��ͣ
    int   reward;                   // ��ɱ����
    int   health;                   // ����ֵ
    static int DeadCount;
    bool  isDead = false;             //�Ƿ�����
    //��̬�������������������ļ�����һ���������
    static Monster* create(const std::string& monsterName,  const std::vector<Vec2>& path, int startIndex,bool pause) {
        Monster* monster = new (std::nothrow) Monster();//ʹ��new����������ڴ棬����һ��Monster����
        if (monster && monster->initWithPath(monsterName,  path, startIndex,pause))//�ж��Ƿ񴴽��ɹ��ͳ�ʼ���ɹ�
        {
            monster->autorelease();
            //���Ѫ��
            monster->_HP = cocos2d::ui::LoadingBar::create("Monsters/HPbar.png");
            monster->_HP->setPercent(100); // ��ʼѪ��Ϊ��Ѫ
            monster->_HP->setPosition(Vec2(monster->getContentSize().width / 2, monster->getContentSize().height * 1.3f));
            monster->addChild(monster->_HP, 10);
            //���Ѫ������
            auto HPBackground = Sprite::create("Monsters/HPBackground.png");
            HPBackground->setPosition(Vec2(monster->getContentSize().width / 2, monster->getContentSize().height * 1.3f));
            monster->addChild(HPBackground, 9);
            return monster;
        }
        CC_SAFE_DELETE(monster);//����������ʼ��ʧ�ܣ��ͷ��ڴ沢����nullptr
        return nullptr;
    }
    //�ù������͹ؿ�·������ʼ�����������
    bool initWithPath(const std::string& monsterName,  const std::vector<Vec2>& path, int startIndex,bool pause);
    //������ƶ��߼�
    void moveAlongPath(const std::vector<Vec2>& path);
    //�ù�������
    void toDie();
    //�������Ƿ�����
    bool checkLive()const{return (!isDead);};
    //��ȡ��������ֵ
    int getHealth()const{return health;};
    //��ȡ���޵�����
    std::string getMonsterName()const{return name;};
    //��ȡ���ﵱǰ��·������
    int getPathIndex()const{return PathIndex;};
    //���ù���Ѫ��
    void setHealth(int health){this->health=health;};
    //�����ڴ�
    void cleanUp(){this->removeFromParentAndCleanup(this);}
    void updateHealthBar() {
        float percentage = (static_cast<float>(health) / maxHp) * 100;
        if (percentage < 0) percentage = 0;
        if (percentage > 100) percentage = 100;
        _HP->setPercent(percentage); // ����Ѫ���ٷֱ�
    }
};

