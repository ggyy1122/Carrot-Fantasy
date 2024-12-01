#pragma once
#include "cocos2d.h";
#include<vector>
#include<string>
USING_NS_CC;

#define TOWER_NUM 2

class Tower {
protected:
	int index, grade;//������ź͵ȼ�
public:
	Tower(int index_):index(index_){}
	static std::string tower_table[TOWER_NUM][3];//��¼����������ͼƬ·��
	static std::string base_table[TOWER_NUM];//��������������ͼƬ·��
	static Vec2 anchorpoint_table[TOWER_NUM][2];//���������͵���ê����趨
	Sprite* remove_mark;
	void build(Scene*,Vec2);
};





class Bottle :public Tower {
protected:
	int range, demage;//��ǰ�������ɹ����ķ�Χ���˺�
public:
	static const int build_cost, up_cost1, up_cost2;//���컨�ѣ����������Ļ���
	static int range_table[3]; //��������������ȼ��ֱ�ɹ����ķ�Χ���˺�
	static int demage_table[3];
	//���캯��
	Bottle(int index_):Tower(index_){}
	/*void attack();*///��������
	//��������Ч������ʱ�Ƿ�ת��
};

