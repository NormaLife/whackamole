//
//  GameScene.hpp
//  WhackAMole
//
//  Created by mac on 16/6/21.
//
//

#ifndef GameScene_hpp
#define GameScene_hpp

#include "cocos2d.h"

class GameScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    CREATE_FUNC(GameScene);
private:
    cocos2d::Vector<cocos2d::Sprite*> _molesVector;
    
    //地鼠随机钻出
    void andomPopMoles(float delta);
    
    //钻出地鼠
    void popMole(cocos2d::Sprite* mole);
    
    void setHit(Ref* pSender);
    void UnHit(Ref* pSender);
    
    void gameOver(float delta);
    
    cocos2d::Label* _levelLabel; //关卡
    cocos2d::Label* _passLabel;  //过关的分数
    cocos2d::Label* _scoreLab; //当前分数
    cocos2d::Label* _moleLabel;  //地鼠总数
    
   
    int _score;
    int _successScore; //过关目标分数
    int _totalMoles;
    float _popDelay;
    
    bool _gameOver;

};

#endif /* GameScene_hpp */
