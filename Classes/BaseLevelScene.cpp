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
// ����ؿ���ͼ�ļ�·������
const std::vector<std::string> BaseLevelScene::mapFiles = {
    "map/map1.tmx",  // �ؿ� 1
    "map/map2.tmx",  // �ؿ� 2
    "map/map3.tmx",  // �ؿ� 3
    // ���Լ�����������ؿ��ĵ�ͼ�ļ�·��
};

void BaseLevelScene::updatemoney(int add) {
    money+=add;
    std::string text = std::to_string(money); // ������ת��Ϊ�ַ���
    m_lable->setString(text);
}
void BaseLevelScene::doublespeed(Ref* pSender) {
    isDoubleSpeed = !isDoubleSpeed; // �л�������״̬
    a.button_music();
    MenuItemImage* button = static_cast<MenuItemImage*>(pSender);
    if (isDoubleSpeed) {
        button->setNormalImage(Sprite::create("CarrotGuardRes/UI/doubleSpeed.png"));
        button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/doubleSpeed.png"));
        scheduler->setTimeScale(2.0f); //ʵ�ּ���Ч��
    }
    else {
        button->setNormalImage(Sprite::create("CarrotGuardRes/UI/normalSpeed.png"));
        button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/normalSpeed.png"));
        scheduler->setTimeScale(1.0f); //ʵ�ּ���Ч��
    }
}

void BaseLevelScene::pause_all(Ref* pSender) {
    isPaused = !isPaused; // �л���ͣ״̬
    a.button_music();
    MenuItemImage* button = static_cast<MenuItemImage*>(pSender);
    if (isPaused) {//��ͣ״̬
        button->setNormalImage(Sprite::create("CarrotGuardRes/UI/continueButton.png"));
        button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/continueButton.png"));
        // ��Ӷ�����ͣ��ʶ
        auto pauseTop = Sprite::create("CarrotGuardRes/UI/pausing.png");
        pauseTop->setName("pauseTop");
        pauseTop->setPosition(450, 610);
        pauseTop->setScale(2.0f);
        this->addChild(pauseTop, 10);
        Director::getInstance()->pause();
    }
    else {//�ָ�
        button->setNormalImage(Sprite::create("CarrotGuardRes/UI/pauseButton.png"));
        button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/pauseButton.png"));
        //�Ƴ���ͣ��ʶ
        this->removeChildByName("pauseTop");
        Director::getInstance()->resume();
    }
}
void BaseLevelScene::menu_all(Ref* pSender) {
    a.button_music();
    Director::getInstance()->pause();
    auto visibleSize = Director::getInstance()->getVisibleSize();//�ֱ��ʴ�С
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size screenSize = Director::getInstance()->getWinSize();    //��ȡ��Ļ��С
    //������ɫ���ֲ�
    auto menuLayer = LayerColor::create(Color4B(0, 0, 0, 150));
    menuLayer->setPosition(Vec2::ZERO);
    this->addChild(menuLayer, 10);
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [menuLayer](Touch* touch, Event* event) {
        return true;
        };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, menuLayer);
    //�����ͣ�˵�����
    auto menuBackground = Sprite::create("CarrotGuardRes/UI/gameMenu.png");
    menuBackground->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    menuBackground->setScale(1.5f);
    menuLayer->addChild(menuBackground, 0);
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    menuLayer->addChild(menu, 1);
    //�����ͣ�˵�����ع��ܰ�ť
    auto continueButton = MenuItemImage::create("CarrotGuardRes/UI/continueNormal.png", "CarrotGuardRes/UI/continueSelected.png");
    continueButton->setPosition(Vec2(screenSize.width * 0.495, screenSize.height * 0.649));
    continueButton->setScale(1.5);
    auto restartButton = MenuItemImage::create("CarrotGuardRes/UI/restartNormal.png", "CarrotGuardRes/UI/restartSelected.png");
    restartButton->setPosition(Vec2(screenSize.width * 0.495, screenSize.height * 0.51));
    restartButton->setScale(1.5);
    auto chooseButton = MenuItemImage::create("CarrotGuardRes/UI/chooseLevelNormal.png", "CarrotGuardRes/UI/chooseLevelSelected.png");
    chooseButton->setPosition(Vec2(screenSize.width * 0.495, screenSize.height * 0.375));
    chooseButton->setScale(1.5);
    // ������Ϸѡ��
    continueButton->setCallback([this, menuLayer](Ref* psender) {
        a.button_music();
        this->removeChild(menuLayer);
        // �ж��ڵ���˵���ť֮ǰ�Ƿ�������ͣ��ť����ֹ����bug
        if (isPaused==0) {
            Director::getInstance()->resume();
        }
        });

    //���¿�ʼ��Ϸѡ��
    restartButton->setCallback([this, menuLayer](Ref* psender) {
        a.button_music();
        this->removeChild(menuLayer);
        auto scene = BaseLevelScene::createScene(current_level);  
        Director::getInstance()->replaceScene(scene);
        Director::getInstance()->resume();
        });

    //ѡ��ؿ�ѡ��
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
// ��������ʱ����ؿ����
Scene* BaseLevelScene::createScene(int level) {
    auto scene = BaseLevelScene::create();  // ���� BaseLevelScene
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
    //��money
    m_lable = Label::createWithTTF("400", "fonts/arial.ttf", 27);
    m_lable->setPosition(Vec2(160, 610));
    this->addChild(m_lable, 3);
    //��ӷ��ذ�ť
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    //����
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
    //��ͣ
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
    //�˵�
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

 

    this->levelId=level;                   //�洢�ؿ����
    this->loadMap();                       // ���ض�Ӧ�ؿ��ĵ�ͼ
    this->placeMonsters ();                //���ù���
    //scheduleUpdate();                    //���������߼�
  
    // 1. ����ֲ��ͼ��
    plantsLayer = Layer::create();  // ����һ���µ�ͼ�㣬���ڴ��ֲ��
    this->addChild(plantsLayer, 10); // ��ֲ��ͼ����ӵ�������zOrderΪ3��ȷ����λ��������֮��
    addMouseListener();  // ���������
    return true;
}

