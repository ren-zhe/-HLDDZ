#include "HelloWorldScene.h"
//#include "cocostudio/CocoStudio.h"
#include"initScene.h"
# include"net.h"
USING_NS_CC;
using namespace cocos2d::ui;

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
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto bg = Sprite::create("dengluBG.png");//登录背景
	bg->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(bg);

	//auto loadingBG = Sprite::create("loadingBG.png");//进度条背景
	//loadingBG->setTag(2);
	//loadingBG->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	//addChild(loadingBG);

	//auto loadingBar = Sprite::create();//进度条
	//loadingBar->setTag(1);
	//auto part1 = Sprite::create("loadingBar.png");
	//part1->setPosition(part1->getContentSize().width / 2, part1->getContentSize().height / 2);
	//loadingBar->addChild(part1);
	//auto part2 = Sprite::create("loadingBar.png");
	//part2->setFlippedX(true);
	//part2->setPosition(part2->getContentSize().width * 3 / 2, part2->getContentSize().height / 2);
	//loadingBar->addChild(part2);
	//loadingBar->setContentSize(Size(part1->getContentSize().width * 2, part1->getContentSize().height));
	//log("%f", visibleSize.width);
	////loadingBar->setPosition(Vec2(visibleSize.width / 4 + loadingBar->getContentSize().width / 3, visibleSize.height / 2 - loadingBar->getContentSize().height));
	//loadingBar->setPosition(Vec2(280 * visibleSize.width / 960, visibleSize.height / 2 - loadingBar->getContentSize().height));
	////loadingBar->setPosition(Vec2(565 * visibleSize.width / 960, visibleSize.height / 2 - loadingBar->getContentSize().height - ));
	//loadingBar->setAnchorPoint(Vec2(0, 0.5));
	//addChild(loadingBar);
	//schedule(schedule_selector(HelloWorld::updateBar), 0.4, 20, 0);
	Net::getInstance()->makeClient();
	auto layerDL = InitScene::create();
	layerDL->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	layerDL->ignoreAnchorPointForPosition(false);
	layerDL->setAnchorPoint(Vec2(0.5, 0.5));
	addChild(layerDL);
	
    return true;
}
cocos2d::Sprite * HelloWorld::makeButton(std::string name, std::string bgSprite)//合成按钮图案
{
	Sprite* button = Sprite::create();
	Sprite* leftPart = Sprite::create(bgSprite);
	button->setContentSize(Size(leftPart->getContentSize().width * 2, leftPart->getContentSize().height));

	leftPart->setPosition(Vec2(button->getContentSize().width / 2, button->getContentSize().height / 2));
	leftPart->setAnchorPoint(Vec2(1, 0.5));
	button->addChild(leftPart);

	Sprite* rightPart = Sprite::create(bgSprite);
	rightPart->setFlippedX(true);
	rightPart->setPosition(Vec2(button->getContentSize().width / 2, button->getContentSize().height / 2));
	rightPart->setAnchorPoint(Vec2(0, 0.5));
	button->addChild(rightPart);

	Sprite* word = Sprite::create(name);
	word->setPosition(Vec2(button->getContentSize().width / 2, button->getContentSize().height / 2));
	button->addChild(word);

	button->setScale(0.3);
	button->setContentSize(button->getContentSize() * 0.3);
	return button;
}

void HelloWorld::updateBar(float dt)//更新进度条
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 pos = Vec2(getChildByTag(1)->getPosition().x + getChildByTag(1)->getContentSize().width / 3, getChildByTag(1)->getPosition().y);
	if (pos.x > 565 * visibleSize.width / 960)//超出右边界
		pos.x = 280 * visibleSize.width / 960;//设置为左边界
	getChildByTag(1)->setPosition(pos);
	static int times = 0;
	if (times == 20)
	{
		auto layerDL = InitScene::create();
		layerDL->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
		layerDL->ignoreAnchorPointForPosition(false);
		layerDL->setAnchorPoint(Vec2(0.5, 0.5));
		addChild(layerDL);
		removeChildByTag(1);
		removeChildByTag(2);
	}
	times++;
}
