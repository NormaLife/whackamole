//
//  GameScene.cpp
//  WhackAMole
//
//  Created by mac on 16/6/21.
//
//

#include "GameScene.hpp"
#include "SimpleAudioEngine.h"
#include "HelloWorldScene.h"

USING_NS_CC;

static int _level = 1;

Scene* GameScene::createScene()
{
    auto scene = Scene::create();
    auto layer  = GameScene::create();
    scene->addChild(layer);
    return scene;
}

bool GameScene::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("whack.caf");
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    auto bg = Sprite::createWithSpriteFrameName("bg_dirt.png");
    bg->setScale(2);
    bg->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
    this->addChild(bg, -2);
    
    auto upperGrass = Sprite::createWithSpriteFrameName("grass_upper.png");
    upperGrass->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    upperGrass->setPosition(visibleSize.width/2, visibleSize.height/2);
    this->addChild(upperGrass, -1);
    
    auto lowerGrass = Sprite::createWithSpriteFrameName("grass_lower.png");
    lowerGrass->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    lowerGrass->setPosition(Vec2(0, visibleSize.height/2));
    this->addChild(lowerGrass, 2);
    
    auto mole1 = Sprite::createWithSpriteFrameName("mole_1.png");
    mole1->setPosition(Vec2(visibleSize.width/2-144, visibleSize.height/2-65));
    this->addChild(mole1, 1);
    _molesVector.pushBack(mole1);
    
    auto mole2 = Sprite::createWithSpriteFrameName("mole_1.png");
    mole2->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2-65));
    this->addChild(mole2, 1);
    _molesVector.pushBack(mole2);
    
    auto mole3 = Sprite::createWithSpriteFrameName("mole_1.png");
    mole3->setPosition(Vec2(visibleSize.width/2+144, visibleSize.height/2-65));
    this->addChild(mole3, 1);
    _molesVector.pushBack(mole3);
    
    _successScore = 15 + (_level*5 - 5);
    _totalMoles = 20 + (_level*5 - 5);
    _popDelay = 0.5 - (_level*0.05 - 0.05);
    
    std::string levelStr = StringUtils::format("关卡：%d", _level);
    _levelLabel = Label::createWithSystemFont(levelStr, "fonts/Marker Felt.ttf", 16);
    _levelLabel->setPosition(visibleSize.width*0.1, visibleSize.height-20);
    this->addChild(_levelLabel,10);
    
    std::string scoreStr = StringUtils::format("本关得分：%d", _score);
    _scoreLab = Label::createWithSystemFont(scoreStr, "fonts/Marker Felt.ttf", 16);
    _scoreLab->setPosition(visibleSize.width*0.3, visibleSize.height-20);
    this->addChild(_scoreLab,10);
    
    std::string moleStr = StringUtils::format("本关地鼠：%d", _totalMoles);
    _moleLabel = Label::createWithSystemFont(moleStr, "fonts/Marker Felt.ttf", 16);
    _moleLabel->setPosition(visibleSize.width*0.55, visibleSize.height-20);
    this->addChild(_moleLabel,10);
    
    std::string successStr = StringUtils::format("过关目标分：%d", _successScore);
    _passLabel = Label::createWithSystemFont(successStr, "fonts/Marker Felt.ttf", 16);
    _passLabel->setPosition(visibleSize.width*0.85, visibleSize.height-20);
    this->addChild(_passLabel,10);
    
    
    this->schedule(schedule_selector(GameScene::andomPopMoles), _popDelay);
    this->schedule(schedule_selector(GameScene::gameOver), _popDelay);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [=](Touch* touch, Event* event){
        Point p = this->convertTouchToNodeSpace(touch);
        for (auto mole : _molesVector) {
            if (mole->getTag() == 0) continue;
            
            if (mole->getBoundingBox().containsPoint(p))
            {
                _score += 1;
                
                auto mallter = Sprite::createWithSpriteFrameName("mallet1.png");
                mallter->setScale(0.8);
                mallter->setPosition(mole->getPosition().x+mole->getContentSize().width/2-10, mole->getPosition().y+80);
                this->addChild(mallter,3);
                
                auto animate = Animate::create(AnimationCache::getInstance()->getAnimation("mallet"));
                mallter->runAction(Sequence::create(animate, CallFunc::create([=]{
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("ow.caf");
                    //播放粒子效果
                    auto aswoon = ParticleSystemQuad::create("aswoon.plist");
                    aswoon->setPosition(mole->getPosition());
                    this->addChild(aswoon);
                    //移除木槌
                    this->removeChild(mallter);
                    
                }), NULL));
                
                 _scoreLab->setString(StringUtils::format("本关得分：%d", _score));
                //地鼠打中后修改tag为0
                mole->setTag(0);
                mole->stopAllActions();
                
                //播放地鼠被打中动画
                auto hitAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("thump"));
                auto moveDown = MoveBy::create(0.2f, Point(0, -mole->getContentSize().height));
                auto easeMoveDown = EaseInOut::create(moveDown, 3.0f);
                mole->runAction(Sequence::create(hitAnimate, easeMoveDown, NULL));
                
            }
            
        }
        
        return true;
    };
    
    //活动监听器
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void GameScene::andomPopMoles(float delta)
{
    for (auto mole : _molesVector) {
        int temp = CCRANDOM_0_1()*10000;
    
        if (temp % 3 == 0) {
            //getNumberOfRunningActions表示地鼠还没有执行动作
            if (mole->getNumberOfRunningActions() == 0) {
                 popMole(mole);
                
            }
           
        }
    }
}

