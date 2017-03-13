//�{�[��1�l��

#include <stdio.h>
#include <Eigen/Core>		// ���`�㐔���C�u����

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