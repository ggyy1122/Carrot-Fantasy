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
#include<string>
using namespace ui;
//#define DEBUG_MODE

#define CELL_SIZE 64



extern float beishu;
extern Music a;
extern bool level_is_win[3];
extern bool isNewGame[3];
GameManager* manager;
const Color3B moneyColor(154, 101, 25);
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in BaseLevelScene.cpp\n");
}
// �ؿ���ͼ�ļ�·������
const std::vector<std::string> BaseLevelScene::mapFiles = {
    "map/map1.tmx",  // �ؿ� 1
    "map/map2.tmx",  // �ؿ� 2
    "map/map3.tmx",  // �ؿ� 3
};

/****************��ǩ����******************/
//���½�ұ�ǩ
void BaseLevelScene::updateMoney(int add) {
    money += add;
    std::string text = std::to_string(money); // ������ת��Ϊ�ַ���
    moneyLable->setString(text);
}
//���µ�ǰ������ǩ
void BaseLevelScene::updateCurrentWaveLabe() {
    _curNumberLabel->setString(StringUtils::format("%d", std::min(manager->getCurrentWaveNum() + 1, manager->getAllWaveNum())));
    CCLOG("ALL WAVE                           %d", manager->getAllWaveNum());
}
/******************************************/


/****************��ť����******************/
//�����ٰ�ť
void BaseLevelScene::doublespeed(Ref* pSender) {
    isDoubleSpeed = !isDoubleSpeed; // �л�������״̬
    a.button_music();
    MenuItemImage* button = static_cast<MenuItemImage*>(pSender);
    if (isDoubleSpeed) {
        button->setNormalImage(Sprite::create("CarrotGuardRes/UI/doubleSpeed.png"));
        button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/doubleSpeed.png"));
        tower_jiasu = 2;
        guaisou_jiansu(2.0f);
        beishu = 2;//ʵ�ּ���Ч��
    }
    else {
        button->setNormalImage(Sprite::create("CarrotGuardRes/UI/normalSpeed.png"));
        button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/normalSpeed.png"));
        tower_jiasu = 1;
        guaisou_jiansu(1.0f);
        beishu = 1; //ʵ�ּ���Ч��
    }
}
//��ͣ��ť
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
        pauseTop->setPosition(464, 610);
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
//�˵���ť
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
        // ȡ���� GameManager ��ص����е�����
        GameManager::getInstance()->stopAllSchedulers();
        //ȡ���¼�����
        manager->removeListener();
        auto scene = BaseLevelScene::createScene(levelId);
        scheduler->setTimeScale(1.0f); //ʵ�ּ���Ч��
        Director::getInstance()->replaceScene(scene);
        Director::getInstance()->resume();
        });

    //ѡ��ؿ�ѡ��
    chooseButton->setCallback([this, menuLayer](Ref* psender) {
        a.button_music();
        manager->saveMonstersDataToJson("level"+std::to_string(levelId)+"Monster.json");
        this->removeChild(menuLayer);
        // ȡ���� GameManager ��ص����е�����
        GameManager::getInstance()->stopAllSchedulers();
        //ȡ���¼�����
        manager->removeListener();
        auto themeScene = themescene::createScene();
        Director::getInstance()->replaceScene(themeScene);
        Director::getInstance()->resume();
        });
    menu->addChild(continueButton, 1);
    menu->addChild(chooseButton, 1);
    menu->addChild(restartButton, 1);
}
void BaseLevelScene::guaisou_jiansu(float guai_jiansu) {
    for (auto it = manager->monsters.begin(); it != manager->monsters.end(); it++) {
        if ((*it)->health > 0) 
            (*it)->speedaction->setSpeed(guai_jiansu);   
    }
}
//����1��ť
void BaseLevelScene::Jineng1(Ref* pSender) {
    a.button_music();
    if (money >= 100) {
        updateMoney(-100);
        manager->Jineng1();
    }
}
//����2��ť
void BaseLevelScene::Jineng2(Ref* pSender) {
    a.button_music();
    if (money >= 100) {
        updateMoney(-100); 
        guaisou_jiansu(0.01f);
        beishu = 0.01f;
        auto delayaction = Sequence::create(
            DelayTime::create(5.0f),
            CallFunc::create([=] {guaisou_jiansu(1); beishu = 1.0f; }),
            nullptr);
        this->runAction(delayaction);
    }
}
//����3��ť
void BaseLevelScene::Jineng3(Ref* pSender) {
    a.button_music();
    if (money >= 100) {
        updateMoney(-100);
        for (auto it = manager->monsters.begin(); it != manager->monsters.end(); it++) {
            if ((*it)->health > 0) {
                (*it)->health = 0;
                (*it)->toDie(this);
            }    
        }
    }
}
//����4��ť
void BaseLevelScene::Jineng4(Ref* pSender) {
    a.button_music();
    if (money >= 100) {
        updateMoney(-100);
        tower_jiasu = 2;
        auto delayaction = Sequence::create(DelayTime::create(5.0f), CallFunc::create([=] {
            tower_jiasu = 1;
            }), nullptr);
        this->runAction(delayaction);
    }
}
//����5��ť
void BaseLevelScene::Jineng5(Ref* pSender) {
    a.button_music();
    if (money >= 100) {
        updateMoney(-100);
        guaisou_jiansu(0.5f);
        beishu = 0.5f;
        auto delayaction = Sequence::create(
            DelayTime::create(5.0f),
            CallFunc::create([=] {guaisou_jiansu(1); beishu = 1.0f; }),
            nullptr);
        this->runAction(delayaction);
    }
}
/******************************************/


