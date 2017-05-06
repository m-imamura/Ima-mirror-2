//ボーン1人分

#include <stdio.h>
#include <Eigen/Core>		// 線形代数ライブラリ
#include <Kinect.h>

#define JOINTS 25			// ジョイントの数
#define BONES 24			// 人体のボーンの数

class Bone{
private:

public:
	// ボーンの情報
	Eigen::Vector4f top_init[BONES];	//初期位置
	Eigen::Vector4f bottom_init[BONES];//初期位置
	Eigen::Vector4f vector_init[BONES];//初期方向
	Eigen::Vector4f top[BONES];		//逐次位置
	Eigen::Vector4f bottom[BONES];		//逐次位置
	Eigen::Vector4f vector[BONES];		//逐次方向 (bottom -> top)
	int parent[BONES];					//親ボーン (使ってない？)
	float length[BONES];				//長さ (vector_initの)

	class Bone_connect{ // ボーン１本分の接続関係
	private:
	public:
		int top;	//先端のジョイント番号
		int bottom;	//根元のジョイント番号
		int parent;	//親ボーン
		double impactrange; //ボーンの影響範囲

	};
	Bone_connect bone_connect[BONES];

	// 関数
	// ジョイントポジションをもらってボーン情報（自前）を更新する関数
	void set_bones_data(IBody* body);

	// ボーンの接続関係を定義する関数（インスタンス時に自動で呼ばれる↓）
	void define_bone_connect(Bone_connect bone_connect[BONES]);

};