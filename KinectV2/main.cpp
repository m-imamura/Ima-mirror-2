#include <iostream>
#include <sstream>
#include <atlbase.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <Eigen/Core>		// ���`�㐔���C�u����
#include <Eigen/Geometry>	// �O�ς̌v�Z�ɕK�v
#include <Kinect.h>
#include <opencv2\opencv.hpp>
#include <time.h> // �������̕\���p

#include "transformation.h"
#include "draw_ellipse.h"
#include "shape_actor.h"
#include "fullscreen_layout.h"
#include "Human.h"


// ���Ђł̉���̂��߂Ƀ}�N���ɂ��Ă��܂��B���ۂɂ͓W�J�����`�Ŏg�����Ƃ��������Ă��������B
#define ERROR_CHECK(ret) \
if ((ret) != S_OK) { \
	std::stringstream ss; \
	ss << "failed " #ret " " << std::hex << ret << std::endl; \
	throw std::runtime_error(ss.str().c_str()); \
}

class ImamirrorApp{
private:

	// �v���p�e�B�̒�`
	//////////////////////////////////////////////////
	CComPtr<IKinectSensor> kinect = nullptr;

	// Body�֘A
	CComPtr<IBodyFrameReader> bodyFrameReader = nullptr;
	IBody* bodies[PEOPLE];	// �f�[�^���擾����l��

	// BodyIndex�֘A
	CComPtr<IBodyIndexFrameReader> bodyIndexFrameReader = nullptr;
	std::vector<BYTE> bodyIndexBuffer;	// �擾����BodyIndex���i�[����o�b�t�@
	int bodyIndexWidth;
	int bodyIndexHeight;

	// Depth�֘A
	CComPtr<IDepthFrameReader> depthFrameReader = nullptr;
	std::vector<UINT16> depthBuffer;	// �擾����Depth���i�[����o�b�t�@
	std::vector<UINT16> background_depthBuffer;//�w�i��Depth���i�[����o�b�t�@
	int depthWidth;
	int depthHeight;

	// Color�֘A
	CComPtr<IColorFrameReader> colorFrameReader = nullptr;
	std::vector<BYTE> colorBuffer;// �\������
	int colorWidth;
	int colorHeight;
	unsigned int colorBytesPerPixel;
	ColorImageFormat colorFormat = ColorImageFormat::ColorImageFormat_Bgra;

	// �\�����[�h�֌W
	bool partner_change = true;// ���҂Ɠ���ւ������Ɠ���ւ���
	bool color_view_tf = false;// �J���[���������D
	bool background_tf = false;// �w�i����Ȃ�
	bool get_background = false;// �w�i���擾�����݂��ĂȂ���

	// �w�i��ۑ�����Mat
	std::vector<BYTE> backgroundBuffer; // std::vector<BYTE> colorBuffer; �Ɠ���

	Human *human;// �l�̃f�[�^
	ShapeActor shape_actor;// �����֌W
	FullscreenLayout full_layout_test;// �t���X�N���[���̃f�[�^

public:

	// �f�X�g���N�^
	~ImamirrorApp()
	{
		if (kinect != nullptr){
			kinect->Close();
		}
	}

	// �֐�
	void run();
	void update();
	void draw();
	void initialize();
	void getInitData();
	void updateBodyFrame();
	void updateBodyIndexFrame();
	void updateDepthFrame();
	void updateColorFrame();
	void drawBodyFrame();
	void drawBodyIndexFrame();
	void drawDepthFrame();
	void drawColorFrame();
	void drawImamirror2();

	//void mouseCallback(int event, int x, int y, int flags, void*);


};