//��ʼ��ui���
void BaseLevelScene::initUI()
{
    // 1. ��ʾ�����˶��ٲ�����  
    _curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", std::min(manager->getCurrentWaveNum(), manager->getAllWaveNum())), "Arial", 32);
    _curNumberLabel->setColor(Color3B::YELLOW);
    _curNumberLabel->setPosition(960 * 0.42, 640 * 0.95);
    this->addChild(_curNumberLabel, 2);
    // 2. һ���ж��ٲ�����
    _numberLabel = Label::createWithSystemFont(StringUtils::format("/ %d times", manager->getAllWaveNum()), "Arial", 32);
    _numberLabel->setColor(Color3B::YELLOW);
    _numberLabel->setPosition(960 * 0.50, 640 * 0.95);
    this->addChild(_numberLabel, 2);
    //��money
    moneyLable = Label::createWithTTF("1000", "fonts/arial.ttf", 27);
    moneyLable->setPosition(Vec2(160, 610));
    this->addChild(moneyLable, 3);
    //��ӷ��ذ�ť
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 2);
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
    auto jineng1Button = MenuItemImage::create("Carrot/jineng1.png", "Carrot/jineng1_1.png", CC_CALLBACK_1(BaseLevelScene::Jineng1, this));
    if (jineng1Button == nullptr)
    {
        problemLoading("'jineng1.png'");
    }
    else
    {
        jineng1Button->setPosition(432, 33);
        jineng1Button->setScale(1.9);
        menu->addChild(jineng1Button);
    }
    auto jineng2Button = MenuItemImage::create("Carrot/jineng2.png", "Carrot/jineng2_1.png", CC_CALLBACK_1(BaseLevelScene::Jineng2, this));
    if (jineng2Button == nullptr)
    {
        problemLoading("'jineng2.png'");
    }
    else
    {
        jineng2Button->setPosition(525, 33);
        jineng2Button->setScale(1.9);
        menu->addChild(jineng2Button);
    }
    auto jineng3Button = MenuItemImage::create("Carrot/jineng3.png", "Carrot/jineng3_1.png", CC_CALLBACK_1(BaseLevelScene::Jineng3, this));
    if (jineng3Button == nullptr)
    {
        problemLoading("'jineng3.png'");
    }
    else
    {
        jineng3Button->setPosition(615, 33);
        jineng3Button->setScale(1.7);
        menu->addChild(jineng3Button);
    }
    auto jineng4Button = MenuItemImage::create("Carrot/jineng4.png", "Carrot/jineng4_1.png", CC_CALLBACK_1(BaseLevelScene::Jineng4, this));
    if (jineng4Button == nullptr)
    {
        problemLoading("'jineng4.png'");
    }
    else
    {
        jineng4Button->setPosition(710, 33);
        jineng4Button->setScale(1.7);
        menu->addChild(jineng4Button);
    }
    auto jineng5Button = MenuItemImage::create("Carrot/jineng5.png", "Carrot/jineng5_1.png", CC_CALLBACK_1(BaseLevelScene::Jineng5, this));
    if (jineng5Button == nullptr)
    {
        problemLoading("'jineng5.png'");
    }
    else
    {
        jineng5Button->setPosition(796, 33);
        jineng5Button->setScale(1.7);
        menu->addChild(jineng5Button);
    }

    auto jinengtiao = Sprite::create("Carrot/jinengtiao.png");
    jinengtiao->setPosition(480, 30);
    jinengtiao->setScale(0.7);
    this->addChild(jinengtiao, 1);
}
//���ֵ���ʱ
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


