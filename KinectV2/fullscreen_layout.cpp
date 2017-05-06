#include <stdio.h>
#include <opencv2\opencv.hpp>
#include <windows.h>

class Fullscreen_Layout{
private:

public:	
	int DisplayWidth;
	int DisplayHeight;
	int margin_x; // x�����̗]��
	int margin_y; // y�����̗]��
	double magnification; // �{��

	void set_fullscreen_layout(double depthWidth, double depthHeight);
};


// �t���X�N���[���\���ݒ�
void Fullscreen_Layout::set_fullscreen_layout(double depthWidth, double depthHeight){

	cv::Point fullScreen_first_pixel;
	cv::Point fullScreen_last_pixel;
	int In_display_width; // �f�B�X�v���C���̃C�}�~���[�̉�ʂ̕�
	int In_display_height;// �f�B�X�v���C���̃C�}�~���[�̉�ʂ̍���

	// �f�B�X�v���C�T�C�Y�擾
	int DisplayWidth = GetSystemMetrics(SM_CXSCREEN);
	int DisplayHeight = GetSystemMetrics(SM_CYSCREEN);

	DisplayWidth = DisplayWidth;
	DisplayHeight = DisplayHeight;

	if ((double)DisplayWidth / (double)DisplayHeight > (double)depthWidth / (double)depthHeight){ // �f�B�X�v���C�������D���Ԃ񂱂̊m��������
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
	else if ((double)DisplayWidth / (double)DisplayHeight < (double)depthWidth / (double)depthHeight) { // �f�B�X�v���C���c��
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
	else{ // depth�摜�̃A�X�y�N�g��ƃf�B�X�v���C�̃A�X�y�N�g�䂪��������
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