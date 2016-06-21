#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "GameScene.hpp"

USING_NS_CC;

#define BACKGROUND_PLIST "background.plist"
#define RESOURCES_PLIST "resources.plist"
#define LAUGH_EFFECT "laugh.caf"
#define OW_EFFECT "ow.caf"
#define WHACK_EFFECT "whack.caf"
#define SECOND_EFFECT "second.mp3"
#define BACKGROUNDMUSIC "whack.caf"

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto bg = Sprite::create("start.png");
//    bg->setScale(1.1);
//    bg->setAnchorPoint(Vec2::ZERO);
    bg->setPosition(visibleSize.width/2, visibleSize.height/2);
    this->addChild(bg);
    
    auto playItem = MenuItemImage::create(
                                          "btn-start.png",
                                          "btn-start.png",
                                          CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    playItem->setPosition(Vec2(origin.x + visibleSize.width/2,
                               origin.y + visibleSize.height*0.3));
    playItem->setTag(100);
    
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));
    closeItem->setTag(101);
    
    auto menu = Menu::create(closeItem,playItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    
    return true;
}

Animation* HelloWorld::createAnimationWithName(std::string name, float delay, int num)
{
    auto animation = Animation::create();
    for (unsigned int i=1; i<=num; i++) {
        std::string frameName = name;
        frameName.append(StringUtils::format("%d",i)).append(".png");
        animation->addSpriteFrameWithFile(frameName.c_str());
    }
    animation->setDelayPerUnit(delay);
    animation->setRestoreOriginalFrame(true);
    return animation;
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    if (((MenuItemImage*)pSender)->getTag() == 100) {
        
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile(BACKGROUND_PLIST);
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile(RESOURCES_PLIST);
        
        CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(BACKGROUNDMUSIC);
        CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(LAUGH_EFFECT);
        CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(OW_EFFECT);
        CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(SECOND_EFFECT);
        
        AnimationCache::getInstance()->addAnimation(createAnimationWithName("mallet", 0.15f, 3), "mallet");
        AnimationCache::getInstance()->addAnimation(createAnimationWithName("mole_laugh", 0.5f, 3), "laugh");
        AnimationCache::getInstance()->addAnimation(createAnimationWithName("mole_thump", 0.3f, 4), "thump");
        
        CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
        Director::getInstance()->replaceScene(GameScene::createScene());
    }
    else
    {
        Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        exit(0);
#endif
    }
}
