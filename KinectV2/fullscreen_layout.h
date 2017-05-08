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
	int margin_x; // x�����̗]��
	int margin_y; // y�����̗]��
	double magnification; // �{��

	void set_fullscreen_layout(double depthWidth, double depthHeight);
};