// ��������ʱ����ؿ����
Scene* BaseLevelScene::createScene(int level) {
    auto scene = BaseLevelScene::create();  // ���� BaseLevelScene
    if (scene && scene->initWithLevel(level))
    {
        return scene;
    }
    return scene;
}




//������Ϸ״̬
void BaseLevelScene::update(float deltaTime) {
    //���µ�ǰ����
    updateCurrentWaveLabe();
    //manager->update(deltaTime);
    //���������͹��޽���
    for (auto it = towers.begin(); it != towers.end(); it++) {

        if (it->second->interval >= it->second->interval_table[it->second->GetIndex()]) {
            it->second->attack(this, GameManager::getInstance()->monsters, isTarget, tar_m, tar_o,tower_jiasu);
            continue;
        }

        it->second->interval += deltaTime*tower_jiasu;
    }
    //���¼�����״̬
    if(manager->CheckLose())
    {
       gameover(false,manager->getCurrentWaveNum(), manager->getAllWaveNum());
    }
    //���¼���Ӯ״̬
    if(manager->CheckWin())
    {
        gameover(true, manager->getCurrentWaveNum(), manager->getAllWaveNum());
    }
}


//��ʼ���ؿ�
bool BaseLevelScene::initWithLevel(int level)
{
    if (!Scene::init())
    {
        return false;
    }
    Director::getInstance()->resume();                             
    this->levelId = level;                     //�洢�ؿ����
    this->loadMap();                           // ���ض�Ӧ�ؿ��ĵ�ͼ
    manager = GameManager::getInstance(this);  // ��ʼ��GameManager
    manager->initLevel(level, !isNewGame[levelId-1]);          //��ʼ������ ·���ʹ浵����Դ
    initUI();                                 //��ʼ��ui
    // ���õ���ʱ���������ݻص�
    CountDown([=] {
        CCLOG("READY");
              //��ʱ��������ܿ�ʼ���޲�
        manager->startMonsterWaves();
        });
        
    this->scheduleUpdate();                    //���������߼�




    // 1. ����ֲ��ͼ��
    plantsLayer = Layer::create();  // ����һ���µ�ͼ�㣬���ڴ��ֲ��
    this->addChild(plantsLayer, 10); // ��ֲ��ͼ����ӵ�������zOrderΪ3��ȷ����λ��������֮��
    addMouseListener();  // ���������

    cell_flag = 1;
    buy_tower[0].push_back("Towers/affordbottle.png"); buy_tower[1].push_back("Towers/unaffordbottle.png");
    buy_tower[0].push_back("Towers/affordsun.png"); buy_tower[1].push_back("Towers/unaffordsun.png");
    buy_tower[0].push_back("Towers/affordplane.png"); buy_tower[1].push_back("Towers/unaffordplane.png");
    index_table.push_back(0);
    index_table.push_back(1);
    index_table.push_back(2);
    InitMapData();
    ProduceObstacles();
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
 // ���ص�ͼ
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

    if (isTarget != 0) {//������������ڼ����κ�λ�ã�����ʹ������ʧ
        isTarget = 0;
        tar_m = nullptr;
        tar_o = nullptr;
        return;
    }

    // �жϸ���Ƭ�Ƿ����ֲ
    auto tileGID = tileMap->getLayer("plantable")->getTileGIDAt(tileCoord);

    if (!cell_flag) {//����Ѿ�������ֲ���������˵������ٴε��ʱ�ж��Ƿ������ֲ������(ɾ��)��������ʹ�˵���ʧ
        if (map_data[int(last_position.x / CELL_SIZE)][int(last_position.y / CELL_SIZE)].flag == 0)
            PlantMenuGone(Vec2(position.x, Director::getInstance()->getVisibleSize().height - position.y));
        else if (map_data[int(last_position.x / CELL_SIZE)][int(last_position.y / CELL_SIZE)].flag == 1)
            UpMenuGone(Vec2(position.x, Director::getInstance()->getVisibleSize().height - position.y));
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

        //�����ط�û��������ô����ֲ�˵�������������˵�
        if (map_data[int(mapPos.x / CELL_SIZE)][int(mapPos.y / CELL_SIZE)].flag == 0)
            PlantMenuAppear(mapPos);
        else if (map_data[int(mapPos.x / CELL_SIZE)][int(mapPos.y / CELL_SIZE)].flag == 1)
            UpMenuAppear(mapPos);
        else if (isTarget == 0) {
            isTarget = 2;
            tar_o = Obstacles[map_data[int(mapPos.x / CELL_SIZE)][int(mapPos.y / CELL_SIZE)].key];
        }
    }

    else
    {
        Monster* tool = IsTargetMonster(Vec2(position.x, Director::getInstance()->getVisibleSize().height - position.y));
        if (tool != nullptr) {
            isTarget = 1;
            tar_m = tool;
            CCLOG("-------------------------------------------------locked");
        }
        CCLOG("Tile at (%f, %f) is not plantable.", tileCoord.x, tileCoord.y);
    }

}


