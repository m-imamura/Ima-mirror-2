//ボーン1人分
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

#define JOINTS 25			// ジョイントの数
#define BONES 24			// 人体のボーンの数

class Bone{
private:

public:
	// ボーンの情報
	Eigen::Vector4f *top_init;	//初期位置
	Eigen::Vector4f *bottom_init;//初期位置
	Eigen::Vector4f *vector_init;//初期方向
	Eigen::Vector4f *top;		//逐次位置
	Eigen::Vector4f *bottom;		//逐次位置
	Eigen::Vector4f *vector;		//逐次方向 (bottom -> top)
	int *parent;					//親ボーン
	float *length;				//長さ (vector_initの)

	class Bone_connect{ // ボーン１本分の接続関係
	private:
	public:
		int top;	//先端のジョイント番号
		int bottom;	//根元のジョイント番号
		int parent;	//親ボーン
		double impactrange; //ボーンの影響範囲
	};
	Bone_connect *bone_connect;

	// 関数

	Bone();//コンストラクタ
	~Bone();//デストラクタ
	
	// ジョイントポジションをもらってボーン情報（自前）を更新する関数
	void set_bones_data(IBody* body);

	// ジョイントポジションをもらって初期のボーン情報を格納する関数
	void set_bones_init_data(IBody* body);

	// ボーンの接続関係を定義する関数（インスタンス時に自動で呼ばれる↓）
	void define_bone_connect(Bone_connect bone_connect[BONES]);
};

// コンストラクタ
Bone::Bone(){
	// 動的割り当て
	top_init = new Eigen::Vector4f[BONES];	//初期位置
	bottom_init = new Eigen::Vector4f[BONES];//初期位置
	vector_init = new Eigen::Vector4f[BONES];//初期方向
	top = new Eigen::Vector4f[BONES];
	bottom = new Eigen::Vector4f[BONES];		//逐次位置
	vector = new Eigen::Vector4f[BONES];		//逐次方向 (bottom -> top)
	parent = new int[BONES];					//親ボーン
	length = new float[BONES];				//長さ (vector_initの)
	bone_connect = new Bone_connect[BONES];
	std::cout << "Bone::Bone(): コンストラクタで動的割り当て\n";

	define_bone_connect(bone_connect);
	std::cout << "Bone::Bone(): コンストラクタでdefine_bone_connect()\n";
}

Bone::~Bone(){
	delete top_init;	//初期位置
	delete bottom_init;//初期位置
	delete vector_init;//初期方向
	delete top;
	delete bottom;		//逐次位置
	delete vector;		//逐次方向 (bottom -> top)
	delete parent;					//親ボーン
	delete length;				//長さ (vector_initの)
	delete bone_connect;
	std::cout << "Boneにて動的割り当て削除\n";
}

void Bone::set_bones_init_data(IBody* body){

	Eigen::Vector4f *joint_position; //関節の位置
	joint_position = new Eigen::Vector4f[JOINTS];
	
	// 関節の位置を取得
	Joint joints[JointType::JointType_Count];
	body->GetJoints(JointType::JointType_Count, joints);
	for (auto joint : joints) {
		if (joint.TrackingState == TrackingState::TrackingState_Tracked) {
			//drawEllipse(bodyImage, joint, 3, cv::Scalar(255, 255, 255));
			joint_position[joint.JointType] << joint.Position.X, joint.Position.Y, joint.Position.Z, 1.0;
		}
		//std::cout << joint.JointType << " " << joint_position[joint.JointType] << "\n";
	}

	// Bodyがあればすべてのボーンについて繰り返し処理
	for (int i = 0; i < BONES; i++){

		// i番目のボーンのtop，bottomがどの関節かを求めて，関節の位置を入れる
		bottom_init[i] = joint_position[bone_connect[i].bottom];
		top_init[i] = joint_position[bone_connect[i].top];

		// i番目のボーンのvector(ボーンの向き)をtop-bottomから求める
		vector_init[i].segment(0, 3) = top_init[i].segment(0, 3) - bottom_init[i].segment(0, 3);
		vector_init[i].w() = 1.0;

		length[i] = vector_init[i].segment(0, 3).norm();
		vector_init[i].segment(0, 3) = vector_init[i].segment(0, 3).normalized();

	}

	delete joint_position;
}

// ボーン情報の更新
void Bone::set_bones_data(IBody* body){ // bodies[6]の１体分をもらってきているIBodyのbody

	Eigen::Vector4f *joint_position; //関節の位置
	joint_position = new Eigen::Vector4f[JOINTS];

	// 関節の位置を取得
	Joint *joints;
	joints = new Joint[JointType::JointType_Count];

	body->GetJoints(JointType::JointType_Count, joints);
	for (int i = 0; i < JointType::JointType_Count; i++) {
		if (joints[i].TrackingState == TrackingState::TrackingState_Tracked) {
			joint_position[i] << joints[i].Position.X, joints[i].Position.Y, joints[i].Position.Z, 1.0;
			//drawEllipse(init_bodyImage, joint, 3, cv::Scalar(255, 255, 255));
		}
	}
	delete joints;

	//すべてのボーンについて繰り返し処理
	for (int i = 0; i < BONES; i++){

		// i番目のボーンのtop，bottomがどの関節かを求めて，関節の位置を入れる
		bottom[i] = joint_position[bone_connect[i].bottom];
		top[i] = joint_position[bone_connect[i].top];

		// i番目のボーンのvector(ボーンの向き)をtop-bottomから求める
		vector[i].segment(0, 3) = top[i].segment(0, 3) - bottom[i].segment(0, 3);
		vector[i].w() = 1.0;
		vector[i].segment(0, 3) = vector[i].segment(0, 3).normalized();
	}

	//printf("腰ボーンのx位置 %f\n", bottom[0].x());

	delete joint_position;
}


