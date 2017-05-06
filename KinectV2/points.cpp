// body１つ分の標本点

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

#define POINTS_MAX 10000	// 描画する点群の数
#define PEOPLE 6			// 人数

class Points{
private:

	cv::Scalar index_colors[PEOPLE];	// プレーヤーの色を格納

public:
	// 変数
	Eigen::Vector4f *points_init;	//初期位置の点(Camera座標) 最大POINTS_XYZ_NUM個
	Eigen::Vector4f *points;		//変換後の点(Camera座標) 最大POINTS_XYZ_NUM個
	Eigen::Vector4f *points_depth;	//変換後の点(Depth座標)
	cv::Scalar *color;				//色
	int points_num;					//点の総数
	int body_num;					//身体番号

	//関数
	// インデックス画面におけるプレイヤーのインデックス色設定　★最初に呼び出す
	void set_players_index_color();

	// getinitしたときのデータをもらって保存しておく関数
	void set_points_data(int person, ICoordinateMapper* mapper,
		std::vector<UINT16> depthBuffer, int depthWidth, int depthHeight,
		std::vector<BYTE> bodyIndexBuffer, int bodyIndexWidth, int bodyIndexHeight,
		std::vector<BYTE> colorBuffer, int colorWidth, int colorHeight,
		std::vector<ColorSpacePoint> colorSpace,
		unsigned int colorBytesPerPixel);
};

void Points::set_players_index_color(){
	// カラー版
	index_colors[0] = cv::Scalar(255, 0, 0);
	index_colors[1] = cv::Scalar(0, 255, 0);
	index_colors[2] = cv::Scalar(0, 0, 255);
	index_colors[3] = cv::Scalar(255, 255, 0);
	index_colors[4] = cv::Scalar(255, 0, 255);
	index_colors[5] = cv::Scalar(0, 255, 255);

	//グレースケール版
	index_colors[0] = cv::Scalar(255, 255, 255);
	index_colors[1] = cv::Scalar(234, 234, 234);
	index_colors[2] = cv::Scalar(231, 213, 213);
	index_colors[3] = cv::Scalar(192, 192, 192);
	index_colors[4] = cv::Scalar(171, 171, 171);
	index_colors[5] = cv::Scalar(150, 150, 150);
}

void Points::set_points_data(int person, ICoordinateMapper* mapper,
	std::vector<UINT16> depthBuffer, int depthWidth, int depthHeight,
	std::vector<BYTE> bodyIndexBuffer, int bodyIndexWidth, int bodyIndexHeight,
	std::vector<BYTE> colorBuffer, int colorWidth, int colorHeight,
	std::vector<ColorSpacePoint> colorSpace,
	unsigned int colorBytesPerPixel){

	// 表示用画面
	cv::Mat points_Image = cv::Mat::zeros(depthHeight, depthWidth, CV_8UC4);

	points_init = new Eigen::Vector4f[POINTS_MAX];
	points = new Eigen::Vector4f[POINTS_MAX];
	points_depth = new Eigen::Vector4f[POINTS_MAX];
	color = new cv::Scalar[POINTS_MAX];

	body_num = person; // 外部から与えられたボディ番号を自分のボディ番号とする． 
	
	// mapperの部分は外で．
	points_num = 0;
	// DepthFrameをなめながら身体の領域内に点群を配置
	for (int i = 0; i < bodyIndexWidth * bodyIndexHeight; i++)
	{
		// i番目の画素が身体の領域内なら
		if (index_colors[bodyIndexBuffer[i]] == index_colors[person] && points_num < POINTS_MAX)
		{
			// i番目の画素のx，y座標を取得
			int x = i % bodyIndexWidth;
			int y = i / bodyIndexWidth;

			// Depth座標系の点群をCamera座標系の点群に変換して配置
			if (x % 4 == 0 && y % 4 == 0){

				DepthSpacePoint depth_point; //Depth座標系の点
				depth_point.X = x; //Depth画像上のxを代入
				depth_point.Y = y; //Depth画像上のyを代入

				CameraSpacePoint camera_point; //Camera座標系の点
				mapper->MapDepthPointToCameraSpace(depth_point, depthBuffer[i], &camera_point);

				// colorSpaceのインデックスを求める
				int colorX = (int)colorSpace[i].X;
				int colorY = (int)colorSpace[i].Y;
				if ((colorX < 0) || (colorWidth <= colorX) || (colorY < 0) || (colorHeight <= colorY)){
					continue;
				}
				int colorIndex = (colorY * colorWidth) + colorX;

				int colorBufferIndex = colorIndex * colorBytesPerPixel;

				cv::Scalar color_point_color = cv::Scalar(colorBuffer[colorBufferIndex], colorBuffer[colorBufferIndex + 1], colorBuffer[colorBufferIndex + 2]);
				points_init[points_num] << camera_point.X, camera_point.Y, camera_point.Z, 1.0;
				color[points_num] = color_point_color;
				points_num++;

				// とったポイントを画面にカラーで出力
				points_Image.data[i*4 + 0] = colorBuffer[colorBufferIndex];
				points_Image.data[i*4 + 1] = colorBuffer[colorBufferIndex+1];
				points_Image.data[i*4 + 2] = colorBuffer[colorBufferIndex+2];

			}
		}
	}
	std::cout << "body_num " <<body_num << ": Points_num " << points_num << "\n";

	cv::imshow("Points_Image", points_Image);
}