#include"Monster.h"
#include"MonsterConfigs.h"

// 初始化怪物

bool Monster::initWithPath(const std::string& monsterName, const std::vector<Vec2>& path, int startIndex) {
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
    this->startPosIndex=startIndex;
    this->health = config.health;
    this->speed = config.speed;
    this->damage = config.damage;
    this->reward = config.reward;
    this->name= monsterName;
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
    this->setPosition(path[startPosIndex]);
    // 记录终点元素
    endPos = path.back();

    // 创建一个数组存储所有的动作
    cocos2d::Vector<cocos2d::FiniteTimeAction*> actions;

    // 遍历路径点，添加移动动作
    float distance;
    float moveTime;
    for (size_t i = startPosIndex+1; i < path.size(); ++i) {
        // 计算两点之间的距离并确定移动时间
        distance = path[i - 1].distance(path[i]);
        moveTime = distance / speed;  // 根据速度计算移动时间

        // 创建移动动作
        auto moveTo = cocos2d::MoveTo::create(moveTime, path[i]);
        //在每个MoveTo后加入一个回调来更新pathIndex
        auto updatePathIndex = cocos2d::CallFunc::create([this, i]() {
            this->PathIndex = i; // 更新怪物的 pathIndex 成员变量
            CCLOG("Monster %p pathIndex updated to %d", this, i);
            });

        // 将每个移动动作添加到动作数组中
        actions.pushBack(moveTo);
        actions.pushBack(updatePathIndex); // 在移动到点后，更新 pathIndex
    }

    // 在路径结束时调用的回调函数，调用 toDie
    auto onPathComplete = cocos2d::CallFunc::create([this]() {
        CCLOG("Monster %p has completed its path. Calling toDie().", this);
        this->toDie(); // 在路径完成后调用 toDie()
        });

    // 动作序列：路径点移动 + 回调（移动完成的逻辑）
    actions.pushBack(onPathComplete); // 在最后添加回调

    // 创建动作序列
    auto sequence = cocos2d::Sequence::create(actions);

    // 运行动作
    this->runAction(sequence);

}
void Monster::toDie()
{
    CCLOG("Monster %p is dying.", this);

    // 1. 标记死亡并隐藏怪物
    this->isDead = true;
    this->setVisible(false); // 隐藏怪物

    // 2. 创建一个新的临时精灵用于播放死亡动画
    auto deathSprite = cocos2d::Sprite::create();
    if (!deathSprite) {
        CCLOG("Failed to create death sprite.");
        return;
    }

    // 3. 将死亡动画精灵添加到与 Monster 相同的父节点上
    this->getParent()->addChild(deathSprite);
    deathSprite->setPosition(this->getPosition());  // 设置死亡动画精灵位置与怪物相同
    deathSprite->setScale(1.5f);
    // 4. 加载死亡动画的 4 张图片（假设你有 "dead_1.png", "dead_2.png", "dead_3.png", "dead_4.png"）
    cocos2d::Vector<cocos2d::SpriteFrame*> frames;
    for (int i = 1; i <= 4; ++i) {
        std::string frameName = "Monsters/dead_" + std::to_string(i) + ".png";
        auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 64, 64)); // 假设每张图片的大小是 64x64
        if (frame) {
            frames.pushBack(frame);
        }
        else {
            CCLOG("Failed to load frame: %s", frameName.c_str());
        }
    }

    // 5. 如果没有加载到动画帧，直接返回
    if (frames.empty()) {
        CCLOG("No frames found for death animation, skipping.");
        return;
    }

    // 6. 创建动画，每帧持续 0.2 秒
    auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
    auto animate = cocos2d::Animate::create(animation);

    // 7. 动画完成后删除临时的死亡动画精灵
    auto onDeathComplete = cocos2d::CallFunc::create([deathSprite]() {
        CCLOG("Death animation completed, removing death sprite.");
        deathSprite->removeFromParent();
        });

    // 8. 播放死亡动画，动画结束后清理临时精灵
    deathSprite->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));

    // 9. 删除怪物本身
    this->removeFromParent();
}