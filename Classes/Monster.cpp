#include"Monster.h"
#include"MonsterConfigs.h"
#include"music.h"
extern Music a;
int Monster::DeadCount=0;
// ��ʼ������

bool Monster::initWithPath(const std::string& monsterName, const std::vector<Vec2>& path, int startIndex,bool pause) {
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
    this->startPosIndex=startIndex;
    this->maxHp=this->health = config.health;
    this->speed = config.speed;
    this->damage = config.damage;
    this->reward = config.reward;
    this->name= monsterName;
    this->pause=pause;
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
        moveAlongPath(path); // ��������·���ƶ��߼�
    }

    return true;
}

void Monster::moveAlongPath(const std::vector<Vec2>& path) {
    if (path.empty()) {
        CCLOG("Path is empty, cannot move the monster.");
        return;
    }

    // ȷ��������·������ʼ��
    this->setPosition(path[startPosIndex]);
    // ��¼�յ�Ԫ��
    endPos = path.back();

    // ����һ������洢���еĶ���
    cocos2d::Vector<cocos2d::FiniteTimeAction*> actions;

    // ����·���㣬����ƶ�����
    float distance;
    float moveTime;
    for (size_t i = startPosIndex+1; i < path.size(); ++i) {
        // ��������֮��ľ��벢ȷ���ƶ�ʱ��
        distance = path[i - 1].distance(path[i]);
        moveTime = distance / speed;  // �����ٶȼ����ƶ�ʱ��

        // �����ƶ�����
        auto moveTo = cocos2d::MoveTo::create(moveTime, path[i]);
        //��ÿ��MoveTo�����һ���ص�������pathIndex
        auto updatePathIndex = cocos2d::CallFunc::create([this, i]() {
            this->PathIndex = i; // ���¹���� pathIndex ��Ա����
            CCLOG("Monster %p pathIndex updated to %d", this, i);
            });

        // ��ÿ���ƶ�������ӵ�����������
        actions.pushBack(moveTo);
        actions.pushBack(updatePathIndex); // ���ƶ�����󣬸��� pathIndex
    }
    // ��·�����ʱ�����¼�
    auto onPathComplete = cocos2d::CallFunc::create([this]() {
        CCLOG("Monster %p has completed its path.", this);

        // �����¼�
        cocos2d::EventCustom event("monster_path_complete");
        // �����޶���ָ�봫�ݸ��¼�
        event.setUserData(this); 

        // ��ȡ�¼��ַ������ַ��¼�
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->dispatchEvent(&event);
        });

    actions.pushBack(onPathComplete);

    // ������������
    auto sequence = cocos2d::Sequence::create(actions);

    if (pause) {
        // ����һ���ӳ�3��Ķ���
        auto delay = DelayTime::create(3.0f);

        // ���ӳٶ��������еĶ������н������
        auto delayedSequence = Sequence::create(delay, sequence, nullptr);

        // ִ���µ����ж��������ӳ�3�룬��ִ��ԭ����sequence��
        this->runAction(delayedSequence);
    }
    else
    {
    this->runAction(sequence);
    }

}
//�ù�������
void Monster::toDie()
{
    if(isDead)
     return;
    DeadCount++;
    CCLOG("Monster %p is dying.", this);

    // 1. ������������ع���
    this->isDead = true;
    this->setVisible(false); // ���ع���

    // 2. ����һ���µ���ʱ�������ڲ�����������
    auto deathSprite = cocos2d::Sprite::create();
    if (!deathSprite) {
        CCLOG("Failed to create death sprite.");
        return;
    }

    // 3. ����������������ӵ��� Monster ��ͬ�ĸ��ڵ���
    this->getParent()->addChild(deathSprite);
    deathSprite->setPosition(this->getPosition());  // ����������������λ���������ͬ
    deathSprite->setScale(1.5f);
    // 4. �������������� 4 ��ͼƬ
    cocos2d::Vector<cocos2d::SpriteFrame*> frames;
    for (int i = 1; i <= 4; ++i) {
        std::string frameName = "Monsters/dead_" + std::to_string(i) + ".png";
        auto frame = cocos2d::SpriteFrame::create(frameName, cocos2d::Rect(0, 0, 64, 64)); // ����ÿ��ͼƬ�Ĵ�С�� 64x64
        if (frame) {
            frames.pushBack(frame);
        }
        else {
            CCLOG("Failed to load frame: %s", frameName.c_str());
        }
    }

    // 5. ���û�м��ص�����֡��ֱ�ӷ���
    if (frames.empty()) {
        CCLOG("No frames found for death animation, skipping.");
        return;
    }

    // 6. ����������ÿ֡���� 0.2 ��
    auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
    auto animate = cocos2d::Animate::create(animation);

    // 7. ������ɺ�ɾ����ʱ��������������
    auto onDeathComplete = cocos2d::CallFunc::create([deathSprite]() {
        CCLOG("Death animation completed, removing death sprite.");
        deathSprite->removeFromParent();
        });

    // 8. ������������������������������ʱ����
    deathSprite->runAction(cocos2d::Sequence::create(animate, onDeathComplete, nullptr));
    a.normalSound();
    // 9. ɾ�����ﱾ��
   this->stopAllActions();
}