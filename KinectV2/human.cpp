// Bone情報とpoints情報を持ったHumanクラス
/*
#include <stdio.h>
#include <stdlib.h>
#include <Kinect.h>
#include <opencv2\opencv.hpp>

#include "bone.h"
#include "points.h"
#include "transformation.h"
#include "draw_ellipse.h"


// 書籍での解説のためにマクロにしています。実際には展開した形で使うことを検討してください。
#define ERROR_CHECK(ret) \
if ((ret) != S_OK) { \
	std::stringstream ss; \
	ss << "failed " #ret " " << std::hex << ret << std::endl; \
	throw std::runtime_error(ss.str().c_str()); \
}
//////////////////////////////////////////////////


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
	void set_shape_points(int person, ICoordinateMapper* mapper,
		std::vector<UINT16> depthBuffer, int depthWidth, int depthHeight,
		std::vector<BYTE> bodyIndexBuffer, int bodyIndexWidth, int bodyIndexHeight,
		std::vector<BYTE> colorBuffer, int colorWidth, int colorHeight,
		std::vector<ColorSpacePoint> colorSpace,
		unsigned int colorBytesPerPixel);

	//  アクターの骨格情報で自分のpoints_dataを変換する根幹処理
	cv::Mat get_translate_body(cv::Mat campus);
};



// データ入れ関数（ただの横流しだけど）
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

// campusを受け取って変換した体を載せてcampusを返す．
cv::Mat Human::get_translate_body(CComPtr<IKinectSensor> kinect, cv::Mat campus, ){

	// shapeの身体でactorの姿勢のときのbottomを計算
	Eigen::Vector4f new_bottom[BONES];
	for (int b = 0; b < BONES; b++){
		new_bottom[b] << 0.0, 0.0, 0.0, 1.0;
		int parent = shape_bone.bone_connect[b].parent; // 接続関係なのでactor_boneでもOK
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

	
	// new_bottomを描画
	//for (int b = 0; b < BONES; b++){
	//	mydrawEllipse(new_bone_bodyImage, new_bottom[b], 3, cv::Scalar(255, 255, 0));
	//
	//	Eigen::Vector4f new_top;
	//	new_top.segment(0, 3) = new_bottom[b].segment(0, 3) + bone_data[shape][b].length * bone_data[actor][b].vector.segment(0, 3).normalized();
	//	mydrawEllipse(new_bone_bodyImage, new_top, 3, cv::Scalar(255, 255, 0));
	//}

	// 点群の変換
	for (int p = 0; p < shape_points.points_num; p++){

		// 点の位置を初期化
		shape_points.points[p] << 0.0, 0.0, 0.0, 0.0;

		for (int b = 0; b < BONES; b++){

			Eigen::Vector4f v1 = shape_bone.top_init[b] - shape_bone.bottom_init[b];// v1 : bottom から top
			Eigen::Vector4f v2 = shape_points.points_init[p] - shape_bone.bottom_init[b];// v2 : Bottomから点
			float t = v1.dot(v2) / v1.dot(v1); // t : 標本点からv1に垂線を下した点のv1内分比

			float d;
			float w;

			// tを[0~1]でクランプする (dクランプと実質同じ処理)
			if (t < 0){
				t = 0;
			}
			else if (t > 1){
				t = 1;
			}

			d = (v2 - v1 * t).norm(); // ボーンと標本点の距離
			w = pow(d + 1.0, -16); // ｗの設定も外に出した．（処理内容は変わらないはず．）逆数じゃなくした


			// ボーンの影響（重み）
			Eigen::Matrix4f w_matrix;
			//w_matrix << w, w, w, 1.0; // うまくいかない．放置．

			if (d < v1.norm() * shape_bone.bone_connect[b].impactrange){ // ←ここで，ボーンごとに許容範囲を変えるとうまくいきそう？

				// 変換行列の全体
				Eigen::Matrix4f M_matrix;
				Eigen::Matrix4f M_inverse;
				Eigen::Matrix4f B_matrix;

				// 変換行列の部分
				Eigen::Matrix4f M_transrate;
				Eigen::Matrix4f M_rotate;
				Eigen::Matrix4f B_transrate;
				Eigen::Matrix4f B_rotate;

				// 基本のベクトル
				Eigen::Vector4f unit_vector(0.0, 0.0, 1.0, 1.0);

				// 行列 M の部分を求める
				trans.transrate(M_transrate, shape_bone.bottom_init[b]);
				trans.rotate(M_rotate, unit_vector, shape_bone.vector_init[b]);

				// 初期ボーンのノルムを求める
				float vi_norm = shape_bone.vector_init[b].norm();

				// 行列 M の全体を求める
				M_matrix = vi_norm * M_transrate * M_rotate;//
				M_inverse = M_matrix.inverse();//(1/vi_norm) * M_rotate * M_transrate;

				// 行列 B の部分を求める
				trans.rotate(B_rotate, unit_vector, actor_bone.vector[b]);
				trans.transrate(B_transrate, new_bottom[b]);

				// 変換後ボーンのノルムを求める
				float v_norm = actor_bone.vector[b].norm();

				// 行列 B の全体を求める
				B_matrix = vi_norm * B_transrate * B_rotate;

				// 変換後の点に b 番ボーンの影響を足し合わせる
				shape_points.points[p] += w * B_matrix * M_inverse * shape_points.points_init[p];

			}
		}

		// Camera 座標系を Depth 座標系に変換
		CComPtr<ICoordinateMapper> mapper;
		ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));

		// Camera 座標系の点を Depth 座標系に戻す
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

		// Depth 座標系の点を新しい座標値に変換
		int x = (int)shape_points.points_depth[p].x();
		int y = (int)shape_points.points_depth[p].y();
		int new_index = y*bodyIndexWidth + x;

		if (new_index >= 0 && new_index < bodyIndexHeight*bodyIndexWidth){//範囲内外判定

			//デプス画像に点を描画
			//depthImage.data[new_index] = 255;//points_data1.points[j].z;

			//フルスクリーンへの円描画
			DrawEllipse draw_ellipse;
			draw_ellipse.drawEllipse_fullScreen(kinect, campus, camera_point, 5, shape_points.color[p],
				full_layout.magnification, full_layout.margin_x, full_layout.margin_y);
		}
		// ↑もうちょっと簡単に書けるはずだから余裕があったら見る
	}
	return campus;
}*/