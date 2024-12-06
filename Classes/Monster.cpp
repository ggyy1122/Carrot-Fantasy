#include"Monster.h"
#include"MonsterConfigs.h"

// ��ʼ������

bool Monster::initWithPath(const std::string& monsterName, int levelId, const std::vector<Vec2>& path) {
    // ���ؾ���֡��Դ
    std::string plistPath = "Monsters/" + monsterName + ".plist";
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistPath);

    // ��龫��֡�Ƿ���سɹ�
    std::string frameName = monsterName + ".png";
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName)) {
        CCLOG("Failed to load SpriteFrame '%s'.", frameName.c_str());
        return false;
    }

    // ��ʼ������֡
    if (!Sprite::initWithSpriteFrameName(frameName)) {
        CCLOG("Failed to initialize sprite with frame '%s'.", frameName.c_str());
        return false;
    }
    //���ع�������
    MonsterConfig config = MonsterManager::getMonsterConfigByName(monsterName);
    // ���ù�������
    this->health = config.health;
    this->speed = config.speed;
    this->damage = config.damage;
    this->reward = config.reward;
    CCLOG("Monster Config - Sprite Frame: %s, Health: %d, Speed: %.2f, Damage: %d, Reward: %d",
        config.spriteFrameName.c_str(), config.health, config.speed, config.damage, config.reward);
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
    for (const auto& point : path) {
        CCLOG("RealPoint: (%f, %f)", point.x, point.y);
    }
    // ȷ��������·������ʼ��
    this->setPosition(path[0]);

    // ����һ������洢���еĶ���
    cocos2d::Vector<cocos2d::FiniteTimeAction*> actions;

    // ����·���㣬����ƶ�����
    //����speed��������ʵ�ʵ��ƶ��ٶ�
    float distance;
    float moveTime;
    CCLOG("%f",speed);
    for (size_t i = 1; i < path.size(); ++i) { 
        // �ӵڶ����㿪ʼ����Ϊ�������ڵ�һ����
        distance=path[i-1].distance(path[i]);//��������֮��ľ���
        moveTime=distance/(speed);
        // ��ӡ�ƶ�ʱ��
        CCLOG("Moving from (%f, %f) to (%f, %f), Distance: %f, MoveTime: %f seconds",
            path[i - 1].x, path[i - 1].y, path[i].x, path[i].y, distance, moveTime);
        auto moveTo = cocos2d::MoveTo::create(moveTime, path[i]); // 1.0f ��ʾ�ƶ�ʱ��
        actions.pushBack(moveTo);
    }

    // �������У�·�����ƶ� + �ص����ƶ���ɵ��߼���
    auto sequence = cocos2d::Sequence::create(actions);

    // ���ж���
    this->runAction(sequence);
}
