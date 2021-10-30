#ifndef ANIMATIONSCONTROLLER_H
#define ANIMATIONSCONTROLLER_H

#include "Core/Interfaces/Object.h"

#include <map>
#include <string>

#include "AnimationContainer.h"

class AnimationsContainerIterator;

class ENGINE_API AnimationsController : public Object
{
protected:
	std::map<std::string, AnimationContainer*> _animations;

public:
	~AnimationsController ();

	AnimationContainer* GetAnimationContainer (const std::string& animationName);
	void AddAnimationContainer (AnimationContainer* animContainer);

	AnimationsContainerIterator begin ();
	AnimationsContainerIterator end ();
};

class ENGINE_API AnimationsContainerIterator
{
	friend AnimationsController;

protected:
	std::map<std::string, AnimationContainer*>::iterator _it;

public:
	AnimationsContainerIterator& operator++ ();
	bool operator != (const AnimationsContainerIterator& other);
	AnimationContainer* operator* ();
protected:
	AnimationsContainerIterator (std::map<std::string, AnimationContainer*>::iterator it);
};

#endif