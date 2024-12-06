#include"GameManager.h"
#include<string>
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

//���ع���ͼ����Դ
void GameManager::loadMonsterResources() {
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/pig.plist");
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("pig.png")) {
        CCLOG("Failed to load SpriteFrame 'pig.png'.");
    }
}

void GameManager::placeMonsters(int levelId) {

    std::string monsterName = "pig";  // ʹ�õĹ�������
    // �������ﲢ��ӵ�����
    auto pigMonster = Monster::create(monsterName);
    if (!pigMonster) {
        CCLOG("Failed to create monster.");
        return;
    }
   // currentScene->addChild(pigMonster, 3);
    monsters.push_back(pigMonster); // ���浽�����б� 
    //����baseLevelScene��Ⱦ
    if(currentScene)
    {
    currentScene->addChild(pigMonster);
    pigMonster->setPosition(screenPath[0]);
    }
}
void GameManager::updateMonsters(float deltaTime) {
    for (auto monster : monsters) {
        if (monster) {
            updateMonsterPosition(monster, deltaTime);  // ����ÿ�������λ��
        }
    }
}
void GameManager::updateMonsterPosition(Monster* monster, float deltaTime) {
    if (path.empty()) return;

    // ��ǰ·�����߼�
    int currentIndex = monster->getCurrentPathIndex();
    if (currentIndex >= path.size() - 1) {
        return; // �ѵ����յ�
    }

    cocos2d::Vec2 currentPos = monster->getPosition();
    cocos2d::Vec2 targetPos = screenPath[currentIndex + 1];

    // ���ٶ��ƶ�
    float speed = monster->getSpeed();
    cocos2d::Vec2 direction = (targetPos - currentPos).getNormalized();
    cocos2d::Vec2 newPos = currentPos + direction * speed * deltaTime;

    if (newPos.distance(targetPos) < speed * deltaTime) {
        newPos = targetPos;
        monster->setCurrentPathIndex(currentIndex + 1);
    }

    // ���¹���λ�ã���һ���Զ�������Ⱦ�����λ����Ⱦ��
    monster->setPosition(newPos);
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

//�߸�����ת��ͼ����Ĺ��ߺ���
Vec2 GameManager::gridToScreenCenter(const Vec2& gridPoint) {
    // �� tileMap ��ȡ��ͼ�߶ȣ�����������
    float mapHeight = currentScene->tileMap->getMapSize().height;
    // ת��Ϊ��Ļ����
    float screenX = gridPoint.x * (currentScene->tileSize.height) + (currentScene->tileSize.width) / 2;
    float screenY = (mapHeight - gridPoint.y - 1) * (currentScene->tileSize.height) + (currentScene->tileSize.height) / 2;
    return Vec2(screenX, screenY);
}