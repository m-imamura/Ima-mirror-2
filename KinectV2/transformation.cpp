/*
#include <stdio.h>
#include <Eigen/Core>		// 線形代数ライブラリ
#include <Eigen/Geometry>	// 外積の計算に必要

class transformation{
private:

public:
	void transrate(Eigen::Matrix4f &mat, const Eigen::Vector4f &t);
	void rotate(Eigen::Matrix4f &mat, const Eigen::Vector4f &u, const Eigen::Vector4f &v);
};

// 平行移動行列を計算 (計算される行列，平行移動ベクトル)
void transrate(Eigen::Matrix4f &mat, const Eigen::Vector4f &t){

	mat = Eigen::Matrix4f::Identity();
	mat.col(3) << t.x(), t.y(), t.z(), 1.0;

	return;
}

// 回転移動行列を計算 (計算される行列，回転元のベクトル，回転後のベクトル)
void rotate(Eigen::Matrix4f &mat, const Eigen::Vector4f &u, const Eigen::Vector4f &v){

	// Vector4f の u, v の x，y，z 要素を切り出す
	Eigen::Vector3f u_v3, v_v3;
	u_v3 = u.segment(0, 3);// .segmentは，「0 から 3 つ要素を取り出す」という表記
	v_v3 = v.segment(0, 3);

	Eigen::Vector3f n = u_v3.cross(v_v3).normalized();
	Eigen::Vector3f l = u_v3.cross(n).normalized();
	Eigen::Vector3f m = v_v3.cross(n).normalized();

	Eigen::Matrix3f mv;
	mv.col(0) = v_v3;
	mv.col(1) = n;
	mv.col(2) = m;

	Eigen::Matrix3f mu;
	mu.col(0) = u_v3;
	mu.col(1) = n;
	mu.col(2) = l;

	Eigen::Matrix3f mu_t = mu.transpose();
	Eigen::Matrix3f mat_v3 = mv * mu_t;

	mat.block(0, 0, 3, 3) = mat_v3;
	mat.row(3) << 0.0, 0.0, 0.0, 1.0;
	mat.col(3) << 0.0, 0.0, 0.0, 1.0;

	return;
}
*/