#include <iostream>
#include <sstream>
#include <atlbase.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <Eigen/Core>		// 線形代数ライブラリ
#include <Eigen/Geometry>	// 外積の計算に必要
#include <Kinect.h>
#include <opencv2\opencv.hpp>
#include <time.h> // 現時刻の表示用

#include "transformation.h"
#include "draw_ellipse.h"
#include "shape_actor.h"
#include "fullscreen_layout.h"
#include "Human.h"


// 書籍での解説のためにマクロにしています。実際には展開した形で使うことを検討してください。
#define ERROR_CHECK(ret) \
if ((ret) != S_OK) { \
	std::stringstream ss; \
	ss << "failed " #ret " " << std::hex << ret << std::endl; \
	throw std::runtime_error(ss.str().c_str()); \
}

class ImamirrorApp{
private:

	// プロパティの定義
	//////////////////////////////////////////////////
	CComPtr<IKinectSensor> kinect = nullptr;

	// Body関連
	CComPtr<IBodyFrameReader> bodyFrameReader = nullptr;
	IBody* bodies[PEOPLE];	// データを取得する人数

	// BodyIndex関連
	CComPtr<IBodyIndexFrameReader> bodyIndexFrameReader = nullptr;
	std::vector<BYTE> bodyIndexBuffer;	// 取得したBodyIndexを格納するバッファ
	int bodyIndexWidth;
	int bodyIndexHeight;

	// Depth関連
	CComPtr<IDepthFrameReader> depthFrameReader = nullptr;
	std::vector<UINT16> depthBuffer;	// 取得したDepthを格納するバッファ
	std::vector<UINT16> background_depthBuffer;//背景のDepthを格納するバッファ
	int depthWidth;
	int depthHeight;

	// Color関連
	CComPtr<IColorFrameReader> colorFrameReader = nullptr;
	std::vector<BYTE> colorBuffer;// 表示部分
	int colorWidth;
	int colorHeight;
	unsigned int colorBytesPerPixel;
	ColorImageFormat colorFormat = ColorImageFormat::ColorImageFormat_Bgra;

	// 表示モード関係
	bool partner_change = true;// 他者と入れ替え可自分と入れ替えか
	bool color_view_tf = false;// カラーか白黒か．
	bool background_tf = false;// 背景あるなし
	bool get_background = false;// 背景を取得した貸してないか

	// 背景を保存するMat
	std::vector<BYTE> backgroundBuffer; // std::vector<BYTE> colorBuffer; と同じ

	Human *human;// 人のデータ
	ShapeActor shape_actor;// 交換関係
	FullscreenLayout full_layout_test;// フルスクリーンのデータ

public:

	// デストラクタ
	~ImamirrorApp()
	{
		if (kinect != nullptr){
			kinect->Close();
		}
	}

