#include"Monster.h"

#include"music.h"
#include"BaseLevelScene.h"
extern Music a;
int DeadCount=0;


//��̬�����������޶���
Monster* Monster::create(const std::string& monsterName, const std::vector<Vec2>& path, int startIndex, bool pause) {
    Monster* monster;
    //�ж��Ƿ���Boss
    if(monsterName.find("Boss")==0)
    {
        if(monsterName=="BossYellow")
       monster=new (std::nothrow) BossYellow();//ʹ��new����������ڴ棬����һ��BossYellow����
        else
        {
            CCLOG("UNKONW BOSS: %s",monsterName.c_str());
            return nullptr;
        }         
    }
    else
    monster = new (std::nothrow) Monster();//ʹ��new����������ڴ棬����һ��Monster����
    if (monster && monster->initWithPath(monsterName, path, startIndex, pause))//�ж��Ƿ񴴽��ɹ��ͳ�ʼ���ɹ�
    {
        monster->autorelease();
        //���Ѫ��
        monster->_HP = cocos2d::ui::LoadingBar::create("Monsters/HPbar.png");
        monster->_HP->setPercent(100); // ��ʼѪ��Ϊ��Ѫ
        monster->_HP->setPosition(Vec2(monster->getContentSize().width / 2, monster->getContentSize().height * 1.3f));
        monster->addChild(monster->_HP, 10);
        //���Ѫ������
        monster->hpback = Sprite::create("Monsters/HPBackground.png");
        monster->hpback->setPosition(Vec2(monster->getContentSize().width / 2, monster->getContentSize().height * 1.3f));
        monster->addChild(monster->hpback, 9);
        monster->_HP->setVisible(false);
        monster->hpback->setVisible(false);
        return monster;
    }
    CC_SAFE_DELETE(monster);//����������ʼ��ʧ�ܣ��ͷ��ڴ沢����nullptr
    return nullptr;
}

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
    this->maxHp=this->health=config.health;
    this->speed=config.speed;
    this->damage=config.damage;
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
//���޵��ƶ��߼�
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
    actions.pushBack(onPathComplete); // ������������
    auto sequence = cocos2d::Sequence::create(actions);
    if (pause) {
        auto delay = DelayTime::create(3.0f); // ����һ���ӳ�3��Ķ���
        auto delayedSequence = Sequence::create(delay, sequence, nullptr);  // ���ӳٶ��������еĶ������н������
        speedaction = Speed::create(delayedSequence, beishu); // ִ���µ����ж��������ӳ�3�룬��ִ��ԭ����sequence��
    }
    else
    {
        speedaction = Speed::create(sequence, beishu);
    }
    this->runAction(speedaction);
}
//�ù�������
void Monster::toDie(BaseLevelScene*my_scene)
{
    if(isDead)
     return;
    DeadCount++;
    CCLOG("Monster %p is dying.", this);
    my_scene->updateMoney(reward);
    // 1. ������������ع���
    this->isDead = true;
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
    this->retain();
    my_scene->removeChild(this);
    if (my_scene->tar_m == this) {//���������ֱ���������ô�����������������
        my_scene->isTarget = 0;
        my_scene->tar_m = nullptr;
    }
}
//����Ѫ���ɼ�
void Monster::setHpVisible(bool isVisible) 
{ 
    _HP->setVisible(isVisible); 
     hpback->setVisible(isVisible); 
     ishpvs = true; 
}
//����Ѫ��
void  Monster::updateHealthBar() {
    float percentage = (static_cast<float>(health) / maxHp) * 100;
    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;
    _HP->setPercent(percentage); // ����Ѫ���ٷֱ�
}
//��������
void  Monster::getHurt(int value)
{
    health-=value;
}