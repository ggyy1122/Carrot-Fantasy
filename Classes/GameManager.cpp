#include"GameManager.h"
#include<vector>
#include "json/document.h"
#include "json/rapidjson.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

#include<fstream>
#include"cocos2d.h"
#include <sstream>
#include "json/document.h"
#include"Music.h"

USING_NS_CC;
extern Music a;



//��̬��Ա�����Ķ���ͳ�ʼ��
GameManager* GameManager::instance = nullptr;
//��������ʵ��
GameManager* GameManager::getInstance(BaseLevelScene* scene) {
    if (!instance) {
        instance = new GameManager();
    }
    // ��������˳���������¹����ĳ���
    if (scene) {
        instance->setScene(scene);
    }
    return instance;
}
// ���õ�ǰ����
void GameManager::setScene(BaseLevelScene* scene) {
    currentScene = scene;
}
// ��ȡ��ǰ����
BaseLevelScene* GameManager::getScene() const {
    return currentScene;
}
// �ͷŵ���ʵ��
void GameManager::destroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}
//��ʼ���ؿ�����
void GameManager::initLevel(int level)
{
    levelId=level;
    path.clear();
    screenPath.clear();
    monsters.clear();
    NumOfDeadMonster=0;
    waveConfigs.clear();
    waveIndex=0;
    ClearMonsters();
    //��ʼ��·��
    initPath();
    //��ʼ������ͼ����Դ
    loadMonsterResources();
    //��ʼ���ܲ�
    initCarrot();
    auto _eventDispatcher = currentScene->getEventDispatcher();
    // ע���¼�������
    auto listener = EventListenerCustom::create("monster_path_complete", CC_CALLBACK_1(GameManager::onMonsterPathComplete, this));
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, currentScene);
    //���Ա�ҧЧ��
    /*
    currentScene->scheduleOnce([this](float dt) {
        carrot->getDamage();
        }, 2.0f, "pause_event");
        */

    //��ʼ������
    /*
    loadGameData("MonsterData_save_10s.json");
    startMonsterWaves();
    */
    //���ع���������Դ
    loadMonsterWaveConfig("MonsterWaves.json", "level"+std::to_string(level));
    //��ʼ���޲�
   // startMonsterWaves();
    
    

}
//��֡����״̬
void GameManager::update(float deltaTime) {
    if(carrot->getHP()<0)
    {
     carrot->changeHP(5);
    }
}

void GameManager::ClearMonsters()
{
    // ���� monsters ��һ�� std::vector<Sprite*>
    for (auto monster : monsters) {
        // �Ӹ��ڵ����Ƴ�����
        if (monster->getParent()) {
            monster->getParent()->removeChild(monster);
        }
        monster->stopAllActions();
        // �ͷž��������ڴ�
        monster->release();
    }

    // ��� vector
    monsters.clear();
}




