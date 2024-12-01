#include "BaseLevelScene.h"
#include<vector>
#include"MonsterConfigs.h"
#include"Monster.h"
#include "json/document.h"
#include "json/rapidjson.h"
#include"Tower.h"
#define DEBUG_MODE

#define CELL_SIZE 64

std::string Tower::tower_table[TOWER_NUM][3] = { {"Towers/bottle0.png","Towers/bottle1.png","Towers/bottle2.png"},{"Towers/windmill0.png","Towers/windmill1.png","Towers/windmill2.png"} };
std::string Tower::base_table[TOWER_NUM] = { "Towers/bottlebase.png" ,"Towers/windmillbase.png" };
Vec2 Tower::anchorpoint_table[TOWER_NUM][2] = { {Vec2(0.5,0.5),Vec2(0.4,0.46)},{Vec2(0.5,0.7),Vec2(0.5,0.35)} };

const int Bottle::build_cost = 160;
const int Bottle::up_cost1 = 220;
const int Bottle::up_cost2 = 280;
int Bottle::range_table[3] = { 100,200,300 };
int Bottle::demage_table[3] = { 100,200,300 };



// 定义关卡地图文件路径数组
const std::vector<std::string> BaseLevelScene::mapFiles = {
    "map/map1.tmx",  // 关卡 1
    "map/map2.tmx",  // 关卡 2
    "map/map3.tmx",  // 关卡 3
    // 可以继续添加其他关卡的地图文件路径
};

// 创建场景时传入关卡编号
Scene* BaseLevelScene::createScene(int level) {
    auto scene = BaseLevelScene::create();  // 创建 BaseLevelScene
    if(scene&& scene->initWithLevel(level))
    {
        return scene;
    }
    return scene;
}
bool BaseLevelScene::initWithLevel(int level)
{
    if(!Scene::init())
    {
        return false;
    }
    this->levelId=level;                   //存储关卡编号
    this->loadMap();                       // 加载对应关卡的地图
    this->placeMonsters ();                //放置怪兽
    //scheduleUpdate();                    //启动更新逻辑
  
    // 1. 创建植物图层
    plantsLayer = Layer::create();  // 创建一个新的图层，用于存放植物
    this->addChild(plantsLayer, 10); // 将植物图层添加到场景，zOrder为3，确保它位于其他层之上
    addMouseListener();  // 添加鼠标监听

    cell_flag = 1;
    buy_tower.push_back("Towers/affordbottle.png");
    buy_tower.push_back("Towers/affordwindmill.png");
    index_table.push_back(0);
    index_table.push_back(1);
    return true;
}

// 默认的 init 方法
bool BaseLevelScene::init() {
    if (!Scene::init()) {
        return false;
    }
    return true;
}


/**************************************************
 *****************怪物相关*************************
 **************************************************/
//加载怪物精灵帧
void  BaseLevelScene::loadMonsters()
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/pig.plist");
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("pig.png")) {
        CCLOG("Failed to load SpriteFrame 'pig.png'.");
    }
}
//根据关卡放置怪物
void  BaseLevelScene::placeMonsters()
{ 
    std::string monsterName = "pig";  // 使用的怪物名称
    if (loadPathForLevel(levelId, "paths.json")) {
        auto& path = pathsCache[levelId];
        // 输出路径点到日志
        CCLOG("Path for level %d:", levelId);
        for (const auto& point : path) {
            CCLOG("Point: (%f, %f)", point.x, point.y);
        }
    }
    // 调用怪物类的静态创建方法
    auto pigMonster = Monster::create(monsterName, levelId, ScreenPaths[levelId]);
    if (!pigMonster) {
        CCLOG("Failed to create monster.");
        return;
    }
    // 添加怪物到场景中
    this->addChild(pigMonster, 3);

}
/**************************************************
 *****************地图相关*************************
 **************************************************/
