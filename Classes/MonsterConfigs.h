#pragma once

#include <string>

struct MonsterConfig {
    std::string spriteFrameName;  // 怪物图片资源
    int health;                   // 生命值
    float speed;                  // 移动速度
    int damage;                   // 攻击力
    int reward;                   // 击杀奖励
};

// 声明各个怪物的配置,只是声明，没有定义
extern MonsterConfig pigConfig;
extern MonsterConfig wolfConfig;
