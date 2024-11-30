#pragma once
#include "cocos2d.h"
USING_NS_CC;

class BaseLevelScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene(int level);  // 创建场景时传入关卡编号
    virtual bool init();                            // 默认的 init() 方法

    CREATE_FUNC(BaseLevelScene);

protected:
    void loadMap(int level);                                // 加载地图的函数
    void addMouseListener();                                // 添加鼠标监听器
    void handleMouseDown(EventMouse* event);                //鼠标点击时间判断是否种植
    void handlePlant(const Vec2& position) ;               // 种植处理
    TMXTiledMap* tileMap = nullptr;                         // 地图对象
    TMXLayer* plantableLayer = nullptr;                     // plantable 层
    Vec2 convertToTileCoord(const Vec2& screenPosition);    //将鼠标坐标转换瓦片坐标
    void drawGrid();                                        //画网格线
    cocos2d::Size tileSize;                                 //每个瓦片的大小

private:
    static const std::vector<std::string> mapFiles;  // 存储地图文件的路径
};