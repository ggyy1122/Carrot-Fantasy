#include "BaseLevelScene.h"
#include<vector>
#include"MonsterConfigs.h"
#include"Monster.h"
#include "json/document.h"
#include "json/rapidjson.h"
#include "ui/CocosGUI.h"
#include"music.h"
#include"themeScene.h"
#include"Tower.h"
using namespace ui;

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


extern Music a;
extern bool level_is_win[3];
extern bool is_newgame[3];

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in BaseLevelScene.cpp\n");
}
bool BaseLevelScene::updatemoney(int add) {
    if (money + add < 0)
        return false;
    money += add;
    std::string text = std::to_string(money); // 将数字转换为字符串
    m_lable->setString(text);
    return true;
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
void BaseLevelScene::changeHP(int change) {     //改变血量,change可正可负
    a.carrotSound();
    carrot_HP += change;
    // 判断游戏是否结束
    if (carrot_HP <= 0)
        gameover(false);
    else if (carrot_HP > 0 && carrot_HP <= 5) {
        Carrot->setSpriteFrame(StringUtils::format("Carrot_%d.png", carrot_HP));
        HP->setSpriteFrame(StringUtils::format("Health_%d.png", carrot_HP));
    }
    else
        return;
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
        if (isPaused == 0) {
            Director::getInstance()->resume();
        }
        });

    //重新开始游戏选项
    restartButton->setCallback([this, menuLayer](Ref* psender) {
        a.button_music();
        this->removeChild(menuLayer);
        auto scene = BaseLevelScene::createScene(levelId);
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

void BaseLevelScene::Jineng1(Ref* pSender) {
    a.button_music();
    if (carrot_HP < 5 && updatemoney(-100)) {
        changeHP(1);
    } 
}
void BaseLevelScene::Jineng2(Ref* pSender) {
    a.button_music();
}
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
    if (scene && scene->initWithLevel(level))
    {
        return scene;
    }
    return scene;
}
bool BaseLevelScene::initWithLevel(int level)
{
    if (!Scene::init())
    {
        return false;
    }
    CountDown();
    
    // 1. 显示出现了多少波怪物
    _curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", std::min(_currNum, _monsterWave)), "Arial", 32);
    _curNumberLabel->setColor(Color3B::YELLOW);
    _curNumberLabel->setPosition(960 * 0.45, 640 * 0.95);
    this->addChild(_curNumberLabel, 2);
    // 2. 一共有多少波怪物
    _numberLabel = Label::createWithSystemFont(StringUtils::format("/%dtimes", _monsterWave), "Arial", 32);
    _numberLabel->setColor(Color3B::YELLOW);
    _numberLabel->setPosition(960 * 0.53, 640 * 0.95);
    this->addChild(_numberLabel, 2);
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
        pauseSpeedButton->setPosition(770, 610);
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
    this->levelId = level;                   //存储关卡编号
    this->loadMap();                       // 加载对应关卡的地图
    auto a = SpriteFrameCache::getInstance();
    a->addSpriteFramesWithFile("CarrotGuardRes/Carrots.plist", "CarrotGuardRes/Carrots.png");
    a->addSpriteFramesWithFile("CarrotGuardRes/Health.plist", "CarrotGuardRes/Health.png");
    // 创建萝卜
    Carrot = Sprite::createWithSpriteFrameName(StringUtils::format("Carrot_%d.png", carrot_HP));
    Carrot->setScale(1.5);
    Carrot->setPosition(dst1[levelId - 1]);
    this->addChild(Carrot, 2);
    // 创建血条
    HP = Sprite::createWithSpriteFrameName(StringUtils::format("Health_%d.png", carrot_HP));
    HP->setScale(1.5);
    HP->setPosition(dst2[levelId - 1]);
    this->addChild(HP, 2);

    auto jineng1Button = MenuItemImage::create("Carrot/jineng1.png", "Carrot/jineng1.png", CC_CALLBACK_1(BaseLevelScene::Jineng1, this));
    if (jineng1Button == nullptr)
    {
        problemLoading("'jineng1.png'");
    }
    else
    {
        jineng1Button->setPosition(800, 100);
        jineng1Button->setScale(2);
        menu->addChild(jineng1Button);
    }
    auto jineng2Button = MenuItemImage::create("Carrot/jineng2.png", "Carrot/jineng2.png", CC_CALLBACK_1(BaseLevelScene::Jineng2, this));
    if (jineng2Button == nullptr)
    {
        problemLoading("'jineng2.png'");
    }
    else
    {
        jineng2Button->setPosition(870, 100);
        jineng2Button->setScale(2);
        menu->addChild(jineng2Button);
    }
    auto delayaction = Sequence::create(DelayTime::create(4.0f), CallFunc::create([=] {
        this->placeMonsters();                //放置怪兽
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
        }), nullptr);
    this->runAction(delayaction);
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
        tileSize = tileMap->getTileSize();
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
            && (location.y > last_position.y + CELL_SIZE / 2 && location.y < last_position.y + 3 * CELL_SIZE / 2)) {
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


void BaseLevelScene::gameover(bool is_win) {
    Director::getInstance()->pause();
    // 设置灰色遮罩层
    auto menuLayer = LayerColor::create(Color4B(0, 0, 0, 150));
    menuLayer->setPosition(Vec2::ZERO);
    this->addChild(menuLayer, 10);
    // 创建菜单
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    menuLayer->addChild(menu, 1);
    // 游戏胜利
    if (is_win) {
        level_is_win[levelId-1] = true;
        //添加游戏获胜界面
        auto gameWinBackground = Sprite::create("CarrotGuardRes/UI/WinGame.png");
        gameWinBackground->setPosition(Vec2(480, 320));
        gameWinBackground->setScale(1.5f);
        menuLayer->addChild(gameWinBackground, 0);
        //添加获胜的金萝卜标识
        auto goldenCarrot = Sprite::create("CarrotGuardRes/UI/goldenCarrot.png");
        goldenCarrot->setPosition(Vec2(960 * 0.493, 640 * 0.7));
        menuLayer->addChild(goldenCarrot, 0);

        // 胜利的相关提示语
        _curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", _currNum > _monsterWave ? _monsterWave : _currNum), "Arial", 32);
        _curNumberLabel->setColor(Color3B::YELLOW);
        _curNumberLabel->setPosition(960 * 0.51, 640 * 0.54);
        Label* loseWordLeft = Label::createWithSystemFont("fought off", "Arial", 30);
        loseWordLeft->setPosition(960 * 0.36, 640 * 0.54);
        Label* loseWordRight = Label::createWithSystemFont("waves", "Arial", 30);
        loseWordRight->setPosition(960 * 0.60, 640 * 0.545);

        this->addChild(_curNumberLabel, 10);
        this->addChild(loseWordLeft, 10);
        this->addChild(loseWordRight, 10);
        //继续游戏按钮
        auto continueButton = MenuItemImage::create("CarrotGuardRes/UI/continueNormal.png", "CarrotGuardRes/UI/continueSelected.png");
        continueButton->setPosition(Vec2(960 * 0.613, 640 * 0.375));
        continueButton->setScale(1.38);
        continueButton->setCallback([this, menuLayer](Ref* psender) {
            a.button_music();
            //若当前未到开放的最后一关，则进行下一关
            if (levelId < 3) {
                auto scene = BaseLevelScene::createScene(levelId + 1);
                Director::getInstance()->replaceScene(scene);
            }
            //若当前已经是开放的最后一关，则返回选择关卡界面
            else {
                auto themeScene = themescene::createScene();
                Director::getInstance()->replaceScene(themeScene);
            }
            });
        menu->addChild(continueButton, 1);
    }
    // 游戏失败
    else {
        auto gameLoseBackground = Sprite::create("CarrotGuardRes/UI/LoseGame.png");
        gameLoseBackground->setPosition(Vec2(960 / 2 + 960 * 0.01, 640 / 2 + 640 * 0.015));
        gameLoseBackground->setScale(1.5f);
        menuLayer->addChild(gameLoseBackground, 0);

        // 游戏失败的相关提示语
        _curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", _currNum - 1), "Arial", 32);// 暂时没搞currnum为什么会大1，所以先-1
        _curNumberLabel->setColor(Color3B::YELLOW);
        _curNumberLabel->setPosition(960 * 0.51, 640 * 0.54);
        Label* loseWordLeft = Label::createWithSystemFont("fought off", "Arial", 30);
        loseWordLeft->setPosition(960 * 0.36, 640 * 0.54);
        Label* loseWordRight = Label::createWithSystemFont("waves", "Arial", 30);
        loseWordRight->setPosition(960 * 0.60, 640 * 0.54);

        this->addChild(_curNumberLabel, 10);
        this->addChild(loseWordLeft, 10);
        this->addChild(loseWordRight, 10);
        //重新游戏按钮
        auto againButton = MenuItemImage::create("CarrotGuardRes/UI/AgainNormal.png", "CarrotGuardRes/UI/AgainSelected.png");
        againButton->setPosition(Vec2(960 * 0.61, 640 * 0.37));
        againButton->setScale(0.9);
        // 重新开始按钮的选项
        againButton->setCallback([this, menuLayer](Ref* psender) {
            a.button_music();
            this->removeChild(menuLayer);
            auto scene = BaseLevelScene::createScene(levelId);
            Director::getInstance()->replaceScene(scene);
            Director::getInstance()->resume();
            });
        menu->addChild(againButton, 1);
    }
    // 选择游戏关卡按钮
    auto chooseButton = MenuItemImage::create("CarrotGuardRes/UI/chooseLevelNormal.png", "CarrotGuardRes/UI/chooseLevelSelected.png");
    chooseButton->setPosition(Vec2(960 * 0.38, 640 * 0.37));
    chooseButton->setScale(1.4);
    //选择关卡选项
    chooseButton->setCallback([this, menuLayer](Ref* psender) {
        a.button_music();
        this->removeChild(menuLayer);
        auto themeScene = themescene::createScene();
        Director::getInstance()->replaceScene(themeScene);
        Director::getInstance()->resume();
        });
    menu->addChild(chooseButton, 1);
}

void BaseLevelScene::CountDown()
{
   
    auto countBackground = Sprite::create("CarrotGuardRes/UI/countBackground.png");
    auto count1 = Sprite::create("CarrotGuardRes/UI/countOne.png");
    auto count2 = Sprite::create("CarrotGuardRes/UI/countTwo.png");
    auto count3 = Sprite::create("CarrotGuardRes/UI/countThree.png");
    Label* count0 = Label::createWithSystemFont("GO", "Arial-BoldMT", 100);

    countBackground->setPosition(480, 320);
    count1->setPosition(480, 320);
    count2->setPosition(480, 320);
    count3->setPosition(480, 320);
    count0->setPosition(480, 320);

    countBackground->setVisible(false);
    count1->setVisible(false);
    count2->setVisible(false);
    count3->setVisible(false);
    count0->setVisible(false);

    this->addChild(countBackground, 2);
    this->addChild(count1, 2);
    this->addChild(count2, 2);
    this->addChild(count3, 2);
    this->addChild(count0, 2);
    // 设置倒数sequence动作
    auto countdown = Sequence::create(CallFunc::create([=] {
        countBackground->setVisible(true);
        count3->setVisible(true);
        a.countSound();
        }), DelayTime::create(1), CallFunc::create([=] {
            this->removeChild(count3);
            }), CallFunc::create([=] {
                count2->setVisible(true);
                a.countSound();
                }), DelayTime::create(1), CallFunc::create([=] {
                    this->removeChild(count2);
                    }), CallFunc::create([=] {
                        count1->setVisible(true);
                        a.countSound();
                        }), DelayTime::create(1), CallFunc::create([=] {
                            this->removeChild(count1);
                            count0->setVisible(true);
                            a.countSound();
                            }), DelayTime::create(1), CallFunc::create([=] {
                                this->removeChild(count0);
                                this->removeChild(countBackground);
                                // 游戏主循环
                                scheduleUpdate();
                                                
                                }), NULL);

    this->runAction(countdown);
}