//��ȡ�ؿ�·��
bool GameManager::loadPathForLevel(int levelId, const std::string& filePath)
{
    // ����Ѿ����ع��ùؿ���·������ֱ�ӷ���
    if (pathsCache.find(levelId) != pathsCache.end()) {
        return true;
    }

    // ���ļ��ж�ȡ JSON ����
    std::string fileContent = cocos2d::FileUtils::getInstance()->getStringFromFile(filePath);
    if (fileContent.empty()) { // ����ļ��Ƿ��ȡ�ɹ�
        CCLOG("Failed to load JSON file: %s", filePath.c_str());
        return false;
    }

    rapidjson::Document document;
    document.Parse(fileContent.c_str());

    // ��� JSON ��ʽ�Ƿ���Ч
    if (document.HasParseError() || !document.IsObject()) {
        CCLOG("Failed to parse JSON or invalid format: %s", filePath.c_str());
        return false;
    }

    // ���� JSON ������ȡָ���ؿ���·��
    for (auto& level : document.GetObject()) {
        int levelIdInFile = std::stoi(level.name.GetString()); // ��ȡ�ļ��еĹؿ����
        if (levelIdInFile == levelId) { // ƥ��ؿ����

            if (!level.value.IsArray()) { // ���·�������Ƿ�������
                CCLOG("Path data for level %d is not an array.", levelId);
                return false;
            }

            const auto& points = level.value.GetArray(); // ·��������
            std::vector<cocos2d::Vec2> path;

            // ʹ���������������
            for (rapidjson::Value::ConstValueIterator it = points.Begin(); it != points.End(); ++it) {
                if (!it->IsArray() || it->Size() != 2) { // ����ĸ�ʽ
                    CCLOG("Invalid point format in level %d.", levelId);
                    continue;
                }

                float x = (*it)[0].GetFloat();
                float y = (*it)[1].GetFloat();
                path.emplace_back(x, y); // ת��Ϊ Vec2 ������·��
            }

            if (path.empty()) { // ���·���Ƿ�Ϊ��
                CCLOG("No valid points found for level %d.", levelId);
                return false;
            }

            pathsCache[levelId] = path; // ����·������
            // ��������·����
            for (const auto& point : pathsCache[levelId]) {
                CCLOG("Grid Point: (%f, %f)", point.x, point.y);  // �����������
                Vec2 screenCenter = gridToScreenCenter(point);
                ScreenPaths[levelId].emplace_back(screenCenter);  // �洢���ĵ���Ļ����
                CCLOG("Center Screen Point: (%f, %f)", screenCenter.x, screenCenter.y);  // �����Ļ����
            }
            return true; // ���سɹ�
        }
    }

    CCLOG("Path for level %d not found in file: %s", levelId, filePath.c_str());
    return false; // δ�ҵ��ùؿ�·��
}
//��ʼ��·��
void GameManager::initPath()
{
    // ����·��
    if (pathsCache.find(levelId) == pathsCache.end()) {
        if (!loadPathForLevel(levelId, "paths.json")) {
            CCLOG("Failed to load path for level %d.", levelId);
            return;
        }
    }

    // ��ȡ���ؿ�·��
    path = pathsCache[levelId];
    CCLOG("Path for level %d:", levelId);
    for (const auto& point : path) {
        CCLOG("Point: (%f, %f)", point.x, point.y);
    }
    CCLOG("%f", currentScene->tileMap->getMapSize().height);
    for (const auto& gridPoint : path) {
        // ʹ�� gridToScreenCenter ��������������תΪ��Ļ����
        screenPath.push_back(gridToScreenCenter(gridPoint));
        float x=gridToScreenCenter(gridPoint).x;
        float y = gridToScreenCenter(gridPoint).y;
        CCLOG("ScreenPoint: (%f, %f)",x , y);
    }

}
//���ݴ浵��ʼ��
void GameManager::loadGameData(const std::string& fileName) {
    waveConfigs.clear();
    WaveConfig waveConfig;
    // 1. �����ļ�·������д·����
    std::string filePath = cocos2d::FileUtils::getInstance()->getWritablePath() + fileName;
    CCLOG("���ڼ��ش浵�ļ�: %s", filePath.c_str());

    // 2. ��ȡ�ļ�����
    std::ifstream ifs(filePath);
    if (!ifs.is_open()) {
        CCLOG("�޷��򿪴浵�ļ���%s", filePath.c_str());
        return;
    }

    std::stringstream buffer;
    buffer << ifs.rdbuf();  // ��ȡ�ļ����ݵ��ַ���
    std::string fileContent = buffer.str();
    ifs.close();
    CCLOG("�ļ�����: %s", fileContent.c_str());

    // 3. ʹ�� RapidJSON ���� JSON
    rapidjson::Document document;
    if (document.Parse(fileContent.c_str()).HasParseError()) {
        CCLOG("JSON ��������");
        return;
    }

    // 4. ��ȡ��һ����: livemonsters
    if (document.HasMember("livemonsters") && document["livemonsters"].IsArray()) {
        const rapidjson::Value& livingMonsters = document["livemonsters"];
        for (rapidjson::SizeType i = 0; i < livingMonsters.Size(); ++i) {
            const rapidjson::Value& monsterData = livingMonsters[i];
            if (monsterData.IsObject()) {
                std::string monsterName = monsterData["monsterName"].GetString();
                int pathIndex = monsterData["pathIndex"].GetInt();
                int health = monsterData["health"].GetInt();
                //���´�������
                produceMonsters(monsterName, pathIndex,health);
                CCLOG("readMonsters: name=%s, pathIndex=%d, health=%d", monsterName.c_str(), pathIndex, health);
            }
        }
    }

    // 5. ��ȡ�ڶ�����: currentWave
    if (document.HasMember("currentWave") && document["currentWave"].IsObject()) {
        const rapidjson::Value& currentWave = document["currentWave"];
        std::string monsterName = currentWave["monsterName"].GetString();
        int count = currentWave["count"].GetInt();
            // ��ȡ��������
            waveConfig.monsterName = monsterName;
            // ��ȡ��������
            waveConfig.count = count;
            // ��Ӳ�����
            waveConfigs.push_back(waveConfig);
            CCLOG("currentWave: name=%s, count=%d", monsterName.c_str(), count);
    }

    // 6. ��ȡ��������: waveIndex
    if (document.HasMember("waveIndex") && document["waveIndex"].IsInt()) {
        int waveIndex = document["waveIndex"].GetInt();
        CCLOG("currentIndex: %d", waveIndex);
        // ��������������õ�ǰ�Ĳ�����
        this->waveIndex = waveIndex;
    }

    // 7. ��ȡ���Ĳ���: upcomingWaves
    if (document.HasMember("upcomingWaves") && document["upcomingWaves"].IsArray()) {
        const rapidjson::Value& upcomingWaves = document["upcomingWaves"];
        for (rapidjson::SizeType i = 0; i < upcomingWaves.Size(); ++i) {
            const rapidjson::Value& waveData = upcomingWaves[i];
            if (waveData.IsObject()) {
                std::string monsterName = waveData["monsterName"].GetString();
                int count = waveData["count"].GetInt();
                // ��ȡ��������
                waveConfig.monsterName = monsterName;
                // ��ȡ��������
                waveConfig.count = count;
                // ��Ӳ�����
                waveConfigs.push_back(waveConfig);
                CCLOG("upComingWave: name=%s, count=%d", monsterName.c_str(), count);
            }
        }
    }

    CCLOG("Read All Data!");
}




