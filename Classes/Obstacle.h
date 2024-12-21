#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include<string>
USING_NS_CC;
using namespace cocos2d::ui;


//�ϰ���
class BaseLevelScene;

struct ObData {
	std::string road_path;//ͼƬ·��
	int size;//�ߴ�,1����1*1��2����1*2��4����2*2
	int money;//�򱬺����õ�Ǯ
	int hp;//Ѫ��
};

class Obstacle {
private:
	
	int index;
	int size;//�ߴ�,1����1*1��2����1*2��4����2*2
	int health;//Ѫ��
	int maxHp;
	int x, y;//ָ�������½��ڵ�ͼ�����е�λ��
	bool isDead = false;
	LoadingBar* _HP;       // ������Ч����ʾѪ��
public:
	
	Sprite* hpback;
	bool ishpvs=false;//HP�Ƿ�ɼ�
	Sprite* curr;
	int reward;//�򱬺����õ�Ǯ
	static ObData Ob[10];
	int GetIndex(){return index;}
	Obstacle(int index) :index(index),size(Ob[index].size),maxHp(Ob[index].hp), reward(Ob[index].money), health(Ob[index].hp) {}
	void Produce(Scene* my_scene, int x_, int y_);//�����ϰ���

	Vec2 getPosition() { return curr->getPosition(); }

	void toDie(BaseLevelScene* my_scene);

	void updateHealthBar(); 

	void addChild(Sprite* c) {
		curr->addChild(c);
	}
	void setHpVisible(bool isVisible){ _HP->setVisible(isVisible); hpback->setVisible(isVisible); ishpvs = true;
	}	//����Ѫ���ɼ�
	bool getHpVisibleState()const { return ishpvs; }
	int  GetSize()const{return size;}
	void getHurt(int value){health-=value;}
	int getHealth()const { return health; }                       //��ȡѪ��
	 void setHealth(int health) { this->health = health; };
};


