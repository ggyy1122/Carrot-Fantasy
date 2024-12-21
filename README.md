# carrotfantasy
### 同济大学 2024程序设计范式大作业
## 项目信息
* 项目选题 ： 保卫萝⼘
* 项目成员 ：官奕、于广淳、周慧星
* 项目Github地址：
[https://github.com/xing05188/CattotFantasy.git]()
* 项目进度时间线：
* 场景的逻辑关系：
 
主场景：
 ![屏幕截图 2024-12-21 192116](https://github.com/user-attachments/assets/6e6dddc9-f078-491c-815c-e4457068534b)

设置界面：
 ![Uploading 屏幕截图 2024-12-21 192410.png…]()

主题界面：
 
关卡选择界面：
 
游戏界面：
 

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
*C++新特征
1.	自动类型推导（auto）
2.	智能指针unique_ptr
std::unique_ptr<Carrot> ret = std::make_unique<Carrot>();
3.	Nullptr
4.	基于范围的for循环




* <img src="MDres\7.png" width = "300" height = "200" alt="" align=center />
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

* 类的设计及继承关系：

* <img src="MDres\8.png" width = "390" height = "200" alt="" align=center />
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
* 代码亮点
    - 多线程使用
        ```cpp
        // 联机服务端
        UDPServer udpserver;
        std::mutex serverMutex; // 互斥锁
        // 开启服务端
        if (IS_BEGAN_SERVER) {
            std::thread serverThread(&GameScene::startServer, this);
            serverThread.detach();
        }
        ```       
    - 异常捕获
        ```cpp
        try {
            _currNum = document["currNum"].GetInt();
            _goldValue = document["goldValue"].GetInt();
            carrotHealth = document["carrotHealth"].GetInt();
            _monsterDeath = document["monsterDeath"].GetInt();
            _monsterNum = document["monsterNum"].GetInt();
            // 获得关卡存档的怪物
            ...
        }
        catch (const std::exception& e) {
            CCLOG("Exception while processing monster data: %s", e.what());
        }
        ``` 
    - json文件读写
        ```cpp
        // 读出
        // rapidjson 对象
        rapidjson::Document document;
        // 根据传递的关卡值selectLevel获得对应的关卡数据文件
        std::string filePath = FileUtils::getInstance()->fullPathForFilename(StringUtils::format("CarrotGuardRes/LeveL_%d.data", currentLevel));
        // 读取文件内容
        std::string contentStr = FileUtils::getInstance()->getStringFromFile(filePath);
        // 解析contentStr中json数据，并存到document中
        document.Parse<0>(contentStr.c_str());   
        ```
        ```cpp
        // 写入
        // 创建一个 rapidjson::StringBuffer 对象，用于存储 JSON 字符串
        rapidjson::StringBuffer buffer;// StringBuffer 是一个可变的字符序列，可以像 std::string 一样使用
        // 创建一个 rapidjson::Writer 对象，用于将 JSON 文档写入到 StringBuffer 中
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        // 将 JSON 文档写入 StringBuffer 中
        document.Accept(writer);// Accept() 接受一个 Document 对象，将其以 JSON 格式写入到 StringBuffer 中
        gameMassageBuffer = buffer.GetString(); // 将 StringBuffer 中的字符输出到 std::string 中
        cocos2d::FileUtils* fileUtils = cocos2d::FileUtils::getInstance();
        std::string path = "Level_" + std::to_string(currentLevel) + "_save.json";
        fileUtils->writeStringToFile(buffer.GetString(), fileUtils->getWritablePath() + path);
        ``` 
    - 多次使用宏调试
        ```cpp
        #define DEBUG
        #ifdef DEBUG
        CCLOG("screenWidth:  %lf, screenHeight:  %lf", _screenWidth, _screenHeight);
        #endif // DEBUG
        ``` 
* 踩坑记录
    - Tiled版本过高导致Tiled地图无法加载到cocos2dx-v4，需要降低Tiled版本；cocos2dx-v4中在Windows环境下加载csv编码的Tiled地图由BUG，通过修改cocos2dx引擎源码解决， 后来发现可以修改Tiled地图的编码格式。
        ```cpp
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
			// Fix bug when tilemap data is in csv format.
			// We have to remove all '\r' from the string
			currentString.erase(std::remove(currentString.begin(), currentString.end(), '\r'), currentString.end());
        #endif
        ``` 
    - 使用多线程开启服务器时，在退出场景时需要关闭该线程。
        ```cpp
        void onExit() {
            udpserver.Stop();
            Layer::onExit();
        }
        ``` 
    - Tiled地图坐标，cocos屏幕坐标，OpneGL坐标互相转换，及各个Node的位置大小调整  
        ```cpp
        // TMX point ->Screen
        // 地图格子坐标转化成屏幕坐标
        Vec2 TMXPosToLocation(Vec2 pos);
        // Screen ->TMX point
        // 屏幕坐标转化成地图格子坐标	
        Vec2 LocationToTMXPos(Vec2 pos);
        ```    
    - 监听器管理十分重要，多个监听器共存会造成游戏执行逻辑混乱
        ```cpp
        //获取当前建造界面是否激活
        int hasBuild = getHasBuild();
        //如果没有激活建造界面
        if (!hasBuild) {
	        //创建新的触摸层和监听器
	        createTouchLayer();
	        createTouchListener();
        }
        ``` 
* 加分项 
    1. 在Github上部署完整项目，并且有详细的使用文档
    2. 团队分工合理，项目开源至GitHub
    3. 界面精美，接近还原游戏原作
    4. 添加了音效和背景音乐，且均支持手动打开关闭
    5. 项目目录结构清晰
    6. 运行时无崩溃情况发生
    7. 没有内存泄露问题
    8. 实现暂停游戏功能
    9. 实现游戏存档功能
    10. 实现观战游戏功能
    11. 使用problemloading和try catch来处理异常



## 游戏文档

### 游戏简介
同保卫萝卜
### 游戏截图
#### 单机模式
* 主界面
* <img src="MDres\1.png" width = "300" height = "200" alt="" align=center />
* 选择关卡
* <img src="MDres\2.png" width = "300" height = "200" alt="" align=center />
* 游戏关卡界面
* <img src="MDres\3.png" width = "300" height = "200" alt="" align=center />
* 游戏结束界面
* <img src="MDres\4.png" width = "300" height = "200" alt="" align=center />
* <img src="MDres\6.png" width = "300" height = "200" alt="" align=center />
#### 联机观战
* 观战
* <img src="MDres\5.png" width = "300" height = "400" alt="" align=center />


### 再一点说明
1. CarrotGuard 萝卜升级表现为点击时，会使用金币回血
2. CarrotGuard_network 实现了联网的观战模式，通过UDP传送可以实时观看正在运行的关卡，但是不能进行操作，有轻微bug，建议在有程序进行游戏时关卡使用
3. 使用Enigma Virtual Box 打包成单个exe时，声音有问题，只能使用这种动态库方法
4. 改了点cocos2dx源码内容，具体见GitHub的commit

## 食用方法
#### 如果想运行我们的代码
1. 请确保安装cocos2dx-v4.0和VisualStudio2022
2. 运行 `git@github.com:MxLearner/TowerDefense.git `
3. 在文件目录下依次运行 `cd proj.win32`，`cmake .. -G "Visual Studio 17 2022" -A win32`，` ./CarrotGuard.sln`
#### 如果想直接体验我们的游戏
请直接下载最新的release版



## 致谢
感谢不厌其烦读完文档的您。
> 联系我们：  
> 王麒懿 2252090@tongji.edu.cn  
> 代仲杰 2253092@tongji.edu.cn
