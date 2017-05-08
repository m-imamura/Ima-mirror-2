#include <iostream>
#include <sstream>
#include <atlbase.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <Kinect.h>

#define PEOPLE 6

// 書籍での解説のためにマクロにしています。実際には展開した形で使うことを検討してください。
#define ERROR_CHECK(ret) \
if ((ret) != S_OK) { \
	std::stringstream ss; \
	ss << "failed " #ret " " << std::hex << ret << std::endl; \
	throw std::runtime_error(ss.str().c_str()); \
}

class ShapeActor{
private:

public:
	int shape[PEOPLE];
	int actor[PEOPLE];

	void set_shape_actor_own(IBody* bodies[PEOPLE]);
	void set_shape_actor_exchange(IBody* bodies[PEOPLE]);
};

void ShapeActor::set_shape_actor_own(IBody* bodies[PEOPLE]){

	// shapeとactorの関係をリセット
	for (int body_num = 0; body_num < PEOPLE; body_num++){
		shape[body_num] = actor[body_num] = -1;
	}
	for (int body_num = 0; body_num < PEOPLE; body_num++){
		IBody* body = bodies[body_num];

		// Bodyがあった！
		BOOLEAN isTracked = false;
		ERROR_CHECK(body->get_IsTracked(&isTracked));
		if (isTracked == true){
			//bodyの行き先
			actor[body_num] = body_num;
		}
	}
	// 入れ替え結果を出力
	for (int body_num = 0; body_num < PEOPLE; body_num++){
		if (actor[body_num] != -1){
			printf("%d → %d\n", body_num, actor[body_num]);
		}
	}
}

void ShapeActor::set_shape_actor_exchange(IBody* bodies[PEOPLE]){
	// shapeとactorの関係をリセット
	for (int body_num = 0; body_num < PEOPLE; body_num++){
		shape[body_num] = actor[body_num] = -1;
	}

	int first_actor = -1; // bodies[]配列に入っていた最初の人．この人の動作は配列の最後に入っていた人の形が割り当てられる
	int next_shape = -1; // 次のアクターが割り当てられるべき形の番号
	for (int body_num = 0; body_num < PEOPLE; body_num++){
		IBody* body = bodies[body_num];

		// Bodyがあった！
		BOOLEAN isTracked = false;
		ERROR_CHECK(body->get_IsTracked(&isTracked));
		if (isTracked == true){
			// bodyの行き先
			if (first_actor == -1){ // 最初のactorを待機させる
				first_actor = body_num;
			}
			else{
				actor[next_shape] = body_num;
			}
			next_shape = body_num;
		}
	}
	if (first_actor != -1){
		actor[next_shape] = first_actor;
	}

	// 入れ替え結果を出力
	for (int body_num = 0; body_num < PEOPLE; body_num++){
		if (actor[body_num] != -1){
			printf("%d → %d\n", body_num, actor[body_num]);
		}
	}
}