// Ĭ�ϵ� init ����
bool BaseLevelScene::init() {
    if (!Scene::init()) {
        return false;
    }
    return true;
}


/**************************************************
 *****************�������*************************
 **************************************************/
//���ع��ﾫ��֡
void  BaseLevelScene::loadMonsters()
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Monsters/pig.plist");
    if (!SpriteFrameCache::getInstance()->getSpriteFrameByName("pig.png")) {
        CCLOG("Failed to load SpriteFrame 'pig.png'.");
    }
}
//���ݹؿ����ù���
void  BaseLevelScene::placeMonsters()
{ 
    std::string monsterName = "pig";  // ʹ�õĹ�������
    if (loadPathForLevel(levelId, "paths.json")) {
        auto& path = pathsCache[levelId];
        // ���·���㵽��־
        CCLOG("Path for level %d:", levelId);
        for (const auto& point : path) {
            CCLOG("Point: (%f, %f)", point.x, point.y);
        }
    }
    // ���ù�����ľ�̬��������
    auto pigMonster = Monster::create(monsterName, levelId, ScreenPaths[levelId]);
    if (!pigMonster) {
        CCLOG("Failed to create monster.");
        return;
    }
    // ��ӹ��ﵽ������
    this->addChild(pigMonster, 3);

}
/**************************************************
 *****************��ͼ���*************************
 **************************************************/
// ���ص�ͼ�ĺ���
void BaseLevelScene::loadMap() {
    if (levelId <= 0 || levelId > mapFiles.size()) {
        CCLOG("Invalid level number!");
        return;
    }

    // ���عؿ���ͼ
     tileMap = TMXTiledMap::create(mapFiles[levelId - 1]);
    if (tileMap) {
        // ��ȡ��Ļ��С
        auto screenSize = Director::getInstance()->getVisibleSize();
        auto tileMapSize = tileMap->getContentSize();
        //��ȡ��Ƭ��С
        tileSize=tileMap->getTileSize();
        CCLOG("Tile size: (%f, %f)", tileSize.width, tileSize.height);
        // ������Ƭ��ͼ��ʹ������������Ļ
        tileMap->setScaleX(screenSize.width / tileMapSize.width);
        tileMap->setScaleY(screenSize.height / tileMapSize.height);

        // ������Ƭ��ͼ��ê��Ϊ (0, 0)��ʹ��ͼ���½�����Ļ���½Ƕ���
        tileMap->setAnchorPoint(Vec2::ZERO);
        tileMap->setPosition(Vec2::ZERO);
        // ��ȡ������ plantable ��
        plantableLayer = tileMap->getLayer("plantable");
        if (plantableLayer) {
            plantableLayer->setVisible(false);  // ����ʾ plantable ��
        }
        else {
            CCLOG("Layer 'plantable' not found in map: %s", mapFiles[levelId - 1].c_str());
        }
        // ����Ƭ��ͼ��ӵ���ǰ������
        this->addChild(tileMap, 0);
#ifdef DEBUG_MODE
        drawGrid();
#endif
    }
    else {
        CCLOG("Failed to load map: %s", mapFiles[levelId - 1].c_str());
    }
}
// �� JSON �ļ��������ĳ���ؿ���·����pathsCache��ͬʱ�洢���Ӧ����Ļ����
bool BaseLevelScene::loadPathForLevel(int levelId, const std::string& filePath)
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
/**************************************************
 *****************�������*************************
 **************************************************/