// 加载地图的函数
void BaseLevelScene::loadMap() {
    if (levelId <= 0 || levelId > mapFiles.size()) {
        CCLOG("Invalid level number!");
        return;
    }

    // 加载关卡地图
     tileMap = TMXTiledMap::create(mapFiles[levelId - 1]);
    if (tileMap) {
        // 获取屏幕大小
        auto screenSize = Director::getInstance()->getVisibleSize();
        auto tileMapSize = tileMap->getContentSize();
        //获取瓦片大小
        tileSize=tileMap->getTileSize();
        CCLOG("Tile size: (%f, %f)", tileSize.width, tileSize.height);
        // 缩放瓦片地图，使其填满整个屏幕
        tileMap->setScaleX(screenSize.width / tileMapSize.width);
        tileMap->setScaleY(screenSize.height / tileMapSize.height);

        // 设置瓦片地图的锚点为 (0, 0)，使地图左下角与屏幕左下角对齐
        tileMap->setAnchorPoint(Vec2::ZERO);
        tileMap->setPosition(Vec2::ZERO);
        // 获取并隐藏 plantable 层
        plantableLayer = tileMap->getLayer("plantable");
        if (plantableLayer) {
            plantableLayer->setVisible(false);  // 不显示 plantable 层
        }
        else {
            CCLOG("Layer 'plantable' not found in map: %s", mapFiles[levelId - 1].c_str());
        }
        // 将瓦片地图添加到当前场景中
        this->addChild(tileMap, 0);
#ifdef DEBUG_MODE
        drawGrid();
#endif
    }
    else {
        CCLOG("Failed to load map: %s", mapFiles[levelId - 1].c_str());
    }
}
// 从 JSON 文件按需加载某个关卡的路径到pathsCache并同时存储其对应的屏幕坐标
bool BaseLevelScene::loadPathForLevel(int levelId, const std::string& filePath)
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
/**************************************************
 *****************炮塔相关*************************
 **************************************************/
