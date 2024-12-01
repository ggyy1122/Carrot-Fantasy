#include"Tower.h"


void Tower::build(Scene* my_scene, Vec2 position)
{
	grade = 0;
	auto base = Sprite::create(base_table[index]);
	base->setAnchorPoint(anchorpoint_table[index][0]);
	base->setPosition(position);
	my_scene->addChild(base);
	auto tower = Sprite::create(tower_table[index][grade]);
	tower->setAnchorPoint(anchorpoint_table[index][1]);
	tower->setPosition(position);
	my_scene->addChild(tower);
	remove_mark = tower;
}