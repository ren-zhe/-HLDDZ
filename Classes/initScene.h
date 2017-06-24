#ifndef __INIT_SCENE_H__
#define __INIT_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
class InitScene : public cocos2d::Layer
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();
	// implement the "static create()" method manually
	CREATE_FUNC(InitScene);

	cocos2d::Sprite* makeButton(std::string name, std::string bgSprite);
};

#endif // __HELLOWORLD_SCENE_H__