// �f�[�^���X�V���ĕ`��
void ImamirrorApp::run()
{
	while (true) {
		update(); //BodyFrame�CBodyIndexFrame�CDepthFrame�����ꂼ��X�V
		draw(); //BodyFrame�CBodyIndexFrame�CDepthFrame�����ꂼ��`��

		auto key = cv::waitKey(10);
		if (key == 'u') { // �g�̌`����Ƃ�
			getInitData();
		}
		if (key == 'q') {
			break;
		}
		if (key == 'o'){ // �����Ɠ���ւ��C���҂Ɠ���ւ���؂�ւ���
			//partner_or_own();
		}
		if (key == 'b'){
			backgroundBuffer = colorBuffer; // �J���[�o�b�t�@�[�̎擾
			background_depthBuffer = depthBuffer; // �f�v�X�o�b�t�@�[�̎擾
			get_background = true;
			std::cout << "�w�i���擾\n";
		}
		if (key == 'v'){
			if (get_background){ // ���ł��w�i�擾���Ă��Ȃ��Ɩ���
				if (background_tf){
					background_tf = false;
					std::cout << "�w�i���\��\n";
				}
				else{
					background_tf = true;
					std::cout << "�w�i��\��\n";
				}
			}
		}
		if (key == 'c'){
			if (color_view_tf){
				color_view_tf = false;
				std::cout << "�l�����\��\n";
			}
			else{
				color_view_tf = true;
				std::cout << "�l���J���[�\��\n";
			}
		}
	}
}


// �f�[�^�X�V�����̌Ăяo��
void ImamirrorApp::update()
{
	updateBodyFrame();
	updateBodyIndexFrame();
	updateDepthFrame();
	updateColorFrame();
}

// �`�揈���̌Ăяo��
void ImamirrorApp::draw()
{
	drawBodyFrame();
	drawBodyIndexFrame();
	drawDepthFrame();
	drawColorFrame();
	drawImamirror2();
}


