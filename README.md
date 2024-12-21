# carrotfantasy
### 同济大学 2024程序设计范式大作业
## 项目信息
* 项目选题 ： 保卫萝⼘
* 项目成员 ：官奕、于广淳、周慧星
* 项目Github地址：
[https://github.com/xing05188/CattotFantasy.git]()
本项目文档将介绍这个游戏项目的主要逻辑和核心代码。

## 1. 项目架构

### 1.1 核心类图
```
GameManager (单例)
├── BaseLevelScene
│   ├── Tower
│   │   ├── Bottle
│   │   ├── Sun
│   │   ├── MyPlane
│   │   └── Shit
│   ├── Monster
│   └── Carrot
├── MenuScene
├── ThemeScene
└── SetScene
```

### 1.2 文件结构
```
Classes/
├── AppDelegate.cpp/h          // 应用程序入口
├── BaseLevelScene.cpp/h       // 基础关卡场景
├── GameManager.cpp/h          // 游戏管理器
├── Monster.cpp/h              // 怪物类
├── Tower.cpp/h               // 防御塔基类
├── Carrot.cpp/h              // 萝卜类
├── Music.cpp/h               // 音效管理
└── Scene/                    // 场景文件
    ├── menuScene.cpp/h
    ├── setScene.cpp/h
    ├── themeScene.cpp/h
    └── advantureScene.cpp/h
```

## 2. 核心系统详解

### 2.1 游戏管理器 (GameManager)

GameManager 采用单例模式，作为整个游戏的核心控制器：

```cpp
class GameManager {
private:
    static GameManager* instance;
    BaseLevelScene* currentScene;
    Carrot* carrot;                    // 萝卜实例
    int AllMonsterNum = 0;             // 总怪物数
    std::vector<WaveConfig> waveConfigs; // 波次配置
    int waveIndex = 0;                 // 当前波次
    int AllWaveNum = 0;                // 总波次数
    std::map<int, std::vector<Vec2>> pathsCache; // 路径缓存
    
public:
    static GameManager* getInstance(BaseLevelScene* scene = nullptr);
    void initLevel(int level, bool ReadMode);
    void update(float deltaTime);
    bool CheckLose();
    bool CheckWin();
    void produceMonsterWave(const WaveConfig& waveConfig);
};
```

### 2.2 防御塔系统

#### 2.2.1 基础塔类
```cpp
class Tower {
protected:
    int index, grade;    // 塔的编号和等级
    int range, demage;   // 攻击范围和伤害
    float interval;      // 攻击间隔
    
public:
    static std::string tower_table[TOWER_NUM][3];  // 塔的图片资源
    static int build_cost[TOWER_NUM];              // 建造费用
    static int up_cost[TOWER_NUM][2];             // 升级费用
    static int range_table[TOWER_NUM][3];         // 攻击范围表
    static int demage_table[TOWER_NUM][3];        // 伤害表
    
    virtual void attack(BaseLevelScene*, std::vector<Monster*>&, char isTarget, 
                       Monster* tar_m, Obstacle* tar_o, int jiasu) = 0;
    void build(BaseLevelScene*, Vec2);
    void destroy(BaseLevelScene*);
    void update(BaseLevelScene*, Vec2);
};
```

#### 2.2.2 特殊塔实现
```cpp
// 太阳塔 - 范围攻击
class Sun : public Tower {
public:
    virtual void attack(BaseLevelScene*, std::vector<Monster*>&, 
                       char isTarget, Monster* tar_m, Obstacle* tar_o, int jiasu);
    void SunAttack(BaseLevelScene* my_scene, std::vector<Monster*>& monsters, int jiasu);
};

// 飞机塔 - 直线攻击
class MyPlane : public Tower {
private:
    float tower_angle;  // 塔的角度
    
public:
    template<class T>
    bool AttackSprite(T* sp, BaseLevelScene* my_scene, 
                     std::vector<Monster*>& monsters, int jiasu);
};
```

### 2.3 怪物系统

#### 2.3.1 怪物配置
```cpp
struct MonsterConfig {
    std::string spriteFrameName;  // 精灵帧名称
    int health;                   // 生命值
    float speed;                  // 移动速度
    int damage;                   // 攻击力
    int reward;                   // 击杀奖励
};
```

#### 2.3.2 怪物类实现
```cpp
class Monster : public cocos2d::Sprite {
private:
    Vec2 endPos;          // 终点位置
    int startPosIndex;    // 起始位置
    std::string name;     // 怪物名称
    int PathIndex;        // 路径索引
    int maxHp;           // 最大生命值
    
public:
    LoadingBar* _HP;      // 血条显示
    float speed;          // 移动速度
    bool pause;           // 暂停状态
    int damage;           // 攻击力
    int reward;           // 奖励金币
    int health;          // 当前生命值
    
    void moveAlongPath(const std::vector<Vec2>& path);
    void toDie(BaseLevelScene* my_scene);
    void updateHealthBar();
};
```

### 2.4 关卡系统

#### 2.4.1 基础关卡场景
```cpp
class BaseLevelScene : public cocos2d::Scene {
private:
    TMXTiledMap* tileMap;     // 地图
    TMXLayer* plantableLayer;  // 可放置层
    int levelId;              // 关卡ID
    Label* moneyLable;        // 金钱显示
    bool isDoubleSpeed;       // 双倍速度
    bool isPaused;           // 暂停状态
    
public:
    void initUI();
    void loadMap();
    void addMouseListener();
    void handleMouseDown(EventMouse* event);
    void plantAt(const Vec2& tileCoord);
    void updateMoney(int add);
    void gameover(bool is_win, int currentWaveNum, int allWaveNum);
};
```

### 2.5 存档系统

#### 2.5.1 存档数据结构
```cpp
// JSON 格式示例
{
    "level": 1,
    "money": 1000,
    "wave": {
        "current": 2,
        "total": 5
    },
    "monsters": [
        {
            "type": "normal",
            "health": 100,
            "position": {
                "x": 100,
                "y": 200
            }
        }
    ],
    "towers": [
        {
            "type": "sun",
            "level": 2,
            "position": {
                "x": 300,
                "y": 400
            }
        }
    ]
}
```

#### 2.5.2 存档管理
```cpp
void GameManager::saveMonstersDataToJson(const std::string& fileName) {
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    
    // 保存基本信息
    document.AddMember("level", levelId, allocator);
    document.AddMember("money", currentScene->money, allocator);
    
    // 保存波次信息
    rapidjson::Value waveInfo(rapidjson::kObjectType);
    waveInfo.AddMember("current", waveIndex, allocator);
    waveInfo.AddMember("total", AllWaveNum, allocator);
    document.AddMember("wave", waveInfo, allocator);
    
    // 保存怪物信息
    rapidjson::Value monstersArray(rapidjson::kArrayType);
    for (auto monster : monsters) {
        if (monster->health > 0) {
            rapidjson::Value monsterObj(rapidjson::kObjectType);
            monsterObj.AddMember("type", 
                rapidjson::Value(monster->getMonsterName().c_str(), allocator),
                allocator);
            monsterObj.AddMember("health", monster->health, allocator);
            monstersArray.PushBack(monsterObj, allocator);
        }
    }
    document.AddMember("monsters", monstersArray, allocator);
}
```

### 2.6 音效系统

```cpp
class Music {
private:
    int music_play;      // 音效开关
    int soundEffectID;   // 音效ID
    
public:
    void preloadSoundEffect(const std::string& music_file) {
        AudioEngine::preload(music_file);
    }
    
    void background_music() {
        if (music_play)
            soundEffectID = AudioEngine::play2d("Music/bgm.mp3", true, 0.5f);
        else
            AudioEngine::pause(soundEffectID);
    }
    
    // 各种音效
    void button_music();
    void build_music();
    void sell_music();
    void upgrade_music();
    void carrotSound();
    void bottleSound();
    void planeSound();
    void ShitSound();
};
```

## 3. 游戏特性

### 3.1 防御塔特性
- 瓶子塔：单体攻击，攻击速度快
- 太阳塔：范围攻击，伤害较高
- 飞机塔：直线攻击，穿透效果
- 便便塔：减速效果，战略价值高

### 3.2 怪物特性
- 不同类型怪物有不同的生命值、移动速度和攻击力
- 怪物死亡会掉落金币
- 怪物有血条显示
- 支持多路径移动

### 3.3 游戏机制
- 金钱系统：建造和升级塔需要消耗金币
- 波次系统：怪物分波次进攻
- 升级系统：防御塔可升级提升属性
- 暂停功能：可随时暂停游戏
- 双倍速度：可加快游戏进程

## 4. 技术亮点

1. 设计模式运用：
   - 单例模式：GameManager
   - 观察者模式：事件系统
   - 工厂模式：怪物和防御塔的创建

2. 数据管理：
   - JSON配置文件管理游戏数据
   - 存档系统支持游戏进度保存

3. 性能优化：
   - 对象池管理
   - 资源预加载
   - 场景管理优化

4. 代码架构：
   - 清晰的类层次结构
   - 高内聚低耦合的模块设计
   - 可扩展的系统架构

* 项目进度时间线：
* 
* 场景的逻辑关系：
*
![屏幕截图 2024-12-21 190132](https://github.com/user-attachments/assets/124578a9-ad6c-4ead-81cc-884500dc3ffd)
*
*主场景：
*
![屏幕截图 2024-12-21 192116](https://github.com/user-attachments/assets/6e6dddc9-f078-491c-815c-e4457068534b)
*
*设置界面：
*
![屏幕截图 2024-12-21 192410](https://github.com/user-attachments/assets/a9d08696-d8b2-4591-9d6a-7e6f3714c44e)
*
*主题界面：
*
![屏幕截图 2024-12-21 192451](https://github.com/user-attachments/assets/fd4b44b4-a588-43a5-b2a4-f3176a026b78)
*
*关卡选择界面：
*
![屏幕截图 2024-12-21 192528](https://github.com/user-attachments/assets/850ad3f5-f1f0-4de8-bcf7-f200f0b5227c)
*
*游戏界面：
*
![屏幕截图 2024-12-21 192557](https://github.com/user-attachments/assets/692894c3-856f-4acf-a801-ab87c193dd59)
*


扩展功能：
*	魔法技能：
*	
1.	超级炸弹：消灭屏幕上的所有怪兽
2.	急救包：萝卜血量恢复加1
3.	急速攻击：塔的攻击速度加倍
4.	全屏冰冻：怪兽停止运动
5.	全屏减速：怪兽速度减半
*	魔法技能介绍日志
*	萝卜可以升级，不同血量有不同萝卜效果
*	萝卜满血抖动效果
*	支持4种塔（火瓶、太阳花、飞机、大便），攻击方式不同
*	支持10余种怪兽
*	怪兽、障碍受到攻击的效果（爆炸、灼伤、大便）
*	大便攻击怪兽时的单体减速效果
*	游戏支持暂停和开启二倍速，且不会有bug
*	各种点击、攻击、建造音效
*	界面精美，已基本还原游戏


## 项目开发文档
* 选用引擎： Cocos2d-x v4.0
* 辅助软件：Tiled-1.1.4、TexturePackerGUI
* 小组分工：
    - 官奕
        - 游戏关卡架构及类设计
        - 基本游戏逻辑实现
        - 地图制作，部分游戏关卡素材制作
        - 存档、联机观战等功能制作
    - 于广淳
        - 游戏UI界面制作
        - 游戏背景音乐与音效功能制作
        - 防御塔建造、升级与出售功能实现
        - 游戏暂停功能制作  
    - 周慧星
      - 游戏暂停功能制作 
 
炮塔设计：

怪兽设计：

场景设计：

UI设计：

地图管理：

音乐系统：

存档功能：

萝卜：

*  评分项完成度
    -    基础功能
           - [x] ⽀持⾄少3种防御塔和防御塔的删除
           - [x] 需要每种防御塔的攻击特效，⾄少要完成发射物弹道
           - [x] 怪物、萝⼘⽣命值显示
           - [x] ⽀持资源功能，资源可⽤于购买防御塔，资源可通过击杀怪物获得
           - [x] ⽀持每种防御塔的升级，⾄少可升级2次
           - [x] ⽀持萝⼘的升级，⾄少可升级2次
           - [x] ⽀持⾄少3种怪物
           - [x] ⽀持⾄少2张地图
           - [x] ⽀持背景⾳乐
           - [x] 需要关卡选择界⾯和保存通关进度记录功能（即已完成哪些关卡，可进⾏哪些关卡，哪些关卡还需解锁）

    - 可选功能
        - [x] ⽀持特殊技能，如AOE/单体伤害技能或增益技能
        - [x] 暂停游戏功能
        - [x] ⽀持中途退出时记录当前状态，下次进⼊同⼀关卡继续上⼀次游戏进程的功能
        - [x] ⽀持攻击、建造、击杀时的⾳效
        - [x] 支持联机观战模式 (测试版，最好在有关卡运行时使用)
  
     -   C++新特性
         - [x] 类型推断
         - [x] 基于范围的for循环
         - [x] 智能指针
         - [x] Lambda表达式
   
    - 其他亮点 
         - [x] 游戏还原度高
            - 除地图建造外，全部使用原版游戏素材
            - 还原原版界面设计
       
         - [x] 使用json数据文件
            - 记录关卡数据：怪物数据、炮塔数据、路径、可建造区域等，增强了数据的可读性和可修改性。
            - 记录存档，保存当前关卡数据
            - 进行联网通信，使用UDP协议传输数据
            - 
项目中使用的 C++11 及以上的新特性：

## 1. 智能指针 (Smart Pointers)

```cpp
std::unique_ptr<GameManager> instance;
std::shared_ptr<Monster> monster;
```

## 2. auto 类型推导
项目中大量使用了 auto 关键字进行类型推导：

```cpp:Classes/BaseLevelScene.cpp
auto visibleSize = Director::getInstance()->getVisibleSize();
auto backgroundImage = Sprite::create("see.png");
auto menu = Menu::create();
```

## 3. Lambda 表达式
在事件处理和回调函数中大量使用了 Lambda 表达式：

```cpp:Classes/themeScene.cpp
rightButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
    if (type == Widget::TouchEventType::ENDED) {
        a.button_music();
        int currentIndex = pageView->getCurrentPageIndex();
        if (currentIndex < mapImages.size() - 1) {
            pageView->scrollToPage(currentIndex + 1);
            clearRelatedButtons();
        }
    }
});
```

## 4. 基于范围的 for 循环
使用了更简洁的循环语法：

```cpp:Classes/GameManager.cpp
for (auto monster : monsters) {
    if (monster->health > 0) {
        // ... 处理逻辑
    }
}
```

## 5. 初始化列表
在类成员初始化时使用：

```cpp:Classes/Tower.h
struct WaveConfig {
    std::array<float, 2> spawnInterval={1,2};  // 使用初始化列表
};
```

## 6. nullptr
替代 NULL 使用更安全的 nullptr：

```cpp:Classes/Monster.cpp
if (sprite == nullptr) {
    return false;
}
```

## 7. std::array
使用标准数组替代 C 风格数组：

```cpp:Classes/GameManager.h
std::array<float, 2> spawnInterval;
```

## 8. 强类型枚举 (enum class)
可以使用强类型枚举提高类型安全性：

```cpp
enum class TowerType {
    Bottle,
    Sun,
    Plane,
    Shit
};
```

## 9. 右值引用和移动语义
在资源管理和性能优化中可以使用：

```cpp
std::vector<Monster*> monsters;
monsters = std::move(tempMonsters);
```

## 10. constexpr
用于编译期常量计算：

```cpp
constexpr float PI = 3.14159265359;
constexpr int TOWER_MAX_LEVEL = 3;
```

## 11. 统一初始化语法
使用花括号初始化：

```cpp
Vec2 position{100.0f, 200.0f};
std::vector<int> costs{100, 200, 300};
```

## 12. std::function 和 std::bind
在回调系统中使用：

```cpp:Classes/BaseLevelScene.h
void CountDown(std::function<void()> onComplete);
```

## 13. 委托构造函数
在构造函数中复用其他构造函数的代码：

```cpp
class Tower {
public:
    Tower(int index) : Tower(index, 0) {}
    Tower(int index, int grade) : index(index), grade(grade) {
        // 初始化代码
    }
};
```

## 14. override 关键字
明确标记覆盖虚函数：

```cpp:Classes/Tower.h
class Sun : public Tower {
public:
    void attack(BaseLevelScene*, std::vector<Monster*>&, 
                char isTarget, Monster* tar_m, 
                Obstacle* tar_o, int jiasu) override;
};
```
