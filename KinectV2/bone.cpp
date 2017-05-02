
//�{�[��1�l��

#include <stdio.h>
#include <Eigen/Core>		// ���`�㐔���C�u����
#include <Kinect.h>

#define JOINTS 25			// �W���C���g�̐�
#define BONES 24			// �l�̂̃{�[���̐�

class Bone{
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

	class Bone_connect{ // �{�[���P�{���̐ڑ��֌W
	private:
	public:
		int top;	//��[�̃W���C���g�ԍ�
		int bottom;	//�����̃W���C���g�ԍ�
		int parent;	//�e�{�[��
		double impactrange; //�{�[���̉e���͈�
	};
	Bone_connect bone_connect[BONES];

	// �֐��Q

	// �W���C���g�|�W�V������������ă{�[�����i���O�j���X�V����֐�
	void set_bones_data(IBody* body);

	// �{�[���̐ڑ��֌W���`����֐��i�C���X�^���X���Ɏ����ŌĂ΂�遫�j
	void define_bone_connect(Bone_connect bone_connect[BONES]);
	
	
	// �R���X�g���N�^
	Bone(){ 
		printf("Bone�N���X�̃R���X�g���N�^�ɓ�����"); // �����Ă��Ȃ�
		// �{�[���̐ڑ��֌W���`����D
		// define_bone_connect(bone_connect);
	}


};


void Bone::set_bones_data(IBody* body){ // bodies[6]�̂P�̕���������Ă��Ă���IBody��body

	Eigen::Vector4f joint_position[JOINTS]; //�֐߂̈ʒu

	//���ׂẴ{�[���ɂ��ČJ��Ԃ����� // ��d���[�v�������ł���H
	for (int i = 0; i < BONES; i++){

		// �֐߂̈ʒu���擾
		Joint joints[JointType::JointType_Count];
		body->GetJoints(JointType::JointType_Count, joints);
		for (auto joint : joints) {
			if (joint.TrackingState == TrackingState::TrackingState_Tracked) {
				//drawEllipse(bodyImage, joint, 3, cv::Scalar(255, 255, 255));
				joint_position[joint.JointType] << joint.Position.X, joint.Position.Y, joint.Position.Z, 1.0;
			}
		}

		// i�Ԗڂ̃{�[����top�Cbottom���ǂ̊֐߂������߂āC�֐߂̈ʒu������
		bottom[i] = joint_position[bone_connect[i].bottom];
		top[i] = joint_position[bone_connect[i].top];

		// i�Ԗڂ̃{�[����vector(�{�[���̌���)��top-bottom���狁�߂�
		vector[i].segment(0, 3) = top[i].segment(0, 3) - bottom[i].segment(0, 3);
		vector[i].w() = 1.0;
		vector[i].segment(0, 3) = vector[i].segment(0, 3).normalized();
	}

	printf("���{�[����x�ʒu %f\n", bottom[0].x());
}

