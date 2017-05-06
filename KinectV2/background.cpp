
/*
#include <stdio.h>
#include <opencv2\opencv.hpp>
#include <Kinect.h>

class Background{
private:


public:
	void drawBackground(cv::Mat campus);// 背景を描画する

	Background(){//コンストラクタ
		
	}
};


void drawBackground(cv::Mat campus){
	
	// 背景をMatに描画する関数にしたい．あとは張り付けるだけでオッケーにしたい

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

			// colorSpaceのインデックスを求める
			int colorX = (int)color_point.X;
			int colorY = (int)color_point.Y;
			if ((colorX < 0) || (colorWidth <= colorX) || (colorY < 0) || (colorHeight <= colorY)){
				continue;
			}

			int colorIndex = (colorY * colorWidth) + colorX;

			int colorBufferIndex = colorIndex * colorBytesPerPixel;

			cv::Scalar color_point_color = cv::Scalar(backgroundBuffer[colorBufferIndex], backgroundBuffer[colorBufferIndex + 1], backgroundBuffer[colorBufferIndex + 2]);

			int fullsc_x = full_layout.magnification*(i % depthWidth) + full_layout.margin_x;
			int fullsc_y = full_layout.magnification*(i / depthWidth) + full_layout.margin_y;


			campus.at<cv::Vec3b>(fullsc_y, fullsc_x)[0] = backgroundBuffer[colorBufferIndex];
			campus.at<cv::Vec3b>(fullsc_y, fullsc_x)[1] = backgroundBuffer[colorBufferIndex + 1];
			campus.at<cv::Vec3b>(fullsc_y, fullsc_x)[2] = backgroundBuffer[colorBufferIndex + 2];


			// フルスクリーン座標に変換
			cv::Point fullScreenPoint;
			fullScreenPoint.x = fullsc_x;
			fullScreenPoint.y = fullsc_y;

			cv::circle(campus, fullScreenPoint, 5, color_point_color, -1);

		}
	}
}*/