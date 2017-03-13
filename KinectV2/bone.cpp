/*
//�{�[��1�l��

#include <stdio.h>
#include <Eigen/Core>		// ���`�㐔���C�u����
#include <Kinect.h>

#define JOINTS 25			// �W���C���g�̐�
#define BONES 24			// �l�̂̃{�[���̐�

class bone{
private:
	// �{�[���̏��
	Eigen::Vector4f top_init[BONES];	//�����ʒu
	Eigen::Vector4f bottom_init[BONES];//�����ʒu
	Eigen::Vector4f vector_init[BONES];//��������
	Eigen::Vector4f top[BONES];		//�����ʒu
	Eigen::Vector4f bottom[BONES];		//�����ʒu
	Eigen::Vector4f vector[BONES];		//�������� (bottom -> top)
	int parent[BONES];					//�e�{�[�� (�g���ĂȂ��H)
	float length[BONES];				//���� (vector_init��)

public:
	// �W���C���g�|�W�V������������ă{�[�������X�V���邩�񂶂̊֐�
	void set_bone_data(Joint joints);
};

void bone::set_bone_data(Joint joints){

	Eigen::Vector4f joint_position[JOINTS]; //�֐߂̈ʒu

	for (auto joint : joints) {
		if (joint.TrackingState == TrackingState::TrackingState_Tracked) {
			joint_position[joint.JointType] << joint.Position.X, joint.Position.Y, joint.Position.Z, 1.0;
		}
	}

	// Body������΂��ׂẴ{�[���ɂ��ČJ��Ԃ�����
	for (int i = 0; i < BONES; i++){

		// i�Ԗڂ̃{�[����top�Cbottom���ǂ̊֐߂������߂āC�֐߂̈ʒu������
		bottom_init[i] = joint_position[bone_set[i].bottom];
		top_init[i] = joint_position[bone_set[i].top];

		// i�Ԗڂ̃{�[����vector(�{�[���̌���)��top-bottom���狁�߂�
		vector_init[i].segment(0, 3) = top_init[i].segment(0, 3) - bottom_init[i].segment(0, 3);
		vector_init[i].w() = 1.0;

		length[i] = vector_init[i].segment(0, 3).norm();
		vector_init[i].segment(0, 3) = vector_init[i].segment(0, 3).normalized();
	}

}

*/