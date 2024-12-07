#include"Monster.h"
#include"MonsterConfigs.h"

// ��ʼ������

bool Monster::initWithPath(const std::string& monsterName, const std::vector<Vec2>& path) {
    // ���ؾ���֡��Դ
    std::string plistPath = "Monsters/" + monsterName + ".plist";
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistPath);

    // ��龫��֡�Ƿ����
    std::string frameName = monsterName + "_0.png";
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName)) {
        CCLOG("Failed to load SpriteFrame '%s'.", frameName.c_str());
        return false;
    }

    // ��ʼ������
    if (!Sprite::initWithSpriteFrameName(frameName)) {
        CCLOG("Failed to initialize sprite with frame '%s'.", frameName.c_str());
        return false;
    }

    // ���ع�������
    MonsterConfig config = MonsterManager::getMonsterConfigByName(monsterName);
    this->health = config.health;
    this->speed = config.speed;
    this->damage = config.damage;
    this->reward = config.reward;
    CCLOG("Monster Config - Sprite Frame: %s, Health: %d, Speed: %.2f, Damage: %d, Reward: %d",
        config.spriteFrameName.c_str(), config.health, config.speed, config.damage, config.reward);

    // ���ع������߶���
    auto walkFrames = cocos2d::Vector<SpriteFrame*>();
    walkFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(monsterName + "_0.png"));
    walkFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(monsterName + "_1.png"));

    auto walkAnimation = Animation::createWithSpriteFrames(walkFrames, 0.5f);  // ��֡�������ٶȿ��Ե���
    auto walkAnimate = Animate::create(walkAnimation);
    auto repeatWalk = RepeatForever::create(walkAnimate);
    this->runAction(repeatWalk); // �����������߶���

    // ���ó�ʼλ��
    if (!path.empty()) {
        this->setPosition(path[0]);
        moveAlongPath(path);  // ����·���ƶ��߼�
    }

    return true;
}

void Monster::moveAlongPath(const std::vector<Vec2>& path) {
    if (path.empty()) {
        CCLOG("Path is empty, cannot move the monster.");
        return;
    }

    // ȷ��������·������ʼ��
    this->setPosition(path[0]);

    // ����һ������洢���еĶ���
    cocos2d::Vector<cocos2d::FiniteTimeAction*> actions;

    // ����·���㣬����ƶ�����
    float distance;
    float moveTime;
    for (size_t i = 1; i < path.size(); ++i) {
        // ��������֮��ľ��벢ȷ���ƶ�ʱ��
        distance = path[i - 1].distance(path[i]);
        moveTime = distance / speed;  // �����ٶȼ����ƶ�ʱ��

        // �����ƶ�����
        auto moveTo = cocos2d::MoveTo::create(moveTime, path[i]);

        // ��ÿ���ƶ�������ӵ�����������
        actions.pushBack(moveTo);
    }

    // �������У�·�����ƶ� + �ص����ƶ���ɵ��߼���
    auto sequence = cocos2d::Sequence::create(actions);

    // ���ж���
    this->runAction(sequence);
}