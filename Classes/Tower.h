#pragma once
#include "cocos2d.h";
#include<vector>
#include<string>
USING_NS_CC;

#define TOWER_NUM 2

class Tower {
protected:
	int index, grade;//炮塔序号和等级
public:
	Tower(int index_):index(index_){}
	static std::string tower_table[TOWER_NUM][3];//记录所有炮塔的图片路径
	static std::string base_table[TOWER_NUM];//所有炮塔底座的图片路径
	static Vec2 anchorpoint_table[TOWER_NUM][2];//所有炮塔和底座锚点的设定
	Sprite* remove_mark;
	void build(Scene*,Vec2);
};





class Bottle :public Tower {
protected:
	int range, demage;//当前防御塔可攻击的范围和伤害
public:
	static const int build_cost, up_cost1, up_cost2;//建造花费，两次升级的花费
	static int range_table[3]; //该类防御塔三个等级分别可攻击的范围和伤害
	static int demage_table[3];
	//构造函数
	Bottle(int index_):Tower(index_){}
	/*void attack();*///攻击怪物
	//攻击的特效，攻击时是否转向
};