//����������
void BaseLevelScene::addMouseListener() {
    auto listener = EventListenerMouse::create();
    listener->onMouseDown = CC_CALLBACK_1(BaseLevelScene::handleMouseDown, this); // ��갴��ʱ�Ļص�

    // ����������ӵ��¼��ɷ���
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
//����������¼�
void BaseLevelScene::handleMouseDown(EventMouse* event) {
    // ��ȡ�����λ��
    Vec2 mousePos = event->getLocation();
    CCLOG("Mouse clicked at screen position: (%f, %f)", mousePos.x, mousePos.y);
    // ���� handlePlant ������������ֲ
    handlePlant(mousePos);
}
//������ֲ�������ж�
void BaseLevelScene::handlePlant(const Vec2& position) {
    CCLOG("position: (%f, %f)", position.x, position.y);
    // �����������Ļ����ת��Ϊ��Ƭ����
    Vec2 tileCoord = Vec2(floor(position.x / tileSize.width), floor(position.y / tileSize.height));
    //������־֤���Ѿ�ʵ���������ת��
    CCLOG("tileCoord: (%f, %f)", tileCoord.x, tileCoord.y);

    // �߽���
    if (tileCoord.x < 0 || tileCoord.y < 0 ||
        tileCoord.x >= tileMap->getMapSize().width ||
        tileCoord.y >= tileMap->getMapSize().height) {
        CCLOG("Tile coordinate is out of bounds.");
        return;
    }

    // �жϸ���Ƭ�Ƿ����ֲ
    auto tileGID = tileMap->getLayer("plantable")->getTileGIDAt(tileCoord);
    if (tileGID != 0) {
        CCLOG("Tile at (%f, %f) is plantable.", tileCoord.x, tileCoord.y);
        // ����ʵ����ֲ�߼�
       // ����Ƭ����ת��Ϊ��Ļ����
       // ��ȡ��ͼ�ĸ߶ȣ���Ƭ��������
        float mapHeight = tileMap->getMapSize().height;

        // ת��Ϊ��Ļ����
        float screenX = tileCoord.x * tileSize.width + tileSize.width / 2;
        float screenY = (mapHeight - tileCoord.y - 1) * tileSize.height + tileSize.height / 2;

        Vec2 mapPos = Vec2(screenX, screenY);

        // �����Ļ����
        CCLOG("Screen Position: (%f, %f)", mapPos.x, mapPos.y);

        // ����ֲ�ﾫ�飬����ֲ��ͼƬ�� "plant.png"
        auto plant = Sprite::create("Towers/TFan/TFan_1.png");
        if (plant) {
            // ����ֲ���λ�ã�ʹ��λ������Ƭ����
            plant->setPosition(mapPos);
            CCLOG("mapPos: (%f, %f)", mapPos.x, mapPos.y);
            // ����ֲ���С�������Ҫ��
            plant->setScale(2.0f);  // ���轫ֲ���С��Сһ�룬������С

            // ��ֲ����ӵ�ֲ��ͼ��
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
//��ֲ����
void BaseLevelScene::plantAt(const cocos2d::Vec2& tileCoord) {
    auto plant = cocos2d::Sprite::create("plant.png");
    auto plantPos = tileMap->getLayer("map")->getPositionAt(tileCoord);
    plant->setPosition(plantPos);
    plantsLayer->addChild(plant);
}

/**************************************************
 *****************���ߺ���*************************
 **************************************************/
//��������
#ifdef DEBUG_MODE
void BaseLevelScene::drawGrid() {
    // ��ȡ��ͼ�Ĵ�С����Ƭ�Ĵ�С
    Size mapSize = tileMap->getContentSize();
    float tileWidth = 64.0f;  // ����ÿ����Ƭ���Ϊ 64 ����
    float tileHeight = 64.0f; // ����ÿ����Ƭ�߶�Ϊ 64 ����

    // ����һ�� DrawNode ���ڻ�������
    auto drawNode = DrawNode::create();
    this->addChild(drawNode, 100);  // �� DrawNode ��ӵ������У��㼶����Ϊ 100��ȷ���ڵ�ͼ֮��

    // ���ƴ�ֱ��
    for (float x = 0; x <= mapSize.width; x += tileWidth) {
        drawNode->drawLine(Vec2(x, 0), Vec2(x, mapSize.height), Color4F::WHITE);
    }

    // ����ˮƽ��
    for (float y = 0; y <= mapSize.height; y += tileHeight) {
        drawNode->drawLine(Vec2(0, y), Vec2(mapSize.width, y), Color4F::WHITE);
    }
}
#endif // DEBUG_MODE

//�߸�����ת��ͼ����Ĺ��ߺ���
Vec2 BaseLevelScene::gridToScreenCenter(const Vec2& gridPoint) {
    // �� tileMap ��ȡ��ͼ�߶ȣ�����������
    float mapHeight = tileMap->getMapSize().height;
    // ת��Ϊ��Ļ����
    float screenX = gridPoint.x * tileSize.height + tileSize.width / 2;
    float screenY = (mapHeight - gridPoint.y - 1) * tileSize.height + tileSize.height / 2;
    return Vec2(screenX, screenY);
}