void GameScene::popMole(cocos2d::Sprite* mole)
{
    if (_totalMoles <= 0) {
        return;
    }
    _totalMoles--;
    //如果上一次被打中，再钻出来还会显示被打中，所有在每次钻出的时候设置成初始图片
    mole->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("mole_1.png"));
    
    auto moveUp = MoveBy::create(0.2f, Point(0, mole->getContentSize().height));
    auto easeMoveUp = EaseInOut::create(moveUp, 3.0f);
    
    auto moveDown = easeMoveUp->reverse();
    
    auto laugtAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("laugh"));
    
   
    mole->runAction(Sequence::create(easeMoveUp,
                                     CallFuncN::create(CC_CALLBACK_1(GameScene::setHit, this)),
                                     laugtAnimate,
                                     moveDown,
                                     CallFuncN::create(CC_CALLBACK_1(GameScene::UnHit, this)),NULL));
}

void GameScene::gameOver(float delta)
{
    if (_totalMoles <=0 ) { //表示所有的地鼠都已钻出
        
        //所有地鼠都已缩回地洞，停止音效和背景音乐，显示菜单
        bool flag = true;
        for (auto mole : _molesVector) {
            if (mole->getNumberOfRunningActions() != 0) {
                flag = false;
                break;
            }
        }
        
        if (flag) {
            _gameOver = true;
            
            MenuItemImage *goItem = nullptr;
            if (_score < _successScore) { //游戏失败
                goItem = MenuItemImage::create("btn-continue.png", "btn-continue.png", [&](Ref *pSender){
                    Director::getInstance()->replaceScene(GameScene::createScene());
                });
            }
            else { //进入下一关
                goItem = MenuItemImage::create("btn-next.png", "btn-next.png", [&](Ref *pSender){
                    _level += 1;
                    Director::getInstance()->replaceScene(GameScene::createScene());
                });
            }
            
            CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
            CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
            
            Size visibleSize = Director::getInstance()->getVisibleSize();
            goItem->setPosition(visibleSize.width/2, visibleSize.height/2+60);
            
            auto againItem = MenuItemImage::create("btn-again.png", "btn-again.png", [&](Ref *pSender){
                Director::getInstance()->replaceScene(HelloWorld::createScene());
            });
            againItem->setPosition(visibleSize.width/2, visibleSize.height/2-60);
            
            auto menu = Menu::create(goItem, againItem, NULL);
            menu->setPosition(Vec2::ZERO);
            this->addChild(menu, 6);
            
            return;
        }
        
    }
}

void GameScene::setHit(Ref* pSender)
{
    Sprite* mole = (Sprite*)pSender;
    mole->setTag(1);
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("laugh.caf");
}

void GameScene::UnHit(Ref* pSender)
{
    Sprite* mole = (Sprite*)pSender;
    mole->setTag(0);
}
