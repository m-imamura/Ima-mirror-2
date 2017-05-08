// Bone情報とpoints情報を持ったHumanクラス

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
	Bone shape_bone;                 // メモリの動的確保がいるか？？
	Bone actor_bone;
	Points shape_points;
	transformation trans;

public:
	// データ入れ関数
	void set_shape_bone(IBody* body);
	void set_actor_bone(IBody* body);
	void set_shape_points(CComPtr<IKinectSensor> kinect, int person,
		std::vector<UINT16> depthBuffer, int depthWidth, int depthHeight,
		std::vector<BYTE> bodyIndexBuffer, int bodyIndexWidth, int bodyIndexHeight,
		std::vector<BYTE> colorBuffer, int colorWidth, int colorHeight,
		unsigned int colorBytesPerPixel);

	//  アクターの骨格情報で自分のpoints_dataを変換する根幹処理
	cv::Mat get_translate_body(CComPtr<IKinectSensor> kinect, cv::Mat campus,
		FullscreenLayout fullscreen_layout, int bodyWidth, int bodyHeight);
};