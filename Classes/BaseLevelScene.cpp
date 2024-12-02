#include "BaseLevelScene.h"
#include<vector>
#include"MonsterConfigs.h"
#include"Monster.h"
#include "json/document.h"
#include "json/rapidjson.h"
#include "ui/CocosGUI.h"
#include"music.h"
#include"themeScene.h"
using namespace ui;
#define DEBUG_MODE
extern Music a;
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in BaseLevelScene.cpp\n");
}
// 定义关卡地图文件路径数组
const std::vector<std::string> BaseLevelScene::mapFiles = {
    "map/map1.tmx",  // 关卡 1
    "map/map2.tmx",  // 关卡 2
    "map/map3.tmx",  // 关卡 3
    // 可以继续添加其他关卡的地图文件路径
};

void BaseLevelScene::updatemoney(int add) {
    money+=add;
    std::string text = std::to_string(money); // 将数字转换为字符串
    m_lable->setString(text);
}
void BaseLevelScene::doublespeed(Ref* pSender) {
    isDoubleSpeed = !isDoubleSpeed; // 切换二倍速状态
    a.button_music();
    MenuItemImage* button = static_cast<MenuItemImage*>(pSender);
    if (isDoubleSpeed) {
        button->setNormalImage(Sprite::create("CarrotGuardRes/UI/doubleSpeed.png"));
        button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/doubleSpeed.png"));
        scheduler->setTimeScale(2.0f); //实现加速效果
    }
    else {
        button->setNormalImage(Sprite::create("CarrotGuardRes/UI/normalSpeed.png"));
        button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/normalSpeed.png"));
        scheduler->setTimeScale(1.0f); //实现减速效果
    }
}

