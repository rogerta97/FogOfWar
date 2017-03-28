#ifndef __p2Log_H__
#define __p2Log_H__

#include <windows.h>
#include <stdio.h>

class p2String;

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__)

void log(const char file[], int line, const char* format, ...);

#endif