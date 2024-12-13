#pragma once
#ifndef __BASE_H__
#define __BASE_H__
#include "cocos2d.h"
#include<string>
#include<map>
#include<vector>
#include<Monster.h>
#include"Tower.h"

USING_NS_CC;

#define Y 9//y
#define X 15//x

struct Cell {//用来表示地图格子信息的结构体
    char flag;//
    int key;
};


class BaseLevelScene : public cocos2d::Scene {

private:
    cocos2d::Node* plantsLayer;                              //植物图层
    TMXLayer* plantableLayer = nullptr;                     // plantable 层
    static const std::vector<std::string> mapFiles;         // 存储地图文件的路径
    int levelId;                                            //关卡编号
    
    Label* money_lable;            //显示当前金钱
    
    bool isDoubleSpeed = false; // 初始化为false，表示游戏初始不是二倍速状态
    bool isPaused = false; // 初始化为false，表示游戏初始不是暂停状态
    Scheduler* scheduler = Director::getInstance()->getScheduler();
    std::vector<Sprite*>remove_table;//remove时借用
    Sprite* curr_cell;//
    std::vector<std::string>buy_tower[2];//存放炮塔的购买图
    std::vector<int>index_table;//记录本关用到的炮塔的序号
    int cell_flag;//
    Vec2 last_position;//记录上一次的位置

    std::map<int, Tower*> towers;//记录场上的塔的信息
    Cell map_data[X][Y];//地图管理

public:
    int money = 1000;

    virtual void update(float deltaTime) override;
    cocos2d::TMXTiledMap* tileMap = nullptr;                // 地图对象
    cocos2d::Size tileSize;                                 //每个瓦片的大小
    static cocos2d::Scene* createScene(int level);          // 创建场景时传入关卡编号
    virtual bool init();                                    // 默认的 init() 方法
    void initUI();                                          //初始化ui
    bool initWithLevel(int level);                          //重写的init函数
    CREATE_FUNC(BaseLevelScene);
    void loadMap();                                         // 加载地图的函数
    void addMouseListener();                                // 添加鼠标监听器
    void handleMouseDown(EventMouse* event);                //鼠标点击时间判断是否种植
    void handlePlant(const Vec2& position);                // 种植判断
    Vec2 convertToTileCoord(const Vec2& screenPosition);    //将鼠标坐标转换瓦片坐标
    void drawGrid();                                        //画网格线
    void plantAt(const cocos2d::Vec2& tileCoord);           //种植
    bool loadPathForLevel(int levelId, const std::string& filePath); // 从 JSON 文件按需加载某个关卡的路径
    void loadMonsterConfig();                                //加载怪物配置
    void moveMonsterAlongPath(Monster* monster, const std::vector<cocos2d::Vec2>& path);   //怪物路径移动
    Vec2 gridToScreenCenter(const Vec2& gridPoint);         //将网格坐标转成屏幕坐标的工具函数
    void  loadMonsters();                                    //加载怪物精灵帧
    //void placeMonsters();                                    //根据关卡防止怪兽
    void updatemoney(int add);           //更新money，add为变化量
    void doublespeed(Ref* pSender);
    void pause_all(Ref* pSender);
    void menu_all(Ref* pSender);


    void CountDown(std::function<void()> onComplete);             //倒计时
    void gameover(bool is_win);   //结束游戏
    void Jineng1(Ref* pSender);   //萝卜血量加1
    void Jineng2(Ref* pSender);   //冰冻，怪兽减速，待实现

    void PlantMenuAppear(Vec2 mapPos);//出现种植菜单
    void PlantMenuGone(Vec2 position);//种植菜单消失
    void InitMapData();//初始化地图信息

    void UpMenuAppear(Vec2& position);//升级菜单出现
    void UpMenuGone(Vec2& position);//升级菜单消失
};
#endif 