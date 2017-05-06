#include <stdio.h>
#include <opencv2\opencv.hpp>

class Fullscreen_Layout{
private:

public:
	int DisplayWidth;
	int DisplayHeight;
	int margin_x; // x•ûŒü‚Ì—]”’
	int margin_y; // y•ûŒü‚Ì—]”’
	double magnification; // ”{—¦

	void set_fullscreen_layout(double depthWidth, double depthHeight);
};