//���޲�����������
void GameManager::loadMonsterWaveConfig(const std::string& filename, const std::string& levelName) {
    // �� JSON �ļ�
    std::string path = FileUtils::getInstance()->fullPathForFilename(filename);
    std::string fileContent = FileUtils::getInstance()->getStringFromFile(path);

    // ʹ�� rapidjson �����ļ�����
    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());

    if (doc.HasParseError()) {
        CCLOG("Error parsing JSON file: %s", filename.c_str());
        return;
    }

    // ����Ƿ����ָ���ؿ�������
    if (doc.HasMember(levelName.c_str())) {
        const rapidjson::Value& waves = doc[levelName.c_str()];
        for (rapidjson::SizeType i = 0; i < waves.Size(); ++i) {
            const rapidjson::Value& wave = waves[i];
            WaveConfig waveConfig;
            // ��ȡ���ﲨ��
            waveConfig.wave = wave["wave"].GetInt();
            // ��ȡ��������
            waveConfig.monsterName = wave["monsterName"].GetString();

            // ��ȡ��������
            waveConfig.count = wave["count"].GetInt();

            // ��ȡ���ɼ��
            const rapidjson::Value& spawnInterval = wave["spawnInterval"];
            waveConfig.spawnInterval[0] = spawnInterval[0].GetFloat();
            waveConfig.spawnInterval[1] = spawnInterval[1].GetFloat();

           

            // ��Ӳ�����
            waveConfigs.push_back(waveConfig);
            // ��ӡ����������Ϣ
            CCLOG("Wave %d - Monster: %s, Count: %d, Spawn Interval: %.2f - %.2f",
                waveConfigs.size(),
                waveConfig.monsterName.c_str(),
                waveConfig.count,
                waveConfig.spawnInterval[0],
                waveConfig.spawnInterval[1]
                );
        }
    }
    else {
        CCLOG("No such level: %s", levelName.c_str());
    }
}
//����һ������
void GameManager::produceMonsterWave(const WaveConfig& waveConfig) {
    // ÿһ����������ɺ���
    float delay = 0;
    for (int i = 0; i < waveConfig.count; ++i) {
        // ����һ�� 0 �� 2 ��֮������ʱ��
        delay+= cocos2d::RandomHelper::random_real(waveConfig.spawnInterval[0], waveConfig.spawnInterval[1]);

        // �ӳ����ɹ��ע���ӳٵ�ʱ��������ڵ�ǰʱ���
        cocos2d::Director::getInstance()->getScheduler()->schedule([=](float) {
            produceMonsters(waveConfig.monsterName,0);
            }, this, 0, 0,delay, false, "produceMonster" + std::to_string(i));
    }
}
//���ɹ��޲�
void GameManager::startMonsterWaves() {
   CCLOG("Starting wave %d", waveIndex);
   produceMonsterWave(waveConfigs[waveIndex]); // ���ɵ�ǰ���Ĺ���
    //++waveIndex; // ������һ��
    cocos2d::Director::getInstance()->getScheduler()->schedule([this](float) {
        if (waveIndex >= waveConfigs.size()-1) {
            CCLOG("All waves are complete.");
            cocos2d::Director::getInstance()->getScheduler()->unschedule("startWave", this); // ֹͣ������
            return;
        }

        ++waveIndex; // ������һ��
        CCLOG("Starting wave %d", waveIndex);
        produceMonsterWave(waveConfigs[waveIndex]); // ���ɵ�ǰ���Ĺ���
        
        }, this, 9.0f, false, "startWave"); // ÿ�� 9 �����һ��
}
//���ع���ͼ����Դ
void GameManager::loadMonsterResources() {
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/pig.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/yellowbat.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/blue.plist");
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("pig_0.png")) {
        CCLOG("Failed to load SpriteFrame 'pig_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("pig_1.png")) {
        CCLOG("Failed to load SpriteFrame 'pig_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("yellowbat_0.png")) {
        CCLOG("Failed to load SpriteFrame 'yellowbat_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("yellowbat_1.png")) {
        CCLOG("Failed to load SpriteFrame 'yellowbat_1.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("blue_0.png")) {
        CCLOG("Failed to load SpriteFrame 'blue_0.png'.");
    }
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("blue_1.png")) {
        CCLOG("Failed to load SpriteFrame 'blue_1.png'.");
    }
}
//�볡��Ч
void GameManager::playSpawnEffect(const cocos2d::Vec2& spawnPosition) {
    // 1. ����һ����ʱ�Ķ������� (�������ڹ���)
    auto spawnEffect = cocos2d::Sprite::create("Monsters/monster_start_1.png"); // �����ĵ�һ֡
    spawnEffect->setPosition(spawnPosition); // ������ʾ������λ��
    currentScene->addChild(spawnEffect); // �Ѷ�������ӵ�����

    // 2. ���ض�����֡
    cocos2d::Vector<cocos2d::SpriteFrame*> frames;
    frames.pushBack(cocos2d::SpriteFrame::create("Monsters/monster_start_1.png", cocos2d::Rect(0, 0, 64, 64)));
    frames.pushBack(cocos2d::SpriteFrame::create("Monsters/monster_start_2.png", cocos2d::Rect(0, 0, 64, 64)));

    // 3. ���� 2 ֡�Ķ��� (0.2s ÿ֡)
    auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
    auto animate = cocos2d::Animate::create(animation);

    // 4. ѭ������ 2 ��
    auto repeatAnimation = cocos2d::Repeat::create(animate, 2);

    // 5. ����������ɺ��Ƴ������ʱ��������
    auto removeEffect = cocos2d::CallFunc::create([spawnEffect]() {
        spawnEffect->removeFromParent(); // �Ƴ���������
        });

    // 6. �������� (���Ŷ��� + �Ƴ�)
    spawnEffect->setScale(1.5f); // �Ŵ󶯻�
    auto sequence = cocos2d::Sequence::create(repeatAnimation, removeEffect, nullptr);
    spawnEffect->runAction(sequence); // ���ж���
}
//����������������
void GameManager::produceMonsters(const std::string monsterName,const int startIndex,int health) {

    if(startIndex==0)
   playSpawnEffect(screenPath[0]);
    // �������ﲢ��ӵ�����
    auto Monster = Monster::create(monsterName,screenPath, startIndex);
    if (!Monster) {
        CCLOG("Failed to create monster.");
        return;
    }
    monsters.push_back(Monster); // ���浽�����б� 
    currentScene->addChild(Monster);
    if(health!=-1)
    {
        Monster->setHealth(health);
    }
}

//���չ��޵����յ��ʱ����źţ�������ﵽ���յ����߼�
void GameManager::onMonsterPathComplete(cocos2d::EventCustom* event)
{
    // ���¼��� userdata ��ȡ���޶���
    Monster* monster = static_cast<Monster*>(event->getUserData());

    if (monster) {
        CCLOG("Monster has completed the path. Perform further actions here.");
        if (monster->getHealth() > 0)//������Ѫ�͹���
        {
            carrot->getDamage();
            CCLOG("carrot'HP    %d",carrot->getHP());
        }
        monster->toDie();  //��������
        NumOfDeadMonster++;
    }
}



//�������ݴ浵����
void GameManager::saveMonstersDataToJson(const std::string& fileName) {
    // ���� JSON �ĵ�����
    rapidjson::Document document;
    document.SetObject();
    // ��ȡ��ǰ��������
    int currentWaveIndex = getCurrentWaveIndex();
    // 1. ������ŵĹ���
    rapidjson::Value livingMonsters(rapidjson::kArrayType);
    for (auto* monster : monsters) {
        if (monster->checkLive()) {  // �жϹ����Ƿ����
            rapidjson::Value monsterData(rapidjson::kObjectType);

            // monsterName
            monsterData.AddMember("monsterName", rapidjson::Value(monster->getMonsterName().c_str(), document.GetAllocator()), document.GetAllocator());

            // pathIndex
            monsterData.AddMember("pathIndex", monster->getPathIndex(), document.GetAllocator());

            // health
            monsterData.AddMember("health", monster->getHealth(), document.GetAllocator());

            // ������������ӵ�������
            livingMonsters.PushBack(monsterData, document.GetAllocator());
        }
    }
    document.AddMember("livemonsters", livingMonsters, document.GetAllocator());

    // 2. ���浱ǰ����Ϣ
    // ���㵱ǰ��ʣ��Ĺ�������
    int totalMonstersCount = 0;
    for (int i = 0; i <= currentWaveIndex; ++i) {
        totalMonstersCount += waveConfigs[i].count;
    }
    int remainingCount = totalMonstersCount - monsters.size();
    rapidjson::Value currentWave(rapidjson::kObjectType);
    currentWave.AddMember("monsterName", rapidjson::Value(waveConfigs[currentWaveIndex].monsterName.c_str(), document.GetAllocator()), document.GetAllocator());
    currentWave.AddMember("count", remainingCount, document.GetAllocator());
    document.AddMember("currentWave", currentWave, document.GetAllocator());
    //3.���浱ǰ���ı��
    document.AddMember("waveIndex", currentWaveIndex, document.GetAllocator()); // ���뵱ǰ������
    // 4. ����������Ĳ���Ϣ
    rapidjson::Value upcomingWaves(rapidjson::kArrayType);
    for (size_t i = currentWaveIndex + 1; i < waveConfigs.size(); ++i) {
        rapidjson::Value waveData(rapidjson::kObjectType);
        waveData.AddMember("monsterName", rapidjson::Value(waveConfigs[i].monsterName.c_str(), document.GetAllocator()), document.GetAllocator());
        waveData.AddMember("count", waveConfigs[i].count, document.GetAllocator());

        // ��ÿ����������ӵ�������
        upcomingWaves.PushBack(waveData, document.GetAllocator());
    }
    document.AddMember("upcomingWaves", upcomingWaves, document.GetAllocator());

    // ��ȡ��д·��
    std::string writablePath = FileUtils::getInstance()->getWritablePath();  // ��ȡ��д·��

    // �����ļ�·��
    std::string filePath = writablePath + fileName;

    // �����д�뵽ָ���ļ�·��
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    // д���ļ�
    std::ofstream ofs(filePath);  // ʹ�ÿ�д·��
    if (ofs.is_open()) {
        ofs << buffer.GetString();
        ofs.close();
        CCLOG("�浵�ɹ���%s", filePath.c_str());
    }
    else {
        CCLOG("�浵ʧ�ܣ�%s", filePath.c_str());
    }
}

//��ʼ���ܲ�
void GameManager::initCarrot() {
    // Ŀ��λ�õ����飬����levelId��1��2��3�ĳ����ؿ�
    std::vector<cocos2d::Vec2> dst1 = { Vec2(804, 640 - 196), Vec2(816, 640 - 284), Vec2(831, 640 - 287) };
    std::vector<cocos2d::Vec2> dst2 = { Vec2(854, 640 - 196), Vec2(866, 640 - 284), Vec2(881, 640 - 287) };
    // �����ܲ���Ѫ��Ϊ5
    carrot = Carrot::create(5, dst1[levelId - 1], dst2[levelId - 1]);
    currentScene->addChild(carrot); // ֱ�ӽ� Carrot ��Ϊһ��Node����ӵ�������
}
//�����״̬
bool GameManager::CheckLose()
{
   if(carrot->getHP()==0)
   {
       CCLOG("LOSE THE GAME!");
       return true;
   }
   return false;
}
//���Ӯ״̬
bool GameManager::CheckWin()
{
    if (carrot->getHP()>0&& NumOfDeadMonster==monsters.size())
    {
        CCLOG("WIN THE GAME!");
        return true;
    }
    return false;
}



//�߸�����ת��ͼ����Ĺ��ߺ���
Vec2 GameManager::gridToScreenCenter(const Vec2& gridPoint) {
    // �� tileMap ��ȡ��ͼ�߶ȣ�����������
    float mapHeight = currentScene->tileMap->getMapSize().height;
    // ת��Ϊ��Ļ����
    float screenX = gridPoint.x * (currentScene->tileSize.height) + (currentScene->tileSize.width) / 2;
    float screenY = (mapHeight - gridPoint.y - 1) * (currentScene->tileSize.height) + (currentScene->tileSize.height) / 2;
    return Vec2(screenX, screenY);
}