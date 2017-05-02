/*
// Bone����points����������Human�N���X
#include <stdio.h>
#include <Kinect.h>

#include "bone_setting.h"
#include "bone.h"
#include "points.h"
#include "transformation.h"


class human{
private:
	bone bones_data;
	points points_data;
	bone_setting bone_setting_data;
	transformation trans;

public:
	//  �A�N�^�[�̍��i���Ŏ�����points_data��ϊ����鍪������
	points get_translate_body(bone actor_bone);
};

points human::get_translate_body(bone actor_bone){

	// shape�̐g�̂�actor�̎p���̂Ƃ���bottom���v�Z
	Eigen::Vector4f new_bottom[BONES];
	for (int b = 0; b < BONES; b++){
		new_bottom[b] << 0.0, 0.0, 0.0, 1.0;
		int parent = bone_set[b].parent;
		if (parent == -1){
			float y_diff = bone_data[actor][b].bottom_init.y() - bone_data[shape][b].bottom_init.y();
			new_bottom[b] = bone_data[actor][b].bottom;
			new_bottom[b].y() -= y_diff;
		}
		else {
			new_bottom[b].segment(0, 3)
				= new_bottom[parent].segment(0, 3)
				+ bone_data[shape][parent].length * bone_data[actor][parent].vector.segment(0, 3).normalized();
			new_bottom[b].w() = 1.0;
		}
	}

	// new_bottom��`��
	for (int b = 0; b < BONES; b++){
		//mydrawEllipse(new_bone_bodyImage, new_bottom[b], 3, cv::Scalar(255, 255, 0));

		// �e�w�C�w��C�ܐ�C���̏ꍇ
		//if (b == 3){
		Eigen::Vector4f new_top;
		new_top.segment(0, 3) = new_bottom[b].segment(0, 3) + bone_data[shape][b].length * bone_data[actor][b].vector.segment(0, 3).normalized();
		//mydrawEllipse(new_bone_bodyImage, new_top, 3, cv::Scalar(255, 255, 0));
		//}
	}

	// �_�Q�̕ϊ�
	for (int p = 0; p < points_data[shape].points_num; p++){

		// �_�̈ʒu��������
		points_data[shape].points[p] << 0.0, 0.0, 0.0, 0.0;

		for (int b = 0; b < BONES; b++){

			Eigen::Vector4f v1 = bone_data[shape][b].top_init - bone_data[shape][b].bottom_init;// v1 : bottom ���� top
			Eigen::Vector4f v2 = points_data[shape].points_init[p] - bone_data[shape][b].bottom_init;// v2 : Bottom����_
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

			if (d < v1.norm() * impact_range.range[b]){ // �������ŁC�{�[�����Ƃɋ��e�͈͂�ς���Ƃ��܂����������H

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
				trans.transrate(M_transrate, bone_data[shape][b].bottom_init);
				trans.rotate(M_rotate, unit_vector, bone_data[shape][b].vector_init);

				// �����{�[���̃m���������߂�
				float vi_norm = bone_data[shape][b].vector_init.norm();

				// �s�� M �̑S�̂����߂�
				M_matrix = vi_norm * M_transrate * M_rotate;//
				M_inverse = M_matrix.inverse();//(1/vi_norm) * M_rotate * M_transrate;

				// �s�� B �̕��������߂�
				trans.rotate(B_rotate, unit_vector, bone_data[actor][b].vector);
				trans.transrate(B_transrate, new_bottom[b]);

				// �ϊ���{�[���̃m���������߂�
				float v_norm = bone_data[actor][b].vector.norm();

				// �s�� B �̑S�̂����߂�
				B_matrix = vi_norm * B_transrate * B_rotate;

				// �ϊ���̓_�� b �ԃ{�[���̉e���𑫂����킹��
				points_data[shape].points[p] += w * B_matrix * M_inverse * points_data[shape].points_init[p];

			}
		}

		// Camera ���W�n�� Depth ���W�n�ɕϊ�
		CComPtr<ICoordinateMapper> mapper;
		ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));

		// Camera ���W�n�̓_�� Depth ���W�n�ɖ߂�
		CameraSpacePoint camera_point;
		camera_point.X = points_data[shape].points[p].x();
		camera_point.Y = points_data[shape].points[p].y();
		camera_point.Z = points_data[shape].points[p].z();
		//printf(" height_ratio %f \n", height_ratio);

		DepthSpacePoint depth_point;
		mapper->MapCameraPointToDepthSpace(camera_point, &depth_point);

		points_data[shape].points_depth[p].x() = depth_point.X;
		points_data[shape].points_depth[p].y() = depth_point.Y;
		points_data[shape].points_depth[p].z() = 255;

		// Depth ���W�n�̓_��V�������W�l�ɕϊ�
		int x = (int)points_data[shape].points_depth[p].x();
		int y = (int)points_data[shape].points_depth[p].y();
		int new_index = y*bodyIndexWidth + x;

		if (new_index >= 0 && new_index < bodyIndexHeight*bodyIndexWidth){//�͈͓��O����

			//�f�v�X�摜�ɓ_��`��
			//depthImage.data[new_index] = 255;//points_data1.points[j].z;

			//�t���X�N���[���ւ̉~�`��
			drawEllipse_fullScreen(Imamirror2_full, camera_point, ellipse_r, points_data[shape].color[p],
				full_layout.magnification, full_layout.margin_x, full_layout.margin_y);
		}
		// ������������ƊȒP�ɏ�����͂�������]�T���������猩��

	}

}
*/