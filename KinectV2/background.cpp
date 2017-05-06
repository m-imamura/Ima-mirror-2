
/*
#include <stdio.h>
#include <opencv2\opencv.hpp>
#include <Kinect.h>

class Background{
private:


public:
	void drawBackground(cv::Mat campus);// �w�i��`�悷��

	Background(){//�R���X�g���N�^
		
	}
};


void drawBackground(cv::Mat campus){
	
	// �w�i��Mat�ɕ`�悷��֐��ɂ������D���Ƃ͒���t���邾���ŃI�b�P�[�ɂ�����

	std::vector<ColorSpacePoint> colorSpace(depthWidth * depthHeight);

	// Depth���W�n�ɑΉ�����Color���W�n�̈ꗗ���擾����
	CComPtr<ICoordinateMapper> mapper;
	ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));
	mapper->MapDepthFrameToColorSpace(background_depthBuffer.size(), &background_depthBuffer[0], colorSpace.size(), &colorSpace[0]);

	for (int i = 0; i < depthHeight * depthWidth; i++){ // �w�i������
		if (i % 4 == 0){
			DepthSpacePoint depth_point; //Depth���W�n�̓_
			depth_point.X = i % depthWidth; //Depth�摜���x����
			depth_point.Y = i / depthWidth; //Depth�摜���y����

			ColorSpacePoint color_point; //Color���W�n�̓_
			//mapper->MapDepthPointToColorSpace(depth_point, depthBuffer[i], &color_point);
			mapper->MapDepthPointToColorSpace(depth_point, background_depthBuffer[i], &color_point);
			// ��mapper���m�I�DdepthBuffer�̐[�x�ɂ���ăJ���[�̃}�b�s���O�ʒu�����߂Ă���D�P���Ȉ������΂��ł͂Ȃ��D

			// colorSpace�̃C���f�b�N�X�����߂�
			int colorX = (int)color_point.X;
			int colorY = (int)color_point.Y;
			if ((colorX < 0) || (colorWidth <= colorX) || (colorY < 0) || (colorHeight <= colorY)){
				continue;
			}

			int colorIndex = (colorY * colorWidth) + colorX;

			int colorBufferIndex = colorIndex * colorBytesPerPixel;

			cv::Scalar color_point_color = cv::Scalar(backgroundBuffer[colorBufferIndex], backgroundBuffer[colorBufferIndex + 1], backgroundBuffer[colorBufferIndex + 2]);

			int fullsc_x = full_layout.magnification*(i % depthWidth) + full_layout.margin_x;
			int fullsc_y = full_layout.magnification*(i / depthWidth) + full_layout.margin_y;


			campus.at<cv::Vec3b>(fullsc_y, fullsc_x)[0] = backgroundBuffer[colorBufferIndex];
			campus.at<cv::Vec3b>(fullsc_y, fullsc_x)[1] = backgroundBuffer[colorBufferIndex + 1];
			campus.at<cv::Vec3b>(fullsc_y, fullsc_x)[2] = backgroundBuffer[colorBufferIndex + 2];


			// �t���X�N���[�����W�ɕϊ�
			cv::Point fullScreenPoint;
			fullScreenPoint.x = fullsc_x;
			fullScreenPoint.y = fullsc_y;

			cv::circle(campus, fullScreenPoint, 5, color_point_color, -1);

		}
	}
}*/