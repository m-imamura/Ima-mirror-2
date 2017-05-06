// Bone����points����������Human�N���X
/*
#include <stdio.h>
#include <stdlib.h>
#include <Kinect.h>
#include <opencv2\opencv.hpp>

#include "bone.h"
#include "points.h"
#include "transformation.h"
#include "draw_ellipse.h"


// ���Ђł̉���̂��߂Ƀ}�N���ɂ��Ă��܂��B���ۂɂ͓W�J�����`�Ŏg�����Ƃ��������Ă��������B
#define ERROR_CHECK(ret) \
if ((ret) != S_OK) { \
	std::stringstream ss; \
	ss << "failed " #ret " " << std::hex << ret << std::endl; \
	throw std::runtime_error(ss.str().c_str()); \
}
//////////////////////////////////////////////////


class Human{
private: 
	Bone shape_bone;                 // �������̓��I�m�ۂ����邩�H�H
	Bone actor_bone;
	Points shape_points;
	transformation trans;

public:
	// �f�[�^����֐�
	void set_shape_bone(IBody* body);
	void set_actor_bone(IBody* body);
	void set_shape_points(int person, ICoordinateMapper* mapper,
		std::vector<UINT16> depthBuffer, int depthWidth, int depthHeight,
		std::vector<BYTE> bodyIndexBuffer, int bodyIndexWidth, int bodyIndexHeight,
		std::vector<BYTE> colorBuffer, int colorWidth, int colorHeight,
		std::vector<ColorSpacePoint> colorSpace,
		unsigned int colorBytesPerPixel);

	//  �A�N�^�[�̍��i���Ŏ�����points_data��ϊ����鍪������
	cv::Mat get_translate_body(cv::Mat campus);
};



// �f�[�^����֐��i�����̉����������ǁj
void Human::set_shape_bone(IBody* body){
	shape_bone.set_bones_data(body);
}
void Human::set_actor_bone(IBody* body){
	actor_bone.set_bones_data(body);
}
void Human::set_shape_points(int person, ICoordinateMapper* mapper,
	std::vector<UINT16> depthBuffer, int depthWidth, int depthHeight,
	std::vector<BYTE> bodyIndexBuffer, int bodyIndexWidth, int bodyIndexHeight,
	std::vector<BYTE> colorBuffer, int colorWidth, int colorHeight,
	std::vector<ColorSpacePoint> colorSpace,
	unsigned int colorBytesPerPixel){
	shape_points.set_points_data(person, mapper, depthBuffer, depthWidth, depthHeight,
		bodyIndexBuffer, bodyIndexWidth, bodyIndexHeight, colorBuffer, colorWidth, colorHeight,
		colorSpace, colorBytesPerPixel);
}

// campus���󂯎���ĕϊ������̂��ڂ���campus��Ԃ��D
cv::Mat Human::get_translate_body(CComPtr<IKinectSensor> kinect, cv::Mat campus, ){

	// shape�̐g�̂�actor�̎p���̂Ƃ���bottom���v�Z
	Eigen::Vector4f new_bottom[BONES];
	for (int b = 0; b < BONES; b++){
		new_bottom[b] << 0.0, 0.0, 0.0, 1.0;
		int parent = shape_bone.bone_connect[b].parent; // �ڑ��֌W�Ȃ̂�actor_bone�ł�OK
		if (parent == -1){
			float y_diff = actor_bone.bottom_init[b].y() - shape_bone.bottom_init[b].y();
			new_bottom[b] = actor_bone.bottom[b];
			new_bottom[b].y() -= y_diff;
		}
		else {
			new_bottom[b].segment(0, 3)
				= new_bottom[parent].segment(0, 3)
				+ shape_bone.length[parent] * actor_bone.vector[parent].segment(0, 3).normalized();
			new_bottom[b].w() = 1.0;
		}
	}

	
	// new_bottom��`��
	//for (int b = 0; b < BONES; b++){
	//	mydrawEllipse(new_bone_bodyImage, new_bottom[b], 3, cv::Scalar(255, 255, 0));
	//
	//	Eigen::Vector4f new_top;
	//	new_top.segment(0, 3) = new_bottom[b].segment(0, 3) + bone_data[shape][b].length * bone_data[actor][b].vector.segment(0, 3).normalized();
	//	mydrawEllipse(new_bone_bodyImage, new_top, 3, cv::Scalar(255, 255, 0));
	//}

	// �_�Q�̕ϊ�
	for (int p = 0; p < shape_points.points_num; p++){

		// �_�̈ʒu��������
		shape_points.points[p] << 0.0, 0.0, 0.0, 0.0;

		for (int b = 0; b < BONES; b++){

			Eigen::Vector4f v1 = shape_bone.top_init[b] - shape_bone.bottom_init[b];// v1 : bottom ���� top
			Eigen::Vector4f v2 = shape_points.points_init[p] - shape_bone.bottom_init[b];// v2 : Bottom����_
			float t = v1.dot(v2) / v1.dot(v1); // t : �W�{�_����v1�ɐ������������_��v1������

			float d;
			float w;

			// t��[0~1]�ŃN�����v���� (d�N�����v�Ǝ�����������)
			if (t < 0){
				t = 0;
			}
			else if (t > 1){
				t = 1;
			}

			d = (v2 - v1 * t).norm(); // �{�[���ƕW�{�_�̋���
			w = pow(d + 1.0, -16); // ���̐ݒ���O�ɏo�����D�i�������e�͕ς��Ȃ��͂��D�j�t������Ȃ�����


			// �{�[���̉e���i�d�݁j
			Eigen::Matrix4f w_matrix;
			//w_matrix << w, w, w, 1.0; // ���܂������Ȃ��D���u�D

			if (d < v1.norm() * shape_bone.bone_connect[b].impactrange){ // �������ŁC�{�[�����Ƃɋ��e�͈͂�ς���Ƃ��܂����������H

				// �ϊ��s��̑S��
				Eigen::Matrix4f M_matrix;
				Eigen::Matrix4f M_inverse;
				Eigen::Matrix4f B_matrix;

				// �ϊ��s��̕���
				Eigen::Matrix4f M_transrate;
				Eigen::Matrix4f M_rotate;
				Eigen::Matrix4f B_transrate;
				Eigen::Matrix4f B_rotate;

				// ��{�̃x�N�g��
				Eigen::Vector4f unit_vector(0.0, 0.0, 1.0, 1.0);

				// �s�� M �̕��������߂�
				trans.transrate(M_transrate, shape_bone.bottom_init[b]);
				trans.rotate(M_rotate, unit_vector, shape_bone.vector_init[b]);

				// �����{�[���̃m���������߂�
				float vi_norm = shape_bone.vector_init[b].norm();

				// �s�� M �̑S�̂����߂�
				M_matrix = vi_norm * M_transrate * M_rotate;//
				M_inverse = M_matrix.inverse();//(1/vi_norm) * M_rotate * M_transrate;

				// �s�� B �̕��������߂�
				trans.rotate(B_rotate, unit_vector, actor_bone.vector[b]);
				trans.transrate(B_transrate, new_bottom[b]);

				// �ϊ���{�[���̃m���������߂�
				float v_norm = actor_bone.vector[b].norm();

				// �s�� B �̑S�̂����߂�
				B_matrix = vi_norm * B_transrate * B_rotate;

				// �ϊ���̓_�� b �ԃ{�[���̉e���𑫂����킹��
				shape_points.points[p] += w * B_matrix * M_inverse * shape_points.points_init[p];

			}
		}

		// Camera ���W�n�� Depth ���W�n�ɕϊ�
		CComPtr<ICoordinateMapper> mapper;
		ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));

		// Camera ���W�n�̓_�� Depth ���W�n�ɖ߂�
		CameraSpacePoint camera_point;
		camera_point.X = shape_points.points[p].x();
		camera_point.Y = shape_points.points[p].y();
		camera_point.Z = shape_points.points[p].z();
		//printf(" height_ratio %f \n", height_ratio);

		DepthSpacePoint depth_point;
		mapper->MapCameraPointToDepthSpace(camera_point, &depth_point);

		shape_points.points_depth[p].x() = depth_point.X;
		shape_points.points_depth[p].y() = depth_point.Y;
		shape_points.points_depth[p].z() = 255;

		// Depth ���W�n�̓_��V�������W�l�ɕϊ�
		int x = (int)shape_points.points_depth[p].x();
		int y = (int)shape_points.points_depth[p].y();
		int new_index = y*bodyIndexWidth + x;

		if (new_index >= 0 && new_index < bodyIndexHeight*bodyIndexWidth){//�͈͓��O����

			//�f�v�X�摜�ɓ_��`��
			//depthImage.data[new_index] = 255;//points_data1.points[j].z;

			//�t���X�N���[���ւ̉~�`��
			DrawEllipse draw_ellipse;
			draw_ellipse.drawEllipse_fullScreen(kinect, campus, camera_point, 5, shape_points.color[p],
				full_layout.magnification, full_layout.margin_x, full_layout.margin_y);
		}
		// ������������ƊȒP�ɏ�����͂�������]�T���������猩��
	}
	return campus;
}*/