#include"GameManager.h"
#include<string>
#include<vector>
#include "json/document.h"
#include "json/rapidjson.h"

//静态成员变量的定义和初始化
GameManager* GameManager::instance = nullptr;
//创建单例实例
GameManager* GameManager::getInstance(BaseLevelScene* scene) {
    if (!instance) {
        instance = new GameManager();
    }
    // 如果传入了场景，则更新关联的场景
    if (scene) {
        instance->setScene(scene);
    }
    return instance;
}
// 设置当前场景
void GameManager::setScene(BaseLevelScene* scene) {
    currentScene = scene;
}
// 获取当前场景
BaseLevelScene* GameManager::getScene() const {
    return currentScene;
}
// 释放单例实例
void GameManager::destroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}
//初始化关卡数据
void GameManager::initLevel(int level)
{
    levelId=level;
    path.clear();
    screenPath.clear();
    monsters.clear();
    initPath();
}
//初始化路径
void GameManager::initPath()
{
    // 加载路径
    if (pathsCache.find(levelId) == pathsCache.end()) {
        if (!loadPathForLevel(levelId, "paths.json")) {
            CCLOG("Failed to load path for level %d.", levelId);
            return;
        }
    }

    // 获取本关卡路径
    path = pathsCache[levelId];
    CCLOG("Path for level %d:", levelId);
    for (const auto& point : path) {
        CCLOG("Point: (%f, %f)", point.x, point.y);
    }
    CCLOG("%f", currentScene->tileMap->getMapSize().height);
    for (const auto& gridPoint : path) {
        // 使用 gridToScreenCenter 函数将网格坐标转为屏幕坐标
        screenPath.push_back(gridToScreenCenter(gridPoint));
        float x=gridToScreenCenter(gridPoint).x;
        float y = gridToScreenCenter(gridPoint).y;
        CCLOG("ScreenPoint: (%f, %f)",x , y);
    }

}

//加载怪兽图像资源
void GameManager::loadMonsterResources() {
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/pig.plist");
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("pig.png")) {
        CCLOG("Failed to load SpriteFrame 'pig.png'.");
    }
}

void GameManager::placeMonsters(int levelId) {

    std::string monsterName = "pig";  // 使用的怪物名称
    // 创建怪物并添加到场景
    auto pigMonster = Monster::create(monsterName);
    if (!pigMonster) {
        CCLOG("Failed to create monster.");
        return;
    }
   // currentScene->addChild(pigMonster, 3);
    monsters.push_back(pigMonster); // 保存到怪物列表 
    //交给baseLevelScene渲染
    if(currentScene)
    {
    currentScene->addChild(pigMonster);
    pigMonster->setPosition(screenPath[0]);
    }
}
void GameManager::updateMonsters(float deltaTime) {
    for (auto monster : monsters) {
        if (monster) {
            updateMonsterPosition(monster, deltaTime);  // 更新每个怪物的位置
        }
    }
}
void GameManager::updateMonsterPosition(Monster* monster, float deltaTime) {
    if (path.empty()) return;

    // 当前路径点逻辑
    int currentIndex = monster->getCurrentPathIndex();
    if (currentIndex >= path.size() - 1) {
        return; // 已到达终点
    }

    cocos2d::Vec2 currentPos = monster->getPosition();
    cocos2d::Vec2 targetPos = screenPath[currentIndex + 1];

    // 按速度移动
    float speed = monster->getSpeed();
    cocos2d::Vec2 direction = (targetPos - currentPos).getNormalized();
    cocos2d::Vec2 newPos = currentPos + direction * speed * deltaTime;

    if (newPos.distance(targetPos) < speed * deltaTime) {
        newPos = targetPos;
        monster->setCurrentPathIndex(currentIndex + 1);
    }

    // 更新怪兽位置（这一行自动触发渲染引擎的位移渲染）
    monster->setPosition(newPos);
}
//读取关卡路径
bool GameManager::loadPathForLevel(int levelId, const std::string& filePath)
{
    // 如果已经加载过该关卡的路径，则直接返回
    if (pathsCache.find(levelId) != pathsCache.end()) {
        return true;
    }

    // 从文件中读取 JSON 数据
    std::string fileContent = cocos2d::FileUtils::getInstance()->getStringFromFile(filePath);
    if (fileContent.empty()) { // 检查文件是否读取成功
        CCLOG("Failed to load JSON file: %s", filePath.c_str());
        return false;
    }

    rapidjson::Document document;
    document.Parse(fileContent.c_str());

    // 检查 JSON 格式是否有效
    if (document.HasParseError() || !document.IsObject()) {
        CCLOG("Failed to parse JSON or invalid format: %s", filePath.c_str());
        return false;
    }

    // 遍历 JSON 对象，提取指定关卡的路径
    for (auto& level : document.GetObject()) {
        int levelIdInFile = std::stoi(level.name.GetString()); // 获取文件中的关卡编号
        if (levelIdInFile == levelId) { // 匹配关卡编号

            if (!level.value.IsArray()) { // 检查路径数据是否是数组
                CCLOG("Path data for level %d is not an array.", levelId);
                return false;
            }

            const auto& points = level.value.GetArray(); // 路径点数组
            std::vector<cocos2d::Vec2> path;

            // 使用数组迭代器遍历
            for (rapidjson::Value::ConstValueIterator it = points.Begin(); it != points.End(); ++it) {
                if (!it->IsArray() || it->Size() != 2) { // 检查点的格式
                    CCLOG("Invalid point format in level %d.", levelId);
                    continue;
                }

                float x = (*it)[0].GetFloat();
                float y = (*it)[1].GetFloat();
                path.emplace_back(x, y); // 转换为 Vec2 并存入路径
            }

            if (path.empty()) { // 检查路径是否为空
                CCLOG("No valid points found for level %d.", levelId);
                return false;
            }

            pathsCache[levelId] = path; // 缓存路径数据
            // 遍历网格路径点
            for (const auto& point : pathsCache[levelId]) {
                CCLOG("Grid Point: (%f, %f)", point.x, point.y);  // 输出网格坐标
                Vec2 screenCenter = gridToScreenCenter(point);
                ScreenPaths[levelId].emplace_back(screenCenter);  // 存储中心点屏幕坐标
                CCLOG("Center Screen Point: (%f, %f)", screenCenter.x, screenCenter.y);  // 输出屏幕坐标
            }
            return true; // 加载成功
        }
    }

    CCLOG("Path for level %d not found in file: %s", levelId, filePath.c_str());
    return false; // 未找到该关卡路径
}

//瓦格坐标转地图坐标的工具函数
Vec2 GameManager::gridToScreenCenter(const Vec2& gridPoint) {
    // 从 tileMap 获取地图高度（网格数量）
    float mapHeight = currentScene->tileMap->getMapSize().height;
    // 转换为屏幕坐标
    float screenX = gridPoint.x * (currentScene->tileSize.height) + (currentScene->tileSize.width) / 2;
    float screenY = (mapHeight - gridPoint.y - 1) * (currentScene->tileSize.height) + (currentScene->tileSize.height) / 2;
    return Vec2(screenX, screenY);
}