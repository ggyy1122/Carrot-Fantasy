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
    bool  isDead;                  //�Ƿ�����
public:
    //��̬�������������������ļ�����һ���������
    static Monster* create(const std::string& monsterName,  const std::vector<Vec2>& path) {
        Monster* monster = new (std::nothrow) Monster();//ʹ��new����������ڴ棬����һ��Monster����
        if (monster && monster->initWithPath(monsterName,  path))//�ж��Ƿ񴴽��ɹ��ͳ�ʼ���ɹ�
        {
            monster->autorelease();
            return monster;
        }
        CC_SAFE_DELETE(monster);//����������ʼ��ʧ�ܣ��ͷ��ڴ沢����nullptr
        return nullptr;
    }
    //�ù������͹ؿ�·������ʼ�����������
    bool initWithPath(const std::string& monsterName,  const std::vector<Vec2>& path);
    //������ƶ��߼�
    void moveAlongPath(const std::vector<Vec2>& path);
};