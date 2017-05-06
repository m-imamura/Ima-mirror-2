// body�P���̕W�{�_

#include <iostream>
#include <sstream>

#include <atlbase.h>
#include <Eigen/Core>		// ���`�㐔���C�u����
#include <Eigen/Geometry>	// �O�ς̌v�Z�ɕK�v
#include <Kinect.h>
#include <opencv2\opencv.hpp>

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define POINTS_MAX 10000	// �`�悷��_�Q�̐�
#define PEOPLE 6			// �l��

class Points{
private:

	cv::Scalar index_colors[PEOPLE];	// �v���[���[�̐F���i�[

public:
	// �ϐ�
	Eigen::Vector4f points_init[POINTS_MAX];	//�����ʒu�̓_(Camera���W) �ő�POINTS_XYZ_NUM��
	Eigen::Vector4f points[POINTS_MAX];			//�ϊ���̓_(Camera���W) �ő�POINTS_XYZ_NUM��
	Eigen::Vector4f points_depth[POINTS_MAX];	//�ϊ���̓_(Depth���W)
	cv::Scalar color[POINTS_MAX];				//�F
	int points_num;								//�_�̑���
	int body_num;								//�g�̔ԍ�

	// �֐�
	// �C���f�b�N�X��ʂɂ�����v���C���[�̃C���f�b�N�X�F�ݒ�@���ŏ��ɌĂяo��
	void set_players_index_color();

	// getinit�����Ƃ��̃f�[�^��������ĕۑ����Ă������񂶂̊֐�
	void set_points_data(int person, ICoordinateMapper* mapper,
		std::vector<UINT16> depthBuffer, int depthWidth, int depthHeight,
		std::vector<BYTE> bodyIndexBuffer, int bodyIndexWidth, int bodyIndexHeight,
		std::vector<BYTE> colorBuffer, int colorWidth, int colorHeight,
		std::vector<ColorSpacePoint> colorSpace,
		unsigned int colorBytesPerPixel);
};