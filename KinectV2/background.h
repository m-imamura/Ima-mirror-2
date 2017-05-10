#include <iostream>
#include <sstream>
#include <atlbase.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <Eigen/Core>		// ���`�㐔���C�u����
#include <Eigen/Geometry>	// �O�ς̌v�Z�ɕK�v
#include <Kinect.h>
#include <opencv2\opencv.hpp>

#include "fullscreen_layout.h"

#pragma once

class Background{
private:

public:
	// �w�i��`�悷��
	cv::Mat drawBackground(
		CComPtr<IKinectSensor> kinect, cv::Mat campus,
		FullscreenLayout full_layout,
		std::vector<UINT16> background_depthBuffer,
		int depthWidth, int depthHeight,
		std::vector<BYTE> background_colorBuffer,
		int colorWidth, int colorHeight,
		int colorBytesPerPixel);
};