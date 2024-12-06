#include"Monster.h"
#include"MonsterConfigs.h"

// 初始化怪物

bool Monster::initWithPath(const std::string& monsterName, int levelId, const std::vector<Vec2>& path) {
    // 加载精灵帧资源
    std::string plistPath = "Monsters/" + monsterName + ".plist";
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistPath);

    // 检查精灵帧是否加载成功
    std::string frameName = monsterName + ".png";
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName)) {
        CCLOG("Failed to load SpriteFrame '%s'.", frameName.c_str());
        return false;
    }

    // 初始化精灵帧
    if (!Sprite::initWithSpriteFrameName(frameName)) {
        CCLOG("Failed to initialize sprite with frame '%s'.", frameName.c_str());
        return false;
    }
    //加载怪物属性
    MonsterConfig config = MonsterManager::getMonsterConfigByName(monsterName);
    // 设置怪物属性
    this->health = config.health;
    this->speed = config.speed;
    this->damage = config.damage;
    this->reward = config.reward;
    CCLOG("Monster Config - Sprite Frame: %s, Health: %d, Speed: %.2f, Damage: %d, Reward: %d",
        config.spriteFrameName.c_str(), config.health, config.speed, config.damage, config.reward);
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
    for (const auto& point : path) {
        CCLOG("RealPoint: (%f, %f)", point.x, point.y);
    }
    // 确保怪物在路径的起始点
    this->setPosition(path[0]);

    // 创建一个数组存储所有的动作
    cocos2d::Vector<cocos2d::FiniteTimeAction*> actions;

    // 遍历路径点，添加移动动作
    //利用speed变量控制实际的移动速度
    float distance;
    float moveTime;
    CCLOG("%f",speed);
    for (size_t i = 1; i < path.size(); ++i) { 
        // 从第二个点开始，因为怪物已在第一个点
        distance=path[i-1].distance(path[i]);//计算两点之间的距离
        moveTime=distance/(speed);
        // 打印移动时间
        CCLOG("Moving from (%f, %f) to (%f, %f), Distance: %f, MoveTime: %f seconds",
            path[i - 1].x, path[i - 1].y, path[i].x, path[i].y, distance, moveTime);
        auto moveTo = cocos2d::MoveTo::create(moveTime, path[i]); // 1.0f 表示移动时间
        actions.pushBack(moveTo);
    }

    // 动作序列：路径点移动 + 回调（移动完成的逻辑）
    auto sequence = cocos2d::Sequence::create(actions);

    // 运行动作
    this->runAction(sequence);
}
