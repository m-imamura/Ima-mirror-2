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
	void set_points_data(CComPtr<IKinectSensor> kinect, int person,
		std::vector<UINT16> depthBuffer, int depthWidth, int depthHeight,
		std::vector<BYTE> bodyIndexBuffer, int bodyIndexWidth, int bodyIndexHeight,
		std::vector<BYTE> colorBuffer, int colorWidth, int colorHeight,
		unsigned int colorBytesPerPixel);
};