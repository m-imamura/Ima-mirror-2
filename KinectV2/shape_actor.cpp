#include <iostream>
#include <sstream>
#include <atlbase.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <Kinect.h>

#define PEOPLE 6

// ���Ђł̉���̂��߂Ƀ}�N���ɂ��Ă��܂��B���ۂɂ͓W�J�����`�Ŏg�����Ƃ��������Ă��������B
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

	// shape��actor�̊֌W�����Z�b�g
	for (int body_num = 0; body_num < PEOPLE; body_num++){
		shape[body_num] = actor[body_num] = -1;
	}
	for (int body_num = 0; body_num < PEOPLE; body_num++){
		IBody* body = bodies[body_num];

		// Body���������I
		BOOLEAN isTracked = false;
		ERROR_CHECK(body->get_IsTracked(&isTracked));
		if (isTracked == true){
			//body�̍s����
			actor[body_num] = body_num;
		}
	}
	// ����ւ����ʂ��o��
	for (int body_num = 0; body_num < PEOPLE; body_num++){
		if (actor[body_num] != -1){
			printf("%d �� %d\n", body_num, actor[body_num]);
		}
	}
}

void ShapeActor::set_shape_actor_exchange(IBody* bodies[PEOPLE]){
	// shape��actor�̊֌W�����Z�b�g
	for (int body_num = 0; body_num < PEOPLE; body_num++){
		shape[body_num] = actor[body_num] = -1;
	}

	int first_actor = -1; // bodies[]�z��ɓ����Ă����ŏ��̐l�D���̐l�̓���͔z��̍Ō�ɓ����Ă����l�̌`�����蓖�Ă���
	int next_shape = -1; // ���̃A�N�^�[�����蓖�Ă���ׂ��`�̔ԍ�
	for (int body_num = 0; body_num < PEOPLE; body_num++){
		IBody* body = bodies[body_num];

		// Body���������I
		BOOLEAN isTracked = false;
		ERROR_CHECK(body->get_IsTracked(&isTracked));
		if (isTracked == true){
			// body�̍s����
			if (first_actor == -1){ // �ŏ���actor��ҋ@������
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

	// ����ւ����ʂ��o��
	for (int body_num = 0; body_num < PEOPLE; body_num++){
		if (actor[body_num] != -1){
			printf("%d �� %d\n", body_num, actor[body_num]);
		}
	}
}