// ������
void ImamirrorApp::initialize()
{
	// �f�t�H���g��Kinect���擾����
	ERROR_CHECK(::GetDefaultKinectSensor(&kinect));
	ERROR_CHECK(kinect->Open());

	// Body�֘A�̏�����
	CComPtr<IBodyFrameSource> bodyFrameSource;
	ERROR_CHECK(kinect->get_BodyFrameSource(&bodyFrameSource));
	ERROR_CHECK(bodyFrameSource->OpenReader(&bodyFrameReader));
	for (auto& body : bodies){
		body = nullptr;
	}

	// BodyIndex�֌W�̏�����
	CComPtr<IBodyIndexFrameSource> bodyIndexFrameSource;
	ERROR_CHECK(kinect->get_BodyIndexFrameSource(&bodyIndexFrameSource));
	ERROR_CHECK(bodyIndexFrameSource->OpenReader(&bodyIndexFrameReader));
	// BodyIndex�摜�̃T�C�Y���擾����
	CComPtr<IFrameDescription> bodyIndexFrameDescription;
	ERROR_CHECK(bodyIndexFrameSource->get_FrameDescription(&bodyIndexFrameDescription));
	ERROR_CHECK(bodyIndexFrameDescription->get_Width(&bodyIndexWidth));
	ERROR_CHECK(bodyIndexFrameDescription->get_Height(&bodyIndexHeight));
	// �o�b�t�@�[���쐬����
	bodyIndexBuffer.resize(bodyIndexWidth * bodyIndexHeight);

	// Depth�֘A�̏�����
	CComPtr<IDepthFrameSource> depthFrameSource;
	ERROR_CHECK(kinect->get_DepthFrameSource(&depthFrameSource));
	ERROR_CHECK(depthFrameSource->OpenReader(&depthFrameReader));
	// Depth�摜�̃T�C�Y���擾����
	CComPtr<IFrameDescription> depthFrameDescription;
	ERROR_CHECK(depthFrameSource->get_FrameDescription(&depthFrameDescription));
	ERROR_CHECK(depthFrameDescription->get_Width(&depthWidth));
	ERROR_CHECK(depthFrameDescription->get_Height(&depthHeight));
	// Depth�̍ő�l�A�ŏ��l���擾����
	UINT16 minDepthReliableDistance;
	UINT16 maxDepthReliableDistance;
	ERROR_CHECK(depthFrameSource->get_DepthMinReliableDistance(&minDepthReliableDistance));
	ERROR_CHECK(depthFrameSource->get_DepthMaxReliableDistance(&maxDepthReliableDistance));
	std::cout << "Depth�ŏ��l : " << minDepthReliableDistance << std::endl;
	std::cout << "Depth�ő�l : " << maxDepthReliableDistance << std::endl;
	// �o�b�t�@�[���쐬����
	depthBuffer.resize(depthWidth * depthHeight);


	// Color�֌W�̏�����
	// �J���[���[�_�[���擾����
	CComPtr<IColorFrameSource> colorFrameSource;
	ERROR_CHECK(kinect->get_ColorFrameSource(&colorFrameSource));
	ERROR_CHECK(colorFrameSource->OpenReader(&colorFrameReader));

	// �f�t�H���g�̃J���[�摜�̃T�C�Y���擾����
	CComPtr<IFrameDescription> defaultColorFrameDescription;
	ERROR_CHECK(colorFrameSource->get_FrameDescription(&defaultColorFrameDescription));
	ERROR_CHECK(defaultColorFrameDescription->get_Width(&colorWidth));
	ERROR_CHECK(defaultColorFrameDescription->get_Height(&colorHeight));
	ERROR_CHECK(defaultColorFrameDescription->get_BytesPerPixel(&colorBytesPerPixel));
	std::cout << "default : " << colorWidth << ", " << colorHeight << ", " << colorBytesPerPixel << std::endl;

	// �J���[�摜�̃T�C�Y���擾����
	CComPtr<IFrameDescription> colorFrameDescription;
	ERROR_CHECK(colorFrameSource->CreateFrameDescription(colorFormat, &colorFrameDescription));
	ERROR_CHECK(colorFrameDescription->get_Width(&colorWidth));
	ERROR_CHECK(colorFrameDescription->get_Height(&colorHeight));
	ERROR_CHECK(colorFrameDescription->get_BytesPerPixel(&colorBytesPerPixel));
	std::cout << "create  : " << colorWidth << ", " << colorHeight << ", " << colorBytesPerPixel << std::endl;

	// �o�b�t�@�[���쐬����
	colorBuffer.resize(colorWidth * colorHeight * colorBytesPerPixel);


	// �}�E�X�N���b�N�̃C�x���g��o�^����
	// �t���X�N���[����ʂ��N���b�N���ē���ւ���D
	//cv::namedWindow("Ima-mirror2_full");
	//cv::setMouseCallback("Ima-mirror2_full", &ImamirrorApp::mouseCallback);

	// shape��actor�̊֌W�����Z�b�g
	// ����͂��Ƃ���getInitData()���ł���Ă����񂾂��ǁCmain()��getInitData()������ɂ���
	// ���̊֐���(initialize())�ł��Ă����ƁC�l�����Ȃ��Ă��N���ł���Ƃ����s���ł����ɏ�����D
	for (int body_num = 0; body_num < PEOPLE; body_num++){
		shape_actor.shape[body_num] = shape_actor.actor[body_num] = -1;
	}

	human = new Human[PEOPLE];

	full_layout_test.set_fullscreen_layout(depthWidth, depthHeight);

	std::cout << "initialize";

}

