#pragma once

//#ifdef CARROT_H
//#define CARROT_H
#ifndef CARROT_H
#define CARROT_H

#include "cocos2d.h"

class Carrot : public cocos2d::Node  // �̳�Node
{
public:
    // ʹ��create��̬�������ж��󴴽�
    static Carrot* create(int initialHP, const cocos2d::Vec2& carrotPos, const cocos2d::Vec2& hpPos);
    virtual bool init(int initialHP, const cocos2d::Vec2& carrotPos, const cocos2d::Vec2& hpPos);

    void changeHP(int change); // �ı�Ѫ��
    void getDamage();          // ����
    int getHP() const;         // ��ȡ��ǰ���ܲ�Ѫ��

private:
    int hp;                         // Ѫ��
    cocos2d::Sprite* carrotSprite;  // �ܲ���ͼƬ
    cocos2d::Sprite* hpSprite;      // Ѫ����ͼƬ

    void updateHPDisplay();         // ����Ѫ�����ܲ���ͼƬ��ʾ
    void gameOver();                // ��Ϸ����
};

#endif

//#endif CARROT_H