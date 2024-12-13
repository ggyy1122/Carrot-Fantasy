#pragma once
#include "cocos2d.h"
#include"MonsterConfigs.h"
USING_NS_CC;
class Monster : public cocos2d::Sprite {
private:
    int   health;                   // ����ֵ
    float  speed;                  // �ƶ��ٶ�
    int   damage;                   // ������
    int   reward;                   // ��ɱ����
    bool  isDead=false;             //�Ƿ�����
    Vec2  endPos;                   //��ֹλ��
    int startPosIndex;              //��ʼλ��
    std::string name;               //������
    int PathIndex=0;                //·������
   
public:


    //��̬�������������������ļ�����һ���������
    static Monster* create(const std::string& monsterName,  const std::vector<Vec2>& path, int startIndex) {
        Monster* monster = new (std::nothrow) Monster();//ʹ��new����������ڴ棬����һ��Monster����
        if (monster && monster->initWithPath(monsterName,  path, startIndex))//�ж��Ƿ񴴽��ɹ��ͳ�ʼ���ɹ�
        {
            monster->autorelease();
            return monster;
        }
        CC_SAFE_DELETE(monster);//����������ʼ��ʧ�ܣ��ͷ��ڴ沢����nullptr
        return nullptr;
    }
    //�ù������͹ؿ�·������ʼ�����������
    bool initWithPath(const std::string& monsterName,  const std::vector<Vec2>& path, int startIndex);
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
};