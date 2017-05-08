// Bone����points����������Human�N���X

#include <stdio.h>
#include <stdlib.h>
#include <Kinect.h>
#include <opencv2\opencv.hpp>

#include "bone.h"
#include "points.h"
#include "transformation.h"
#include "draw_ellipse.h"
#include "fullscreen_layout.h"

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
	void set_shape_points(CComPtr<IKinectSensor> kinect, int person,
		std::vector<UINT16> depthBuffer, int depthWidth, int depthHeight,
		std::vector<BYTE> bodyIndexBuffer, int bodyIndexWidth, int bodyIndexHeight,
		std::vector<BYTE> colorBuffer, int colorWidth, int colorHeight,
		unsigned int colorBytesPerPixel);

	//  �A�N�^�[�̍��i���Ŏ�����points_data��ϊ����鍪������
	cv::Mat get_translate_body(CComPtr<IKinectSensor> kinect, cv::Mat campus,
		FullscreenLayout fullscreen_layout, int bodyWidth, int bodyHeight);
};