void Bone::define_bone_connect(Bone::Bone_connect bone_connect[BONES]){
	//�̊�
	bone_connect[0].bottom = JointType_SpineBase;//�����̃W���C���g��ݒ�
	bone_connect[0].top = JointType_SpineMid;//��[�̃W���C���g��ݒ�
	bone_connect[0].parent = -1;//�e�̃{�[����ݒ�i���Έʒu�̊�j
	bone_connect[0].impactrange = 0.8;

	bone_connect[1].bottom = JointType_SpineMid;
	bone_connect[1].top = JointType_SpineShoulder;
	bone_connect[1].parent = 0;
	bone_connect[1].impactrange = 0.8;

	bone_connect[2].bottom = JointType_SpineShoulder;
	bone_connect[2].top = JointType_Neck;
	bone_connect[2].parent = 1;
	bone_connect[2].impactrange = 0.4;

	bone_connect[3].bottom = JointType_Neck;
	bone_connect[3].top = JointType_Head;
	bone_connect[3].parent = 2;
	bone_connect[3].impactrange = 1.0;

	//���r
	bone_connect[4].bottom = JointType_SpineShoulder;
	bone_connect[4].top = JointType_ShoulderLeft;
	bone_connect[4].parent = 1;//�}������ bone2�i��j�Ɠ�������
	//bone_connect[4] = 0.4;

	bone_connect[5].bottom = JointType_ShoulderLeft;
	bone_connect[5].top = JointType_ElbowLeft;
	bone_connect[5].parent = 4;
	//bone_connect[5].impactrange = 0.4;

	bone_connect[6].bottom = JointType_ElbowLeft;
	bone_connect[6].top = JointType_WristLeft;
	bone_connect[6].parent = 5;
	//bone_connect[6].impactrange = 0.4;

	bone_connect[7].bottom = JointType_WristLeft;
	bone_connect[7].top = JointType_HandLeft;
	bone_connect[7].parent = 6;
	//bone_connect[7].impactrange = 0.5;

	bone_connect[8].bottom = JointType_HandLeft;
	bone_connect[8].top = JointType_HandTipLeft;
	bone_connect[8].parent = 7;
	//bone_connect[8].impactrange = 0.4;

	bone_connect[9].bottom = JointType_HandLeft;
	bone_connect[9].top = JointType_ThumbLeft;
	bone_connect[9].parent = 7;
	//bone_connect[9].impactrange = 0.4;

	//�E�r
	bone_connect[10].bottom = JointType_SpineShoulder;
	bone_connect[10].top = JointType_ShoulderRight;
	bone_connect[10].parent = 1;//�}������ bone2�i��j�Ɠ�������

	bone_connect[11].bottom = JointType_ShoulderRight;
	bone_connect[11].top = JointType_ElbowRight;
	bone_connect[11].parent = 10;

	bone_connect[12].bottom = JointType_ElbowRight;
	bone_connect[12].top = JointType_WristRight;
	bone_connect[12].parent = 11;

	bone_connect[13].bottom = JointType_WristRight;
	bone_connect[13].top = JointType_HandRight;
	bone_connect[13].parent = 12;

	bone_connect[14].bottom = JointType_HandRight;
	bone_connect[14].top = JointType_HandTipRight;
	bone_connect[14].parent = 13;

	bone_connect[15].bottom = JointType_HandRight;
	bone_connect[15].top = JointType_ThumbRight;
	bone_connect[15].parent = 13;

	//���r
	bone_connect[16].bottom = JointType_SpineBase;
	bone_connect[16].top = JointType_HipLeft;
	bone_connect[16].parent = -1;//���r��Ɨ��ƍl����D�e�͂Ȃ�

	bone_connect[17].bottom = JointType_HipLeft;
	bone_connect[17].top = JointType_KneeLeft;
	bone_connect[17].parent = 16;

	bone_connect[18].bottom = JointType_KneeLeft;
	bone_connect[18].top = JointType_AnkleLeft;
	bone_connect[18].parent = 17;

	bone_connect[19].bottom = JointType_AnkleLeft;
	bone_connect[19].top = JointType_FootLeft;
	bone_connect[19].parent = 18;

	//�E�r
	bone_connect[20].bottom = JointType_SpineBase;
	bone_connect[20].top = JointType_HipRight;
	bone_connect[20].parent = -1;//�E�r��Ɨ��ƍl����D�e�͂Ȃ�

	bone_connect[21].bottom = JointType_HipRight;
	bone_connect[21].top = JointType_KneeRight;
	bone_connect[21].parent = 20;

	bone_connect[22].bottom = JointType_KneeRight;
	bone_connect[22].top = JointType_AnkleRight;
	bone_connect[22].parent = 21;

	bone_connect[23].bottom = JointType_AnkleRight;
	bone_connect[23].top = JointType_FootRight;
	bone_connect[23].parent = 22;


	// �e�X�g�p�o��
	for (int i = 0; i < BONES; i++){
		printf("�{�[�� %d : �{�g�� %d�@�g�b�v %d\n",i, bone_connect[i].bottom, bone_connect[i].top);
	}
}
