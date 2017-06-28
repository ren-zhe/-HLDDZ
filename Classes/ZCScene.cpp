#include "ZCScene.h"
//#include "cocostudio/CocoStudio.h"


USING_NS_CC;
using namespace cocos2d::ui;

Scene* ZC::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = ZC::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool ZC::init()
{
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg = Sprite::create("dengluBG1.png");//´´½¨µÇÂ¼½çÃæ±³¾°
	bg->setScale(2);
	bg->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(bg, 1);


	auto bgDL = LayerColor::create(Color4B(255, 255, 255, 255), 200 * visibleSize.width / 960, 30 * visibleSize.width / 960);//´´½¨ÕËºÅÊäÈë¿ò±³¾°
	bgDL->ignoreAnchorPointForPosition(false);
	bgDL->setAnchorPoint(Vec2(0.5, 0.5));
	bgDL->setPosition(Vec2(visibleSize.width / 2.0f + 20 * visibleSize.width / 960, visibleSize.height / 2.0f + 40 * visibleSize.width / 960));
	addChild(bgDL, 2);
	auto spriteDL = Sprite::create("zhanghao.png");
	spriteDL->setScale(0.7);
	spriteDL->setAnchorPoint(Vec2(1, 0));
	//spriteDL->setPosition(Vec2(visibleSize.width / 2.0f - 60, visibleSize.height / 2.0f + 40));
	bgDL->addChild(spriteDL);

	auto bgMM = LayerColor::create(Color4B(255, 255, 255, 255), 200 * visibleSize.width / 960, 30 * visibleSize.width / 960);//´´½¨ÃÜÂëÊäÈë¿ò±³¾°
	bgMM->ignoreAnchorPointForPosition(false);
	bgMM->setAnchorPoint(Vec2(0.5, 0.5));
	bgMM->setPosition(Vec2(visibleSize.width / 2.0f + 20, visibleSize.height / 2.0f - 40));
	addChild(bgMM, 2);
	auto spriteMM = Sprite::create("mima.png");
	spriteMM->setScale(0.7);
	spriteMM->setAnchorPoint(Vec2(1, 0));
	//spriteDL->setPosition(Vec2(visibleSize.width / 2.0f - 60, visibleSize.height / 2.0f + 40));
	bgMM->addChild(spriteMM);

	TextField* textFieldAccount = TextField::create("input account", "Arial", 30 * visibleSize.width / 960);//ÉèÖÃÕËºÅÊäÈë¿ò
	textFieldAccount->setTouchSize(Size(200, 50));
	textFieldAccount->setTextColor(Color4B(255, 255, 255, 255));
	textFieldAccount->setMaxLengthEnabled(true);
	textFieldAccount->setMaxLength(10);
	textFieldAccount->setTextColor(Color4B(255, 0, 0, 255));
	textFieldAccount->setPlaceHolderColor(Color3B(255, 0, 0));
	textFieldAccount->setContentSize(Size(bg->getContentSize().width, 50));
	textFieldAccount->setPosition(Vec2(visibleSize.width / 2.0f + 20, visibleSize.height / 2.0f + 40));
	textFieldAccount->addEventListener(CC_CALLBACK_2(ZC::textFieldEvent, this));
	addChild(textFieldAccount, 3);

	TextField* textFieldPassword = TextField::create("input password", "Arial", 30 * visibleSize.width / 960);//ÉèÖÃÃÜÂëÊäÈë¿ò
	textFieldPassword->setPasswordEnabled(true);
	textFieldPassword->setTouchSize(Size(200, 50));
	textFieldPassword->setTextColor(Color4B(255, 255, 255, 255));
	textFieldPassword->setMaxLengthEnabled(true);
	textFieldPassword->setMaxLength(10);
	textFieldPassword->setTextColor(Color4B(255, 0, 0, 255));
	textFieldPassword->setPlaceHolderColor(Color3B(255, 0, 0));
	textFieldPassword->setContentSize(Size(bg->getContentSize().width, 50));
	textFieldPassword->setPosition(Vec2(visibleSize.width / 2.0f + 20 * visibleSize.width / 960, visibleSize.height / 2.0f - 40 * visibleSize.width / 960));
	textFieldPassword->addEventListener(CC_CALLBACK_2(ZC::textFieldEvent1, this));
	addChild(textFieldPassword, 3);
	/*auto normalSprite = makeButton("denglu.png", "anniuDL1.png");
	auto selectedSprite = makeButton("denglu.png", "anniuDL2.png");*/
	auto normalSprite = Sprite::create("denglu.png");
	normalSprite->setScale(0.6);
	normalSprite->setContentSize(normalSprite->getContentSize() * 0.6);
	auto selectedSprite = Sprite::create("denglu.png");
	selectedSprite->setScale(0.63);
	selectedSprite->setContentSize(selectedSprite->getContentSize() * 0.63);
	MenuItemSprite* buttonDL = MenuItemSprite::create(normalSprite, selectedSprite, [](Ref* sender) {
		log("ok");
		Net::getInstance()->sendStringC("wo zhu che le !");
	});

	auto nqSprite = Sprite::create("quxiao.png");
	nqSprite->setScale(0.6);
	nqSprite->setContentSize(nqSprite->getContentSize() * 0.6);
	auto sqSprite = Sprite::create("quxiao.png");
	sqSprite->setScale(0.63);
	sqSprite->setContentSize(sqSprite->getContentSize() * 0.63);
	MenuItemSprite* buttonQX = MenuItemSprite::create(nqSprite, sqSprite, [&](Ref* sender) {
		((MenuItemSprite*)(getParent()->getChildByTag(2)))->setEnabled(true);//»Ö¸´µÇÂ¼°´Å¥
		((MenuItemSprite*)(getParent()->getChildByTag(3)))->setEnabled(true);//»Ö¸´×¢²á°´Å¥
		/*auto temp = getParent()->getChildByTag(2);*/getParent()->removeChildByTag(11);

	});
	auto menu = Menu::create(buttonDL, buttonQX, NULL);
	menu->alignItemsHorizontallyWithPadding(visibleSize.width / 15);
	menu->setPosition(visibleSize.width / 2.0f, visibleSize.height / 3.0f);
	addChild(menu, 2);
	return true;
}
void ZC::textFieldEvent(Ref *pSender, TextField::EventType type)
{
	switch (type)
	{
	case TextField::EventType::ATTACH_WITH_IME:
	{
		TextField* textField = dynamic_cast<TextField*>(pSender);
		Size screenSize = Director::getInstance()->getWinSize();
		if (textField->getString().empty())
		{
			textField->setPlaceHolder("|");
			textField->setTouchSize(Size(200, 50));
		}
		/*textField->runAction(MoveTo::create(0.225f,
		Vec2(screenSize.width / 2.0f, screenSize.height / 2.0f + textField->getContentSize().height / 2.0f)));*/
		//_displayValueLabel->setString(StringUtils::format("attach with IME"));
	}
	break;

	case TextField::EventType::DETACH_WITH_IME:
	{
		TextField* textField = dynamic_cast<TextField*>(pSender);
		Size screenSize = Director::getInstance()->getWinSize();
		if (textField->getString().empty())
			textField->setPlaceHolder("input account");
		//textField->setString("please input your account");
		/*	textField->runAction(MoveTo::create(0.175f, Vec2(screenSize.width / 2.0f, screenSize.height / 2.0f)));*/
		//_displayValueLabel->setString(StringUtils::format("detach with IME"));
	}
	break;

	case TextField::EventType::INSERT_TEXT:
		//_displayValueLabel->setString(StringUtils::format("insert words"));
		break;

	case TextField::EventType::DELETE_BACKWARD:
		//_displayValueLabel->setString(StringUtils::format("delete word"));
		break;

	default:
		break;
	}
}

