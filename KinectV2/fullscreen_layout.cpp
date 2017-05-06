#include <stdio.h>
#include <opencv2\opencv.hpp>
#include <windows.h>

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


// フルスクリーン表示設定
void Fullscreen_Layout::set_fullscreen_layout(double depthWidth, double depthHeight){

	cv::Point fullScreen_first_pixel;
	cv::Point fullScreen_last_pixel;
	int In_display_width; // ディスプレイ中のイマミラーの画面の幅
	int In_display_height;// ディスプレイ中のイマミラーの画面の高さ

	// ディスプレイサイズ取得
	int DisplayWidth = GetSystemMetrics(SM_CXSCREEN);
	int DisplayHeight = GetSystemMetrics(SM_CYSCREEN);

	DisplayWidth = DisplayWidth;
	DisplayHeight = DisplayHeight;

	if ((double)DisplayWidth / (double)DisplayHeight > (double)depthWidth / (double)depthHeight){ // ディスプレイが横長．たぶんこの確率が高い
		In_display_width = (double)DisplayHeight * ((double)depthWidth / (double)depthHeight);
		In_display_height = DisplayHeight;
		fullScreen_first_pixel.x = (DisplayWidth - In_display_width) / 2;
		fullScreen_first_pixel.y = 0;
		fullScreen_last_pixel.x = (DisplayWidth - In_display_width) / 2 + In_display_width;
		fullScreen_last_pixel.y = DisplayHeight;
		margin_x = fullScreen_first_pixel.x;
		margin_y = 0;
		magnification = (double)In_display_height / (double)depthHeight;
	}
	else if ((double)DisplayWidth / (double)DisplayHeight < (double)depthWidth / (double)depthHeight) { // ディスプレイが縦長
		In_display_width = DisplayWidth;
		In_display_height = (double)DisplayWidth * ((double)depthHeight / (double)depthWidth);
		fullScreen_first_pixel.x = 0;
		fullScreen_first_pixel.y = (DisplayHeight - In_display_height) / 2;
		fullScreen_last_pixel.x = DisplayWidth;
		fullScreen_last_pixel.y = (DisplayHeight - In_display_height) / 2 + In_display_height;
		margin_x = 0;
		margin_y = fullScreen_first_pixel.y;
		magnification = (double)In_display_width / (double)depthWidth;
	}
	else{ // depth画像のアスペクト比とディスプレイのアスペクト比がいっしょ
		In_display_width = DisplayWidth;
		In_display_height = DisplayHeight;
		fullScreen_first_pixel.x = 0.0;
		fullScreen_first_pixel.y = 0.0;
		fullScreen_last_pixel.x = In_display_width;
		fullScreen_last_pixel.y = In_display_height;
		margin_x = 0;
		margin_y = 0;
		magnification = (double)In_display_width / (double)depthWidth;
	}

}