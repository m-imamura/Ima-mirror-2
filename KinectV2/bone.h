//ボーン1人分

#include <stdio.h>
#include <Eigen/Core>		// 線形代数ライブラリ

#define JOINTS 25			// ジョイントの数
#define BONES 24			// 人体のボーンの数

class bone{
private:
	// ボーンの情報
	Eigen::Vector4f top_init[BONES];	//初期位置
	Eigen::Vector4f bottom_init[BONES];//初期位置
	Eigen::Vector4f vector_init[BONES];//初期方向
	Eigen::Vector4f top[BONES];		//逐次位置
	Eigen::Vector4f bottom[BONES];		//逐次位置
	Eigen::Vector4f vector[BONES];		//逐次方向 (bottom -> top)
	int parent[BONES];					//親ボーン (使ってない？)
	float length[BONES];				//長さ (vector_initの)


public:

	// ジョイントポジションをもらってボーン情報を更新するかんじの関数
	void set_bone_data(Joint joints);
};