void BaseLevelScene::pause_all(Ref* pSender) {
    isPaused = !isPaused; // 切换暂停状态
    a.button_music();
    MenuItemImage* button = static_cast<MenuItemImage*>(pSender);
    if (isPaused) {//暂停状态
        button->setNormalImage(Sprite::create("CarrotGuardRes/UI/continueButton.png"));
        button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/continueButton.png"));
        // 添加顶部暂停标识
        auto pauseTop = Sprite::create("CarrotGuardRes/UI/pausing.png");
        pauseTop->setName("pauseTop");
        pauseTop->setPosition(450, 610);
        pauseTop->setScale(2.0f);
        this->addChild(pauseTop, 10);
        Director::getInstance()->pause();
    }
    else {//恢复
        button->setNormalImage(Sprite::create("CarrotGuardRes/UI/pauseButton.png"));
        button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/pauseButton.png"));
        //移除暂停标识
        this->removeChildByName("pauseTop");
        Director::getInstance()->resume();
    }
}
void BaseLevelScene::menu_all(Ref* pSender) {
    a.button_music();
    Director::getInstance()->pause();
    auto visibleSize = Director::getInstance()->getVisibleSize();//分辨率大小
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size screenSize = Director::getInstance()->getWinSize();    //获取屏幕大小
    //创建灰色遮罩层
    auto menuLayer = LayerColor::create(Color4B(0, 0, 0, 150));
    menuLayer->setPosition(Vec2::ZERO);
    this->addChild(menuLayer, 10);
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [menuLayer](Touch* touch, Event* event) {
        return true;
        };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, menuLayer);
    //添加暂停菜单背景
    auto menuBackground = Sprite::create("CarrotGuardRes/UI/gameMenu.png");
    menuBackground->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    menuBackground->setScale(1.5f);
    menuLayer->addChild(menuBackground, 0);
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    menuLayer->addChild(menu, 1);
    //添加暂停菜单上相关功能按钮
    auto continueButton = MenuItemImage::create("CarrotGuardRes/UI/continueNormal.png", "CarrotGuardRes/UI/continueSelected.png");
    continueButton->setPosition(Vec2(screenSize.width * 0.495, screenSize.height * 0.649));
    continueButton->setScale(1.5);
    auto restartButton = MenuItemImage::create("CarrotGuardRes/UI/restartNormal.png", "CarrotGuardRes/UI/restartSelected.png");
    restartButton->setPosition(Vec2(screenSize.width * 0.495, screenSize.height * 0.51));
    restartButton->setScale(1.5);
    auto chooseButton = MenuItemImage::create("CarrotGuardRes/UI/chooseLevelNormal.png", "CarrotGuardRes/UI/chooseLevelSelected.png");
    chooseButton->setPosition(Vec2(screenSize.width * 0.495, screenSize.height * 0.375));
    chooseButton->setScale(1.5);
    // 继续游戏选项
    continueButton->setCallback([this, menuLayer](Ref* psender) {
        a.button_music();
        this->removeChild(menuLayer);
        // 判断在点击菜单按钮之前是否点击过暂停按钮，防止出现bug
        if (isPaused==0) {
            Director::getInstance()->resume();
        }
        });

    //重新开始游戏选项
    restartButton->setCallback([this, menuLayer](Ref* psender) {
        a.button_music();
        this->removeChild(menuLayer);
        auto scene = BaseLevelScene::createScene(current_level);  
        Director::getInstance()->replaceScene(scene);
        Director::getInstance()->resume();
        });

    //选择关卡选项
    chooseButton->setCallback([this, menuLayer](Ref* psender) {
        a.button_music();
        this->removeChild(menuLayer);
        auto themeScene = themescene::createScene();
        Director::getInstance()->replaceScene(themeScene);
        Director::getInstance()->resume();
        });
    menu->addChild(continueButton, 1);
    menu->addChild(chooseButton, 1);
    menu->addChild(restartButton, 1);
}
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
    current_level = level;
    //加money
    m_lable = Label::createWithTTF("400", "fonts/arial.ttf", 27);
    m_lable->setPosition(Vec2(160, 610));
    this->addChild(m_lable, 3);
    //添加返回按钮
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    //倍数
    auto doubleSpeedButton = MenuItemImage::create("CarrotGuardRes/UI/normalSpeed.png", "CarrotGuardRes/UI/doubleSpeed.png", CC_CALLBACK_1(BaseLevelScene::doublespeed, this));
    if (doubleSpeedButton == nullptr)
    {
        problemLoading("'normalSpeed.png' and 'doubleSpeed.png'");
    }
    else
    {
        doubleSpeedButton->setPosition(670, 610);
        doubleSpeedButton->setScale(1);
        menu->addChild(doubleSpeedButton);
    }
    //暂停
    auto pauseSpeedButton = MenuItemImage::create("CarrotGuardRes/UI/pauseButton.png", "CarrotGuardRes/UI/continueButton.png", CC_CALLBACK_1(BaseLevelScene::pause_all, this));
    if (pauseSpeedButton == nullptr)
    {
        problemLoading("'pauseButton.png' and 'continueButton.png'");
    }
    else
    {
        pauseSpeedButton->setPosition(770,610);
        pauseSpeedButton->setScale(1);
        menu->addChild(pauseSpeedButton);
    }
    //菜单
    auto menuButton = MenuItemImage::create("CarrotGuardRes/UI/gameMenuNormal.png", "CarrotGuardRes/UI/gameMenuSelected.png", CC_CALLBACK_1(BaseLevelScene::menu_all, this));
    if (menuButton == nullptr)
    {
        problemLoading("'gameMenuNormal.png' and 'gameMenuSelected'");
    }
    else
    {
        menuButton->setPosition(870, 610);
        menuButton->setScale(1);
        menu->addChild(menuButton);
    }

 

    this->levelId=level;                   //存储关卡编号
    this->loadMap();                       // 加载对应关卡的地图
    this->placeMonsters ();                //放置怪兽
    //scheduleUpdate();                    //启动更新逻辑
  
    // 1. 创建植物图层
    plantsLayer = Layer::create();  // 创建一个新的图层，用于存放植物
    this->addChild(plantsLayer, 10); // 将植物图层添加到场景，zOrder为3，确保它位于其他层之上
    addMouseListener();  // 添加鼠标监听
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
        auto plant = Sprite::create("Towers/TFan/TFan_1.png");
        if (plant) {
            // 设置植物的位置，使其位置与瓦片对齐
            plant->setPosition(mapPos);
            CCLOG("mapPos: (%f, %f)", mapPos.x, mapPos.y);
            // 调整植物大小（如果需要）
            plant->setScale(2.0f);  // 假设将植物大小缩小一半，调整大小

            // 将植物添加到植物图层
            plantsLayer->addChild(plant);

            CCLOG("Planted at (%f, %f)", tileCoord.x, tileCoord.y);
        }
        else {
            CCLOG("Failed to create plant.");
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
