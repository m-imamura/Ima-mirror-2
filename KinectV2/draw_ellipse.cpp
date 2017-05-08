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

// 書籍での解説のためにマクロにしています。実際には展開した形で使うことを検討してください。
#define ERROR_CHECK(ret) \
	if ((ret) != S_OK) { \
		std::stringstream ss; \
		ss << "failed " #ret " " << std::hex << ret << std::endl; \
		throw std::runtime_error(ss.str().c_str()); \
	}
	//////////////////////////////////////////////////

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



// DepthImage上に円を描画 (DepthImage，Camera座標系の描画位置，描画半径，描画色)
void DrawEllipse::drawEllipse(CComPtr<IKinectSensor> kinect, cv::Mat &bodyImage, const Joint &joint, int r, const cv::Scalar &color)
{
	// Camera座標系をDepth座標系に変換する
	CComPtr<ICoordinateMapper> mapper;
	ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));
	DepthSpacePoint point;
	mapper->MapCameraPointToDepthSpace(joint.Position, &point);

	cv::circle(bodyImage, cv::Point(point.X, point.Y), r, color, -1);
}

// Eigen版：DepthImage上に円を描画 (DepthImage，Camera座標系の描画位置，描画半径，描画色)
void DrawEllipse::mydrawEllipse(CComPtr<IKinectSensor> kinect, cv::Mat &bodyImage, const Eigen::Vector4f &joint, int r, const cv::Scalar &color)
{
	// Camera座標系をDepth座標系に変換する
	CComPtr<ICoordinateMapper> mapper;
	ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));

	DepthSpacePoint point;
	CameraSpacePoint joint3;
	joint3.X = joint.x();
	joint3.Y = joint.y();
	joint3.Z = joint.z();
	mapper->MapCameraPointToDepthSpace(joint3, &point);

	cv::circle(bodyImage, cv::Point(point.X, point.Y), r, color, -1);
}

// フルスクリーン用のdrawWllipse
void DrawEllipse::drawEllipse_fullScreen(
	CComPtr<IKinectSensor> kinect,
	cv::Mat &campus,
	CameraSpacePoint &cam_point,
	int r, const cv::Scalar &color,
	FullscreenLayout full_layout)
{
	// Camera座標系をDepth座標系に変換する
	CComPtr<ICoordinateMapper> mapper;
	ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));

	DepthSpacePoint point;
	mapper->MapCameraPointToDepthSpace(cam_point, &point);

	// ↑ここまでmydrawEllipseと同様（変数名とか違うけど）

	// フルスクリーン座標に変換
	cv::Point fullScreenPoint;
	fullScreenPoint.x = (int)((double)point.X*full_layout.magnification + full_layout.margin_x);
	fullScreenPoint.y = point.Y*full_layout.magnification + full_layout.margin_y;

	cv::circle(campus, fullScreenPoint, r, color, -1);
}

void DrawEllipse::draw_circle(
	CComPtr<IKinectSensor> kinect,
	cv::Mat &bodyImage,
	DepthSpacePoint depth_point,
	int r, const cv::Scalar &color,
	FullscreenLayout full_layout){

	// フルスクリーン座標に変換
	cv::Point fullScreenPoint;
	fullScreenPoint.x = depth_point.X*full_layout.magnification + full_layout.margin_x;
	fullScreenPoint.y = depth_point.Y*full_layout.magnification + full_layout.margin_y;

	cv::circle(bodyImage, fullScreenPoint, r, color, -1);
}