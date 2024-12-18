#include"Tower.h"
#include"BaseLevelScene.h"
#include<cmath>

#define CELL_SIZE 64
const double PI = 3.1415926;
std::string Tower::tower_table[TOWER_NUM][3] = { {"Towers/bottle0.png","Towers/bottle1.png","Towers/bottle2.png"},
	{"Towers/windmill0.png","Towers/windmill1.png","Towers/windmill2.png"} };
std::string Tower::base_table[TOWER_NUM] = { "Towers/bottlebase.png" ,"Towers/windmillbase.png" };
Vec2 Tower::anchorpoint_table[TOWER_NUM][2] = { {Vec2(0.5,0.5),Vec2(0.4,0.46)},{Vec2(0.5,0.7),Vec2(0.5,0.35)} };
int Tower::build_cost[TOWER_NUM] = { 100,160 };
int Tower::demage_table[TOWER_NUM][3] = { {20,35,50},{30,45,60} };
int Tower::range_table[TOWER_NUM][3] = { {200,300,400},{400,500,600} };
int Tower::up_cost[TOWER_NUM][2] = { {180,260},{220,260} };
int Tower::sell_money[TOWER_NUM][3] = { {80,224,432},{128,304,512} };
Sprite* Tower::curr_sale;
Sprite* Tower::curr_up;
Sprite* Tower::curr_range;


std::map<int, std::string> Tower::sale_graph = { {80,"Towers/sale_80.png"} ,{ 128,"Towers/sale_128.png" } ,
	{ 144,"Towers/sale_144.png" } ,{ 224,"Towers/sale_224.png" }, { 304,"Towers/sale_304.png" },
	{ 352,"Towers/sale_352.png" } ,{ 432,"Towers/sale_432.png" } ,{ 512,"Towers/sale_512.png" } ,
{ 608,"Towers/sale_608.png" } };

std::map<int, std::string> Tower::up_graph = { {180,"Towers/up_180.png"},{220,"Towers/up_220.png"},
	{260,"Towers/up_260.png"},{320,"Towers/up_320.png"} };

std::map<int, std::string> Tower::noup_graph = { {180,"Towers/noup_180.png"},{220,"Towers/noup_220.png"},
	{260,"Towers/noup_260.png"},{320,"Towers/noup_320.png"} };
const float Bottle::speed = 800;
float Tower::interval_table[TOWER_NUM] = { 1.4,0.8 };
std::string Bottle::bottle_shell[3] = { "Towers/shell1-1.png","Towers/shell1-2.png" ,"Towers/shell1-3.png" };

Tower* createTower(int index)
{
	switch (index) {
		case 0:return new Bottle(index);
		case 1:return new Windmill(index);
		default:return nullptr;
	}
}
void Tower::build(BaseLevelScene* my_scene, Vec2 position)
{//判断钱够不够的，不在这里实现，钱不够时，不会创建这个对象
	auto base = Sprite::create(base_table[index]);
	base->setAnchorPoint(anchorpoint_table[index][0]);
	base->setPosition(position);
	my_scene->addChild(base);
	sp_base = base;
	auto tower = Sprite::create(tower_table[index][grade]);
	tower->setAnchorPoint(anchorpoint_table[index][1]);
	tower->setPosition(position);
	my_scene->addChild(tower);
	sprite_mark = tower;
	my_scene->updateMoney(-build_cost[index]);
	pos = position;
	interval = 0;
}

void Tower::destroy(BaseLevelScene* my_scene)
{
	my_scene->removeChild(sprite_mark);
	my_scene->removeChild(sp_base);
	sprite_mark->release();
	sp_base->release();
	my_scene->updateMoney(sell_money[index][grade]);
}

void Tower::update(BaseLevelScene* my_scene, Vec2 position)
{
	if (grade >= 2 || my_scene->money < up_cost[index][grade]) return;
	my_scene->updateMoney(-up_cost[index][grade]);
	my_scene->removeChild(sprite_mark);
	sprite_mark->release();
	grade += 1;
	auto tower = Sprite::create(tower_table[index][grade]);
	tower->setAnchorPoint(anchorpoint_table[index][1]);
	tower->setPosition(position);
	my_scene->addChild(tower);
	sprite_mark = tower;
	range = range_table[index][grade];
	demage = demage_table[index][grade];
}

void Tower::UpMenuAppear(BaseLevelScene* my_scene, Vec2& position)
{
	Sprite* up;
	if (grade < 2 && my_scene->money >= up_cost[index][grade]) up = Sprite::create(up_graph[up_cost[index][grade]]);
	else if (grade < 2) up = Sprite::create(noup_graph[up_cost[index][grade]]);
	else up = Sprite::create("Towers/upgrade_top.png");
	auto sale = Sprite::create(sale_graph[sell_money[index][grade]]);
	auto range = Sprite::create("Towers/range.png");
	range->setScale(range_table[index][grade] / range->getContentSize().width);
	up->setScale(CELL_SIZE / up->getContentSize().width);
	sale->setScale(CELL_SIZE / sale->getContentSize().width);
	range->setPosition(position);
	up->setPosition(Vec2(position.x, position.y + CELL_SIZE));
	sale->setPosition(Vec2(position.x, position.y - CELL_SIZE));
	my_scene->addChild(up);
	my_scene->addChild(sale);
	my_scene->addChild(range);
	curr_up = up; curr_sale = sale; curr_range = range;
}

