#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include "p2Defs.h"
#include "p2List.h"
#include "j1Render.h"
#include <list>
#include <string>

// -----------------------------------------
// -----------------------------------------

		  // USEFULL FUNCTIONS //

// -----------------------------------------
// -----------------------------------------

// Returns the angle from two points
float AngleFromTwoPoints(float x1, float y1, float x2, float y2);

// Returns the distance from two points(can be a negative distance)
float DistanceFromTwoPoints(float x1, float y1, float x2, float y2);

// Compares two char*, true if success
bool TextCmp(const char* text1, const char* text2);

//Separate a string into tokens using the indicated iterator
void Tokenize(std::string string, const char separator, std::list<std::string>& tokens);

//Puts al chars to lower case
void ToLowerCase(std::string str);

// Loads an xml and and load it's animations
void LoadAnimationFromXML(list<SDL_Rect>& rects, const char* file, const char* animation_name);

#endif //__FUNCTIONS_H__