/*
// �}�E�X�C�x���g�̃R�[���o�b�N  Imamirror2��ʂŏ����ʒu���Ƃ邽�߂̃}�E�X����
void ImamirrorApp::mouseCallback(int event, int x, int y, int flags, void*)
{
	if (event == CV_EVENT_LBUTTONDOWN) {
		getInitData();
	}
}
*/
// �ϊ����̃f�[�^���擾
void ImamirrorApp::getInitData()
{
	// ������ԋL�^�p
	cv::Mat init_bodyImage = cv::Mat::zeros(424, 512, CV_8UC4);
	cv::Mat init_depthImage = cv::Mat::zeros(depthHeight, depthWidth, CV_8UC1);

	while (true) {

		// �ϊ����̃f�[�^���擾�����v���C���[�̔ԍ�
		int gotBody = -1;


		// BodyFrame�̎擾//////////////////////////////////////////////////
		// �t���[�����擾����
		CComPtr<IBodyFrame> bodyFrame;
		auto ret1 = bodyFrameReader->AcquireLatestFrame(&bodyFrame);
		if (FAILED(ret1)){
			continue;
		}

		// �O���Body���������
		for (int body_num = 0; body_num < PEOPLE; body_num++){
			if (bodies[body_num] != nullptr){
				bodies[body_num]->Release();
				bodies[body_num] = nullptr;
			}
		}

		// Bodies�f�[�^���擾����
		ERROR_CHECK(bodyFrame->GetAndRefreshBodyData(PEOPLE, &bodies[0]));


		// BodyIndexFrame�̎擾//////////////////////////////////////////////////
		// �t���[�����擾����
		CComPtr<IBodyIndexFrame> bodyIndexFrame;
		auto ret2 = bodyIndexFrameReader->AcquireLatestFrame(&bodyIndexFrame);
		if (FAILED(ret2)){
			continue;
		}

		// bodyIndex�f�[�^���擾����
		ERROR_CHECK(bodyIndexFrame->CopyFrameDataToArray(bodyIndexBuffer.size(), &bodyIndexBuffer[0]));


		// DepthFrame�̎擾//////////////////////////////////////////////////
		// �t���[�����擾����
		CComPtr<IDepthFrame> depthFrame;
		auto ret3 = depthFrameReader->AcquireLatestFrame(&depthFrame);
		if (FAILED(ret3)){
			continue;
		}

		// Depth�f�[�^���擾����
		ERROR_CHECK(depthFrame->CopyFrameDataToArray(depthBuffer.size(), &depthBuffer[0]));


		// �����֌W�����߂�
		if (partner_change){
			shape_actor.set_shape_actor_exchange(bodies);
		}
		else{
			shape_actor.set_shape_actor_own(bodies);
		}

		// ����bone���擾����
		// ���ׂĂ�Body�ɂ��ČJ��Ԃ� //bone�̏��������擾
		for (int body_num = 0; body_num < PEOPLE; body_num++){
			IBody* body = bodies[body_num];

			// Body���Ȃ�������I���
			if (body == nullptr){
				continue;
			}

			// �ǐՂł��ĂȂ�������I���
			BOOLEAN isTracked = false;
			ERROR_CHECK(body->get_IsTracked(&isTracked));
			if (!isTracked) {
				continue;
			}

			// ��ŋ��߂��A�N�^�[�̃{�f�B�ԍ����擾
			int actor_body = shape_actor.actor[body_num];

			human[body_num].set_shape_bone(body);
			//human[body_num].set_actor_bone(bodies[actor_body]); // ���ꂢ��H

			gotBody = body_num;
		}

		// �����_�Q���擾����
		if (gotBody != -1) { // body���P�ł����Ă�����
			for (int person = 0; person < PEOPLE; person++){

				// Body���Ȃ�������I���
				if (bodies[person] == nullptr){
					continue;
				}

				// �ǐՂł��ĂȂ�������I���
				BOOLEAN isTracked = false;
				ERROR_CHECK(bodies[person]->get_IsTracked(&isTracked));
				if (!isTracked) {
					continue;
				}

				human[person].set_shape_points(kinect, person,
					depthBuffer, depthWidth, depthHeight,
					bodyIndexBuffer, bodyIndexWidth, bodyIndexHeight,
					colorBuffer, colorWidth, colorHeight, colorBytesPerPixel);
			}
		}

		// �ϊ����̃f�[�^���擾�ł��Ă���I���
		if (gotBody != -1) {
			break;
		}

	}

	// ������Ԃ�`��
	//cv::imshow("init_BodyImage", init_bodyImage);
	//cv::imshow("init_DepthImage", init_depthImage);

}



