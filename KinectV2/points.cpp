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
	Eigen::Vector4f *points_init;	//�����ʒu�̓_(Camera���W) �ő�POINTS_XYZ_NUM��
	Eigen::Vector4f *points;		//�ϊ���̓_(Camera���W) �ő�POINTS_XYZ_NUM��
	Eigen::Vector4f *points_depth;	//�ϊ���̓_(Depth���W)
	cv::Scalar *color;				//�F
	int points_num;					//�_�̑���
	int body_num;					//�g�̔ԍ�

	//�֐�
	// �C���f�b�N�X��ʂɂ�����v���C���[�̃C���f�b�N�X�F�ݒ�@���ŏ��ɌĂяo��
	void set_players_index_color();

	// getinit�����Ƃ��̃f�[�^��������ĕۑ����Ă����֐�
	void set_points_data(int person, ICoordinateMapper* mapper,
		std::vector<UINT16> depthBuffer, int depthWidth, int depthHeight,
		std::vector<BYTE> bodyIndexBuffer, int bodyIndexWidth, int bodyIndexHeight,
		std::vector<BYTE> colorBuffer, int colorWidth, int colorHeight,
		std::vector<ColorSpacePoint> colorSpace,
		unsigned int colorBytesPerPixel);
};

void Points::set_players_index_color(){
	// �J���[��
	index_colors[0] = cv::Scalar(255, 0, 0);
	index_colors[1] = cv::Scalar(0, 255, 0);
	index_colors[2] = cv::Scalar(0, 0, 255);
	index_colors[3] = cv::Scalar(255, 255, 0);
	index_colors[4] = cv::Scalar(255, 0, 255);
	index_colors[5] = cv::Scalar(0, 255, 255);

	//�O���[�X�P�[����
	index_colors[0] = cv::Scalar(255, 255, 255);
	index_colors[1] = cv::Scalar(234, 234, 234);
	index_colors[2] = cv::Scalar(231, 213, 213);
	index_colors[3] = cv::Scalar(192, 192, 192);
	index_colors[4] = cv::Scalar(171, 171, 171);
	index_colors[5] = cv::Scalar(150, 150, 150);
}

void Points::set_points_data(int person, ICoordinateMapper* mapper,
	std::vector<UINT16> depthBuffer, int depthWidth, int depthHeight,
	std::vector<BYTE> bodyIndexBuffer, int bodyIndexWidth, int bodyIndexHeight,
	std::vector<BYTE> colorBuffer, int colorWidth, int colorHeight,
	std::vector<ColorSpacePoint> colorSpace,
	unsigned int colorBytesPerPixel){

	// �\���p���
	cv::Mat points_Image = cv::Mat::zeros(depthHeight, depthWidth, CV_8UC4);

	points_init = new Eigen::Vector4f[POINTS_MAX];
	points = new Eigen::Vector4f[POINTS_MAX];
	points_depth = new Eigen::Vector4f[POINTS_MAX];
	color = new cv::Scalar[POINTS_MAX];

	body_num = person; // �O������^����ꂽ�{�f�B�ԍ��������̃{�f�B�ԍ��Ƃ���D 
	
	// mapper�̕����͊O�ŁD
	points_num = 0;
	// DepthFrame���Ȃ߂Ȃ���g�̗̂̈���ɓ_�Q��z�u
	for (int i = 0; i < bodyIndexWidth * bodyIndexHeight; i++)
	{
		// i�Ԗڂ̉�f���g�̗̂̈���Ȃ�
		if (index_colors[bodyIndexBuffer[i]] == index_colors[person] && points_num < POINTS_MAX)
		{
			// i�Ԗڂ̉�f��x�Cy���W���擾
			int x = i % bodyIndexWidth;
			int y = i / bodyIndexWidth;

			// Depth���W�n�̓_�Q��Camera���W�n�̓_�Q�ɕϊ����Ĕz�u
			if (x % 4 == 0 && y % 4 == 0){

				DepthSpacePoint depth_point; //Depth���W�n�̓_
				depth_point.X = x; //Depth�摜���x����
				depth_point.Y = y; //Depth�摜���y����

				CameraSpacePoint camera_point; //Camera���W�n�̓_
				mapper->MapDepthPointToCameraSpace(depth_point, depthBuffer[i], &camera_point);

				// colorSpace�̃C���f�b�N�X�����߂�
				int colorX = (int)colorSpace[i].X;
				int colorY = (int)colorSpace[i].Y;
				if ((colorX < 0) || (colorWidth <= colorX) || (colorY < 0) || (colorHeight <= colorY)){
					continue;
				}
				int colorIndex = (colorY * colorWidth) + colorX;

				int colorBufferIndex = colorIndex * colorBytesPerPixel;

				cv::Scalar color_point_color = cv::Scalar(colorBuffer[colorBufferIndex], colorBuffer[colorBufferIndex + 1], colorBuffer[colorBufferIndex + 2]);
				points_init[points_num] << camera_point.X, camera_point.Y, camera_point.Z, 1.0;
				color[points_num] = color_point_color;
				points_num++;

				// �Ƃ����|�C���g����ʂɃJ���[�ŏo��
				points_Image.data[i*4 + 0] = colorBuffer[colorBufferIndex];
				points_Image.data[i*4 + 1] = colorBuffer[colorBufferIndex+1];
				points_Image.data[i*4 + 2] = colorBuffer[colorBufferIndex+2];

			}
		}
	}
	std::cout << "body_num " <<body_num << ": Points_num " << points_num << "\n";

	cv::imshow("Points_Image", points_Image);
}