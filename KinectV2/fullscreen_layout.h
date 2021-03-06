#include <iostream>
#include <sstream>
#include <atlbase.h>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2\opencv.hpp>
#include <windows.h>

#pragma once

class FullscreenLayout{
private:

public:
	int DisplayWidth;
	int DisplayHeight;
	int margin_x; // x方向の余白
	int margin_y; // y方向の余白
	double magnification; // 倍率

	void set_fullscreen_layout(double depthWidth, double depthHeight);
};
