#include <stdio.h>
#include <opencv2\opencv.hpp>

class Fullscreen_Layout{
private:

public:
	int DisplayWidth;
	int DisplayHeight;
	int margin_x; // x方向の余白
	int margin_y; // y方向の余白
	double magnification; // 倍率

	void set_fullscreen_layout(double depthWidth, double depthHeight);
};