// BodyFrame�̍X�V
void ImamirrorApp::updateBodyFrame()
{
	// �t���[�����擾����
	CComPtr<IBodyFrame> bodyFrame;
	auto ret = bodyFrameReader->AcquireLatestFrame(&bodyFrame);
	if (FAILED(ret)){
		return;
	}

	// �O���Body���������
	for (auto& body : bodies){
		if (body != nullptr){
			body->Release();
			body = nullptr;
		}
	}

	// �f�[�^���擾����
	ERROR_CHECK(bodyFrame->GetAndRefreshBodyData(PEOPLE, &bodies[0]));
}

// BodyIndexFrame�̍X�V
void ImamirrorApp::updateBodyIndexFrame()
{
	// �t���[�����擾����
	CComPtr<IBodyIndexFrame> bodyIndexFrame;
	auto ret = bodyIndexFrameReader->AcquireLatestFrame(&bodyIndexFrame);
	if (FAILED(ret)){
		return;
	}

	// �f�[�^���擾����
	ERROR_CHECK(bodyIndexFrame->CopyFrameDataToArray(bodyIndexBuffer.size(), &bodyIndexBuffer[0]));
}

// DepthFrame�̍X�V
void ImamirrorApp::updateDepthFrame()
{
	// �t���[�����擾����
	CComPtr<IDepthFrame> depthFrame;
	auto ret = depthFrameReader->AcquireLatestFrame(&depthFrame);
	if (FAILED(ret)){
		return;
	}

	// �f�[�^���擾����
	ERROR_CHECK(depthFrame->CopyFrameDataToArray(depthBuffer.size(), &depthBuffer[0]));


}

// ColorFrame�̍X�V
void ImamirrorApp::updateColorFrame()
{
	// �t���[�����擾����
	CComPtr<IColorFrame> colorFrame;
	auto ret = colorFrameReader->AcquireLatestFrame(&colorFrame);
	if (FAILED(ret)){
		return;
	}

	// �w��̌`���Ńf�[�^���擾����
	ERROR_CHECK(colorFrame->CopyConvertedFrameDataToArray(
		colorBuffer.size(), &colorBuffer[0], colorFormat));
}

// BodyFrame�̕`��
void ImamirrorApp::drawBodyFrame()
{
	// �֐߂̈ʒu��Depth���W�n�ɕϊ����ĕ\������
	cv::Mat bodyImage = cv::Mat::zeros(424, 512, CV_8UC4);

	// ���ׂĂ�Body�ɂ��ČJ��Ԃ��`��
	for (int body = 0; body < PEOPLE; body++){

		// Body���Ȃ�������I���
		if (bodies[body] == nullptr){
			continue;
		}

		// �ǐՂł��ĂȂ�������I���
		BOOLEAN isTracked = false;
		ERROR_CHECK(bodies[body]->get_IsTracked(&isTracked));
		if (!isTracked) {
			continue;
		}

		// ���̕Ӄe�X�g
		int actor_body = shape_actor.actor[body];
		//human[body].set_actor_bone(bodies[actor_body]);
		

		// �|�[�Y�F��
		//check_base_posture(bone_data[body]);
	}

	// �\��
	cv::imshow("BodyImage", bodyImage);
}

