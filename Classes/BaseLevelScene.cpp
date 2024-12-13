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
#include"GameManager.h"
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
GameManager* manager;

extern bool level_is_win[3];
extern bool is_newgame[3];
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in BaseLevelScene.cpp\n");
}
void BaseLevelScene::updatemoney(int add) {
    money += add;
    std::string text = std::to_string(money); // ������ת��Ϊ�ַ���
    money_lable->setString(text);
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
        if (isPaused == 0) {
            Director::getInstance()->resume();
        }
        });

    //���¿�ʼ��Ϸѡ��
    restartButton->setCallback([this, menuLayer](Ref* psender) {
        a.button_music();
        this->removeChild(menuLayer);
        auto scene = BaseLevelScene::createScene(levelId);   
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

//����ʱ
void BaseLevelScene::CountDown(std::function<void()> onComplete)
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
    // ���õ���sequence����
    auto countdown = Sequence::create(
        CallFunc::create([=] {
            countBackground->setVisible(true);
            count3->setVisible(true);
            a.countSound();
            }),
        DelayTime::create(1),

        CallFunc::create([=] {
            this->removeChild(count3);
            }),
        CallFunc::create([=] {
            count2->setVisible(true);
            a.countSound();
            }),
        DelayTime::create(1),

        CallFunc::create([=] {
            this->removeChild(count2);
            }),
        CallFunc::create([=] {
            count1->setVisible(true);
            a.countSound();
            }),
        DelayTime::create(1),

        CallFunc::create([=] {
            this->removeChild(count1);
            count0->setVisible(true);
            a.countSound();
            }),
        DelayTime::create(1),

        CallFunc::create([=] {
            this->removeChild(count0);
            this->removeChild(countBackground);

            // ����ʱ�����������ص�
            if (onComplete) {
                onComplete();  // ִ�лص�����
            }
            }),
        nullptr  // Sequence����
    );

    // ���е���ʱ����
    this->runAction(countdown);
}
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
    if (scene && scene->initWithLevel(level))
    {
        return scene;
    }
    return scene;
}
//��ʼ��ui���
void BaseLevelScene::initUI()
{
    //��money
    money_lable = Label::createWithTTF("400", "fonts/arial.ttf", 27);
    money_lable->setPosition(Vec2(160, 610));
    this->addChild(money_lable, 3);
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
        pauseSpeedButton->setPosition(770, 610);
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
    /*
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
    */
}
//����
void BaseLevelScene::update(float deltaTime) {
    manager->update(deltaTime);
    if(manager->CheckLose())
    {
   // gameover(false);
    }
}
//��ʼ���ؿ�
bool BaseLevelScene::initWithLevel(int level)
{
    if (!Scene::init())
    {
        return false;
    }
    initUI();                                  //��ʼ��ui
    this->levelId = level;                     //�洢�ؿ����
    this->loadMap();                           // ���ض�Ӧ�ؿ��ĵ�ͼ
    manager = GameManager::getInstance(this);  // ��ʼ��GameManager
    manager->initLevel(level);                 //��ʼ������ ·���ʹ浵����Դ
    // ���õ���ʱ���������ݻص�
    CountDown([=] {
              //��ʱ��������ܿ�ʼ���޲�
        manager->startMonsterWaves();
        });
    this->scheduleUpdate();                    //���������߼�




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
        tileSize = tileMap->getTileSize();
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





void BaseLevelScene::gameover(bool is_win) {
    Director::getInstance()->pause();
    // ���û�ɫ���ֲ�
    auto menuLayer = LayerColor::create(Color4B(0, 0, 0, 150));
    menuLayer->setPosition(Vec2::ZERO);
    this->addChild(menuLayer, 10);
    // �����˵�
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    menuLayer->addChild(menu, 1);
    // ��Ϸʤ��
    if (is_win) {
        level_is_win[levelId - 1] = true;
        //�����Ϸ��ʤ����
        auto gameWinBackground = Sprite::create("CarrotGuardRes/UI/WinGame.png");
        gameWinBackground->setPosition(Vec2(480, 320));
        gameWinBackground->setScale(1.5f);
        menuLayer->addChild(gameWinBackground, 0);
        //��ӻ�ʤ�Ľ��ܲ���ʶ
        auto goldenCarrot = Sprite::create("CarrotGuardRes/UI/goldenCarrot.png");
        goldenCarrot->setPosition(Vec2(960 * 0.493, 640 * 0.7));
        menuLayer->addChild(goldenCarrot, 0);

        // ʤ���������ʾ��
        /*
        _curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", _currNum > _monsterWave ? _monsterWave : _currNum), "Arial", 32);
        _curNumberLabel->setColor(Color3B::YELLOW);
        _curNumberLabel->setPosition(960 * 0.51, 640 * 0.54);
        */
        Label* loseWordLeft = Label::createWithSystemFont("fought off", "Arial", 30);
        loseWordLeft->setPosition(960 * 0.36, 640 * 0.54);
        Label* loseWordRight = Label::createWithSystemFont("waves", "Arial", 30);
        loseWordRight->setPosition(960 * 0.60, 640 * 0.545);

        //this->addChild(_curNumberLabel, 10);
        this->addChild(loseWordLeft, 10);
        this->addChild(loseWordRight, 10);
        //������Ϸ��ť
        auto continueButton = MenuItemImage::create("CarrotGuardRes/UI/continueNormal.png", "CarrotGuardRes/UI/continueSelected.png");
        continueButton->setPosition(Vec2(960 * 0.613, 640 * 0.375));
        continueButton->setScale(1.38);
        continueButton->setCallback([this, menuLayer](Ref* psender) {
            a.button_music();
            //����ǰδ�����ŵ����һ�أ��������һ��
            if (levelId < 3) {
                auto scene = BaseLevelScene::createScene(levelId + 1);
                Director::getInstance()->replaceScene(scene);
            }
            //����ǰ�Ѿ��ǿ��ŵ����һ�أ��򷵻�ѡ��ؿ�����
            else {
                auto themeScene = themescene::createScene();
                Director::getInstance()->replaceScene(themeScene);
            }
            });
        menu->addChild(continueButton, 1);
    }
    // ��Ϸʧ��
    else {
        auto gameLoseBackground = Sprite::create("CarrotGuardRes/UI/LoseGame.png");
        gameLoseBackground->setPosition(Vec2(960 / 2 + 960 * 0.01, 640 / 2 + 640 * 0.015));
        gameLoseBackground->setScale(1.5f);
        menuLayer->addChild(gameLoseBackground, 0);

        // ��Ϸʧ�ܵ������ʾ��
        /*
        _curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", _currNum - 1), "Arial", 32);// ��ʱû��currnumΪʲô���1��������-1
        _curNumberLabel->setColor(Color3B::YELLOW);
        _curNumberLabel->setPosition(960 * 0.51, 640 * 0.54);*/
        Label* loseWordLeft = Label::createWithSystemFont("fought off", "Arial", 30);
       
        loseWordLeft->setPosition(960 * 0.36, 640 * 0.54);
        Label* loseWordRight = Label::createWithSystemFont("waves", "Arial", 30);
        loseWordRight->setPosition(960 * 0.60, 640 * 0.54);

        //this->addChild(_curNumberLabel, 10);
        this->addChild(loseWordLeft, 10);
        this->addChild(loseWordRight, 10);
        //������Ϸ��ť
        auto againButton = MenuItemImage::create("CarrotGuardRes/UI/AgainNormal.png", "CarrotGuardRes/UI/AgainSelected.png");
        againButton->setPosition(Vec2(960 * 0.61, 640 * 0.37));
        againButton->setScale(0.9);
        // ���¿�ʼ��ť��ѡ��
        againButton->setCallback([this, menuLayer](Ref* psender) {
            a.button_music();
            this->removeChild(menuLayer);
            auto scene = BaseLevelScene::createScene(levelId);
            Director::getInstance()->replaceScene(scene);
            Director::getInstance()->resume();
            });
        menu->addChild(againButton, 1);
    }
    // ѡ����Ϸ�ؿ���ť
    auto chooseButton = MenuItemImage::create("CarrotGuardRes/UI/chooseLevelNormal.png", "CarrotGuardRes/UI/chooseLevelSelected.png");
    chooseButton->setPosition(Vec2(960 * 0.38, 640 * 0.37));
    chooseButton->setScale(1.4);
    //ѡ��ؿ�ѡ��
    chooseButton->setCallback([this, menuLayer](Ref* psender) {
        a.button_music();
        this->removeChild(menuLayer);
        auto themeScene = themescene::createScene();
        Director::getInstance()->replaceScene(themeScene);
        Director::getInstance()->resume();
        });
    menu->addChild(chooseButton, 1);
}
