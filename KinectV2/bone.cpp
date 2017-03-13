/*
//ボーン1人分

#include <stdio.h>
#include <Eigen/Core>		// 線形代数ライブラリ
#include <Kinect.h>

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

void bone::set_bone_data(Joint joints){

	Eigen::Vector4f joint_position[JOINTS]; //関節の位置

	for (auto joint : joints) {
		if (joint.TrackingState == TrackingState::TrackingState_Tracked) {
			joint_position[joint.JointType] << joint.Position.X, joint.Position.Y, joint.Position.Z, 1.0;
		}
	}

	// Bodyがあればすべてのボーンについて繰り返し処理
	for (int i = 0; i < BONES; i++){

		// i番目のボーンのtop，bottomがどの関節かを求めて，関節の位置を入れる
		bottom_init[i] = joint_position[bone_set[i].bottom];
		top_init[i] = joint_position[bone_set[i].top];

		// i番目のボーンのvector(ボーンの向き)をtop-bottomから求める
		vector_init[i].segment(0, 3) = top_init[i].segment(0, 3) - bottom_init[i].segment(0, 3);
		vector_init[i].w() = 1.0;

		length[i] = vector_init[i].segment(0, 3).norm();
		vector_init[i].segment(0, 3) = vector_init[i].segment(0, 3).normalized();
	}

}

*/