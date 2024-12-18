#pragma once
#include "cocos2d.h"
#include"Monster.h"
#include"Obstacle.h"
#include<vector>
#include<string>
#include<map>
USING_NS_CC;

#define TOWER_NUM 3

class BaseLevelScene;
class Tower;
class Obstacle;

class Tower {
protected:
	int index, grade;//������ź͵ȼ�
	int range, demage;//��ǰ�������ɹ����ķ�Χ���˺�
public:
	Tower(int index_) :index(index_), grade(0) { range = range_table[index][grade]; demage = demage_table[index][grade]; }
	static std::string tower_table[TOWER_NUM][3];//��¼����������ͼƬ·��
	static std::string base_table[TOWER_NUM];//��������������ͼƬ·��
	static Vec2 anchorpoint_table[TOWER_NUM][2];//���������͵���ê����趨
	static int build_cost[TOWER_NUM];//���з������Ľ��컨��
	static int up_cost[TOWER_NUM][2];//���з���������������
	static int range_table[TOWER_NUM][3];//���з������ķ�Χ
	static int demage_table[TOWER_NUM][3];//���з��������˺�
	static int sell_money[TOWER_NUM][3];//����ÿ���������ӵ�Ǯ
	static std::map<int, std::string>sale_graph;//������ͼ��
	static std::map<int, std::string>up_graph;//��������ͼ��
	static std::map<int, std::string>noup_graph;//�����ͼ��
	static float interval_table[TOWER_NUM];

	Sprite* sprite_mark, * sp_base;
	Vec2 pos;//����
	float interval;//�������ʱ��

	void build(BaseLevelScene*, Vec2);
	void destroy(BaseLevelScene*);
	//��������������
	static Sprite* curr_up, * curr_sale, * curr_range;
	void update(BaseLevelScene*, Vec2);
	void UpMenuAppear(BaseLevelScene* my_scene, Vec2& position);//�����˵�����
	void UpMenuGone(BaseLevelScene*);//�����˵���ʧ
	int GetIndex()const { return index; }

	virtual void attack(BaseLevelScene*, std::vector<Monster*>&, char isTarget, Monster* tar_m, Obstacle* tar_o,int jiasu) = 0;
};


Tower* createTower(int index);



class Bottle :public Tower {
protected:
	static const float speed;//�ڵ����ٶ�,ÿ���������
	float tower_angle;//��������ĽǶȣ�������Ϊ0�ȣ���ʱ������
	Sprite* curr_shell;//ָ���ӵ�
	float curr_dis, de_time;
	//bool Clockwise;//�Ƿ�˳ʱ��ת��
public:

	//���캯��
	Bottle(int index_) :Tower(index_), tower_angle(0) { }

	virtual void attack(BaseLevelScene*, std::vector<Monster*>&, char isTarget, Monster* tar_m, Obstacle* tar_o, int jiasu);//����

	void ShellProduct(Scene* my_scene);//�����ڵ�

	template<class T>
	void ShellDemage(BaseLevelScene* my_scene, T* sp);

	template<class T>
	bool  AttackSprite(T* sp, BaseLevelScene* my_scene, int jiasu);//����ĳ������
	static std::string bottle_shell[3];
};


class Sun :public Tower {
private:
	Sprite* curr_halo;
public:
	Sun(int index_) :Tower(index_) {}
	virtual void attack(BaseLevelScene*, std::vector<Monster*>&, char isTarget, Monster* tar_m, Obstacle* tar_o, int jiasu);//��������
	void SunAttack(BaseLevelScene* my_scene, std::vector<Monster*>& monsters, int jiasu);

};



class MyPlane :public Tower {
private:
	Sprite* curr_ray;
	float tower_angle;//��������ĽǶȣ�������Ϊ0�ȣ���ʱ������
	float de_ang;
public:
	MyPlane(int index_) :Tower(index_), tower_angle(90) {}
	virtual void attack(BaseLevelScene*, std::vector<Monster*>&, char isTarget, Monster* tar_m, Obstacle* tar_o, int jiasu);//��������

	template<class T>
	bool  AttackSprite(T* sp, BaseLevelScene* my_scene, std::vector<Monster*>& monsters, int jiasu);//����ĳ������

	void PlaneDemage(BaseLevelScene* my_scene, std::vector<Monster*>& monsters);
	static std::string ray[3];
};
