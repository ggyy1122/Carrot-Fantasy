/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
USING_NS_CC;
using namespace ui;

static cocos2d::Size designResolutionSize = cocos2d::Size(960, 640);

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. 添加带有“X”图像的菜单项，单击该菜单项退出程序
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"  加标签
    // create and initialize a label

    auto label = Label::createWithTTF("11111", "fonts/Marker Felt.ttf", 24);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);
    }


    Size screenSize = Director::getInstance()->getWinSize();

    // 设置设计分辨率
    auto glview = Director::getInstance()->getOpenGLView();
    if (glview) {
        glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    }




    
    // 加背景
    auto sprite = Sprite::create("map/map1.png");
    float scaleX = screenSize.width / sprite->getContentSize().width;
    float scaleY = screenSize.height / sprite->getContentSize().height;
    if (sprite == nullptr)
    {
        problemLoading("'map/map1.png'");
    }
    else
    {

        

        sprite->setScaleX(scaleX);
        sprite->setScaleY(scaleY);

        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }

    //放女孩
    auto girl = Sprite::create("Carrot/Carrot_5.png");
    if (girl == nullptr)
    {
        problemLoading("'Carrot/Carrot_5.png'");
    }
    else
    {
        girl->setPosition(Vec2(1390 * scaleX, 758 * scaleY));
        girl->setScale(1.5);
        this->addChild(girl, 0);
    }


    //放大叔
    auto dashu = Sprite::create("Monsters/Monster_1.png");
    if (dashu == nullptr)
    {
        problemLoading("'Monsters/Monster_1.png'");
    }
    else
    {
        dashu->setPosition(Vec2(241 * scaleX, 758 * scaleY));
        dashu->setScale(0.3);
        this->addChild(dashu, 0);
    }


    Vector<FiniteTimeAction*> moveActions;
    auto a1 = MoveTo::create(1, Vec2(241 * scaleX, 344 * scaleY));
    auto a2 = MoveTo::create(1, Vec2(600 * scaleX, 344 * scaleY));
    auto a3 = MoveTo::create(1, Vec2(600 * scaleX, 479 * scaleY));
    auto a4 = MoveTo::create(1, Vec2(1000 * scaleX, 479 * scaleY));
    auto a5 = MoveTo::create(1, Vec2(1000 * scaleX, 344 * scaleY));
    auto a6 = MoveTo::create(1, Vec2(1380 * scaleX, 344 * scaleY));
    auto a7 = MoveTo::create(1, Vec2(1380 * scaleX, 758 * scaleY));
    auto seq = Sequence::create(a1,a2,a3,a4,a5,a6,a7,nullptr);
    dashu->runAction(seq);















    return true;
}



void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
