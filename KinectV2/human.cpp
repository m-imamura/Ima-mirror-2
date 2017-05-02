/*
// Bone情報とpoints情報を持ったHumanクラス
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
	//  アクターの骨格情報で自分のpoints_dataを変換する根幹処理
	points get_translate_body(bone actor_bone);
};

points human::get_translate_body(bone actor_bone){

	// shapeの身体でactorの姿勢のときのbottomを計算
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

	// new_bottomを描画
	for (int b = 0; b < BONES; b++){
		//mydrawEllipse(new_bone_bodyImage, new_bottom[b], 3, cv::Scalar(255, 255, 0));

		// 親指，指先，つま先，頭の場合
		//if (b == 3){
		Eigen::Vector4f new_top;
		new_top.segment(0, 3) = new_bottom[b].segment(0, 3) + bone_data[shape][b].length * bone_data[actor][b].vector.segment(0, 3).normalized();
		//mydrawEllipse(new_bone_bodyImage, new_top, 3, cv::Scalar(255, 255, 0));
		//}
	}

	// 点群の変換
	for (int p = 0; p < points_data[shape].points_num; p++){

		// 点の位置を初期化
		points_data[shape].points[p] << 0.0, 0.0, 0.0, 0.0;

		for (int b = 0; b < BONES; b++){

			Eigen::Vector4f v1 = bone_data[shape][b].top_init - bone_data[shape][b].bottom_init;// v1 : bottom から top
			Eigen::Vector4f v2 = points_data[shape].points_init[p] - bone_data[shape][b].bottom_init;// v2 : Bottomから点
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

			if (d < v1.norm() * impact_range.range[b]){ // ←ここで，ボーンごとに許容範囲を変えるとうまくいきそう？

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
				trans.transrate(M_transrate, bone_data[shape][b].bottom_init);
				trans.rotate(M_rotate, unit_vector, bone_data[shape][b].vector_init);

				// 初期ボーンのノルムを求める
				float vi_norm = bone_data[shape][b].vector_init.norm();

				// 行列 M の全体を求める
				M_matrix = vi_norm * M_transrate * M_rotate;//
				M_inverse = M_matrix.inverse();//(1/vi_norm) * M_rotate * M_transrate;

				// 行列 B の部分を求める
				trans.rotate(B_rotate, unit_vector, bone_data[actor][b].vector);
				trans.transrate(B_transrate, new_bottom[b]);

				// 変換後ボーンのノルムを求める
				float v_norm = bone_data[actor][b].vector.norm();

				// 行列 B の全体を求める
				B_matrix = vi_norm * B_transrate * B_rotate;

				// 変換後の点に b 番ボーンの影響を足し合わせる
				points_data[shape].points[p] += w * B_matrix * M_inverse * points_data[shape].points_init[p];

			}
		}

		// Camera 座標系を Depth 座標系に変換
		CComPtr<ICoordinateMapper> mapper;
		ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));

		// Camera 座標系の点を Depth 座標系に戻す
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

		// Depth 座標系の点を新しい座標値に変換
		int x = (int)points_data[shape].points_depth[p].x();
		int y = (int)points_data[shape].points_depth[p].y();
		int new_index = y*bodyIndexWidth + x;

		if (new_index >= 0 && new_index < bodyIndexHeight*bodyIndexWidth){//範囲内外判定

			//デプス画像に点を描画
			//depthImage.data[new_index] = 255;//points_data1.points[j].z;

			//フルスクリーンへの円描画
			drawEllipse_fullScreen(Imamirror2_full, camera_point, ellipse_r, points_data[shape].color[p],
				full_layout.magnification, full_layout.margin_x, full_layout.margin_y);
		}
		// ↑もうちょっと簡単に書けるはずだから余裕があったら見る

	}

}
*/