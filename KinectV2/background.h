#include <iostream>
#include <sstream>
#include <atlbase.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <Eigen/Core>		// 線形代数ライブラリ
#include <Eigen/Geometry>	// 外積の計算に必要
#include <Kinect.h>
#include <opencv2\opencv.hpp>

#include "fullscreen_layout.h"

#pragma once

class Background{
private:

public:
	// 背景を描画する
	cv::Mat drawBackground(
		CComPtr<IKinectSensor> kinect, cv::Mat campus,
		FullscreenLayout full_layout,
		std::vector<UINT16> background_depthBuffer,
		int depthWidth, int depthHeight,
		std::vector<BYTE> background_colorBuffer,
		int colorWidth, int colorHeight,
		int colorBytesPerPixel);
};