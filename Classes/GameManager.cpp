#include"GameManager.h"
#include<vector>
#include "json/document.h"
#include "json/rapidjson.h"

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
    initPath();
    // 1. �ȼ��ز��ε�����
    loadMonsterWaveConfig("MonsterWaves.json", "level"+std::to_string(level));
    // 2. �������������߼�
    startMonsterWaves();
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

//���޲�����������
void GameManager::loadMonsterWaveConfig(const std::string& filename, const std::string& levelName) {
    waveConfigs.clear();
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

            // ��ȡ·������
            const rapidjson::Value& path = wave["path"];
            for (rapidjson::SizeType j = 0; j < path.Size(); ++j) {
                Vec2 point(path[j][0].GetFloat(), path[j][1].GetFloat());
                waveConfig.path.push_back(point);
            }

            // ��Ӳ�����
            waveConfigs.push_back(waveConfig);
            // ��ӡ����������Ϣ
            CCLOG("Wave %d - Monster: %s, Count: %d, Spawn Interval: %.2f - %.2f, Path Size: %d",
                waveConfigs.size(),
                waveConfig.monsterName.c_str(),
                waveConfig.count,
                waveConfig.spawnInterval[0],
                waveConfig.spawnInterval[1],
                waveConfig.path.size());
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
            produceMonsters(waveConfig.monsterName);
            }, this, 0, 0,delay, false, "produceMonster" + std::to_string(i));
    }
}
//���ɹ��޲�
void GameManager::startMonsterWaves() {
    waveIndex = 0; // ��ʼ����������
    produceMonsterWave(waveConfigs[waveIndex]); // ���ɵ�ǰ���Ĺ���
    ++waveIndex; // ������һ��
    cocos2d::Director::getInstance()->getScheduler()->schedule([this](float) {
        if (waveIndex >= waveConfigs.size()) {
            CCLOG("All waves are complete.");
            cocos2d::Director::getInstance()->getScheduler()->unschedule("startWave", this); // ֹͣ������
            return;
        }

        CCLOG("Starting wave %d", waveIndex + 1);
        produceMonsterWave(waveConfigs[waveIndex]); // ���ɵ�ǰ���Ĺ���
        ++waveIndex; // ������һ��
        }, this, 12.0f, false, "startWave"); // ÿ�� 12 �����һ��
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
//����������������
void GameManager::produceMonsters(const std::string monsterName) {
    // �������ﲢ��ӵ�����
    auto Monster = Monster::create(monsterName,screenPath);
    if (!Monster) {
        CCLOG("Failed to create monster.");
        return;
    }
    monsters.push_back(Monster); // ���浽�����б� 
    currentScene->addChild(Monster);
    numOfLiveMonster++;
}
//��֡���¹����״̬
void GameManager::updateMonsters(float deltaTime) {
    for (auto monster : monsters) {
        if (monster) {
         
        }
    }
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