void Bone::define_bone_connect(Bone::Bone_connect bone_connect[BONES]){
	
	for (int i = 0; i < BONES; i++){
		bone_connect[i].impactrange = 0.5;
	}

	//体幹
	bone_connect[0].bottom = JointType_SpineBase;//根元のジョイントを設定
	bone_connect[0].top = JointType_SpineMid;//先端のジョイントを設定
	bone_connect[0].parent = -1;//親のボーンを設定（相対位置の基準）
	bone_connect[0].impactrange = 0.8;

	bone_connect[1].bottom = JointType_SpineMid;
	bone_connect[1].top = JointType_SpineShoulder;
	bone_connect[1].parent = 0;
	bone_connect[1].impactrange = 0.8;

	bone_connect[2].bottom = JointType_SpineShoulder;
	bone_connect[2].top = JointType_Neck;
	bone_connect[2].parent = 1;
	bone_connect[2].impactrange = 0.4;

	bone_connect[3].bottom = JointType_Neck;
	bone_connect[3].top = JointType_Head;
	bone_connect[3].parent = 2;
	bone_connect[3].impactrange = 1.0;

	//左腕
	bone_connect[4].bottom = JointType_SpineShoulder;
	bone_connect[4].top = JointType_ShoulderLeft;
	bone_connect[4].parent = 1;//枝分かれ bone2（首）と同じ立場
	//bone_connect[4] = 0.4;

	bone_connect[5].bottom = JointType_ShoulderLeft;
	bone_connect[5].top = JointType_ElbowLeft;
	bone_connect[5].parent = 4;
	//bone_connect[5].impactrange = 0.4;

	bone_connect[6].bottom = JointType_ElbowLeft;
	bone_connect[6].top = JointType_WristLeft;
	bone_connect[6].parent = 5;
	//bone_connect[6].impactrange = 0.4;

	bone_connect[7].bottom = JointType_WristLeft;
	bone_connect[7].top = JointType_HandLeft;
	bone_connect[7].parent = 6;
	//bone_connect[7].impactrange = 0.5;

	bone_connect[8].bottom = JointType_HandLeft;
	bone_connect[8].top = JointType_HandTipLeft;
	bone_connect[8].parent = 7;
	//bone_connect[8].impactrange = 0.4;

	bone_connect[9].bottom = JointType_HandLeft;
	bone_connect[9].top = JointType_ThumbLeft;
	bone_connect[9].parent = 7;
	//bone_connect[9].impactrange = 0.4;

	//右腕
	bone_connect[10].bottom = JointType_SpineShoulder;
	bone_connect[10].top = JointType_ShoulderRight;
	bone_connect[10].parent = 1;//枝分かれ bone2（首）と同じ立場

	bone_connect[11].bottom = JointType_ShoulderRight;
	bone_connect[11].top = JointType_ElbowRight;
	bone_connect[11].parent = 10;

	bone_connect[12].bottom = JointType_ElbowRight;
	bone_connect[12].top = JointType_WristRight;
	bone_connect[12].parent = 11;

	bone_connect[13].bottom = JointType_WristRight;
	bone_connect[13].top = JointType_HandRight;
	bone_connect[13].parent = 12;

	bone_connect[14].bottom = JointType_HandRight;
	bone_connect[14].top = JointType_HandTipRight;
	bone_connect[14].parent = 13;

	bone_connect[15].bottom = JointType_HandRight;
	bone_connect[15].top = JointType_ThumbRight;
	bone_connect[15].parent = 13;

	//左脚
	bone_connect[16].bottom = JointType_SpineBase;
	bone_connect[16].top = JointType_HipLeft;
	bone_connect[16].parent = -1;//左脚を独立と考える．親はなし

	bone_connect[17].bottom = JointType_HipLeft;
	bone_connect[17].top = JointType_KneeLeft;
	bone_connect[17].parent = 16;

	bone_connect[18].bottom = JointType_KneeLeft;
	bone_connect[18].top = JointType_AnkleLeft;
	bone_connect[18].parent = 17;

	bone_connect[19].bottom = JointType_AnkleLeft;
	bone_connect[19].top = JointType_FootLeft;
	bone_connect[19].parent = 18;

	//右脚
	bone_connect[20].bottom = JointType_SpineBase;
	bone_connect[20].top = JointType_HipRight;
	bone_connect[20].parent = -1;//右脚を独立と考える．親はなし

	bone_connect[21].bottom = JointType_HipRight;
	bone_connect[21].top = JointType_KneeRight;
	bone_connect[21].parent = 20;

	bone_connect[22].bottom = JointType_KneeRight;
	bone_connect[22].top = JointType_AnkleRight;
	bone_connect[22].parent = 21;

	bone_connect[23].bottom = JointType_AnkleRight;
	bone_connect[23].top = JointType_FootRight;
	bone_connect[23].parent = 22;


	// テスト用出力
	for (int i = 0; i < BONES; i++){
	//	printf("Bone::define_bone_connect(): ボーン %d : ボトム %d　トップ %d\n",i, bone_connect[i].bottom, bone_connect[i].top);
	}
}
