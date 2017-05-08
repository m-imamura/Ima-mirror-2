// 円描画機能

#include <iostream>
#include <sstream>

#include <atlbase.h>
#include <Eigen/Core>		// 線形代数ライブラリ
#include <Eigen/Geometry>	// 外積の計算に必要
#include <Kinect.h>
#include <opencv2\opencv.hpp>

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "fullscreen_layout.h"

#pragma once


class DrawEllipse{
private:

public:
	void drawEllipse(CComPtr<IKinectSensor> kinect, cv::Mat &bodyImage, const Joint &joint, int r, const cv::Scalar &color);
	void mydrawEllipse(CComPtr<IKinectSensor> kinect, cv::Mat &bodyImage, const Eigen::Vector4f &joint, int r, const cv::Scalar &color);
	void drawEllipse_fullScreen(
		CComPtr<IKinectSensor> kinect,
		cv::Mat &campus,
		CameraSpacePoint &cam_point,
		int r, const cv::Scalar &color,
		FullscreenLayout full_layout);
	void draw_circle(
		CComPtr<IKinectSensor> kinect,
		cv::Mat &bodyImage,
		DepthSpacePoint depth_point,
		int r, const cv::Scalar &color,
		FullscreenLayout full_layout);
};