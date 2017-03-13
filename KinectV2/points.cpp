// body１つ分の標本点

#include <stdio.h>
#include <Eigen/Core>		// 線形代数ライブラリ
#include <opencv2\opencv.hpp>

#define POINTS_MAX 30000	// 描画する点群の数

class points{
private:
	Eigen::Vector4f points_init[POINTS_MAX];	//初期位置の点(Camera座標) 最大POINTS_XYZ_NUM個
	Eigen::Vector4f points[POINTS_MAX];			//変換後の点(Camera座標) 最大POINTS_XYZ_NUM個
	Eigen::Vector4f points_depth[POINTS_MAX];	//変換後の点(Depth座標)
	cv::Scalar color[POINTS_MAX];				//色
	int points_num;								//点の総数

public:
	// getinitしたときのデータをもらって保存しておくかんじの関数
	void set_points_data();
};

void points::set_points_data(){
	int i = 100;
	return;
}