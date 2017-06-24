#include "initScene.h"
//#include "cocostudio/CocoStudio.h"
#include"DLScene.h"
#include"ZCScene.h"
USING_NS_CC;
using namespace cocos2d::ui;

Scene* InitScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = InitScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool InitScene::init()
{
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg = Sprite::create("dengluBG1.png");//界面背景
	bg->setScaleX(4);
	bg->setScaleY(3.5);
	//bg->setContentSize(bg->getContentSize() * 3);
	bg->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(bg, 1);

	auto titleSprite = Sprite::create("biaoti.png");//创建标题图标
	titleSprite->setPosition(Vec2(visibleSize.width / 2, visibleSize.height ));
	titleSprite->setAnchorPoint(Vec2(0.5, 1));
	addChild(titleSprite, 2);

	auto ndSprite = makeButton("denglu.png", "anniuDL1.png");//登录按钮
	auto sdSprite = makeButton("denglu.png", "anniuDL2.png");
	MenuItemSprite* buttonDL = MenuItemSprite::create(ndSprite, sdSprite, [&](Ref* sender) {
		log("dl");
		Size visibleSize = Director::getInstance()->getVisibleSize();
		auto layerDL = DL::create();
		layerDL->setTag(10);
		layerDL->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
		layerDL->ignoreAnchorPointForPosition(false);
		layerDL->setAnchorPoint(Vec2(0.5, 0.5));
		addChild(layerDL, 3);
		((MenuItemSprite*)getChildByTag(2))->setEnabled(false);//禁用登录按钮
		((MenuItemSprite*)getChildByTag(3))->setEnabled(false);//禁用注册按钮
	});
	buttonDL->setTag(2);
	addChild(buttonDL);
	auto nzSprite = makeButton("zhuce.png", "anniuDL1.png");//注册按钮
	auto szSprite = makeButton("zhuce.png", "anniuDL2.png");
	MenuItemSprite* buttonZC = MenuItemSprite::create(nzSprite, szSprite, [&](Ref* sender) {
		log("zc");
		Size visibleSize = Director::getInstance()->getVisibleSize();
		auto layerDL = ZC::create();
		layerDL->setTag(11);
		layerDL->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
		layerDL->ignoreAnchorPointForPosition(false);
		layerDL->setAnchorPoint(Vec2(0.5, 0.5));
		addChild(layerDL, 3);
		((MenuItemSprite*)getChildByTag(2))->setEnabled(false);
		((MenuItemSprite*)getChildByTag(3))->setEnabled(false);
	});
	buttonZC->setTag(3);
	addChild(buttonZC);
	auto menu = Menu::create(buttonDL, buttonZC, NULL);
	menu->alignItemsHorizontallyWithPadding(visibleSize.width / 11);
	menu->setAnchorPoint(Vec2(0.5, 0.5));
	menu->setPosition(visibleSize.width / 2.0f, visibleSize.height / 5.0f);
	addChild(menu, 2);
	return true;
}

cocos2d::Sprite * InitScene::makeButton(std::string name, std::string bgSprite)
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
	button->setScale(0.6);
	button->setContentSize(button->getContentSize() * 0.6);
	return button;
}
