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

// 書籍での解説のためにマクロにしています。実際には展開した形で使うことを検討してください。
#define ERROR_CHECK(ret) \
if ((ret) != S_OK) { \
	std::stringstream ss; \
	ss << "failed " #ret " " << std::hex << ret << std::endl; \
	throw std::runtime_error(ss.str().c_str()); \
}

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

// campusを受け取って背景をつけて返す
cv::Mat Background::drawBackground(
	CComPtr<IKinectSensor> kinect, cv::Mat campus,
	FullscreenLayout full_layout,
	std::vector<UINT16> background_depthBuffer,
	int depthWidth, int depthHeight,
	std::vector<BYTE> background_colorBuffer,
	int colorWidth, int colorHeight,
	int colorBytesPerPixel){
	
	std::vector<ColorSpacePoint> colorSpace(depthWidth * depthHeight);

	// Depth座標系に対応するColor座標系の一覧を取得する
	CComPtr<ICoordinateMapper> mapper;
	ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));
	mapper->MapDepthFrameToColorSpace(background_depthBuffer.size(), &background_depthBuffer[0], colorSpace.size(), &colorSpace[0]);
	
	for (int i = 0; i < depthHeight * depthWidth; i++){ // 背景をつける
		if (i % 4 == 0){
			DepthSpacePoint depth_point; //Depth座標系の点
			depth_point.X = i % depthWidth; //Depth画像上のxを代入
			depth_point.Y = i / depthWidth; //Depth画像上のyを代入

			ColorSpacePoint color_point; //Color座標系の点
			//mapper->MapDepthPointToColorSpace(depth_point, depthBuffer[i], &color_point);
			mapper->MapDepthPointToColorSpace(depth_point, background_depthBuffer[i], &color_point);
			// ↑mapperが知的．depthBufferの深度によってカラーのマッピング位置を決めている．単純な引き延ばしではない．
			//std::cout << "Background::drawBackground(): " << i << "\n";
			// colorSpaceのインデックスを求める
			int colorX = (int)color_point.X;
			int colorY = (int)color_point.Y;
			if ((colorX < 0) || (colorWidth <= colorX) || (colorY < 0) || (colorHeight <= colorY)){
				continue;
			}

			int colorIndex = (colorY * colorWidth) + colorX;

			int colorBufferIndex = colorIndex * colorBytesPerPixel;

			cv::Scalar color_point_color
				= cv::Scalar(background_colorBuffer[colorBufferIndex], background_colorBuffer[colorBufferIndex + 1], background_colorBuffer[colorBufferIndex + 2]);

			int fullsc_x = full_layout.magnification*(i % depthWidth) + full_layout.margin_x;
			int fullsc_y = full_layout.magnification*(i / depthWidth) + full_layout.margin_y;

			campus.at<cv::Vec3b>(fullsc_y, fullsc_x)[0] = background_colorBuffer[colorBufferIndex];
			campus.at<cv::Vec3b>(fullsc_y, fullsc_x)[1] = background_colorBuffer[colorBufferIndex + 1];
			campus.at<cv::Vec3b>(fullsc_y, fullsc_x)[2] = background_colorBuffer[colorBufferIndex + 2];


			// フルスクリーン座標に変換
			cv::Point fullScreenPoint;
			fullScreenPoint.x = fullsc_x;
			fullScreenPoint.y = fullsc_y;

			cv::circle(campus, fullScreenPoint, 5, color_point_color, -1);
		}
	}
	return campus;
}