void ZC::textFieldEvent1(cocos2d::Ref * pSender, cocos2d::ui::TextField::EventType type)
{
	switch (type)
	{
	case TextField::EventType::ATTACH_WITH_IME:
	{
		TextField* textField = dynamic_cast<TextField*>(pSender);
		Size screenSize = Director::getInstance()->getWinSize();
		if (textField->getString().empty())
		{
			textField->setPlaceHolder("|");
			textField->setTouchSize(Size(200, 50));
		}
		/*textField->runAction(MoveTo::create(0.225f,
		Vec2(screenSize.width / 2.0f, screenSize.height / 2.0f + textField->getContentSize().height / 2.0f)));*/
		//_displayValueLabel->setString(StringUtils::format("attach with IME"));
	}
	break;

	case TextField::EventType::DETACH_WITH_IME:
	{
		TextField* textField = dynamic_cast<TextField*>(pSender);
		Size screenSize = Director::getInstance()->getWinSize();
		if (textField->getString().empty())
			textField->setPlaceHolder("input password");
		//textField->setString("please input your account");
		/*	textField->runAction(MoveTo::create(0.175f, Vec2(screenSize.width / 2.0f, screenSize.height / 2.0f)));*/
		//_displayValueLabel->setString(StringUtils::format("detach with IME"));
	}
	break;

	case TextField::EventType::INSERT_TEXT:
		//_displayValueLabel->setString(StringUtils::format("insert words"));
		break;

	case TextField::EventType::DELETE_BACKWARD:
		//_displayValueLabel->setString(StringUtils::format("delete word"));
		break;

	default:
		break;
	}
}

cocos2d::Sprite * ZC::makeButton(std::string name, std::string bgSprite)
{
	Sprite* button = Sprite::create();

	Sprite* leftPart = Sprite::create(bgSprite);
	button->setContentSize(Size(leftPart->getContentSize().width * 2, leftPart->getContentSize().height));
	leftPart->setPosition(Vec2(button->getContentSize().width / 2, button->getContentSize().height / 2));
	leftPart->setAnchorPoint(Vec2(1, 0.5));
	button->addChild(leftPart);

	Sprite* rightPart = Sprite::create(bgSprite);
	rightPart->setFlipX(true);
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
