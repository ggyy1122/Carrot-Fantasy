#include"Monster.h"
#include"MonsterConfigs.h"

// 初始化怪物

bool Monster::initWithPath(const std::string& monsterName, const std::vector<Vec2>& path) {
    // 加载精灵帧资源
    std::string plistPath = "Monsters/" + monsterName + ".plist";
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistPath);

    // 检查精灵帧是否加载
    std::string frameName = monsterName + "_0.png";
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName)) {
        CCLOG("Failed to load SpriteFrame '%s'.", frameName.c_str());
        return false;
    }

    // 初始化精灵
    if (!Sprite::initWithSpriteFrameName(frameName)) {
        CCLOG("Failed to initialize sprite with frame '%s'.", frameName.c_str());
        return false;
    }

    // 加载怪物配置
    MonsterConfig config = MonsterManager::getMonsterConfigByName(monsterName);
    this->health = config.health;
    this->speed = config.speed;
    this->damage = config.damage;
    this->reward = config.reward;
    CCLOG("Monster Config - Sprite Frame: %s, Health: %d, Speed: %.2f, Damage: %d, Reward: %d",
        config.spriteFrameName.c_str(), config.health, config.speed, config.damage, config.reward);

    // 加载怪物行走动画
    auto walkFrames = cocos2d::Vector<SpriteFrame*>();
    walkFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(monsterName + "_0.png"));
    walkFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(monsterName + "_1.png"));

    auto walkAnimation = Animation::createWithSpriteFrames(walkFrames, 0.5f);  // 两帧动画，速度可以调节
    auto walkAnimate = Animate::create(walkAnimation);
    auto repeatWalk = RepeatForever::create(walkAnimate);
    this->runAction(repeatWalk); // 持续播放行走动画

    // 设置初始位置
    if (!path.empty()) {
        this->setPosition(path[0]);
        moveAlongPath(path);  // 启动路径移动逻辑
    }

    return true;
}

void Monster::moveAlongPath(const std::vector<Vec2>& path) {
    if (path.empty()) {
        CCLOG("Path is empty, cannot move the monster.");
        return;
    }

    // 确保怪物在路径的起始点
    this->setPosition(path[0]);

    // 创建一个数组存储所有的动作
    cocos2d::Vector<cocos2d::FiniteTimeAction*> actions;

    // 遍历路径点，添加移动动作
    float distance;
    float moveTime;
    for (size_t i = 1; i < path.size(); ++i) {
        // 计算两点之间的距离并确定移动时间
        distance = path[i - 1].distance(path[i]);
        moveTime = distance / speed;  // 根据速度计算移动时间

        // 创建移动动作
        auto moveTo = cocos2d::MoveTo::create(moveTime, path[i]);

        // 将每个移动动作添加到动作数组中
        actions.pushBack(moveTo);
    }

    // 动作序列：路径点移动 + 回调（移动完成的逻辑）
    auto sequence = cocos2d::Sequence::create(actions);

    // 运行动作
    this->runAction(sequence);
}