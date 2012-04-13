#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

CCScene* HelloWorld::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::node();
	
	// 'layer' is an autorelease object
	HelloWorld *layer = HelloWorld::node();

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
	if ( !CCLayerColor::initWithColor(ccc4(255,255,255,255)))
	{
		return false;
	}

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	CCMenuItemImage *pCloseItem = CCMenuItemImage::itemFromNormalImage(
										"CloseNormal.png",
										"CloseSelected.png",
										this,
										menu_selector(HelloWorld::menuCloseCallback) );
	pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );

	// create menu, it's an autorelease object
	CCMenu* pMenu = CCMenu::menuWithItems(pCloseItem, NULL);
	pMenu->setPosition( CCPointZero );
	this->addChild(pMenu, 1);

	/////////////////////////////
	// 3. add your codes below...

	// add a label shows "Hello World"
	// create and initialize a label
	CCLabelTTF* pLabel = CCLabelTTF::labelWithString("Hello World", "Thonburi", 34);

	// ask director the window size
	CCSize size = CCDirector::sharedDirector()->getWinSize();

	// position the label on the center of the screen
	pLabel->setPosition( ccp(size.width / 2, size.height - 20) );

	// add the label as a child to this layer
	this->addChild(pLabel, 1);

	// add "HelloWorld" splash screen"
	//CCSprite* pSprite = CCSprite::spriteWithFile("HelloWorld.png");
    CCSprite* pSprite = CCSprite::spriteWithFile("Player.png", CCRectMake(0, 0, 27, 40));

	// position the sprite on the center of the screen
	//pSprite->setPosition( ccp(size.width/2, size.height/2) );
    pSprite->setPosition(ccp(pSprite->getContentSize().width/2, size.height/2));
	// add the sprite as a child to this layer
	this->addChild(pSprite, 0);
    
    this->schedule(schedule_selector(HelloWorld::gameLogic), 1.0);
    
    this->setIsTouchEnabled(true);
    
    _targets = new CCMutableArray<CCSprite*>;
    _projectiles = new CCMutableArray<CCSprite*>;
    
    this->schedule(schedule_selector(HelloWorld::update));
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("background-music-aac.wav", true);
	return true;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
	CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void HelloWorld::addTarget()
{
    CCSprite* target = CCSprite::spriteWithFile("Target.png", CCRectMake(0, 0, 27, 40));
    CCSize  winSize = CCDirector::sharedDirector()->getWinSize();
    int     minY = target->getContentSize().height/2;
    int     maxY = winSize.height - minY;
    int     rangeY = maxY - minY;
    int     actualY = (rand() % rangeY) + minY;
    target->setPosition(ccp(winSize.width + target->getContentSize().width/2, actualY));
    this->addChild(target);
    
    int minDuration = 2;
    int maxDuration = 4;
    int rangeDuration = maxDuration - minDuration;
    int actualDuraiton = (rand() % rangeDuration) + minDuration;
    CCFiniteTimeAction* actionMove = 
        CCMoveTo::actionWithDuration((ccTime)actualDuraiton, ccp(0 - target->getContentSize().width/2, actualY));
    CCFiniteTimeAction* actionMoveDone = 
    CCCallFuncN::actionWithTarget(this, callfuncN_selector(HelloWorld::spriteMoveFinished));
    target->runAction(CCSequence::actions(actionMove, actionMoveDone, NULL));   
    target->setTag(1);
    _targets->addObject(target);
}

void HelloWorld::spriteMoveFinished(cocos2d::CCNode *sender)
{
    CCSprite* sprite = (CCSprite*) sender;
    this->removeChild(sprite, true);
    if (sprite->getTag() == 1) {
        _targets->removeObject(sprite);
    }
    else {
        assert (sprite->getTag() == 2);
        _projectiles->removeObject(sprite);
    }
}

void HelloWorld::gameLogic(ccTime dt)
{
    this->addTarget();
}

