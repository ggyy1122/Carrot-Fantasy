#pragma once
#include "cocos2d.h"
#include<string>
#include<map>
#include<vector>
#include<Monster.h>
USING_NS_CC;

class BaseLevelScene : public cocos2d::Scene {

private:
    cocos2d::TMXTiledMap* tileMap = nullptr;                // 地图对象
    cocos2d::Node*plantsLayer;                              //植物图层
    TMXLayer* plantableLayer = nullptr;                     // plantable 层
    static const std::vector<std::string> mapFiles;         // 存储地图文件的路径
    cocos2d::Size tileSize;                                 //每个瓦片的大小
    std::map<int,std::vector<cocos2d::Vec2>>pathsCache;     //存储已经加载过的关卡的网格路径
    std::map<int, std::vector<cocos2d::Vec2>>ScreenPaths;   //存储已经加载过的关卡的屏幕路径
    int levelId;                                            //关卡编号

    std::vector<Sprite*>remove_table;//remove时借用
    Sprite* curr_cell;//
    std::vector<std::string>buy_tower;//存放炮塔的购买图
    std::vector<int>index_table;//记录本关用到的炮塔的序号
    int cell_flag;//
    int money;//钱
    Vec2 last_position;//记录上一次的位置
public:
    static cocos2d::Scene* createScene(int level);          // 创建场景时传入关卡编号
    virtual bool init();                                    // 默认的 init() 方法
    bool initWithLevel(int level);                          //重写的init函数
    CREATE_FUNC(BaseLevelScene);
    void loadMap();                                         // 加载地图的函数
    void addMouseListener();                                // 添加鼠标监听器
    void handleMouseDown(EventMouse* event);                //鼠标点击时间判断是否种植
    void handlePlant(const Vec2& position) ;                // 种植判断
    Vec2 convertToTileCoord(const Vec2& screenPosition);    //将鼠标坐标转换瓦片坐标
    void drawGrid();                                        //画网格线
    void plantAt(const cocos2d::Vec2& tileCoord);           //种植
    bool loadPathForLevel(int levelId, const std::string& filePath); // 从 JSON 文件按需加载某个关卡的路径
    void loadMonsterConfig();                                //加载怪物配置
    void moveMonsterAlongPath(Monster* monster, const std::vector<cocos2d::Vec2>& path);   //怪物路径移动
    Vec2 gridToScreenCenter(const Vec2& gridPoint);         //将网格坐标转成屏幕坐标的工具函数
    void  loadMonsters();                                    //加载怪物精灵帧
    void placeMonsters();                                    //根据关卡防止怪兽

};