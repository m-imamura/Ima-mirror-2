//�{�[��1�l��
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

#define JOINTS 25			// �W���C���g�̐�
#define BONES 24			// �l�̂̃{�[���̐�

#pragma once

class Bone{
private:

public:
	// �{�[���̏��
	Eigen::Vector4f top_init[BONES];	//�����ʒu
	Eigen::Vector4f bottom_init[BONES];//�����ʒu
	Eigen::Vector4f vector_init[BONES];//��������
	Eigen::Vector4f top[BONES];		//�����ʒu
	Eigen::Vector4f bottom[BONES];		//�����ʒu
	Eigen::Vector4f vector[BONES];		//�������� (bottom -> top)
	int parent[BONES];					//�e�{�[�� (�g���ĂȂ��H)
	float length[BONES];				//���� (vector_init��)

	class Bone_connect{ // �{�[���P�{���̐ڑ��֌W
	private:
	public:
		int top;	//��[�̃W���C���g�ԍ�
		int bottom;	//�����̃W���C���g�ԍ�
		int parent;	//�e�{�[��
		double impactrange; //�{�[���̉e���͈�

	};
	Bone_connect bone_connect[BONES];

	// �֐�
	// �W���C���g�|�W�V������������ă{�[�����i���O�j���X�V����֐�
	void set_bones_data(IBody* body);

	// �W���C���g�|�W�V������������ď����̃{�[�������i�[����֐�
	void set_bones_init_data(IBody* body);

	// �{�[���̐ڑ��֌W���`����֐��i�C���X�^���X���Ɏ����ŌĂ΂�遫�j
	void define_bone_connect(Bone_connect bone_connect[BONES]);

};