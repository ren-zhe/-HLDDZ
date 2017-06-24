#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
	cocos2d::Sprite* makeButton(std::string name, std::string bgSprite);
	void updateBar(float dt);
	/*void textFieldEvent(cocos2d::Ref* sender, cocos2d::ui::TextField::EventType type);
	void textFieldEvent1(cocos2d::Ref* sender, cocos2d::ui::TextField::EventType type);*/
};

#endif // __HELLOWORLD_SCENE_H__
