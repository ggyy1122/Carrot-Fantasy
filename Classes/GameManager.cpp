#include"GameManager.h"
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
    // 1. 先加载波次的配置
    loadMonsterWaveConfig("MonsterWaves.json", "level"+std::to_string(level));
    // 2. 启动波次生成逻辑
    startMonsterWaves();
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

//怪兽波的数据配置
void GameManager::loadMonsterWaveConfig(const std::string& filename, const std::string& levelName) {
    waveConfigs.clear();
    // 打开 JSON 文件
    std::string path = FileUtils::getInstance()->fullPathForFilename(filename);
    std::string fileContent = FileUtils::getInstance()->getStringFromFile(path);

    // 使用 rapidjson 解析文件内容
    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());

    if (doc.HasParseError()) {
        CCLOG("Error parsing JSON file: %s", filename.c_str());
        return;
    }

    // 检查是否存在指定关卡的配置
    if (doc.HasMember(levelName.c_str())) {
        const rapidjson::Value& waves = doc[levelName.c_str()];
        for (rapidjson::SizeType i = 0; i < waves.Size(); ++i) {
            const rapidjson::Value& wave = waves[i];
            WaveConfig waveConfig;
            // 获取怪物波次
            waveConfig.wave = wave["wave"].GetInt();
            // 获取怪物名称
            waveConfig.monsterName = wave["monsterName"].GetString();

            // 获取怪物数量
            waveConfig.count = wave["count"].GetInt();

            // 获取生成间隔
            const rapidjson::Value& spawnInterval = wave["spawnInterval"];
            waveConfig.spawnInterval[0] = spawnInterval[0].GetFloat();
            waveConfig.spawnInterval[1] = spawnInterval[1].GetFloat();

            // 获取路径数据
            const rapidjson::Value& path = wave["path"];
            for (rapidjson::SizeType j = 0; j < path.Size(); ++j) {
                Vec2 point(path[j][0].GetFloat(), path[j][1].GetFloat());
                waveConfig.path.push_back(point);
            }

            // 添加波配置
            waveConfigs.push_back(waveConfig);
            // 打印波的配置信息
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
//产生一波怪兽
void GameManager::produceMonsterWave(const WaveConfig& waveConfig) {
    // 每一波怪物的生成函数
    float delay = 0;
    for (int i = 0; i < waveConfig.count; ++i) {
        // 生成一个 0 到 2 秒之间的随机时间
        delay+= cocos2d::RandomHelper::random_real(waveConfig.spawnInterval[0], waveConfig.spawnInterval[1]);

        // 延迟生成怪物，注意延迟的时长是相对于当前时间的
        cocos2d::Director::getInstance()->getScheduler()->schedule([=](float) {
            produceMonsters(waveConfig.monsterName);
            }, this, 0, 0,delay, false, "produceMonster" + std::to_string(i));
    }
}
//生成怪兽波
void GameManager::startMonsterWaves() {
    waveIndex = 0; // 初始化波次索引
    produceMonsterWave(waveConfigs[waveIndex]); // 生成当前波的怪物
    ++waveIndex; // 进入下一波
    cocos2d::Director::getInstance()->getScheduler()->schedule([this](float) {
        if (waveIndex >= waveConfigs.size()) {
            CCLOG("All waves are complete.");
            cocos2d::Director::getInstance()->getScheduler()->unschedule("startWave", this); // 停止调度器
            return;
        }

        CCLOG("Starting wave %d", waveIndex + 1);
        produceMonsterWave(waveConfigs[waveIndex]); // 生成当前波的怪物
        ++waveIndex; // 进入下一波
        }, this, 12.0f, false, "startWave"); // 每隔 12 秒调度一次
}
//加载怪兽图像资源
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
//单个怪兽生产函数
void GameManager::produceMonsters(const std::string monsterName) {
    // 创建怪物并添加到场景
    auto Monster = Monster::create(monsterName,screenPath);
    if (!Monster) {
        CCLOG("Failed to create monster.");
        return;
    }
    monsters.push_back(Monster); // 保存到怪物列表 
    currentScene->addChild(Monster);
    numOfLiveMonster++;
}
//逐帧更新怪物的状态
void GameManager::updateMonsters(float deltaTime) {
    for (auto monster : monsters) {
        if (monster) {
         
        }
    }
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