/**************************************************
 *****************���ߺ���*************************
 **************************************************/
 //��������
//#ifdef DEBUG_MODE
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
//#endif // DEBUG_MODE

//�߸�����ת��ͼ����Ĺ��ߺ���
Vec2 BaseLevelScene::gridToScreenCenter(const Vec2& gridPoint) {
    // �� tileMap ��ȡ��ͼ�߶ȣ�����������
    float mapHeight = tileMap->getMapSize().height;
    // ת��Ϊ��Ļ����
    float screenX = gridPoint.x * tileSize.height + tileSize.width / 2;
    float screenY = (mapHeight - gridPoint.y - 1) * tileSize.height + tileSize.height / 2;
    return Vec2(screenX, screenY);
}




//��Ϸ�����Ľ��洦��
void BaseLevelScene::gameover(bool is_win, int currentWaveNum, int allWaveNum) {
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
       
        _curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", currentWaveNum > allWaveNum ? allWaveNum : currentWaveNum), "Arial", 32);
        _curNumberLabel->setColor(Color3B::YELLOW);
        _curNumberLabel->setPosition(960 * 0.51, 640 * 0.54);
        Label* loseWordLeft = Label::createWithSystemFont("fought off", "Arial", 30);
        loseWordLeft->setPosition(960 * 0.36, 640 * 0.54);
        Label* loseWordRight = Label::createWithSystemFont("waves", "Arial", 30);
        loseWordRight->setPosition(960 * 0.60, 640 * 0.545);

        this->addChild(_curNumberLabel, 10);
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
        
        _curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", currentWaveNum - 1), "Arial", 32);// ��ʱû��currnumΪʲô���1��������-1
        _curNumberLabel->setColor(Color3B::YELLOW);
        _curNumberLabel->setPosition(960 * 0.51, 640 * 0.54);
        Label* loseWordLeft = Label::createWithSystemFont("fought off", "Arial", 30);
       
        loseWordLeft->setPosition(960 * 0.36, 640 * 0.54);
        Label* loseWordRight = Label::createWithSystemFont("waves", "Arial", 30);
        loseWordRight->setPosition(960 * 0.60, 640 * 0.54);

        this->addChild(_curNumberLabel, 10);
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
            // ȡ���� GameManager ��ص����е�����
           // GameManager::getInstance()->stopAllSchedulers();
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
        manager->saveMonstersDataToJson("level" + std::to_string(levelId) + "Monster.json");
        this->removeChild(menuLayer);
        // ȡ���� GameManager ��ص����е�����
        GameManager::getInstance()->stopAllSchedulers();
        //ȡ���¼�����
        manager->removeListener();
        auto themeScene = themescene::createScene();
        Director::getInstance()->replaceScene(themeScene);
        Director::getInstance()->resume();
        });
    menu->addChild(chooseButton, 1);
   
}