void HelloWorld::ccTouchesEnded(CCSet *touches, CCEvent *event)
{
    CCTouch * touch = (CCTouch*)touches->anyObject();
    CCPoint location = touch->locationInView(touch->view());
    location = CCDirector::sharedDirector()->convertToGL(location);
    
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    CCSprite* projectile = CCSprite::spriteWithFile("Projectile.png", CCRectMake(0, 0, 20, 20));
    projectile->setPosition(ccp(20, size.height/2));
    
    int offX = location.x - projectile->getPosition().x;
    int offY = location.y - projectile->getPosition().y;
    
    if (offX <= 0) return;
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("pew-pew-lei.wav");
    
    this->addChild(projectile);
    projectile->setTag(2);
    _projectiles->addObject(projectile);
    
    
    // Determine where we wish to shoot the projectile to
    int realX = size.width + (projectile->getContentSize().width/2);
    float ratio = (float)offY / (float)offX;
    int realY = (realX * ratio) + projectile->getPosition().y;
    CCPoint realDest = ccp(realX, realY);
    // Determine the length of how far we're shooting
    int offRealX = realX - projectile->getPosition().x;
    int offRealY = realY - projectile->getPosition().y;
    float length = sqrtf((offRealX * offRealX) 
     + (offRealY*offRealY));
    float velocity = 240/1; // 480pixels/1sec
    float realMoveDuration = length/velocity;
    // Move projectile to actual endpoint
    projectile->runAction( CCSequence::actions(
        CCMoveTo::actionWithDuration(realMoveDuration, realDest),
        CCCallFuncN::actionWithTarget(this, 
        callfuncN_selector(HelloWorld::spriteMoveFinished)),  
        NULL) );
}

void HelloWorld::update(ccTime dt)
{
    CCMutableArray<CCSprite*> *projectilesToDelete =
    new CCMutableArray<CCSprite*>;
    CCMutableArray<CCSprite*>::CCMutableArrayIterator it, jt;
    
    for (it = _projectiles->begin(); it != _projectiles->end(); it++)
    {
        CCSprite *projectile =*it;
        CCRect projectileRect = CCRectMake(
                                           projectile->getPosition().x
                                           - (projectile->getContentSize().width/2),
                                           projectile->getPosition().y
                                           - (projectile->getContentSize().height/2),
                                           projectile->getContentSize().width,
                                           projectile->getContentSize().height);
        
        CCMutableArray<CCSprite*>*targetsToDelete
        = new CCMutableArray<CCSprite*>;
        
        for (jt = _targets->begin(); jt != _targets->end(); jt++)
        {
            CCSprite *target =*jt;
            CCRect targetRect = CCRectMake(
                                           target->getPosition().x - (target->getContentSize().width/2),
                                           target->getPosition().y - (target->getContentSize().height/2),
                                           target->getContentSize().width,
                                           target->getContentSize().height);
            
            if (CCRect::CCRectIntersectsRect(projectileRect, targetRect))
            {
                targetsToDelete->addObject(target);
            }
        }
        
        for (jt = targetsToDelete->begin();
             jt != targetsToDelete->end();
             jt++)
        {
            CCSprite *target =*jt;
            _targets->removeObject(target);
            this->removeChild(target, true);
        }
        
        if (targetsToDelete->count() >0)
        {
            projectilesToDelete->addObject(projectile);
        }
        targetsToDelete->release();
    }
    
    for (it = projectilesToDelete->begin();
         it != projectilesToDelete->end();
         it++)
    {
        CCSprite* projectile =*it;
        _projectiles->removeObject(projectile);
        this->removeChild(projectile, true);
    }
    projectilesToDelete->release();
}


HelloWorld::~HelloWorld()
{
    if (_targets) {
        _targets->release();
        _targets = NULL;
    }
    
    if (_projectiles) {
        _projectiles->release();
        _projectiles = NULL;
    }
    
}