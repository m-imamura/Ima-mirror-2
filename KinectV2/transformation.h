#include <Eigen/Core>		// 線形代数ライブラリ
#include <Eigen/Geometry>	// 外積の計算に必要

#pragma once

class transformation{
	private:

	public:
		void transrate(Eigen::Matrix4f &mat, const Eigen::Vector4f &t);
		void rotate(Eigen::Matrix4f &mat, const Eigen::Vector4f &u, const Eigen::Vector4f &v);
};