void Tower::UpMenuGone(BaseLevelScene* my_scene)
{
	my_scene->removeChild(curr_up);
	my_scene->removeChild(curr_sale);
	my_scene->removeChild(curr_range);
	curr_up->release();
	curr_sale->release();
	curr_range->release();
}


void Bottle::attack(BaseLevelScene* my_scene, std::vector<Monster*>& monsters,int jia_su)
{
	interval = 0;
	int size = monsters.size();

	for (auto it = monsters.begin(); it != monsters.end(); it++) {
		if ((*it)->isDead==true) continue;
		Vec2 m_pos = (*it)->getPosition();
		//计算距离
		double distance = sqrt((m_pos.x - pos.x) * (m_pos.x - pos.x) + (m_pos.y - pos.y) * (m_pos.y - pos.y));
		if (distance < range / 2) {//若在范围内，则发动攻击
			Vec2 direction = m_pos - pos;
			direction.normalize();
			auto ang = direction.getAngle();
			ang = m_pos.y < pos.y ? (ang < 0 ? fabs(ang / PI) * 180 : ang / PI * 180) : (ang > 0 ? -ang / PI * 180 : ang / PI * 180);
			auto action1 = RotateTo::create(fabs(ang / (360*jia_su)), ang);//炮塔转向



			auto callback2 = CallFunc::create([this, my_scene, it, m_pos, ang,jia_su]() {

				tower_angle = ang;//角度更新
				ShellProduct(my_scene, (*it));//子弹的生成
				auto callback3 = CallFunc::create([it, this,jia_su]() {//子弹追踪
					auto m_pos = (*it)->getPosition();
					auto s_pos = curr_shell->getPosition();

					auto move = m_pos - s_pos;
					move.normalize();
					move = move * 10;
					//检测子弹位置和怪兽位置，并向该位置移动10个像素点
					auto action3 = MoveBy::create(10 / (speed*jia_su), move);
					curr_shell->runAction(action3);
					});
				auto callback4 = CallFunc::create([my_scene, it, this]() {
					if (((*it)->getPosition() - curr_shell->getPosition()).distance(Vec2(0, 0)) < 10.f) {
						curr_shell->stopAllActions();//一旦距离足够小，则停止动作
						ShellDemage(my_scene, it);
					}
					});
				de_time = 10 / (speed*jia_su);//不断重复检测与移动，以实现追击效果
				auto sequence3 = Sequence::create(callback3, DelayTime::create(de_time), callback4, nullptr);
				auto repeat = RepeatForever::create(sequence3);
				curr_shell->runAction(repeat);
				});
			auto sequence2 = Sequence::create(action1, callback2, nullptr);

			sprite_mark->runAction(sequence2);
			break;
		}
	}


}

void Bottle::ShellProduct(Scene* my_scene, Monster* monster)//产生子弹
{
	auto shell = Sprite::create(bottle_shell[grade]);
	Vec2 start_pos;
	start_pos.x = pos.x;/*+ (sprite_mark->getContentSize().height+shell->getContentSize().width)* cos(PI*tower_angle/180)/2;*/
	start_pos.y = pos.y;/* + (sprite_mark->getContentSize().height+shell->getContentSize().width)*sin(PI * tower_angle/180)/ 2;*/
	shell->setPosition(start_pos);
	shell->setScale(0.6);
	/*shell->setAnchorPoint(Vec2(0.5, 0.5));*/
	/*shell->runAction(RotateBy::create(0.001, -tower_angle));*/
	my_scene->addChild(shell);
	curr_shell = shell;
}



void Bottle::ShellDemage(BaseLevelScene* my_scene, std::vector<Monster*>::iterator it)//子弹对怪兽造成伤害后，子弹消失
{
	curr_shell->removeFromParent();//将子弹清除，并将其引用计数减一，自动释放
	CCLOG("(*it)->health        %d", (*it)->health);
	(*it)->health -= demage;
	CCLOG("(*it)->health        %d", (*it)->health);
	(*it)->updateHealthBar();
	if ((*it)->health <= 0) {
		my_scene->updateMoney((*it)->reward);
		(*it)->retain();
	   (*it)->toDie();
	}

}

























//Windmill------------------------------------------------------------------------------------------------------------
void Windmill::attack(BaseLevelScene*, std::vector<Monster*>& monsters,int jia_su)
{

}
