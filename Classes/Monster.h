#pragma once
#include "cocos2d.h"
#include"MonsterConfigs.h"
class Monster : public cocos2d::Sprite {
private:
    int health;                   // ����ֵ
    float speed;                  // �ƶ��ٶ�
    int damage;                   // ������
    int reward;                   // ��ɱ����
    bool isDead;                  //�Ƿ�����
public:
    //��̬�������������������ļ�����һ���������
    static Monster* create(const MonsterConfig& config) {
        Monster* monster = new (std::nothrow) Monster();//ʹ��new����������ڴ棬����һ��Monster����
        if (monster && monster->init(config))//�ж��Ƿ񴴽��ɹ��ͳ�ʼ���ɹ�
        {
            monster->autorelease();
            return monster;
        }
        CC_SAFE_DELETE(monster);//����������ʼ��ʧ�ܣ��ͷ��ڴ沢����nullptr
        return nullptr;
    }
    //��ʼ�����������
    virtual bool init(const MonsterConfig& config) {
        //���ø���ĳ�ʼ��������������ָ���ľ���֡����
        if (!Sprite::initWithSpriteFrameName(config.spriteFrameName)) {
            return false;
        }

        this->health = config.health;
        this->speed = config.speed;
        this->damage = config.damage;
        this->reward = config.reward;
        this->isDead = false;

        return true;
    }
};