/*
#include <stdio.h>
#include <Eigen/Core>		// ���`�㐔���C�u����
#include <Eigen/Geometry>	// �O�ς̌v�Z�ɕK�v

class transformation{
private:

public:
	void transrate(Eigen::Matrix4f &mat, const Eigen::Vector4f &t);
	void rotate(Eigen::Matrix4f &mat, const Eigen::Vector4f &u, const Eigen::Vector4f &v);
};

// ���s�ړ��s����v�Z (�v�Z�����s��C���s�ړ��x�N�g��)
void transrate(Eigen::Matrix4f &mat, const Eigen::Vector4f &t){

	mat = Eigen::Matrix4f::Identity();
	mat.col(3) << t.x(), t.y(), t.z(), 1.0;

	return;
}

// ��]�ړ��s����v�Z (�v�Z�����s��C��]���̃x�N�g���C��]��̃x�N�g��)
void rotate(Eigen::Matrix4f &mat, const Eigen::Vector4f &u, const Eigen::Vector4f &v){

	// Vector4f �� u, v �� x�Cy�Cz �v�f��؂�o��
	Eigen::Vector3f u_v3, v_v3;
	u_v3 = u.segment(0, 3);// .segment�́C�u0 ���� 3 �v�f�����o���v�Ƃ����\�L
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