	// 関数
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


// データを更新して描画
void ImamirrorApp::run()
{
	while (true) {
		update(); //BodyFrame，BodyIndexFrame，DepthFrameをそれぞれ更新
		draw(); //BodyFrame，BodyIndexFrame，DepthFrameをそれぞれ描画

		auto key = cv::waitKey(10);
		if (key == 'u') { // 身体形状をとる
			getInitData();
		}
		if (key == 'q') {
			break;
		}
		if (key == 'o'){ // 自分と入れ替え，他者と入れ替えを切り替える
			//partner_or_own();
		}
		if (key == 'b'){
			backgroundBuffer = colorBuffer; // カラーバッファーの取得
			background_depthBuffer = depthBuffer; // デプスバッファーの取得
			get_background = true;
			std::cout << "背景を取得\n";
		}
		if (key == 'v'){
			if (get_background){ // 一回でも背景取得していないと無効
				if (background_tf){
					background_tf = false;
					std::cout << "背景を非表示\n";
				}
				else{
					background_tf = true;
					std::cout << "背景を表示\n";
				}
			}
		}
		if (key == 'c'){
			if (color_view_tf){
				color_view_tf = false;
				std::cout << "人物白表示\n";
			}
			else{
				color_view_tf = true;
				std::cout << "人物カラー表示\n";
			}
		}
	}
}


// データ更新処理の呼び出し
void ImamirrorApp::update()
{
	updateBodyFrame();
	updateBodyIndexFrame();
	updateDepthFrame();
	updateColorFrame();
}

// 描画処理の呼び出し
void ImamirrorApp::draw()
{
	drawBodyFrame();
	drawBodyIndexFrame();
	drawDepthFrame();
	drawColorFrame();
	drawImamirror2();
}


// 初期化
void ImamirrorApp::initialize()
{
	// デフォルトのKinectを取得する
	ERROR_CHECK(::GetDefaultKinectSensor(&kinect));
	ERROR_CHECK(kinect->Open());

	// Body関連の初期化
	CComPtr<IBodyFrameSource> bodyFrameSource;
	ERROR_CHECK(kinect->get_BodyFrameSource(&bodyFrameSource));
	ERROR_CHECK(bodyFrameSource->OpenReader(&bodyFrameReader));
	for (auto& body : bodies){
		body = nullptr;
	}

	// BodyIndex関係の初期化
	CComPtr<IBodyIndexFrameSource> bodyIndexFrameSource;
	ERROR_CHECK(kinect->get_BodyIndexFrameSource(&bodyIndexFrameSource));
	ERROR_CHECK(bodyIndexFrameSource->OpenReader(&bodyIndexFrameReader));
	// BodyIndex画像のサイズを取得する
	CComPtr<IFrameDescription> bodyIndexFrameDescription;
	ERROR_CHECK(bodyIndexFrameSource->get_FrameDescription(&bodyIndexFrameDescription));
	ERROR_CHECK(bodyIndexFrameDescription->get_Width(&bodyIndexWidth));
	ERROR_CHECK(bodyIndexFrameDescription->get_Height(&bodyIndexHeight));
	// バッファーを作成する
	bodyIndexBuffer.resize(bodyIndexWidth * bodyIndexHeight);

	// Depth関連の初期化
	CComPtr<IDepthFrameSource> depthFrameSource;
	ERROR_CHECK(kinect->get_DepthFrameSource(&depthFrameSource));
	ERROR_CHECK(depthFrameSource->OpenReader(&depthFrameReader));
	// Depth画像のサイズを取得する
	CComPtr<IFrameDescription> depthFrameDescription;
	ERROR_CHECK(depthFrameSource->get_FrameDescription(&depthFrameDescription));
	ERROR_CHECK(depthFrameDescription->get_Width(&depthWidth));
	ERROR_CHECK(depthFrameDescription->get_Height(&depthHeight));
	// Depthの最大値、最小値を取得する
	UINT16 minDepthReliableDistance;
	UINT16 maxDepthReliableDistance;
	ERROR_CHECK(depthFrameSource->get_DepthMinReliableDistance(&minDepthReliableDistance));
	ERROR_CHECK(depthFrameSource->get_DepthMaxReliableDistance(&maxDepthReliableDistance));
	std::cout << "Depth最小値 : " << minDepthReliableDistance << std::endl;
	std::cout << "Depth最大値 : " << maxDepthReliableDistance << std::endl;
	// バッファーを作成する
	depthBuffer.resize(depthWidth * depthHeight);


	// Color関係の初期化
	// カラーリーダーを取得する
	CComPtr<IColorFrameSource> colorFrameSource;
	ERROR_CHECK(kinect->get_ColorFrameSource(&colorFrameSource));
	ERROR_CHECK(colorFrameSource->OpenReader(&colorFrameReader));

	// デフォルトのカラー画像のサイズを取得する
	CComPtr<IFrameDescription> defaultColorFrameDescription;
	ERROR_CHECK(colorFrameSource->get_FrameDescription(&defaultColorFrameDescription));
	ERROR_CHECK(defaultColorFrameDescription->get_Width(&colorWidth));
	ERROR_CHECK(defaultColorFrameDescription->get_Height(&colorHeight));
	ERROR_CHECK(defaultColorFrameDescription->get_BytesPerPixel(&colorBytesPerPixel));
	std::cout << "default : " << colorWidth << ", " << colorHeight << ", " << colorBytesPerPixel << std::endl;

	// カラー画像のサイズを取得する
	CComPtr<IFrameDescription> colorFrameDescription;
	ERROR_CHECK(colorFrameSource->CreateFrameDescription(colorFormat, &colorFrameDescription));
	ERROR_CHECK(colorFrameDescription->get_Width(&colorWidth));
	ERROR_CHECK(colorFrameDescription->get_Height(&colorHeight));
	ERROR_CHECK(colorFrameDescription->get_BytesPerPixel(&colorBytesPerPixel));
	std::cout << "create  : " << colorWidth << ", " << colorHeight << ", " << colorBytesPerPixel << std::endl;

	// バッファーを作成する
	colorBuffer.resize(colorWidth * colorHeight * colorBytesPerPixel);


	// マウスクリックのイベントを登録する
	// フルスクリーン画面をクリックして入れ替える．
	//cv::namedWindow("Ima-mirror2_full");
	//cv::setMouseCallback("Ima-mirror2_full", &ImamirrorApp::mouseCallback);

	// shapeとactorの関係をリセット
	// これはもともとgetInitData()内でやっていたんだけど，main()でgetInitData()よりも先にある
	// この関数内(initialize())でしておくと，人がいなくても起動できるという都合でここに書くよ．
	for (int body_num = 0; body_num < PEOPLE; body_num++){
		shape_actor.shape[body_num] = shape_actor.actor[body_num] = -1;
	}

	human = new Human[PEOPLE];

	full_layout_test.set_fullscreen_layout(depthWidth, depthHeight);

	std::cout << "initialize";

}

/*
// マウスイベントのコールバック  Imamirror2画面で初期位置をとるためのマウス処理
void ImamirrorApp::mouseCallback(int event, int x, int y, int flags, void*)
{
	if (event == CV_EVENT_LBUTTONDOWN) {
		getInitData();
	}
}
*/
// 変換元のデータを取得
void ImamirrorApp::getInitData()
{
	// 初期状態記録用
	cv::Mat init_bodyImage = cv::Mat::zeros(424, 512, CV_8UC4);
	cv::Mat init_depthImage = cv::Mat::zeros(depthHeight, depthWidth, CV_8UC1);

	while (true) {

		// 変換元のデータを取得したプレイヤーの番号
		int gotBody = -1;


		// BodyFrameの取得//////////////////////////////////////////////////
		// フレームを取得する
		CComPtr<IBodyFrame> bodyFrame;
		auto ret1 = bodyFrameReader->AcquireLatestFrame(&bodyFrame);
		if (FAILED(ret1)){
			continue;
		}

		// 前回のBodyを解放する
		for (int body_num = 0; body_num < PEOPLE; body_num++){
			if (bodies[body_num] != nullptr){
				bodies[body_num]->Release();
				bodies[body_num] = nullptr;
			}
		}

		// Bodiesデータを取得する
		ERROR_CHECK(bodyFrame->GetAndRefreshBodyData(PEOPLE, &bodies[0]));


		// BodyIndexFrameの取得//////////////////////////////////////////////////
		// フレームを取得する
		CComPtr<IBodyIndexFrame> bodyIndexFrame;
		auto ret2 = bodyIndexFrameReader->AcquireLatestFrame(&bodyIndexFrame);
		if (FAILED(ret2)){
			continue;
		}

		// bodyIndexデータを取得する
		ERROR_CHECK(bodyIndexFrame->CopyFrameDataToArray(bodyIndexBuffer.size(), &bodyIndexBuffer[0]));


		// DepthFrameの取得//////////////////////////////////////////////////
		// フレームを取得する
		CComPtr<IDepthFrame> depthFrame;
		auto ret3 = depthFrameReader->AcquireLatestFrame(&depthFrame);
		if (FAILED(ret3)){
			continue;
		}

		// Depthデータを取得する
		ERROR_CHECK(depthFrame->CopyFrameDataToArray(depthBuffer.size(), &depthBuffer[0]));


		// 交換関係を決める
		if (partner_change){
			shape_actor.set_shape_actor_exchange(bodies);
		}
		else{
			shape_actor.set_shape_actor_own(bodies);
		}

		// 初期boneを取得する
		// すべてのBodyについて繰り返し //boneの初期情報を取得
		for (int body_num = 0; body_num < PEOPLE; body_num++){
			IBody* body = bodies[body_num];

			// Bodyがなかったら終わり
			if (body == nullptr){
				continue;
			}

			// 追跡できてなかったら終わり
			BOOLEAN isTracked = false;
			ERROR_CHECK(body->get_IsTracked(&isTracked));
			if (!isTracked) {
				continue;
			}

			// 上で求めたアクターのボディ番号を取得
			int actor_body = shape_actor.actor[body_num];

			human[body_num].set_shape_bone(body);
			//human[body_num].set_actor_bone(bodies[actor_body]); // これいる？

			gotBody = body_num;
		}

		// 初期点群を取得する
		if (gotBody != -1) { // bodyが１つでも取れていたら
			for (int person = 0; person < PEOPLE; person++){

				// Bodyがなかったら終わり
				if (bodies[person] == nullptr){
					continue;
				}

				// 追跡できてなかったら終わり
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

		// 変換元のデータが取得できてたら終わり
		if (gotBody != -1) {
			break;
		}

	}

	// 初期状態を描画
	//cv::imshow("init_BodyImage", init_bodyImage);
	//cv::imshow("init_DepthImage", init_depthImage);

}



// BodyFrameの更新
void ImamirrorApp::updateBodyFrame()
{
	// フレームを取得する
	CComPtr<IBodyFrame> bodyFrame;
	auto ret = bodyFrameReader->AcquireLatestFrame(&bodyFrame);
	if (FAILED(ret)){
		return;
	}

	// 前回のBodyを解放する
	for (auto& body : bodies){
		if (body != nullptr){
			body->Release();
			body = nullptr;
		}
	}

	// データを取得する
	ERROR_CHECK(bodyFrame->GetAndRefreshBodyData(PEOPLE, &bodies[0]));
}

// BodyIndexFrameの更新
void ImamirrorApp::updateBodyIndexFrame()
{
	// フレームを取得する
	CComPtr<IBodyIndexFrame> bodyIndexFrame;
	auto ret = bodyIndexFrameReader->AcquireLatestFrame(&bodyIndexFrame);
	if (FAILED(ret)){
		return;
	}

	// データを取得する
	ERROR_CHECK(bodyIndexFrame->CopyFrameDataToArray(bodyIndexBuffer.size(), &bodyIndexBuffer[0]));
}

// DepthFrameの更新
void ImamirrorApp::updateDepthFrame()
{
	// フレームを取得する
	CComPtr<IDepthFrame> depthFrame;
	auto ret = depthFrameReader->AcquireLatestFrame(&depthFrame);
	if (FAILED(ret)){
		return;
	}

	// データを取得する
	ERROR_CHECK(depthFrame->CopyFrameDataToArray(depthBuffer.size(), &depthBuffer[0]));


}

// ColorFrameの更新
void ImamirrorApp::updateColorFrame()
{
	// フレームを取得する
	CComPtr<IColorFrame> colorFrame;
	auto ret = colorFrameReader->AcquireLatestFrame(&colorFrame);
	if (FAILED(ret)){
		return;
	}

	// 指定の形式でデータを取得する
	ERROR_CHECK(colorFrame->CopyConvertedFrameDataToArray(
		colorBuffer.size(), &colorBuffer[0], colorFormat));
}

// BodyFrameの描画
void ImamirrorApp::drawBodyFrame()
{
	// 関節の位置をDepth座標系に変換して表示する
	cv::Mat bodyImage = cv::Mat::zeros(424, 512, CV_8UC4);

	// すべてのBodyについて繰り返し描画
	for (int body = 0; body < PEOPLE; body++){

		// Bodyがなかったら終わり
		if (bodies[body] == nullptr){
			continue;
		}

		// 追跡できてなかったら終わり
		BOOLEAN isTracked = false;
		ERROR_CHECK(bodies[body]->get_IsTracked(&isTracked));
		if (!isTracked) {
			continue;
		}

		// この辺テスト
		int actor_body = shape_actor.actor[body];
		//human[body].set_actor_bone(bodies[actor_body]);
		

		// ポーズ認識
		//check_base_posture(bone_data[body]);
	}

	// 表示
	cv::imshow("BodyImage", bodyImage);
}

// BodyIndexFrameの描画
void ImamirrorApp::drawBodyIndexFrame()
{
	/*
	// BodyIndexをカラーデータに変換して表示する
	cv::Mat bodyIndexImage(bodyIndexHeight, bodyIndexWidth, CV_8UC4);

	for (int i = 0; i < bodyIndexWidth * bodyIndexHeight; ++i){
		int index = i * 4;

		// BodyIndexBufferが255以外ならば(=人がいれば)
		if (bodyIndexBuffer[i] != 255){
			auto color = colors[bodyIndexBuffer[i]];
			if (bodyIndexBuffer[i] == bib_player){
				color = bib_player_color;
			}
			bodyIndexImage.data[index + 0] = color[0];
			bodyIndexImage.data[index + 1] = color[1];
			bodyIndexImage.data[index + 2] = color[2];

		}
		// 人がいなければ
		else{
			bodyIndexImage.data[index + 0] = 0;
			bodyIndexImage.data[index + 1] = 0;
			bodyIndexImage.data[index + 2] = 0;
		}
	}*/

	// 表示
	//cv::imshow("BodyIndexImage", bodyIndexImage);


}

// DepthFrameの描画
void ImamirrorApp::drawDepthFrame()
{
	// Depthイメージを作っている．初期化もしている
	cv::Mat depthImage = cv::Mat::zeros(depthHeight, depthWidth, CV_8UC1);

	// 通常表示
	//cv::imshow("DepthImage", depthImage);

}

// カラーデータの表示処理
void ImamirrorApp::drawColorFrame()
{
#if 0
	// カラーデータを表示する
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
	// ↑CV_8UC4→CV_8UC3にしたら，メイン画面に画像を貼れるようになった．

	//drawBackground(Imamirror2_full);

	/*
	// すべてのBodyについて繰り返し描画
	for (int body = 0; body < PEOPLE; body++){
		if (shape_actor.actor[body] == -1){ // もしactorがいなかったら

			// Bodyがなかったら終わり
			if (bodies[body] == nullptr){
				continue;
			}

			// 追跡できてなかったら終わり
			BOOLEAN isTracked = false;
			ERROR_CHECK(bodies[body]->get_IsTracked(&isTracked));
			if (!isTracked) {
				continue;
			}

			// ここで根幹処理を呼び出す
			//human[body].get_translate_body(kinect, Imamirror2_full, full_layout_test, bodyIndexWidth, bodyIndexHeight);
		}
	}
	*/
	// フルスクリーン表示
	cvNamedWindow("Ima-mirror2_full");
	cv::setWindowProperty("Ima-mirror2_full", cv::WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN); // フルスクリーン設定
	cv::imshow("Ima-mirror2_full-----", Imamirror2_full); // 表示
	
	//cv::imshow("new_body_", new_bone_bodyImage);

}


int main(int argc, char *argv[])
{
	std::cout << "mainひらいてるよ";
	
	try {
		ImamirrorApp app;		// インスタンスを生成して，
		app.initialize();	// いろいろ初期化して，
		//app.getInitData();	// 変換元のデータを取得して，
		app.run();			// ひたすらループを回す
	}
	catch (std::exception& ex){
		std::cout << ex.what() << std::endl;
	}

	return 0;
}