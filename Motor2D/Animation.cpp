#include "Animation.h"
#include "Functions.h"
#include "p2Log.h"

Animation::Animation(const char* _name, list<SDL_Rect>& rects, float speed, bool loop) : speed(speed), loop(loop)
{

	for (list<SDL_Rect>::iterator it = rects.begin(); it != rects.end(); it++)
		frames.push_back(*it);

	name = _name;
}

Animation::~Animation()
{
}

SDL_Rect& Animation::GetAnimationFrame(float dt)
{
	curr_frame += (speed * dt);

	if (curr_frame >= frames.size())
	{
		if (!loop)
			curr_frame = frames.size() - 1;
		else
			curr_frame = 0.0f;

		loops++;
	}

	int counter = 0;
	for (list<SDL_Rect>::iterator it = frames.begin(); it != frames.end(); it++, counter++)
	{
		if (counter == (int)(curr_frame))
		{
			return (*it);
		}
	}

	SDL_Rect ret = NULLRECT;
	return ret;
}

SDL_Rect& Animation::GetActualFrame()
{
	int counter = 0;
	for (list<SDL_Rect>::iterator it = frames.begin(); it != frames.end(); it++, counter++)
	{
		if (counter == (int)curr_frame)
		{
			return (*it);
		}
	}

	SDL_Rect ret = NULLRECT;
	return ret;
}

float Animation::GetFrameIndex() const
{
	return (int)curr_frame;
}

SDL_Rect & Animation::GetFrame(int frame)
{
	int counter = 0;
	for (list<SDL_Rect>::iterator it = frames.begin(); it != frames.end(); it++, counter++)
	{
		if (counter == frame)
		{
			return (*it);
		}
	}

	SDL_Rect ret = NULLRECT;
	return ret;
}

void Animation::SetCurrFrame(int frame)
{
	curr_frame = frame;
}

void Animation::SetSpeed(float _speed)
{
	speed = _speed;
}

void Animation::SetLoop(bool _loop)
{
	loop = _loop;
}

bool Animation::IsCurrFrame(int frame)
{
	return (frame == curr_frame);
}

bool Animation::Finished() const
{
	return loops > 0;
}

void Animation::Reset()
{
	curr_frame = 0.0f;
	loops = 0;
}

const char* Animation::GetName()
{
	if (name.size() > 0)
		return name.c_str();
	else
		return "";
}

Animator::Animator()
{
}

Animator::~Animator()
{
	for (list<Animation*>::iterator it = animations.begin(); it != animations.end(); it++)
		delete (*it);
}

void Animator::AddAnimation(Animation* animation)
{
	animations.push_back(animation);
}

void Animator::LoadAnimationsFromXML(pugi::xml_document &doc)
{

	for (pugi::xml_node anim = doc.child("file").child("animations").child("anim"); anim != NULL; anim = anim.next_sibling("anim")) 
	{
	    list<SDL_Rect> anim_rects;
		float speed = anim.attribute("speed").as_float(1.0f);
		string name = anim.attribute("name").as_string("null");
		bool loop = anim.attribute("loop").as_bool(true);

		for (pugi::xml_node frame = anim.child("rect"); frame != NULL; frame = frame.next_sibling("rect")) 
		{
			SDL_Rect new_frame = { frame.attribute("x").as_int(0),frame.attribute("y").as_int(0),frame.attribute("w").as_int(0),frame.attribute("h").as_int(0) };
			anim_rects.push_back(new_frame);
		}

		Animation* animation = new Animation(name.c_str(), anim_rects, speed, loop);
		AddAnimation(animation);
	}
}

void Animator::SetAnimation(const char* name)
{
	if (next_animation != nullptr && TextCmp(next_animation->GetName(), name))
		return;

	for (list<Animation*>::iterator it = animations.begin(); it != animations.end(); it++)
	{
		if (TextCmp(name, (*it)->GetName()))
		{
			next_animation = (*it);
			break;
		}
	}
}

void Animator::SetAnimationTransition(const char * transition_name, const char * a1, const char * a2)
{
	anim_trans at(transition_name, a1, a2);
	anim_trans_list.push_back(at);
}

Animation* Animator::GetAnimation(const char * name)
{
	Animation* ret = nullptr;

	for (list<Animation*>::iterator it = animations.begin(); it != animations.end(); it++)
	{
		if (TextCmp(name, (*it)->GetName()))
		{
			ret = (*it);
			break;
		}
	}

	return ret;
}

Animation* Animator::GetCurrentAnimation()
{
	for (list<anim_trans>::iterator it = anim_trans_list.begin(); it != anim_trans_list.end(); it++)
	{
		if (TextCmp(next_animation->GetName(), (*it).a2.c_str()) && TextCmp(current_animation->GetName(), (*it).a1.c_str()))
		{
			if (!GetAnimation((*it).transition_name.c_str())->Finished())
				return GetAnimation((*it).transition_name.c_str());
			else
				GetAnimation((*it).transition_name.c_str())->Reset();
		}
	}

	if(current_animation != next_animation || current_animation == nullptr || anim_trans_list.size() == 0)
		current_animation = next_animation;

	return current_animation;
}

bool Animator::IsCurrentAnimation(const char * name)
{
	if (current_animation != nullptr)
	{
		if (TextCmp(current_animation->GetName(), name))
			return true;
	}
	return false;
}