// BodyIndexFrame�̕`��
void ImamirrorApp::drawBodyIndexFrame()
{
	/*
	// BodyIndex���J���[�f�[�^�ɕϊ����ĕ\������
	cv::Mat bodyIndexImage(bodyIndexHeight, bodyIndexWidth, CV_8UC4);

	for (int i = 0; i < bodyIndexWidth * bodyIndexHeight; ++i){
		int index = i * 4;

		// BodyIndexBuffer��255�ȊO�Ȃ��(=�l�������)
		if (bodyIndexBuffer[i] != 255){
			auto color = colors[bodyIndexBuffer[i]];
			if (bodyIndexBuffer[i] == bib_player){
				color = bib_player_color;
			}
			bodyIndexImage.data[index + 0] = color[0];
			bodyIndexImage.data[index + 1] = color[1];
			bodyIndexImage.data[index + 2] = color[2];

		}
		// �l�����Ȃ����
		else{
			bodyIndexImage.data[index + 0] = 0;
			bodyIndexImage.data[index + 1] = 0;
			bodyIndexImage.data[index + 2] = 0;
		}
	}*/

	// �\��
	//cv::imshow("BodyIndexImage", bodyIndexImage);


}

// DepthFrame�̕`��
void ImamirrorApp::drawDepthFrame()
{
	// Depth�C���[�W������Ă���D�����������Ă���
	cv::Mat depthImage = cv::Mat::zeros(depthHeight, depthWidth, CV_8UC1);

	// �ʏ�\��
	//cv::imshow("DepthImage", depthImage);

}

// �J���[�f�[�^�̕\������
void ImamirrorApp::drawColorFrame()
{
#if 0
	// �J���[�f�[�^��\������
	cv::Mat colorImage(colorHeight, colorWidth, CV_8UC4, &colorBuffer[0]);
	cv::imshow("Color Image", colorImage);
#else
	cv::Mat colorImage(colorHeight, colorWidth, CV_8UC4, &colorBuffer[0]);
	cv::Mat harfImage;
	cv::resize(colorImage, harfImage, cv::Size(), 0.5, 0.5);
	cv::imshow("Harf Image", harfImage);


#endif
}

void ImamirrorApp::drawImamirror2(){

	
	cv::Mat Imamirror2_full = cv::Mat::zeros(full_layout_test.DisplayHeight, full_layout_test.DisplayWidth, CV_8UC3);
	// ��CV_8UC4��CV_8UC3�ɂ�����C���C����ʂɉ摜��\���悤�ɂȂ����D

	//drawBackground(Imamirror2_full);

	/*
	// ���ׂĂ�Body�ɂ��ČJ��Ԃ��`��
	for (int body = 0; body < PEOPLE; body++){
		if (shape_actor.actor[body] == -1){ // ����actor�����Ȃ�������

			// Body���Ȃ�������I���
			if (bodies[body] == nullptr){
				continue;
			}

			// �ǐՂł��ĂȂ�������I���
			BOOLEAN isTracked = false;
			ERROR_CHECK(bodies[body]->get_IsTracked(&isTracked));
			if (!isTracked) {
				continue;
			}

			// �����ō����������Ăяo��
			//human[body].get_translate_body(kinect, Imamirror2_full, full_layout_test, bodyIndexWidth, bodyIndexHeight);
		}
	}
	*/
	// �t���X�N���[���\��
	cvNamedWindow("Ima-mirror2_full");
	cv::setWindowProperty("Ima-mirror2_full", cv::WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN); // �t���X�N���[���ݒ�
	cv::imshow("Ima-mirror2_full-----", Imamirror2_full); // �\��
	
	//cv::imshow("new_body_", new_bone_bodyImage);

}


int main(int argc, char *argv[])
{
	std::cout << "main�Ђ炢�Ă��";
	
	try {
		ImamirrorApp app;		// �C���X�^���X�𐶐����āC
		app.initialize();	// ���낢�돉�������āC
		//app.getInitData();	// �ϊ����̃f�[�^���擾���āC
		app.run();			// �Ђ����烋�[�v����
	}
	catch (std::exception& ex){
		std::cout << ex.what() << std::endl;
	}

	return 0;
}