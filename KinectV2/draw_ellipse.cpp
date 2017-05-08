// �~�`��@�\

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

#include "fullscreen_layout.h"

// ���Ђł̉���̂��߂Ƀ}�N���ɂ��Ă��܂��B���ۂɂ͓W�J�����`�Ŏg�����Ƃ��������Ă��������B
#define ERROR_CHECK(ret) \
	if ((ret) != S_OK) { \
		std::stringstream ss; \
		ss << "failed " #ret " " << std::hex << ret << std::endl; \
		throw std::runtime_error(ss.str().c_str()); \
	}
	//////////////////////////////////////////////////

class DrawEllipse{
private:

public:
	void drawEllipse(CComPtr<IKinectSensor> kinect, cv::Mat &bodyImage, const Joint &joint, int r, const cv::Scalar &color);
	void mydrawEllipse(CComPtr<IKinectSensor> kinect, cv::Mat &bodyImage, const Eigen::Vector4f &joint, int r, const cv::Scalar &color);
	void drawEllipse_fullScreen(
		CComPtr<IKinectSensor> kinect,
		cv::Mat &campus,
		CameraSpacePoint &cam_point,
		int r, const cv::Scalar &color,
		FullscreenLayout full_layout);
	void draw_circle(
		CComPtr<IKinectSensor> kinect,
		cv::Mat &bodyImage,
		DepthSpacePoint depth_point,
		int r, const cv::Scalar &color,
		FullscreenLayout full_layout);
};



// DepthImage��ɉ~��`�� (DepthImage�CCamera���W�n�̕`��ʒu�C�`�攼�a�C�`��F)
void DrawEllipse::drawEllipse(CComPtr<IKinectSensor> kinect, cv::Mat &bodyImage, const Joint &joint, int r, const cv::Scalar &color)
{
	// Camera���W�n��Depth���W�n�ɕϊ�����
	CComPtr<ICoordinateMapper> mapper;
	ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));
	DepthSpacePoint point;
	mapper->MapCameraPointToDepthSpace(joint.Position, &point);

	cv::circle(bodyImage, cv::Point(point.X, point.Y), r, color, -1);
}

// Eigen�ŁFDepthImage��ɉ~��`�� (DepthImage�CCamera���W�n�̕`��ʒu�C�`�攼�a�C�`��F)
void DrawEllipse::mydrawEllipse(CComPtr<IKinectSensor> kinect, cv::Mat &bodyImage, const Eigen::Vector4f &joint, int r, const cv::Scalar &color)
{
	// Camera���W�n��Depth���W�n�ɕϊ�����
	CComPtr<ICoordinateMapper> mapper;
	ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));

	DepthSpacePoint point;
	CameraSpacePoint joint3;
	joint3.X = joint.x();
	joint3.Y = joint.y();
	joint3.Z = joint.z();
	mapper->MapCameraPointToDepthSpace(joint3, &point);

	cv::circle(bodyImage, cv::Point(point.X, point.Y), r, color, -1);
}

// �t���X�N���[���p��drawWllipse
void DrawEllipse::drawEllipse_fullScreen(
	CComPtr<IKinectSensor> kinect,
	cv::Mat &campus,
	CameraSpacePoint &cam_point,
	int r, const cv::Scalar &color,
	FullscreenLayout full_layout)
{
	// Camera���W�n��Depth���W�n�ɕϊ�����
	CComPtr<ICoordinateMapper> mapper;
	ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));

	DepthSpacePoint point;
	mapper->MapCameraPointToDepthSpace(cam_point, &point);

	// �������܂�mydrawEllipse�Ɠ��l�i�ϐ����Ƃ��Ⴄ���ǁj

	// �t���X�N���[�����W�ɕϊ�
	cv::Point fullScreenPoint;
	fullScreenPoint.x = (int)((double)point.X*full_layout.magnification + full_layout.margin_x);
	fullScreenPoint.y = point.Y*full_layout.magnification + full_layout.margin_y;

	cv::circle(campus, fullScreenPoint, r, color, -1);
}

void DrawEllipse::draw_circle(
	CComPtr<IKinectSensor> kinect,
	cv::Mat &bodyImage,
	DepthSpacePoint depth_point,
	int r, const cv::Scalar &color,
	FullscreenLayout full_layout){

	// �t���X�N���[�����W�ɕϊ�
	cv::Point fullScreenPoint;
	fullScreenPoint.x = depth_point.X*full_layout.magnification + full_layout.margin_x;
	fullScreenPoint.y = depth_point.Y*full_layout.magnification + full_layout.margin_y;

	cv::circle(bodyImage, fullScreenPoint, r, color, -1);
}