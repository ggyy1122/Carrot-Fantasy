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



// ����ؿ���ͼ�ļ�·������
const std::vector<std::string> BaseLevelScene::mapFiles = {
    "map/map1.tmx",  // �ؿ� 1
    "map/map2.tmx",  // �ؿ� 2
    "map/map3.tmx",  // �ؿ� 3
    // ���Լ�����������ؿ��ĵ�ͼ�ļ�·��
};

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
    this->levelId=level;                   //�洢�ؿ����
    this->loadMap();                       // ���ض�Ӧ�ؿ��ĵ�ͼ
    this->placeMonsters ();                //���ù���
    //scheduleUpdate();                    //���������߼�
  
    // 1. ����ֲ��ͼ��
    plantsLayer = Layer::create();  // ����һ���µ�ͼ�㣬���ڴ��ֲ��
    this->addChild(plantsLayer, 10); // ��ֲ��ͼ����ӵ�������zOrderΪ3��ȷ����λ��������֮��
    addMouseListener();  // ���������

    cell_flag = 1;
    buy_tower.push_back("Towers/affordbottle.png");
    buy_tower.push_back("Towers/affordwindmill.png");
    index_table.push_back(0);
    index_table.push_back(1);
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
        //auto plant = Sprite::create("Towers/TFan/TFan_1.png");
        //if (plant) {
        //    // ����ֲ���λ�ã�ʹ��λ������Ƭ����
        //    plant->setPosition(mapPos);
        //    CCLOG("mapPos: (%f, %f)", mapPos.x, mapPos.y);
        //    // ����ֲ���С�������Ҫ��
        //    plant->setScale(2.0f);  // ���轫ֲ���С��Сһ�룬������С

        //    // ��ֲ����ӵ�ֲ��ͼ��
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