//开启鼠标监听
void BaseLevelScene::addMouseListener() {
    auto listener = EventListenerMouse::create();
    listener->onMouseDown = CC_CALLBACK_1(BaseLevelScene::handleMouseDown, this); // 鼠标按下时的回调

    // 将监听器添加到事件派发器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
//处理鼠标点击事件
void BaseLevelScene::handleMouseDown(EventMouse* event) {
    // 获取鼠标点击位置
    Vec2 mousePos = event->getLocation();
    CCLOG("Mouse clicked at screen position: (%f, %f)", mousePos.x, mousePos.y);
    // 调用 handlePlant 方法来处理种植
    handlePlant(mousePos);
}
//处理种植炮塔的判断
void BaseLevelScene::handlePlant(const Vec2& position) {
    CCLOG("position: (%f, %f)", position.x, position.y);
    // 将鼠标点击的屏幕坐标转换为瓦片坐标
    Vec2 tileCoord = Vec2(floor(position.x / tileSize.width), floor(position.y / tileSize.height));
    //下面日志证明已经实现了网格的转化
    CCLOG("tileCoord: (%f, %f)", tileCoord.x, tileCoord.y);

    // 边界检查
    if (tileCoord.x < 0 || tileCoord.y < 0 ||
        tileCoord.x >= tileMap->getMapSize().width ||
        tileCoord.y >= tileMap->getMapSize().height) {
        CCLOG("Tile coordinate is out of bounds.");
        return;
    }

    // 判断该瓦片是否可种植
    auto tileGID = tileMap->getLayer("plantable")->getTileGIDAt(tileCoord);
    if (!cell_flag) {
        auto location = Vec2(position.x, Director::getInstance()->getVisibleSize().height - position.y);
        int size = remove_table.size();
        if ((location.x > last_position.x - CELL_SIZE * size / 2 && location.x < last_position.x + CELL_SIZE * size / 2)
            && (location.y > last_position.y + CELL_SIZE / 2 && location.y < last_position.y +  3*CELL_SIZE / 2)) {
            int index = index_table[(location.x - last_position.x + CELL_SIZE * size / 2) / CELL_SIZE];
            Tower a(index);
            a.build(this, last_position);
        }
        this->removeChild(curr_cell);
        for (int i = 0; i < remove_table.size(); i++) this->removeChild(remove_table[i]);
        remove_table.clear();
        cell_flag = 1;
        return;
    }
    if (tileGID != 0) {
        CCLOG("Tile at (%f, %f) is plantable.", tileCoord.x, tileCoord.y);
        // 这里实现种植逻辑
       // 将瓦片坐标转换为屏幕坐标
       // 获取地图的高度（瓦片的行数）
        float mapHeight = tileMap->getMapSize().height;

        // 转换为屏幕坐标
        float screenX = tileCoord.x * tileSize.width + tileSize.width / 2;
        float screenY = (mapHeight - tileCoord.y - 1) * tileSize.height + tileSize.height / 2;

        Vec2 mapPos = Vec2(screenX, screenY);

        // 输出屏幕坐标
        CCLOG("Screen Position: (%f, %f)", mapPos.x, mapPos.y);

        // 创建植物精灵，假设植物图片是 "plant.png"
        //auto plant = Sprite::create("Towers/TFan/TFan_1.png");
        //if (plant) {
        //    // 设置植物的位置，使其位置与瓦片对齐
        //    plant->setPosition(mapPos);
        //    CCLOG("mapPos: (%f, %f)", mapPos.x, mapPos.y);
        //    // 调整植物大小（如果需要）
        //    plant->setScale(2.0f);  // 假设将植物大小缩小一半，调整大小

        //    // 将植物添加到植物图层
        //    plantsLayer->addChild(plant);

        //    CCLOG("Planted at (%f, %f)", tileCoord.x, tileCoord.y);
        //}
        //else {
        //    CCLOG("Failed to create plant.");
        //}
        cell_flag = 0;
        auto location = mapPos;
        last_position = location;
        auto cell = Sprite::create("Towers/cell.png");
        cell->setScale((float)64 / 200);
        cell->setPosition(location);
        this->addChild(cell); curr_cell = cell;
        for (int i = 0, size = buy_tower.size(); i < size; i++) {
            auto tower_graph = Sprite::create(buy_tower[i]);
            tower_graph->setScale((float)64 / 79);
            tower_graph->setPosition(Vec2(location.x - CELL_SIZE * (size - 1) / 2 + CELL_SIZE * i, location.y + CELL_SIZE));
            this->addChild(tower_graph); remove_table.push_back(tower_graph);
        }
    }
 
    else 
    {
        CCLOG("Tile at (%f, %f) is not plantable.", tileCoord.x, tileCoord.y);
    }
   
}
//种植操作
void BaseLevelScene::plantAt(const cocos2d::Vec2& tileCoord) {
    auto plant = cocos2d::Sprite::create("plant.png");
    auto plantPos = tileMap->getLayer("map")->getPositionAt(tileCoord);
    plant->setPosition(plantPos);
    plantsLayer->addChild(plant);
}

/**************************************************
 *****************工具函数*************************
 **************************************************/
//画网格线
#ifdef DEBUG_MODE
void BaseLevelScene::drawGrid() {
    // 获取地图的大小和瓦片的大小
    Size mapSize = tileMap->getContentSize();
    float tileWidth = 64.0f;  // 假设每个瓦片宽度为 64 像素
    float tileHeight = 64.0f; // 假设每个瓦片高度为 64 像素

    // 创建一个 DrawNode 用于绘制网格
    auto drawNode = DrawNode::create();
    this->addChild(drawNode, 100);  // 将 DrawNode 添加到场景中，层级设置为 100，确保在地图之上

    // 绘制垂直线
    for (float x = 0; x <= mapSize.width; x += tileWidth) {
        drawNode->drawLine(Vec2(x, 0), Vec2(x, mapSize.height), Color4F::WHITE);
    }

    // 绘制水平线
    for (float y = 0; y <= mapSize.height; y += tileHeight) {
        drawNode->drawLine(Vec2(0, y), Vec2(mapSize.width, y), Color4F::WHITE);
    }
}
#endif // DEBUG_MODE

//瓦格坐标转地图坐标的工具函数
Vec2 BaseLevelScene::gridToScreenCenter(const Vec2& gridPoint) {
    // 从 tileMap 获取地图高度（网格数量）
    float mapHeight = tileMap->getMapSize().height;
    // 转换为屏幕坐标
    float screenX = gridPoint.x * tileSize.height + tileSize.width / 2;
    float screenY = (mapHeight - gridPoint.y - 1) * tileSize.height + tileSize.height / 2;
    return Vec2(screenX, screenY);
}