//��ֲ�������
void BaseLevelScene::PlantMenuAppear(Vec2 mapPos)
{
    cell_flag = 0;
    auto location = mapPos;
    last_position = location;
    auto cell = Sprite::create("Towers/cell.png");
    cell->setScale((float)64 / 200);
    cell->setPosition(location);
    this->addChild(cell); curr_cell = cell;
    for (int i = 0, size = buy_tower[0].size(); i < size; i++) {
        int tool = Tower::build_cost[index_table[i]] > money ? 1 : 0;
        auto tower_graph = Sprite::create(buy_tower[tool][i]);
        tower_graph->setScale((float)CELL_SIZE / tower_graph->getContentSize().width);
        tower_graph->setPosition(Vec2(location.x - CELL_SIZE * (size - 1) / 2 + CELL_SIZE * i, location.y + CELL_SIZE));
        this->addChild(tower_graph); remove_table.push_back(tower_graph);
    }
}
//��ֲ������ʧ
void BaseLevelScene::PlantMenuGone(Vec2 position)
{
    auto location = position;
    int size = remove_table.size();
    if ((location.x > last_position.x - CELL_SIZE * size / 2 && location.x < last_position.x + CELL_SIZE * size / 2)
        && (location.y > last_position.y + CELL_SIZE / 2 && location.y < last_position.y + 3 * CELL_SIZE / 2)) {
        int index = index_table[(location.x - last_position.x + CELL_SIZE * size / 2) / CELL_SIZE];
        if (money >= Tower::build_cost[index]) {
            Tower* this_tower = createTower(index);
            this_tower->build(this, last_position);
            int x = int(last_position.x / CELL_SIZE), y = int(last_position.y / CELL_SIZE);
            towers[map_data[x][y].key] = this_tower;
            map_data[x][y].flag = 1;
        }
    }
    this->removeChild(curr_cell); curr_cell->release();
    for (int i = 0; i < remove_table.size(); i++) { this->removeChild(remove_table[i]); remove_table[i]->release(); }
    remove_table.clear();
    cell_flag = 1;
}


