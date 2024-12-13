#include"Tower.h"
#include"BaseLevelScene.h"
#include<cmath>

#define CELL_SIZE 64
const double PI = 3.1415926;

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
	my_scene->updatemoney(-build_cost[index]);
	pos = position;
	interval = 0;
}

void Tower::destroy(BaseLevelScene* my_scene)
{
	my_scene->removeChild(sprite_mark);
	my_scene->removeChild(sp_base);
	sprite_mark->release();
	sp_base->release();
	my_scene->updatemoney(sell_money[index][grade]);
}

void Tower::update(BaseLevelScene* my_scene, Vec2 position)
{
	if (grade >= 2 || my_scene->money < up_cost[index][grade]) return;
	my_scene->updatemoney(-up_cost[index][grade]);
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


void Bottle::attack(BaseLevelScene* my_scene, std::vector<Monster*>& monsters)
{
	/*Director::getInstance()->end();*/
	/*Director::getInstance()->end();*/
	interval = 0;
	int size = monsters.size();

	for (auto it = monsters.begin(); it != monsters.end(); it++) {
		if ((*it)->health <= 0) continue;
		Vec2 m_pos = (*it)->getPosition();
		//计算距离
		double distance = sqrt((m_pos.x - pos.x) * (m_pos.x - pos.x) + (m_pos.y - pos.y) * (m_pos.y - pos.y));
		if (distance < range / 2) {//若在范围内，则发动攻击


			Vec2 direction = m_pos - pos;
			direction.normalize();
			auto ang = direction.getAngle();
			ang = m_pos.y < pos.y ? (ang < 0 ? fabs(ang / PI) * 180 : ang / PI * 180) : (ang > 0 ? -ang / PI * 180 : ang / PI * 180);
			auto action1 = RotateTo::create(fabs(ang / 360), ang);//炮塔转向



			auto callback2 = CallFunc::create([this, my_scene, it, m_pos, ang]() {

				tower_angle = ang;//角度更新
				ShellProduct(my_scene, (*it));//子弹的生成
				//Vec2 start_pos = curr_shell->getPosition();//获取初始位置
				//curr_dis = sqrt((m_pos.x - start_pos.x) * (m_pos.x - start_pos.x) + (m_pos.y - start_pos.y) * (m_pos.y - start_pos.y)); 
				//auto action2 = MoveTo::create(curr_dis / speed, m_pos);
				//auto callback1 = CallFunc::create([this, my_scene, &monsters, it, ang]() {
				//	ShellDemage(my_scene, monsters, it); });
				//auto sequence1 = Sequence::create(action2, callback1, nullptr);
				//curr_shell->runAction(sequence1); 

				auto callback3 = CallFunc::create([it, this]() {//子弹追踪
					auto m_pos = (*it)->getPosition();
					auto s_pos = curr_shell->getPosition();

					auto move = m_pos - s_pos;
					move.normalize();
					move = move * 10;
					//检测子弹位置和怪兽位置，并向该位置移动10个像素点
					auto action3 = MoveBy::create(10 / speed, move);
					curr_shell->runAction(action3);
					});
				auto callback4 = CallFunc::create([my_scene, it, this]() {
					if (((*it)->getPosition() - curr_shell->getPosition()).distance(Vec2(0, 0)) < 10.f) {
						curr_shell->stopAllActions();//一旦距离足够小，则停止动作
						ShellDemage(my_scene, it);
					}
					});
				de_time = 10 / speed;//不断重复检测与移动，以实现追击效果
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
	(*it)->health -= demage;

	if ((*it)->health <= 0) {
		my_scene->updatemoney((*it)->reward);
		(*it)->retain();
		my_scene->removeChild(*it);
	}

}

























//Windmill------------------------------------------------------------------------------------------------------------
void Windmill::attack(BaseLevelScene*, std::vector<Monster*>& monsters)
{

}
