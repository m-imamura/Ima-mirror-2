// body�P���̕W�{�_

#include <stdio.h>
#include <Eigen/Core>		// ���`�㐔���C�u����
#include <opencv2\opencv.hpp>

#define POINTS_MAX 30000	// �`�悷��_�Q�̐�

class points{
private:
	Eigen::Vector4f points_init[POINTS_MAX];	//�����ʒu�̓_(Camera���W) �ő�POINTS_XYZ_NUM��
	Eigen::Vector4f points[POINTS_MAX];			//�ϊ���̓_(Camera���W) �ő�POINTS_XYZ_NUM��
	Eigen::Vector4f points_depth[POINTS_MAX];	//�ϊ���̓_(Depth���W)
	cv::Scalar color[POINTS_MAX];				//�F
	int points_num;								//�_�̑���

public:
	// getinit�����Ƃ��̃f�[�^��������ĕۑ����Ă������񂶂̊֐�
	void set_points_data();
};

void points::set_points_data(){
	int i = 100;
	return;
}