void BaseLevelScene::UpMenuAppear(Vec2& position)
{
    last_position = position;
    int key = map_data[int(position.x / CELL_SIZE)][int(position.y / CELL_SIZE)].key;
    towers[key]->UpMenuAppear(this, position);
    cell_flag = 0;
}
void BaseLevelScene::UpMenuGone(Vec2& position)
{
    int key = BaseLevelScene::map_data[int(last_position.x / CELL_SIZE)][int(last_position.y / CELL_SIZE)].key;
    towers[key]->UpMenuGone(this);
    if (position.x > last_position.x - CELL_SIZE / 2 && position.x < last_position.x + CELL_SIZE / 2 &&
        position.y > last_position.y - 3 * CELL_SIZE / 2 && position.y < last_position.y - CELL_SIZE / 2) {
        towers[key]->destroy(this);
        delete towers[key];
        towers.erase(key);
        map_data[int(last_position.x / CELL_SIZE)][int(last_position.y / CELL_SIZE)].flag = 1;
    }
    else if (position.x > last_position.x - CELL_SIZE / 2 && position.x < last_position.x + CELL_SIZE / 2 &&
        position.y > last_position.y + CELL_SIZE / 2 && position.y < last_position.y + 3 * CELL_SIZE / 2) {
        towers[key]->update(this, last_position);
    }
    cell_flag = 1;
}


void BaseLevelScene::InitMapData()//��ʼ����ͼ����
{
    for (int i = 0; i < X; i++)
        for (int j = 0; j < Y; j++) { map_data[i][j].flag = 0; map_data[i][j].key = j * X + i; }
    for (int i = 2; i <= 11; i++) {
        for (int j = 5; j <= 8; j++) { map_data[i][j].flag = 3; map_data[i][j].key = j * X + i; }
        for (int j = 1; j <= 2; j++) { map_data[i][j].flag = 3; map_data[i][j].key = j * X + i; }
    }//3��ʾ�˵ؿ����������ϰ���ں��������ϰ���ʱ���õ� 
}


void BaseLevelScene::ProduceObstacles()//�ڳ����������ϰ���
{
    srand(time(0));
    for (int i = 0; i < X; i++)
        for (int j = 0; j < Y; j++) {
            if (map_data[i][j].flag == 3) {
                int rd;//�������
                if (i + 1 < X && j + 1 < Y && map_data[i + 1][j].flag == 3 && map_data[i][j + 1].flag == 3 && map_data[i + 1][j + 1].flag == 3)
                    rd = 10;
                else if (i + 1 < X && map_data[i + 1][j].flag == 3) rd = 6;
                else rd = 5;
                int index = rand() % (rd + 5);
                if (index >= rd) {//��������ÿ���������ϰ���ĵط��������ϰ���
                    map_data[i][j].flag = 0;
                    continue;
                }
                map_data[i][j].flag = 2;
                auto obb = new Obstacle(index);
                obb->Produce(this, i, j);
                Obstacles[map_data[i][j].key] = obb;
                if (index == 5) {
                    map_data[i + 1][j].flag = 2;
                    Obstacles[map_data[i + 1][j].key] = obb;
                }
                else if (index > 5) {
                    map_data[i][j + 1].flag = map_data[i + 1][j].flag = map_data[i + 1][j + 1].flag = 2;
                    Obstacles[map_data[i + 1][j].key] = Obstacles[map_data[i][j + 1].key] = Obstacles[map_data[i + 1][j + 1].key] = obb;
                }
            }
        }
}

Monster* BaseLevelScene::IsTargetMonster(const Vec2& pos)//�ж��Ƿ��й��ﱻ����
{
    for (auto it = GameManager::getInstance()->monsters.begin(); it != GameManager::getInstance()->monsters.end(); it++) {
        if ((*it)->health <= 0)continue;
        auto m_pos = (*it)->getPosition();
        auto m_size = (*it)->getContentSize();
        if (pos.x > m_pos.x - m_size.width && pos.x < m_pos.x + m_size.width
            && pos.y>m_pos.y - m_size.height && pos.y < m_pos.y + m_size.height) return (*it);
    }
    return nullptr;
}
