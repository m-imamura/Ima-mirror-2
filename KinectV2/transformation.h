#include <Eigen/Core>		// ���`�㐔���C�u����
#include <Eigen/Geometry>	// �O�ς̌v�Z�ɕK�v

#pragma once

class transformation{
	private:

	public:
		void transrate(Eigen::Matrix4f &mat, const Eigen::Vector4f &t);
		void rotate(Eigen::Matrix4f &mat, const Eigen::Vector4f &u, const Eigen::Vector4f &v);
};