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
    NumOfDeadMonster=0;
    waveConfigs.clear();
    waveIndex=0;
    ClearMonsters();
    //初始化路径
    initPath();
    //初始化怪兽图像资源
    loadMonsterResources();
    //初始化萝卜
    initCarrot();
    auto _eventDispatcher = currentScene->getEventDispatcher();
    // 注册事件监听器
    auto listener = EventListenerCustom::create("monster_path_complete", CC_CALLBACK_1(GameManager::onMonsterPathComplete, this));
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, currentScene);
    //测试被咬效果
    /*
    currentScene->scheduleOnce([this](float dt) {
        carrot->getDamage();
        }, 2.0f, "pause_event");
        */

    //初始化怪兽
    /*
    loadGameData("MonsterData_save_10s.json");
    startMonsterWaves();
    */
    //加载怪兽数据资源
    loadMonsterWaveConfig("MonsterWaves.json", "level"+std::to_string(level));
    //开始怪兽波
   // startMonsterWaves();
    
    

}
//逐帧更新状态
void GameManager::update(float deltaTime) {
    if(carrot->getHP()<0)
    {
     carrot->changeHP(5);
    }
}

void GameManager::ClearMonsters()
{
    // 假设 monsters 是一个 std::vector<Sprite*>
    for (auto monster : monsters) {
        // 从父节点中移除精灵
        if (monster->getParent()) {
            monster->getParent()->removeChild(monster);
        }
        monster->stopAllActions();
        // 释放精灵对象的内存
        monster->release();
    }

    // 清空 vector
    monsters.clear();
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
//根据存档初始化
void GameManager::loadGameData(const std::string& fileName) {
    waveConfigs.clear();
    WaveConfig waveConfig;
    // 1. 构造文件路径（可写路径）
    std::string filePath = cocos2d::FileUtils::getInstance()->getWritablePath() + fileName;
    CCLOG("正在加载存档文件: %s", filePath.c_str());

    // 2. 读取文件内容
    std::ifstream ifs(filePath);
    if (!ifs.is_open()) {
        CCLOG("无法打开存档文件：%s", filePath.c_str());
        return;
    }

    std::stringstream buffer;
    buffer << ifs.rdbuf();  // 读取文件内容到字符串
    std::string fileContent = buffer.str();
    ifs.close();
    CCLOG("文件内容: %s", fileContent.c_str());

    // 3. 使用 RapidJSON 解析 JSON
    rapidjson::Document document;
    if (document.Parse(fileContent.c_str()).HasParseError()) {
        CCLOG("JSON 解析错误");
        return;
    }

    // 4. 读取第一部分: livemonsters
    if (document.HasMember("livemonsters") && document["livemonsters"].IsArray()) {
        const rapidjson::Value& livingMonsters = document["livemonsters"];
        for (rapidjson::SizeType i = 0; i < livingMonsters.Size(); ++i) {
            const rapidjson::Value& monsterData = livingMonsters[i];
            if (monsterData.IsObject()) {
                std::string monsterName = monsterData["monsterName"].GetString();
                int pathIndex = monsterData["pathIndex"].GetInt();
                int health = monsterData["health"].GetInt();
                //重新创建怪兽
                produceMonsters(monsterName, pathIndex,health);
                CCLOG("readMonsters: name=%s, pathIndex=%d, health=%d", monsterName.c_str(), pathIndex, health);
            }
        }
    }

    // 5. 读取第二部分: currentWave
    if (document.HasMember("currentWave") && document["currentWave"].IsObject()) {
        const rapidjson::Value& currentWave = document["currentWave"];
        std::string monsterName = currentWave["monsterName"].GetString();
        int count = currentWave["count"].GetInt();
            // 获取怪物名称
            waveConfig.monsterName = monsterName;
            // 获取怪物数量
            waveConfig.count = count;
            // 添加波配置
            waveConfigs.push_back(waveConfig);
            CCLOG("currentWave: name=%s, count=%d", monsterName.c_str(), count);
    }

    // 6. 读取第三部分: waveIndex
    if (document.HasMember("waveIndex") && document["waveIndex"].IsInt()) {
        int waveIndex = document["waveIndex"].GetInt();
        CCLOG("currentIndex: %d", waveIndex);
        // 你可以在这里设置当前的波索引
        this->waveIndex = waveIndex;
    }

    // 7. 读取第四部分: upcomingWaves
    if (document.HasMember("upcomingWaves") && document["upcomingWaves"].IsArray()) {
        const rapidjson::Value& upcomingWaves = document["upcomingWaves"];
        for (rapidjson::SizeType i = 0; i < upcomingWaves.Size(); ++i) {
            const rapidjson::Value& waveData = upcomingWaves[i];
            if (waveData.IsObject()) {
                std::string monsterName = waveData["monsterName"].GetString();
                int count = waveData["count"].GetInt();
                // 获取怪物名称
                waveConfig.monsterName = monsterName;
                // 获取怪物数量
                waveConfig.count = count;
                // 添加波配置
                waveConfigs.push_back(waveConfig);
                CCLOG("upComingWave: name=%s, count=%d", monsterName.c_str(), count);
            }
        }
    }

    CCLOG("Read All Data!");
}




//怪兽波的数据配置
void GameManager::loadMonsterWaveConfig(const std::string& filename, const std::string& levelName) {
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

           

            // 添加波配置
            waveConfigs.push_back(waveConfig);
            // 打印波的配置信息
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
//产生一波怪兽
void GameManager::produceMonsterWave(const WaveConfig& waveConfig) {
    // 每一波怪物的生成函数
    float delay = 0;
    for (int i = 0; i < waveConfig.count; ++i) {
        // 生成一个 0 到 2 秒之间的随机时间
        delay+= cocos2d::RandomHelper::random_real(waveConfig.spawnInterval[0], waveConfig.spawnInterval[1]);

        // 延迟生成怪物，注意延迟的时长是相对于当前时间的
        cocos2d::Director::getInstance()->getScheduler()->schedule([=](float) {
            produceMonsters(waveConfig.monsterName,0);
            }, this, 0, 0,delay, false, "produceMonster" + std::to_string(i));
    }
}
//生成怪兽波
void GameManager::startMonsterWaves() {
   CCLOG("Starting wave %d", waveIndex);
   produceMonsterWave(waveConfigs[waveIndex]); // 生成当前波的怪物
    //++waveIndex; // 进入下一波
    cocos2d::Director::getInstance()->getScheduler()->schedule([this](float) {
        if (waveIndex >= waveConfigs.size()-1) {
            CCLOG("All waves are complete.");
            cocos2d::Director::getInstance()->getScheduler()->unschedule("startWave", this); // 停止调度器
            return;
        }

        ++waveIndex; // 进入下一波
        CCLOG("Starting wave %d", waveIndex);
        produceMonsterWave(waveConfigs[waveIndex]); // 生成当前波的怪物
        
        }, this, 9.0f, false, "startWave"); // 每隔 9 秒调度一次
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
//入场特效
void GameManager::playSpawnEffect(const cocos2d::Vec2& spawnPosition) {
    // 1. 创建一个临时的动画精灵 (不依附于怪物)
    auto spawnEffect = cocos2d::Sprite::create("Monsters/monster_start_1.png"); // 动画的第一帧
    spawnEffect->setPosition(spawnPosition); // 动画显示在生成位置
    currentScene->addChild(spawnEffect); // 把动画精灵加到场景

    // 2. 加载动画的帧
    cocos2d::Vector<cocos2d::SpriteFrame*> frames;
    frames.pushBack(cocos2d::SpriteFrame::create("Monsters/monster_start_1.png", cocos2d::Rect(0, 0, 64, 64)));
    frames.pushBack(cocos2d::SpriteFrame::create("Monsters/monster_start_2.png", cocos2d::Rect(0, 0, 64, 64)));

    // 3. 生成 2 帧的动画 (0.2s 每帧)
    auto animation = cocos2d::Animation::createWithSpriteFrames(frames, 0.2f);
    auto animate = cocos2d::Animate::create(animation);

    // 4. 循环播放 2 次
    auto repeatAnimation = cocos2d::Repeat::create(animate, 2);

    // 5. 动画播放完成后，移除这个临时动画精灵
    auto removeEffect = cocos2d::CallFunc::create([spawnEffect]() {
        spawnEffect->removeFromParent(); // 移除动画精灵
        });

    // 6. 动画序列 (播放动画 + 移除)
    spawnEffect->setScale(1.5f); // 放大动画
    auto sequence = cocos2d::Sequence::create(repeatAnimation, removeEffect, nullptr);
    spawnEffect->runAction(sequence); // 运行动画
}
//单个怪兽生产函数
void GameManager::produceMonsters(const std::string monsterName,const int startIndex,int health) {

    if(startIndex==0)
   playSpawnEffect(screenPath[0]);
    // 创建怪物并添加到场景
    auto Monster = Monster::create(monsterName,screenPath, startIndex);
    if (!Monster) {
        CCLOG("Failed to create monster.");
        return;
    }
    monsters.push_back(Monster); // 保存到怪物列表 
    currentScene->addChild(Monster);
    if(health!=-1)
    {
        Monster->setHealth(health);
    }
}

//接收怪兽到达终点的时间的信号，处理怪物到达终点后的逻辑
void GameManager::onMonsterPathComplete(cocos2d::EventCustom* event)
{
    // 从事件的 userdata 获取怪兽对象
    Monster* monster = static_cast<Monster*>(event->getUserData());

    if (monster) {
        CCLOG("Monster has completed the path. Perform further actions here.");
        if (monster->getHealth() > 0)//怪物有血就攻击
        {
            carrot->getDamage();
            CCLOG("carrot'HP    %d",carrot->getHP());
        }
        monster->toDie();  //怪兽死亡
        NumOfDeadMonster++;
    }
}



//怪兽数据存档函数
void GameManager::saveMonstersDataToJson(const std::string& fileName) {
    // 创建 JSON 文档对象
    rapidjson::Document document;
    document.SetObject();
    // 获取当前波的索引
    int currentWaveIndex = getCurrentWaveIndex();
    // 1. 保存活着的怪物
    rapidjson::Value livingMonsters(rapidjson::kArrayType);
    for (auto* monster : monsters) {
        if (monster->checkLive()) {  // 判断怪物是否活着
            rapidjson::Value monsterData(rapidjson::kObjectType);

            // monsterName
            monsterData.AddMember("monsterName", rapidjson::Value(monster->getMonsterName().c_str(), document.GetAllocator()), document.GetAllocator());

            // pathIndex
            monsterData.AddMember("pathIndex", monster->getPathIndex(), document.GetAllocator());

            // health
            monsterData.AddMember("health", monster->getHealth(), document.GetAllocator());

            // 将怪物数据添加到数组中
            livingMonsters.PushBack(monsterData, document.GetAllocator());
        }
    }
    document.AddMember("livemonsters", livingMonsters, document.GetAllocator());

    // 2. 保存当前波信息
    // 计算当前波剩余的怪物数量
    int totalMonstersCount = 0;
    for (int i = 0; i <= currentWaveIndex; ++i) {
        totalMonstersCount += waveConfigs[i].count;
    }
    int remainingCount = totalMonstersCount - monsters.size();
    rapidjson::Value currentWave(rapidjson::kObjectType);
    currentWave.AddMember("monsterName", rapidjson::Value(waveConfigs[currentWaveIndex].monsterName.c_str(), document.GetAllocator()), document.GetAllocator());
    currentWave.AddMember("count", remainingCount, document.GetAllocator());
    document.AddMember("currentWave", currentWave, document.GetAllocator());
    //3.保存当前波的编号
    document.AddMember("waveIndex", currentWaveIndex, document.GetAllocator()); // 存入当前波索引
    // 4. 保存接下来的波信息
    rapidjson::Value upcomingWaves(rapidjson::kArrayType);
    for (size_t i = currentWaveIndex + 1; i < waveConfigs.size(); ++i) {
        rapidjson::Value waveData(rapidjson::kObjectType);
        waveData.AddMember("monsterName", rapidjson::Value(waveConfigs[i].monsterName.c_str(), document.GetAllocator()), document.GetAllocator());
        waveData.AddMember("count", waveConfigs[i].count, document.GetAllocator());

        // 将每波的数据添加到数组中
        upcomingWaves.PushBack(waveData, document.GetAllocator());
    }
    document.AddMember("upcomingWaves", upcomingWaves, document.GetAllocator());

    // 获取可写路径
    std::string writablePath = FileUtils::getInstance()->getWritablePath();  // 获取可写路径

    // 构造文件路径
    std::string filePath = writablePath + fileName;

    // 将结果写入到指定文件路径
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    // 写入文件
    std::ofstream ofs(filePath);  // 使用可写路径
    if (ofs.is_open()) {
        ofs << buffer.GetString();
        ofs.close();
        CCLOG("存档成功：%s", filePath.c_str());
    }
    else {
        CCLOG("存档失败：%s", filePath.c_str());
    }
}

//初始化萝卜
void GameManager::initCarrot() {
    // 目标位置的数组，假设levelId是1、2、3的场景关卡
    std::vector<cocos2d::Vec2> dst1 = { Vec2(804, 640 - 196), Vec2(816, 640 - 284), Vec2(831, 640 - 287) };
    std::vector<cocos2d::Vec2> dst2 = { Vec2(854, 640 - 196), Vec2(866, 640 - 284), Vec2(881, 640 - 287) };
    // 生成萝卜，血量为5
    carrot = Carrot::create(5, dst1[levelId - 1], dst2[levelId - 1]);
    currentScene->addChild(carrot); // 直接将 Carrot 作为一个Node，添加到场景中
}
//检查输状态
bool GameManager::CheckLose()
{
   if(carrot->getHP()==0)
   {
       CCLOG("LOSE THE GAME!");
       return true;
   }
   return false;
}
//检查赢状态
bool GameManager::CheckWin()
{
    if (carrot->getHP()>0&& NumOfDeadMonster==monsters.size())
    {
        CCLOG("WIN THE GAME!");
        return true;
    }
    return false;
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