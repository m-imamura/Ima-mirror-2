//
//
////////////////////////////////////////////////////
//// 背景取得後
//// 2017-05-02-作成
//// コードを分割中です
////
//// Ima-mirror 2 このコードのメモ
////
//// 操作方法
//// ・実行して人が認識されてから動作する．→人が認識されなくても始まる．
//// ・フルスクリーンの画面上でクリックすると入れ替わる．
//// ・キーボードの'o'で入れ替わるか，入れ替わらない（自分を動かす）か切り替え．
//// ・キーボードの'b'で背景を取得する．(new)
//// ・キーボードの'v'で背景を表示する・しない切り替え(new)
//// ・キーボードの'c'で人物のカラーかモノクロか切り替え(new)←以前は接触のバーの位置の切り替えに使用していた．
//// ・コンソールを閉じるか，'q'で終了．
////
//// 使えない機能
//// ・接触判定は関数が残っていても機能しない．
//// ・KinectのBodyIndexやBodyなどは表示しないようにしている．(cv::imshow()をしないようにしているだけ)
//// ・ガイド表示を出していたけど，予備実験で落ちる不具合があったので機能停止．
////
//// 最近の進捗
//// ・カラー表示できるようにした．
//// ・背景を表示できるようにした．
//// ・コードの一部分割に成功（transformation.cpp）
////
//// 以下の機能はまだ開発中
//// ・基本姿勢ガイド
//// ・複数人の基本姿勢を認識したら自動で入れ替え
////
//// TODO
//// ・複数ファイルに分ける．
//// ・関節が滑らかに曲がるようにしたい．（標本点とボーンとの距離による重みwが機能していないっぽいので先生に聞く）
//// ・背景をcv:Matとして保存→毎フレーム貼り付ける方針にする．
////////////////////////////////////////////////////
//
//
//
//// 外部リソース
////////////////////////////////////////////////////
//#include <iostream>
//#include <sstream>
//#include <atlbase.h>
//#include <windows.h>
//#include <stdio.h>
//#include <stdlib.h>
//
//#include <Eigen/Core>		// 線形代数ライブラリ
//#include <Eigen/Geometry>	// 外積の計算に必要
//#include <Kinect.h>
//#include <opencv2\opencv.hpp>
//#include <time.h> // 現時刻の表示用
//
//#include "hoge.h"
//#include "transformation.h"
//#include "draw_ellipse.h"
//#include "fullscreen_layout.h"
//#include "Human.h"
////////////////////////////////////////////////////
//
//
//// 定数の定義
////////////////////////////////////////////////////
////#define BONES 24			// 人体のボーンの数
////#define JOINTS 25			// ジョイントの数
////#define POINTS_MAX 3000		// 描画する点群の数
////#define PEOPLE 6			// 人数
////////////////////////////////////////////////////
//
//
//// 型定義
////////////////////////////////////////////////////
//// 交換関係
//typedef struct shape_actor{
//	int shape;
//	int actor; //変換先のbody番号
//}SHAPE_ACTOR;
//
///*
//// ボーンの接続関係
//typedef struct Bone_set{
//	int top;	//先端のジョイント番号
//	int bottom;	//根元のジョイント番号
//	int parent;	//親ボーン
//}BONE_SET;
//
//// ボーンの影響範囲
//typedef struct impact_range{
//	double range[BONES];
//}IMPACT_RANGE;
//
//// 各ボーンの位置・方向等
//typedef struct Bone_data{
//	Eigen::Vector4f top_init;	//初期位置
//	Eigen::Vector4f bottom_init;//初期位置
//	Eigen::Vector4f vector_init;//初期方向
//	Eigen::Vector4f top;		//逐次位置
//	Eigen::Vector4f bottom;		//逐次位置
//	Eigen::Vector4f vector;		//逐次方向 (bottom -> top)
//	int parent;					//親ボーン (使ってない？)
//	float length;				//長さ (vector_initの)
//}BONE_DATA;
//
//// 点群
//typedef struct Points_data{
//	Eigen::Vector4f points_init[POINTS_MAX];	//初期位置の点(Camera座標) 最大POINTS_XYZ_NUM個
//	Eigen::Vector4f points[POINTS_MAX];			//変換後の点(Camera座標) 最大POINTS_XYZ_NUM個
//	Eigen::Vector4f points_depth[POINTS_MAX];	//変換後の点(Depth座標)
//	cv::Scalar color[POINTS_MAX];				//色
//	int points_num;								//点の総数
//}POINTS_DATA;
//
//// フルスクリーン設定
//typedef struct fullScreen_layout{
//	int DisplayWidth;
//	int DisplayHeight;
//	int margin_x; // x方向の余白
//	int margin_y; // y方向の余白
//	double magnification; // 倍率
//}FULLSCREEN_LAYOUT;
////////////////////////////////////////////////////
//*/
////
//// 変数の定義
////////////////////////////////////////////////////
////Eigen::Vector4f joint_position[PEOPLE][JOINTS]; //関節の位置
////
////BONE_SET bone_set[BONES]; //ボーンの接続関係
////BONE_DATA bone_data[PEOPLE][BONES]; //1人目の各ボーンの位置・方向等
////POINTS_DATA points_data[PEOPLE]; //1人目の点群
////SHAPE_ACTOR shape_actor[PEOPLE];
////IMPACT_RANGE impact_range;
//// FULLSCREEN_LAYOUT full_layout;
//
////////////////////////////////////////////////////
//
//// グローバル変数
////////////////////////////////////////////////////
//// 接触提示関係
//HANDLE arduino;
//bool Ret;
//char start_data = 'o';
//char stop_data = 'p';
//bool Before_time_bib = false;
////////////////////////////////////////////////////
//
//
//// おまじない？
////////////////////////////////////////////////////
//// 次のように使います
//// ERROR_CHECK( ::GetDefaultKinectSensor( &kinect ) );
//// 書籍での解説のためにマクロにしています。実際には展開した形で使うことを検討してください。
//#define ERROR_CHECK(ret) \
//	if ((ret) != S_OK) { \
//		std::stringstream ss; \
//		ss << "failed " #ret " " << std::hex << ret << std::endl; \
//		throw std::runtime_error(ss.str().c_str()); \
//	}
//	//////////////////////////////////////////////////
//
//
//
//class KinectApp
//{
//private:
//
//	// プロパティの定義
//	//////////////////////////////////////////////////
//	CComPtr<IKinectSensor> kinect = nullptr;
//
//	// Body関連
//	CComPtr<IBodyFrameReader> bodyFrameReader = nullptr;
//	IBody* bodies[PEOPLE];	// データを取得する人数
//
//	// BodyIndex関連
//	CComPtr<IBodyIndexFrameReader> bodyIndexFrameReader = nullptr;
//	std::vector<BYTE> bodyIndexBuffer;	// 取得したBodyIndexを格納するバッファ
//
//	int bodyIndexWidth;
//	int bodyIndexHeight;
//
//	// Depth関連
//	CComPtr<IDepthFrameReader> depthFrameReader = nullptr;
//	std::vector<UINT16> depthBuffer;	// 取得したDepthを格納するバッファ
//	std::vector<UINT16> background_depthBuffer;//背景のDepthを格納するバッファ
//
//	int depthWidth;
//	int depthHeight;
//
//	// Color関連
//	CComPtr<IColorFrameReader> colorFrameReader = nullptr;
//	// 表示部分
//	std::vector<BYTE> colorBuffer;
//
//	int colorWidth;
//	int colorHeight;
//	unsigned int colorBytesPerPixel;
//
//	ColorImageFormat colorFormat = ColorImageFormat::ColorImageFormat_Bgra;
//
//	// 振動関係
//	cv::Scalar colors[PEOPLE];	// プレーヤーの色を格納
//	cv::Scalar bib_player_color;
//
//	// 振動を提示するプレイヤー
//	int bib_player = -1;
//
//	// オブジェクト位置
//	int appearLocation = -4;
//
//	// 接触の時間を測る
//	clock_t start;
//	clock_t end;
//
//
//	// 表示モード関係
//	// 他者と入れ替え可自分と入れ替えか
//	bool partner_change = true;
//	// カラーか白黒か．
//	bool color_view_tf = false;
//	// 背景あるなし
//	bool background_tf = false;
//
//	// 背景を保存するMat
//	std::vector<BYTE> backgroundBuffer; // std::vector<BYTE> colorBuffer; と同じ
//
//	bool get_background = false;
//
//	
//	
//	Human human[PEOPLE];// 人のデータ
//	SHAPE_ACTOR shape_actor[PEOPLE];
//	FullscreenLayout full_layout_test;// フルスクリーンのデータ
//
//	//////////////////////////////////////////////////
//
//
//public:
//
//	// デストラクタ
//	~KinectApp()
//	{
//		if (kinect != nullptr){
//			kinect->Close();
//		}
//	}
//
//
//	// データを更新して描画
//	void run()
//	{
//		while (true) {
//			update(); //BodyFrame，BodyIndexFrame，DepthFrameをそれぞれ更新
//			draw(); //BodyFrame，BodyIndexFrame，DepthFrameをそれぞれ描画
//
//
//			auto key = cv::waitKey(10);
//			if (key == 'u') { // 身体形状をとる
//				getInitData();
//			}
//			if (key == 'q') {
//				break;
//			}
//			/*if (key == 'c'){ // バーの位置を変える
//				ObjectAppearChange();
//			}*/
//			//if (key == 'e'){ // ただの区切り入れ
//			//	std::cout << "-----one paturn experiment end------\n";
//			//}
//			if (key == 'o'){ // 自分と入れ替え，他者と入れ替えを切り替える
//				partner_or_own();
//			}
//			if (key == 'b'){
//				//if (get_background == false){
//					backgroundBuffer = colorBuffer; // カラーバッファーの取得
//					background_depthBuffer = depthBuffer; // デプスバッファーの取得
//					get_background = true;
//					std::cout << "背景を取得\n";
//				//}
//				//else{
//				//	get_background = false;
//				//}
//			}
//			if (key == 'v'){
//				if (get_background){ // 一回でも背景取得していないと無効
//					if (background_tf){
//						background_tf = false;
//						std::cout << "背景を非表示\n";
//					}
//					else{
//						background_tf = true;
//						std::cout << "背景を表示\n";
//					}
//				}
//			}
//			if (key == 'c'){
//				if (color_view_tf){
//					color_view_tf = false;
//					std::cout << "人物白表示\n";
//				}
//				else{
//					color_view_tf = true;
//					std::cout << "人物カラー表示\n";
//				}
//			}
//		}
//	}
//
//
//	// データ更新処理の呼び出し
//	void update()
//	{
//		updateBodyFrame();
//		updateBodyIndexFrame();
//		updateDepthFrame();
//		updateColorFrame();
//	}
//
//	// 描画処理の呼び出し
//	void draw()
//	{
//		drawBodyFrame();
//		drawBodyIndexFrame();
//		drawDepthFrame();
//		drawColorFrame();
//	}
//
//
//	// ↑基幹の処理
//	//////////////////////////////////////////////////
//	//////////////////////////////////////////////////
//	//////////////////////////////////////////////////
//	// ↓呼び出される各メンバ
//
//	// 初期化
//	void initialize()
//	{
//		// デフォルトのKinectを取得する
//		ERROR_CHECK(::GetDefaultKinectSensor(&kinect));
//		ERROR_CHECK(kinect->Open());
//
//
//		// Body関連の初期化
//		//////////////////////////////////////////////////
//		CComPtr<IBodyFrameSource> bodyFrameSource;
//		ERROR_CHECK(kinect->get_BodyFrameSource(&bodyFrameSource));
//		ERROR_CHECK(bodyFrameSource->OpenReader(&bodyFrameReader));
//
//		for (auto& body : bodies){
//			body = nullptr;
//		}
//		//////////////////////////////////////////////////
//
//
//		// BodyIndex関係の初期化
//		//////////////////////////////////////////////////
//		CComPtr<IBodyIndexFrameSource> bodyIndexFrameSource;
//		ERROR_CHECK(kinect->get_BodyIndexFrameSource(&bodyIndexFrameSource));
//		ERROR_CHECK(bodyIndexFrameSource->OpenReader(&bodyIndexFrameReader));
//
//		// BodyIndex画像のサイズを取得する
//		CComPtr<IFrameDescription> bodyIndexFrameDescription;
//		ERROR_CHECK(bodyIndexFrameSource->get_FrameDescription(&bodyIndexFrameDescription));
//		ERROR_CHECK(bodyIndexFrameDescription->get_Width(&bodyIndexWidth));
//		ERROR_CHECK(bodyIndexFrameDescription->get_Height(&bodyIndexHeight));
//
//		// バッファーを作成する
//		bodyIndexBuffer.resize(bodyIndexWidth * bodyIndexHeight);
//
//		// プレイヤーのindex色を設定する
//		colors[0] = cv::Scalar(255, 255, 255);
//		colors[1] = cv::Scalar(234, 234, 234);
//		colors[2] = cv::Scalar(231, 213, 213);
//		colors[3] = cv::Scalar(192, 192, 192);
//		colors[4] = cv::Scalar(171, 171, 171);
//		colors[5] = cv::Scalar(150, 150, 150);
//
//		// 振動提示プレイヤーのindex色
//		bib_player_color = cv::Scalar(255, 255, 0);
//		//////////////////////////////////////////////////
//
//
//		// Depth関連の初期化
//		//////////////////////////////////////////////////
//		CComPtr<IDepthFrameSource> depthFrameSource;
//		ERROR_CHECK(kinect->get_DepthFrameSource(&depthFrameSource));
//		ERROR_CHECK(depthFrameSource->OpenReader(&depthFrameReader));
//
//		// Depth画像のサイズを取得する
//		CComPtr<IFrameDescription> depthFrameDescription;
//		ERROR_CHECK(depthFrameSource->get_FrameDescription(&depthFrameDescription));
//		ERROR_CHECK(depthFrameDescription->get_Width(&depthWidth));
//		ERROR_CHECK(depthFrameDescription->get_Height(&depthHeight));
//
//		// Depthの最大値、最小値を取得する
//		UINT16 minDepthReliableDistance;
//		UINT16 maxDepthReliableDistance;
//		ERROR_CHECK(depthFrameSource->get_DepthMinReliableDistance(&minDepthReliableDistance));
//		ERROR_CHECK(depthFrameSource->get_DepthMaxReliableDistance(&maxDepthReliableDistance));
//		//std::cout << "Depth最小値 : " << minDepthReliableDistance << std::endl;
//		//std::cout << "Depth最大値 : " << maxDepthReliableDistance << std::endl;
//
//		// バッファーを作成する
//		depthBuffer.resize(depthWidth * depthHeight);
//		//////////////////////////////////////////////////
//
//
//		// Color関係の初期化
//		//////////////////////////////////////////////////
//		// カラーリーダーを取得する
//		CComPtr<IColorFrameSource> colorFrameSource;
//		ERROR_CHECK(kinect->get_ColorFrameSource(&colorFrameSource));
//		ERROR_CHECK(colorFrameSource->OpenReader(&colorFrameReader));
//
//		// デフォルトのカラー画像のサイズを取得する
//		CComPtr<IFrameDescription> defaultColorFrameDescription;
//		ERROR_CHECK(colorFrameSource->get_FrameDescription(&defaultColorFrameDescription));
//		ERROR_CHECK(defaultColorFrameDescription->get_Width(&colorWidth));
//		ERROR_CHECK(defaultColorFrameDescription->get_Height(&colorHeight));
//		ERROR_CHECK(defaultColorFrameDescription->get_BytesPerPixel(&colorBytesPerPixel));
//		std::cout << "default : " << colorWidth << ", " << colorHeight << ", " << colorBytesPerPixel << std::endl;
//
//		// カラー画像のサイズを取得する
//		CComPtr<IFrameDescription> colorFrameDescription;
//		ERROR_CHECK(colorFrameSource->CreateFrameDescription(
//			colorFormat, &colorFrameDescription));
//		ERROR_CHECK(colorFrameDescription->get_Width(&colorWidth));
//		ERROR_CHECK(colorFrameDescription->get_Height(&colorHeight));
//		ERROR_CHECK(colorFrameDescription->get_BytesPerPixel(&colorBytesPerPixel));
//		std::cout << "create  : " << colorWidth << ", " << colorHeight << ", " << colorBytesPerPixel << std::endl;
//
//		// バッファーを作成する
//		colorBuffer.resize(colorWidth * colorHeight * colorBytesPerPixel);
//		//////////////////////////////////////////////////
//
//
//		// マウスクリックのイベントを登録する
//		// フルスクリーン画面をクリックして入れ替える．
//		cv::namedWindow("Ima-mirror2_full");
//		cv::setMouseCallback("Ima-mirror2_full", &KinectApp::mouseCallback, this);
//
//		// マウスクリックのイベントを登録する
//		// BodyIndex画面をクリックして接触提示する人を決める．
//		//cv::namedWindow("BodyIndexImage");
//		//cv::setMouseCallback("BodyIndexImage", &KinectApp::mouseCallback_bib, this);
//
//
//		// ボーンごとのtopとbottom，親ボーンを設定，影響範囲の設定
//		//////////////////////////////////////////////////
//
//
//		/*
//		// 一時的な影響範囲の設定
//		for (int i = 0; i < BONES; i++){
//			impact_range.range[i] = 0.5;
//		}
//
//		//体幹
//		bone_set[0].bottom = JointType_SpineBase;//根元のジョイントを設定
//		bone_set[0].top = JointType_SpineMid;//先端のジョイントを設定
//		bone_set[0].parent = -1;//親のボーンを設定（相対位置の基準）
//		impact_range.range[0] = 0.8;
//
//		bone_set[1].bottom = JointType_SpineMid;
//		bone_set[1].top = JointType_SpineShoulder;
//		bone_set[1].parent = 0;
//		impact_range.range[1] = 0.8;
//
//		bone_set[2].bottom = JointType_SpineShoulder;
//		bone_set[2].top = JointType_Neck;
//		bone_set[2].parent = 1;
//		impact_range.range[2] = 0.4;
//
//		bone_set[3].bottom = JointType_Neck;
//		bone_set[3].top = JointType_Head;
//		bone_set[3].parent = 2;
//		impact_range.range[3] = 1.0;
//
//		//左腕
//		bone_set[4].bottom = JointType_SpineShoulder;
//		bone_set[4].top = JointType_ShoulderLeft;
//		bone_set[4].parent = 1;//枝分かれ bone2（首）と同じ立場
//		//impact_range.range[4] = 0.4;
//
//		bone_set[5].bottom = JointType_ShoulderLeft;
//		bone_set[5].top = JointType_ElbowLeft;
//		bone_set[5].parent = 4;
//		//impact_range.range[5] = 0.4;
//
//		bone_set[6].bottom = JointType_ElbowLeft;
//		bone_set[6].top = JointType_WristLeft;
//		bone_set[6].parent = 5;
//		//impact_range.range[6] = 0.4;
//
//		bone_set[7].bottom = JointType_WristLeft;
//		bone_set[7].top = JointType_HandLeft;
//		bone_set[7].parent = 6;
//		//impact_range.range[7] = 0.5;
//
//		bone_set[8].bottom = JointType_HandLeft;
//		bone_set[8].top = JointType_HandTipLeft;
//		bone_set[8].parent = 7;
//		//impact_range.range[8] = 0.4;
//
//		bone_set[9].bottom = JointType_HandLeft;
//		bone_set[9].top = JointType_ThumbLeft;
//		bone_set[9].parent = 7;
//		//impact_range.range[9] = 0.4;
//
//		//右腕
//		bone_set[10].bottom = JointType_SpineShoulder;
//		bone_set[10].top = JointType_ShoulderRight;
//		bone_set[10].parent = 1;//枝分かれ bone2（首）と同じ立場
//
//		bone_set[11].bottom = JointType_ShoulderRight;
//		bone_set[11].top = JointType_ElbowRight;
//		bone_set[11].parent = 10;
//
//		bone_set[12].bottom = JointType_ElbowRight;
//		bone_set[12].top = JointType_WristRight;
//		bone_set[12].parent = 11;
//
//		bone_set[13].bottom = JointType_WristRight;
//		bone_set[13].top = JointType_HandRight;
//		bone_set[13].parent = 12;
//
//		bone_set[14].bottom = JointType_HandRight;
//		bone_set[14].top = JointType_HandTipRight;
//		bone_set[14].parent = 13;
//
//		bone_set[15].bottom = JointType_HandRight;
//		bone_set[15].top = JointType_ThumbRight;
//		bone_set[15].parent = 13;
//
//		//左脚
//		bone_set[16].bottom = JointType_SpineBase;
//		bone_set[16].top = JointType_HipLeft;
//		bone_set[16].parent = -1;//左脚を独立と考える．親はなし
//
//		bone_set[17].bottom = JointType_HipLeft;
//		bone_set[17].top = JointType_KneeLeft;
//		bone_set[17].parent = 16;
//
//		bone_set[18].bottom = JointType_KneeLeft;
//		bone_set[18].top = JointType_AnkleLeft;
//		bone_set[18].parent = 17;
//
//		bone_set[19].bottom = JointType_AnkleLeft;
//		bone_set[19].top = JointType_FootLeft;
//		bone_set[19].parent = 18;
//
//		//右脚
//		bone_set[20].bottom = JointType_SpineBase;
//		bone_set[20].top = JointType_HipRight;
//		bone_set[20].parent = -1;//右脚を独立と考える．親はなし
//
//		bone_set[21].bottom = JointType_HipRight;
//		bone_set[21].top = JointType_KneeRight;
//		bone_set[21].parent = 20;
//
//		bone_set[22].bottom = JointType_KneeRight;
//		bone_set[22].top = JointType_AnkleRight;
//		bone_set[22].parent = 21;
//
//		bone_set[23].bottom = JointType_AnkleRight;
//		bone_set[23].top = JointType_FootRight;
//		bone_set[23].parent = 22;
//		//////////////////////////////////////////////////
//		*/
//
//		// shapeとactorの関係をリセット
//		// これはもともとgetInitData()内でやっていたんだけど，main()でgetInitData()よりも先にあるこの関数内(initialize())でしておくと，人がいなくても起動できるという都合でここに書くよ．
//		for (int body_num = 0; body_num < PEOPLE; body_num++){
//			shape_actor[body_num].shape = shape_actor[body_num].actor = -1;
//		}
//
//
//		// フルスクリーン設定
//		//full_layout = fullScreen_layout_set(depthWidth, depthHeight);
//
//		full_layout_test.set_fullscreen_layout(depthWidth, depthHeight);
//
//	}
//
//	// 変換元のデータを取得
//	void getInitData()
//	{
//
//		// 初期状態記録用
//		cv::Mat init_bodyImage = cv::Mat::zeros(424, 512, CV_8UC4);
//		cv::Mat init_depthImage = cv::Mat::zeros(depthHeight, depthWidth, CV_8UC1);
//
//		while (true) {
//
//			// 変換元のデータを取得したプレイヤーの番号
//			int gotBody = -1;
//
//			// 'g'が押下された時の人体データを取得
//			//if (GetAsyncKeyState('g')) {
//
//			// BodyFrameの取得
//			//////////////////////////////////////////////////
//			// フレームを取得する
//			CComPtr<IBodyFrame> bodyFrame;
//			auto ret1 = bodyFrameReader->AcquireLatestFrame(&bodyFrame);
//			if (FAILED(ret1)){
//				continue;
//			}
//
//			// 前回のBodyを解放する
//			for (int body_num = 0; body_num < PEOPLE; body_num++){
//				if (bodies[body_num] != nullptr){
//					bodies[body_num]->Release();
//					bodies[body_num] = nullptr;
//				}
//			}
//
//
//
//			// データを取得する
//			ERROR_CHECK(bodyFrame->GetAndRefreshBodyData(PEOPLE, &bodies[0]));
//			//////////////////////////////////////////////////
//
//			// BodyIndexFrameの取得
//			//////////////////////////////////////////////////
//			// フレームを取得する
//			CComPtr<IBodyIndexFrame> bodyIndexFrame;
//			auto ret2 = bodyIndexFrameReader->AcquireLatestFrame(&bodyIndexFrame);
//			if (FAILED(ret2)){
//				continue;
//			}
//
//			// データを取得する
//			ERROR_CHECK(bodyIndexFrame->CopyFrameDataToArray(bodyIndexBuffer.size(), &bodyIndexBuffer[0]));
//			//////////////////////////////////////////////////
//
//			// DepthFrameの取得
//			//////////////////////////////////////////////////
//			// フレームを取得する
//			CComPtr<IDepthFrame> depthFrame;
//			auto ret3 = depthFrameReader->AcquireLatestFrame(&depthFrame);
//			if (FAILED(ret3)){
//				continue;
//			}
//
//			// データを取得する
//			ERROR_CHECK(depthFrame->CopyFrameDataToArray(depthBuffer.size(), &depthBuffer[0]));
//			//////////////////////////////////////////////////
//
//
//
//			// 身体入れ替え機構
//
//			// shapeとactorの関係をリセット
//			for (int body_num = 0; body_num < PEOPLE; body_num++){
//				shape_actor[body_num].shape = shape_actor[body_num].actor = -1;
//			}
//
//			if (partner_change == false){// もし自分に入れ替えなら
//
//				for (int body_num = 0; body_num < PEOPLE; body_num++){
//					IBody* body = bodies[body_num];
//
//					// Bodyがあった！
//					BOOLEAN isTracked = false;
//					ERROR_CHECK(body->get_IsTracked(&isTracked));
//					if (isTracked == true){
//						//bodyの行き先
//						shape_actor[body_num].actor = body_num;
//					}
//				}
//
//			}
//			else{// 複数人の入れ替えを決める
//
//				int first_actor = -1; // bodies[]配列に入っていた最初の人．この人の動作は配列の最後に入っていた人の形が割り当てられる
//				int next_shape = -1; // 次のアクターが割り当てられるべき形の番号
//				for (int body_num = 0; body_num < PEOPLE; body_num++){
//					IBody* body = bodies[body_num];
//
//					// Bodyがあった！
//					BOOLEAN isTracked = false;
//					ERROR_CHECK(body->get_IsTracked(&isTracked));
//					if (isTracked == true){
//						// bodyの行き先
//						if (first_actor == -1){ // 最初のactorを待機させる
//							first_actor = body_num;
//						}
//						else{
//							shape_actor[next_shape].actor = body_num;
//						}
//						next_shape = body_num;
//					}
//				}
//				if (first_actor != -1){
//					shape_actor[next_shape].actor = first_actor;
//				}
//			}
//
//			// 入れ替え結果を出力
//			for (int body_num = 0; body_num < PEOPLE; body_num++){
//				if (shape_actor[body_num].actor != -1){
//					printf("%d → %d\n", body_num, shape_actor[body_num].actor);
//				}
//			}
//			// 身体入れ替え機構ここまで
//
//
//
//			// ここから分割テスト
//			// すべてのBodyについて繰り返し //boneの初期情報を取得
//			for (int body_num = 0; body_num < PEOPLE; body_num++){
//				IBody* body = bodies[body_num];
//
//				// Bodyがなかったら終わり
//				if (body == nullptr){
//					continue;
//				}
//
//				// 追跡できてなかったら終わり
//				BOOLEAN isTracked = false;
//				ERROR_CHECK(body->get_IsTracked(&isTracked));
//				if (!isTracked) {
//					continue;
//				}
//
//				// 上で求めたアクターのボディ番号を取得
//				int actor_body = shape_actor[body_num].actor;
//
//				human[body_num].set_shape_bone(body);
//				human[body_num].set_actor_bone(bodies[actor_body]);
//
//				/*
//				// ここからset_bones_init_data()関数にしたい
//
//				// 関節の位置を取得
//				Joint joints[JointType::JointType_Count];
//				body->GetJoints(JointType::JointType_Count, joints);
//
//
//				for (auto joint : joints) {
//					if (joint.TrackingState == TrackingState::TrackingState_Tracked) {
//						joint_position[body_num][joint.JointType] << joint.Position.X, joint.Position.Y, joint.Position.Z, 1.0;
//						drawEllipse(init_bodyImage, joint, 3, cv::Scalar(255, 255, 255));
//					}
//				}
//
//				// Bodyがあればすべてのボーンについて繰り返し処理
//				for (int i = 0; i < BONES; i++){
//
//					// i番目のボーンのtop，bottomがどの関節かを求めて，関節の位置を入れる
//					bone_data[body_num][i].bottom_init = joint_position[body_num][bone_set[i].bottom];
//					bone_data[body_num][i].top_init = joint_position[body_num][bone_set[i].top];
//
//					// i番目のボーンのvector(ボーンの向き)をtop-bottomから求める
//					bone_data[body_num][i].vector_init.segment(0, 3) = bone_data[body_num][i].top_init.segment(0, 3) - bone_data[body_num][i].bottom_init.segment(0, 3);
//					bone_data[body_num][i].vector_init.w() = 1.0;
//
//					bone_data[body_num][i].length = bone_data[body_num][i].vector_init.segment(0, 3).norm();
//					bone_data[body_num][i].vector_init.segment(0, 3) = bone_data[body_num][i].vector_init.segment(0, 3).normalized();
//				}
//				// set_bones_init_data()関数ここまで
//				*/
//				
//				gotBody = body_num;
//			}
//			//////////////////////////////////////////////////
//			
//
//			// BodyFrameから変換元のデータが取得できていたらDepthFrameも取得
//			/*if (gotBody != -1) {
//				
//				// 座標変換のためのICoordinateMapperインターフェースを取得
//				ICoordinateMapper* pCoordinateMapper;
//				HRESULT hResult = kinect->get_CoordinateMapper(&pCoordinateMapper);
//				if (FAILED(hResult)){
//					std::cerr << "Error : IKinectSensor::get_CoordinateMapper()" << std::endl;
//					return;
//				}
//
//				int points_num = 0; // 身体に配置される点群の数
//
//				// 座標系返還のためのmapper
//				CComPtr<ICoordinateMapper> mapper;
//				ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));
//
//				// Depth座標系に対応するColor座標系の一覧を取得する
//				std::vector<ColorSpacePoint> colorSpace(depthWidth * depthHeight);
//				mapper->MapDepthFrameToColorSpace(depthBuffer.size(), &depthBuffer[0], colorSpace.size(), &colorSpace[0]);
//
//				// DepthFrameをなめながら身体の領域内に点群を配置
//				for (int i = 0; i < PEOPLE; i++){
//					points_data[i].points_num = 0;
//				}
//				for (int i = 0; i < bodyIndexWidth * bodyIndexHeight; i++)
//				{
//					// i番目の画素が身体の領域内なら
//					if (bodyIndexBuffer[i] != 255)
//					{
//						// i番目の画素のx，y座標を取得
//						int x = i % bodyIndexWidth;
//						int y = i / bodyIndexWidth;
//
//						// Depth座標系の点群をCamera座標系の点群に変換して配置
//						if (points_num < POINTS_MAX && x % 3 == 0 && y % 3 == 0){
//
//							// 初期状態出力画面に通常出力
//
//							init_depthImage.data[i] = 255;
//
//							// Depth座標系をCamera座標系に変換する // mapperはfor()外に移動
//							//CComPtr<ICoordinateMapper> mapper;
//							//ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));
//
//							DepthSpacePoint depth_point; //Depth座標系の点
//							depth_point.X = x; //Depth画像上のxを代入
//							depth_point.Y = y; //Depth画像上のyを代入
//
//							CameraSpacePoint camera_point; //Camera座標系の点
//							mapper->MapDepthPointToCameraSpace(depth_point, depthBuffer[i], &camera_point);
//
//
//							// colorSpaceのインデックスを求める
//							int colorX = (int)colorSpace[i].X;
//							int colorY = (int)colorSpace[i].Y;
//							if ((colorX < 0) || (colorWidth <= colorX) || (colorY < 0) || (colorHeight <= colorY)){
//								continue;
//							}
//
//							int colorIndex = (colorY * colorWidth) + colorX;
//							//int bodyIndex = bodyIndexBuffer[i];
//
//							//int colorImageIndex = i *colorBytesPerPixel;
//							int colorBufferIndex = colorIndex * colorBytesPerPixel;
//
//							//int color_Buffer_point = (color_point.Y*colorWidth + color_point.X)*colorBytesPerPixel;
//							cv::Scalar color_point_color = cv::Scalar(colorBuffer[colorBufferIndex], colorBuffer[colorBufferIndex + 1], colorBuffer[colorBufferIndex + 2]);
//
//							for (int person = 0; person < PEOPLE; person++){
//								if (colors[bodyIndexBuffer[i]] == colors[person] && points_data[person].points_num < POINTS_MAX){
//									points_data[person].points_init[points_data[person].points_num] << camera_point.X, camera_point.Y, camera_point.Z, 1.0;
//									points_data[person].color[points_data[person].points_num] = color_point_color;
//									points_data[person].points_num++;
//								}
//							}
//
//						}
//					}
//				}
//
//			}*/
//
//
//			// ここから分割テスト
//			// BodyFrameから変換元のデータが取得できていたらDepthFrameも取得
//			if (gotBody != -1) {
//				for (int person = 0; person < PEOPLE; person++){
//
//					// Bodyがなかったら終わり
//					if (bodies[person] == nullptr){
//						continue;
//					}
//
//					// 追跡できてなかったら終わり
//					BOOLEAN isTracked = false;
//					ERROR_CHECK(bodies[person]->get_IsTracked(&isTracked));
//					if (!isTracked) {
//						continue;
//					}
//
//					Points testpoints;
//					testpoints.set_players_index_color();
//					testpoints.set_points_data(kinect, person,
//						depthBuffer, depthWidth, depthHeight,
//						bodyIndexBuffer, bodyIndexWidth, bodyIndexHeight,
//						colorBuffer, colorWidth, colorHeight, colorBytesPerPixel);
//				}
//			}
//
//
//
//			// 変換元のデータが取得できてたら終わり
//			if (gotBody != -1) {
//				break;
//			}
//
//		}
//
//		// 初期状態を描画
//		//cv::imshow("init_BodyImage", init_bodyImage);
//		//cv::imshow("init_DepthImage", init_depthImage);
//
//	}
//
//
//	// マウスイベントのコールバック
//	/////////////////////////////////////////////////////
//
//	// Imamirror2画面で初期位置をとるためのマウス処理
//	static void mouseCallback(int event, int x, int y, int flags, void* userdata)
//	{
//		// 引数に渡したthisポインタを経由してメンバ関数に渡す
//		auto pThis = (KinectApp*)userdata;
//		pThis->mouseCallback(event, x, y, flags);
//	}
//
//	void mouseCallback(int event, int x, int y, int flags)
//	{
//		if (event == CV_EVENT_LBUTTONDOWN) {
//			getInitData();
//		}
//	}
//
//	// BodyIndexImageで振動する対象を選択するためのマウス処理
//	static void mouseCallback_bib(int event, int x, int y, int flags, void* userdata)
//	{
//		// 引数に渡したthisポインタを経由してメンバ関数に渡す
//		auto pThis = (KinectApp*)userdata;
//		pThis->mouseCallback_bib(event, x, y, flags);
//	}
//
//	void mouseCallback_bib(int event, int x, int y, int flags)
//	{
//		if (event == CV_EVENT_LBUTTONDOWN) {
//			//printf("\nbodyIndexImage-click %d,%d\n",x,y);
//
//			// クリックした場所が何番のbodyIndex領域かを取得
//
//			int i = y*bodyIndexWidth + x;
//			if (bodyIndexBuffer[i] == 255){
//				//printf("no player\n");
//			}
//			else{
//				printf("プレイヤー %d にバイブレーション設定\n", bodyIndexBuffer[i]);
//				bib_player = bodyIndexBuffer[i];
//			}
//
//		}
//	}
//
//	/////////////////////////////////////////////////////
//
//	void partner_or_own(){
//		if (partner_change == false){
//			partner_change = true;
//			std::cout << "入れ替える\n";
//			return;
//		}
//		else{
//			partner_change = false;
//			std::cout << "入れ替えない\n";
//			return;
//		}
//	}
//
//	// BodyFrameの更新
//	void updateBodyFrame()
//	{
//		// フレームを取得する
//		CComPtr<IBodyFrame> bodyFrame;
//		auto ret = bodyFrameReader->AcquireLatestFrame(&bodyFrame);
//		if (FAILED(ret)){
//			return;
//		}
//
//		// 前回のBodyを解放する
//		for (auto& body : bodies){
//			if (body != nullptr){
//				body->Release();
//				body = nullptr;
//			}
//		}
//
//		// データを取得する
//		ERROR_CHECK(bodyFrame->GetAndRefreshBodyData(PEOPLE, &bodies[0]));
//	}
//
//	// BodyIndexFrameの更新
//	void updateBodyIndexFrame()
//	{
//		// フレームを取得する
//		CComPtr<IBodyIndexFrame> bodyIndexFrame;
//		auto ret = bodyIndexFrameReader->AcquireLatestFrame(&bodyIndexFrame);
//		if (FAILED(ret)){
//			return;
//		}
//
//		// データを取得する
//		ERROR_CHECK(bodyIndexFrame->CopyFrameDataToArray(bodyIndexBuffer.size(), &bodyIndexBuffer[0]));
//	}
//
//	// DepthFrameの更新
//	void updateDepthFrame()
//	{
//		// フレームを取得する
//		CComPtr<IDepthFrame> depthFrame;
//		auto ret = depthFrameReader->AcquireLatestFrame(&depthFrame);
//		if (FAILED(ret)){
//			return;
//		}
//
//		// データを取得する
//		ERROR_CHECK(depthFrame->CopyFrameDataToArray(depthBuffer.size(), &depthBuffer[0]));
//		
//
//	}
//
//	// ColorFrameの更新
//	void updateColorFrame()
//	{
//		// フレームを取得する
//		CComPtr<IColorFrame> colorFrame;
//		auto ret = colorFrameReader->AcquireLatestFrame(&colorFrame);
//		if (FAILED(ret)){
//			return;
//		}
//
//		// 指定の形式でデータを取得する
//		ERROR_CHECK(colorFrame->CopyConvertedFrameDataToArray(
//			colorBuffer.size(), &colorBuffer[0], colorFormat));
//	}
//
//	// BodyFrameの描画
//	void drawBodyFrame()
//	{
//		// 関節の位置をDepth座標系に変換して表示する
//		cv::Mat bodyImage = cv::Mat::zeros(424, 512, CV_8UC4);
//
//		// すべてのBodyについて繰り返し描画
//		for (int body = 0; body < PEOPLE; body++){
//
//			// Bodyがなかったら終わり
//			if (bodies[body] == nullptr){
//				continue;
//			}
//
//			// 追跡できてなかったら終わり
//			BOOLEAN isTracked = false;
//			ERROR_CHECK(bodies[body]->get_IsTracked(&isTracked));
//			if (!isTracked) {
//				continue;
//			}
//
//			// この辺テスト
//			int actor_body = shape_actor[body].actor;
//			human[body].set_actor_bone(bodies[actor_body]);
//			//
//
//			/*
//			//ここから関数化したい
//			/////////////////////////////////////////////////////////////
//			//すべてのボーンについて繰り返し処理 // 二重ループを解消できる？
//			for (int i = 0; i < BONES; i++){
//
//				// 関節の位置を取得
//				Joint joints[JointType::JointType_Count];
//				bodies[body]->GetJoints(JointType::JointType_Count, joints);
//				for (auto joint : joints) {
//					if (joint.TrackingState == TrackingState::TrackingState_Tracked) {
//						drawEllipse(bodyImage, joint, 3, cv::Scalar(255, 255, 255));
//						joint_position[body][joint.JointType] << joint.Position.X, joint.Position.Y, joint.Position.Z, 1.0;
//					}
//				}
//
//				// i番目のボーンのtop，bottomがどの関節かを求めて，関節の位置を入れる
//				bone_data[body][i].bottom = joint_position[body][bone_set[i].bottom];
//				bone_data[body][i].top = joint_position[body][bone_set[i].top];
//
//				// i番目のボーンのvector(ボーンの向き)をtop-bottomから求める
//				bone_data[body][i].vector.segment(0, 3) = bone_data[body][i].top.segment(0, 3) - bone_data[body][i].bottom.segment(0, 3);
//				bone_data[body][i].vector.w() = 1.0;
//
//				bone_data[body][i].vector.segment(0, 3) = bone_data[body][i].vector.segment(0, 3).normalized();
//
//				//printf("%d %f %f\n", i, bone_data[body][i].bottom.x(), bone_data[body][i].top.x());
//			}
//			////////////////////////////////////////////////////////////
//			//ここまで関数化したい
//			*/
//			// ポーズ認識
//			//check_base_posture(bone_data[body]);
//		}
//
//		// 表示
//		//cv::imshow("BodyImage", bodyImage);
//	}
//
//	// BodyIndexFrameの描画
//	void drawBodyIndexFrame()
//	{
//		// BodyIndexをカラーデータに変換して表示する
//		cv::Mat bodyIndexImage(bodyIndexHeight, bodyIndexWidth, CV_8UC4);
//
//		for (int i = 0; i < bodyIndexWidth * bodyIndexHeight; ++i){
//			int index = i * 4;
//
//			// BodyIndexBufferが255以外ならば(=人がいれば)
//			if (bodyIndexBuffer[i] != 255){
//				auto color = colors[bodyIndexBuffer[i]];
//				if (bodyIndexBuffer[i] == bib_player){
//					color = bib_player_color;
//				}
//				bodyIndexImage.data[index + 0] = color[0];
//				bodyIndexImage.data[index + 1] = color[1];
//				bodyIndexImage.data[index + 2] = color[2];
//
//			}
//			// 人がいなければ
//			else{
//				bodyIndexImage.data[index + 0] = 0;
//				bodyIndexImage.data[index + 1] = 0;
//				bodyIndexImage.data[index + 2] = 0;
//			}
//		}
//
//		// 表示
//		//cv::imshow("BodyIndexImage", bodyIndexImage);
//
//
//	}
//
//	// DepthFrameの描画
//	void drawDepthFrame()
//	{
//		// Depthイメージを作っている．初期化もしている
//		cv::Mat depthImage = cv::Mat::zeros(depthHeight, depthWidth, CV_8UC1);
//
//		cv::Mat Imamirror2_full = cv::Mat::zeros(full_layout_test.DisplayHeight, full_layout_test.DisplayWidth, CV_8UC3);
//		// ↑CV_8UC4→CV_8UC3にしたら，メイン画面に画像を貼れるようになった．
//
//		//drawObject(depthImage, 128);
//
//		//drawBackground(Imamirror2_full);
//
//
//		// すべてのBodyについて繰り返し描画
//		for (int body = 0; body < PEOPLE; body++){
//			if (shape_actor[body].actor == -1){ // もしactorがいなかったら
//		
//				// Bodyがなかったら終わり
//				if (bodies[body] == nullptr){
//					continue;
//				}
//
//				// 追跡できてなかったら終わり
//				BOOLEAN isTracked = false;
//				ERROR_CHECK(bodies[body]->get_IsTracked(&isTracked));
//				if (!isTracked) {
//					continue;
//				}
//
//				/*
//				// ここにガイド画像表示処理
//				// 画像を貼る
//				/////////////////////////////////////////////////////////////////////////////////////
//
//				// actorの頭の位置と身長を求める
//				//actorのheadのcamera座標
//				CameraSpacePoint head_camera_point;
//				head_camera_point.X = bone_data[body][3].top.x();
//				head_camera_point.Y = bone_data[body][3].top.y();
//				head_camera_point.Z = bone_data[body][3].top.z();
//				// actorのfootのcamera座標
//				CameraSpacePoint foot_camera_point;
//				foot_camera_point.X = bone_data[body][19].top.x(); // or23
//				foot_camera_point.Y = bone_data[body][19].top.y();
//				foot_camera_point.Z = bone_data[body][19].top.z();
//				// Camera 座標系を Depth 座標系に変換
//				// mapper
//				CComPtr<ICoordinateMapper> mapper_head;
//				ERROR_CHECK(kinect->get_CoordinateMapper(&mapper_head));
//				// depthpoint
//				DepthSpacePoint head_depth_point; // 頭の位置
//				mapper_head->MapCameraPointToDepthSpace(head_camera_point, &head_depth_point);
//				DepthSpacePoint foot_depth_point; // 足の位置
//				mapper_head->MapCameraPointToDepthSpace(foot_camera_point, &foot_depth_point);
//				
//				int height = foot_depth_point.Y - head_depth_point.Y; // 頭と足の位置から画面上での身長を求める
//				height *= full_layout.magnification; // フルスクリーンに合わせる
//				if (height < 0){ // 0以下にならないように配慮
//					height = 100;
//				}
//
//				// ガイド画像を貼りつける位置を決める
//				int guide_paste_x = full_layout.margin_x + head_depth_point.X * full_layout.magnification;
//				int guide_paset_y = full_layout.margin_y + head_depth_point.Y * full_layout.magnification;
//
//				// base_posture_src = ガイド画像の元画像　Imamirror2_full = 貼られる画像
//				cv::Mat base_posture_src = cv::imread("base_posture.png"); // ガイド画像の読み込み, cv::IMREAD_UNCHANGED
//				double base_posture_aspect = (double)base_posture_src.cols / (double)base_posture_src.rows; // ガイド画像のアスペクト比 横/縦
//
//				double dst_resize = (double)height / (double)base_posture_src.rows; // 元ガイド画像から表示ガイド画像へのリサイズ割合（縦横共通）．actorの身長による．
//				//std::cout << "resize_xy " << resize_xy << " " << base_posture_src.rows*resize_xy << " " << base_posture_src.cols*resize_xy << "\n";
//				
//				guide_paste_x = full_layout.margin_x + head_depth_point.X * full_layout.magnification - base_posture_src.cols*dst_resize / 2;
//
//				// ここでフルスクリーン画面を超えないようにbase_posture_dstのサイズを調整する
//				if (guide_paset_y + base_posture_src.rows*dst_resize > full_layout.DisplayHeight + full_layout.margin_y ){
//					std::cout << "yとびでる\n";
//					
//					// 表示位置を調整するパターン
//					int offset_y = guide_paset_y + base_posture_src.rows*dst_resize - full_layout.DisplayHeight + full_layout.margin_y;
//					//guide_paset_y -= ofset_y;
//
//					// 表示サイズを調整するパターン
//					int new_dst_resize = (base_posture_src.rows * dst_resize - offset_y) / base_posture_src.rows;
//					dst_resize = new_dst_resize;
//
//				}
//				if (guide_paste_x + base_posture_src.cols*dst_resize > full_layout.DisplayWidth + full_layout.margin_x ){
//					std::cout << "xとびでる\n";
//
//					// 表示位置を調整するパターン
//					int offset_x = guide_paste_x + base_posture_src.cols*dst_resize - full_layout.DisplayWidth + full_layout.margin_x;
//					//guide_paste_x -= ofset_x;
//
//					// 表示サイズを調整するパターン
//					int new_dst_resize = (base_posture_src.cols * dst_resize - offset_x) / base_posture_src.cols;
//					dst_resize = new_dst_resize;
//				}
//
//				// 求めた値を基に表示画像base_posture_dstをつくる
//				cv::Mat base_posture_dst = cv::Mat::zeros(base_posture_src.cols*dst_resize, base_posture_src.rows*dst_resize, CV_8UC3); // 表示ガイド画像を貼るためのMatを用意．
//				// base_posture_srcをリサイズしてbase_posture_dstに出力する
//				cv::resize(base_posture_src, base_posture_dst, cv::Size(), dst_resize, dst_resize);
//				//cv::imshow("test-base_posture_dst", base_posture_dst); // 表示 // 注意：imshow()は表示するものが面積を持たない（縦横どっちかが0）だと落ちるぞ！
//				
//
//				// 貼られる画像の中の貼る画像（base_posture_dst）分の関心領域をroi_fullscreenに取り出す
//				cv::Mat roi_fullsc = Imamirror2_full(cv::Rect(guide_paste_x, guide_paset_y, base_posture_dst.cols, base_posture_dst.rows));
//				//関心領域roi_fullscにbase_posture_dstを貼りつける．
//				base_posture_dst.copyTo(roi_fullsc);
//				/////////////////////////////////////////////////////////////////////////////////////
//				*/
//
//
//				/*
//				// ★　スケルトンを描くコードをBoneクラスに入れる
//				// 以下にスケルトン表示を描く
//				//std::cout << body <<"  " << shape_actor[body].shape << "\n";
//				for (int b = 0; b < BONES; b++){
//					CameraSpacePoint cam_bone_bottom;
//					cam_bone_bottom.X = bone_data[body][b].bottom.x();
//					cam_bone_bottom.Y = bone_data[body][b].bottom.y();
//					cam_bone_bottom.Z = bone_data[body][b].bottom.z();
//					CameraSpacePoint cam_bone_top;
//					cam_bone_top.X = bone_data[body][b].top.x();
//					cam_bone_top.Y = bone_data[body][b].top.y();
//					cam_bone_top.Z = bone_data[body][b].top.z();
//
//					drawEllipse_fullScreen(Imamirror2_full, cam_bone_bottom, 6, cv::Scalar(255, 255, 255), full_layout.magnification, full_layout.margin_x, full_layout.margin_y);
//					drawEllipse_fullScreen(Imamirror2_full, cam_bone_top, 6, cv::Scalar(255, 255, 255), full_layout.magnification, full_layout.margin_x, full_layout.margin_y);
//				}*/
//			}
//		}
//
//		
//
//		/*
//		// 下の変換の根幹部分を関数化したい
//
//		// 関節の位置をDepth座標系に変換して表示する
//		cv::Mat new_bone_bodyImage = cv::Mat::zeros(424, 512, CV_8UC4);
//
//		// 身体形状（shape）が変換元，動作（actor）が変換先
//		for (int shape = 0; shape < PEOPLE; shape++){
//
//			// actorがいる時だけ描画する
//			if (shape_actor[shape].actor != -1){
//				// shapeに対する動作者
//				//int actor = shape_actor[shape].actor;
//
//				human[shape].get_translate_body(kinect, Imamirror2_full, full_layout_test, 
//					bodyIndexWidth, bodyIndexHeight);
//
//				
//				// shapeの身体でactorの姿勢のときのbottomを計算
//				Eigen::Vector4f new_bottom[BONES];
//				for (int b = 0; b < BONES; b++){
//					new_bottom[b] << 0.0, 0.0, 0.0, 1.0;
//					int parent = bone_set[b].parent;
//					if (parent == -1){
//						float y_diff = bone_data[actor][b].bottom_init.y() - bone_data[shape][b].bottom_init.y();
//						new_bottom[b] = bone_data[actor][b].bottom;
//						new_bottom[b].y() -= y_diff;
//					}
//					else {
//						new_bottom[b].segment(0, 3)
//							= new_bottom[parent].segment(0, 3)
//							+ bone_data[shape][parent].length * bone_data[actor][parent].vector.segment(0, 3).normalized();
//						new_bottom[b].w() = 1.0;
//					}
//				}
//
//				// new_bottomを描画
//				for (int b = 0; b < BONES; b++){
//					mydrawEllipse(new_bone_bodyImage, new_bottom[b], 3, cv::Scalar(255, 255, 0));
//
//					// 親指，指先，つま先，頭の場合
//					//if (b == 3){
//					Eigen::Vector4f new_top;
//					new_top.segment(0, 3)
//						= new_bottom[b].segment(0, 3)
//						+ bone_data[shape][b].length * bone_data[actor][b].vector.segment(0, 3).normalized();
//					mydrawEllipse(new_bone_bodyImage, new_top, 3, cv::Scalar(255, 255, 0));
//					//}
//				}
//
//
//				
//
//				// 振動提示の処理 /////////////////////////////////////////////////////////////
//
//				//4.送信
//				//DWORD dwSendSize;
//				//DWORD dwErrorMask;
//
//				if (actor == bib_player){
//					int new_LeftHand = 8; // 変換後の骨格の左手の位置（配列の番号）
//					int new_RightHand = 14; // 変換後の骨格の右手の位置（配列の番号）
//
//					// Camera座標系への変換
//					CameraSpacePoint bib_joint;
//					bib_joint.X = new_bottom[new_RightHand].x();
//					bib_joint.Y = new_bottom[new_RightHand].y();
//					bib_joint.Z = new_bottom[new_RightHand].z();
//
//					if (in_area(bib_joint) == true){ // 振動提示の条件　簡単にできるように関数とかにしたい．
//						if (Before_time_bib == false){
//							//Ret = WriteFile(arduino, &start_data, sizeof(start_data), &dwSendSize, NULL);
//							Before_time_bib = true;
//							//Beep(440, 200);
//
//							// 現時刻を表示する
//							end = clock();
//							std::cout << "end = " << end << "sec.\n";
//							std::cout << "time = " << (double)(end - start) / CLOCKS_PER_SEC << "sec.\n";
//
//						}
//						//printf("|");
//						//DWORD error = GetLastError();// エラー処理
//						//std::cout << error << std::endl;
//						drawObject(depthImage, 200);
//					}
//					else{
//						if (Before_time_bib == true){
//							//Ret = WriteFile(arduino, &stop_data, sizeof(start_data), &dwSendSize, NULL);
//							Before_time_bib = false;
//						}
//						//printf("_");
//						//DWORD error = GetLastError();// エラー処理
//						//std::cout << error << std::endl;
//					}
//
//					FlushFileBuffers(arduino); // CreateFile()に対してのflush．引数はファイルのハンドル
//					//Sleep(10);
//				}
//
//				///////////////////////////////////////////////////////////////////////////////
//
//
//				// 点群の変換
//				for (int p = 0; p < points_data[shape].points_num; p++){
//
//					// 点の位置を初期化
//					points_data[shape].points[p] << 0.0, 0.0, 0.0, 0.0;
//
//					for (int b = 0; b < BONES; b++){
//
//						Eigen::Vector4f v1 = bone_data[shape][b].top_init - bone_data[shape][b].bottom_init;// v1 : bottom から top
//						Eigen::Vector4f v2 = points_data[shape].points_init[p] - bone_data[shape][b].bottom_init;// v2 : Bottomから点
//						float t = v1.dot(v2) / v1.dot(v1); // t : 標本点からv1に垂線を下した点のv1内分比
//
//						float d;
//						float w;
//
//						// tを[0~1]でクランプする (dクランプと実質同じ処理)
//						if (t < 0){
//							t = 0;
//						}
//						else if (t > 1){
//							t = 1;
//						}
//
//						d = (v2 - v1 * t).norm(); // ボーンと標本点の距離
//						w = pow(d + 1.0, -16); // ｗの設定も外に出した．（処理内容は変わらないはず．）逆数じゃなくした
//
//						/*
//						// d をクランプする // 2016－11－2改善．今まで根幹内に入っていたdクランプを外に出したら表現がまともになった．（不自然な割れがなくなった）→tクランプへ
//						//if (t < 0){
//						//d = v2.norm();
//						//} else if (t > 1.0){
//						//d = (v2 - v1).norm();
//						//}
//
//						// ボーンの影響（重み）
//						Eigen::Matrix4f w_matrix;
//						//w_matrix << w, w, w, 1.0; // うまくいかない．放置．
//
//						if (d < v1.norm() * impact_range.range[b]){ // ←ここで，ボーンごとに許容範囲を変えるとうまくいきそう？
//
//							// 変換行列の全体
//							Eigen::Matrix4f M_matrix;
//							Eigen::Matrix4f M_inverse;
//							Eigen::Matrix4f B_matrix;
//
//							// 変換行列の部分
//							Eigen::Matrix4f M_transrate;
//							Eigen::Matrix4f M_rotate;
//							Eigen::Matrix4f B_transrate;
//							Eigen::Matrix4f B_rotate;
//
//							// 基本のベクトル
//							Eigen::Vector4f unit_vector(0.0, 0.0, 1.0, 1.0);
//
//							transformation trans;
//
//							// 行列 M の部分を求める
//							trans.transrate(M_transrate, bone_data[shape][b].bottom_init);
//							trans.rotate(M_rotate, unit_vector, bone_data[shape][b].vector_init);
//
//							// 初期ボーンのノルムを求める
//							float vi_norm = bone_data[shape][b].vector_init.norm();
//
//							// 行列 M の全体を求める
//							M_matrix = vi_norm * M_transrate * M_rotate;//
//							M_inverse = M_matrix.inverse();//(1/vi_norm) * M_rotate * M_transrate;
//
//							// 行列 B の部分を求める
//							trans.rotate(B_rotate, unit_vector, bone_data[actor][b].vector);
//							trans.transrate(B_transrate, new_bottom[b]);
//
//							// 変換後ボーンのノルムを求める
//							float v_norm = bone_data[actor][b].vector.norm();
//
//							// 行列 B の全体を求める
//							B_matrix = vi_norm * B_transrate * B_rotate;
//
//							// 変換後の点に b 番ボーンの影響を足し合わせる
//							points_data[shape].points[p] += w * B_matrix * M_inverse * points_data[shape].points_init[p];
//
//						}
//					}
//
//					// Camera 座標系を Depth 座標系に変換
//					CComPtr<ICoordinateMapper> mapper;
//					ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));
//
//					// Camera 座標系の点を Depth 座標系に戻す
//					CameraSpacePoint camera_point;
//					camera_point.X = points_data[shape].points[p].x();
//					camera_point.Y = points_data[shape].points[p].y();
//					camera_point.Z = points_data[shape].points[p].z();
//					//printf(" height_ratio %f \n", height_ratio);
//
//					DepthSpacePoint depth_point;
//					mapper->MapCameraPointToDepthSpace(camera_point, &depth_point);
//
//					points_data[shape].points_depth[p].x() = depth_point.X;
//					points_data[shape].points_depth[p].y() = depth_point.Y;
//					points_data[shape].points_depth[p].z() = 255;
//
//					// Depth 座標系の点を新しい座標値に変換
//					int x = (int)points_data[shape].points_depth[p].x();
//					int y = (int)points_data[shape].points_depth[p].y();
//					int new_index = y*bodyIndexWidth + x;
//
//					if (new_index >= 0 && new_index < bodyIndexHeight*bodyIndexWidth){//範囲内外判定
//						
//						//デプス画像に点を描画
//						depthImage.data[new_index] = 255;//points_data1.points[j].z;
//						
//						// 円の大きさを決める
//						//int ellipse_r = 100/(100*camera_point.Z);
//						//if (ellipse_r < 3){
//						//	ellipse_r = 3;
//						//}
//						//else if (ellipse_r > 8){
//						//	ellipse_r = 8;
//						//}
//
//						//フルスクリーンへの円描画
//						// カラー円か白円か
//						cv::Scalar full_paint_color;
//						if (color_view_tf){
//							full_paint_color = points_data[shape].color[p];
//						}
//						else{
//							full_paint_color = cv::Scalar(255, 255, 255);
//						}
//
//						DrawEllipse draw_ellipse;
//						draw_ellipse.drawEllipse_fullScreen(kinect, Imamirror2_full, camera_point, 3, full_paint_color, full_layout_test);
//
//						//drawEllipse_fullScreen(Imamirror2_full, camera_point, 3, full_paint_color,
//						//	full_layout.magnification, full_layout.margin_x, full_layout.margin_y);
//					}
//					// ↑もうちょっと簡単に書けるはずだから余裕があったら見る
//
//				}
//			}
//		}
//
//		*/
//
//		// フルスクリーン表示
//		cvNamedWindow("Ima-mirror2_full");
//		cv::setWindowProperty("Ima-mirror2_full", cv::WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN); // フルスクリーン設定
//		cv::imshow("Ima-mirror2_full", Imamirror2_full); // 表示
//
//
//		// 通常表示
//		//cv::imshow("Ima-mirror2", depthImage);
//
//		//cv::imshow("new_body_", new_bone_bodyImage);
//
//	}
//
//	// カラーデータの表示処理
//	void drawColorFrame()
//	{
//#if 0
//		// カラーデータを表示する
//		cv::Mat colorImage(colorHeight, colorWidth, CV_8UC4, &colorBuffer[0]);
//		cv::imshow("Color Image", colorImage);
//#else
//		cv::Mat colorImage(colorHeight, colorWidth, CV_8UC4, &colorBuffer[0]);
//		cv::Mat harfImage;
//		cv::resize(colorImage, harfImage, cv::Size(), 0.5, 0.5);
//		cv::imshow("Harf Image", harfImage);
//
//
//#endif
//	}
//
//	/*
//	// フルスクリーン表示設定
//	FULLSCREEN_LAYOUT fullScreen_layout_set(double depthWidth, double depthHeight){
//
//		cv::Point fullScreen_first_pixel;
//		cv::Point fullScreen_last_pixel;
//		int In_display_width; // ディスプレイ中のイマミラーの画面の幅
//		int In_display_height;// ディスプレイ中のイマミラーの画面の高さ
//
//		FULLSCREEN_LAYOUT layout;
//
//		// ディスプレイサイズ取得
//		int DisplayWidth = GetSystemMetrics(SM_CXSCREEN);
//		int DisplayHeight = GetSystemMetrics(SM_CYSCREEN);
//
//		layout.DisplayWidth = DisplayWidth;
//		layout.DisplayHeight = DisplayHeight;
//
//		if ((double)DisplayWidth / (double)DisplayHeight > (double)depthWidth / (double)depthHeight){ // ディスプレイが横長．たぶんこの確率が高い
//			In_display_width = (double)DisplayHeight * ((double)depthWidth / (double)depthHeight);
//			In_display_height = DisplayHeight;
//			fullScreen_first_pixel.x = (DisplayWidth - In_display_width) / 2;
//			fullScreen_first_pixel.y = 0;
//			fullScreen_last_pixel.x = (DisplayWidth - In_display_width) / 2 + In_display_width;
//			fullScreen_last_pixel.y = DisplayHeight;
//			layout.margin_x = fullScreen_first_pixel.x;
//			layout.margin_y = 0;
//			layout.magnification = (double)In_display_height / (double)depthHeight;
//		}
//		else if ((double)DisplayWidth / (double)DisplayHeight < (double)depthWidth / (double)depthHeight) { // ディスプレイが縦長
//			In_display_width = DisplayWidth;
//			In_display_height = (double)DisplayWidth * ((double)depthHeight / (double)depthWidth);
//			fullScreen_first_pixel.x = 0;
//			fullScreen_first_pixel.y = (DisplayHeight - In_display_height) / 2;
//			fullScreen_last_pixel.x = DisplayWidth;
//			fullScreen_last_pixel.y = (DisplayHeight - In_display_height) / 2 + In_display_height;
//			layout.margin_x = 0;
//			layout.margin_y = fullScreen_first_pixel.y;
//			layout.magnification = (double)In_display_width / (double)depthWidth;
//		}
//		else{ // depth画像のアスペクト比とディスプレイのアスペクト比がいっしょ
//			In_display_width = DisplayWidth;
//			In_display_height = DisplayHeight;
//			fullScreen_first_pixel.x = 0.0;
//			fullScreen_first_pixel.y = 0.0;
//			fullScreen_last_pixel.x = In_display_width;
//			fullScreen_last_pixel.y = In_display_height;
//			layout.margin_x = 0;
//			layout.margin_y = 0;
//			layout.magnification = (double)In_display_width / (double)depthWidth;
//		}
//
//		return layout;
//
//	}*/
//
//	// オブジェクトの描画（モノクロイメージ用）
//	void drawObject(cv::Mat depthImage, int color){
//		for (int i = 0; i < depthHeight*depthWidth; i++){
//			CameraSpacePoint camera_point;// オブジェクトのcamera座標系
//			DepthSpacePoint depth_point;
//			depth_point.X = i % depthWidth;
//			depth_point.Y = i / depthWidth;
//			int depth_z = 1000;//depthImage.data[i];
//
//			// Depth座標系をCamera座標系に変換する
//			// camera_pointにCamera座標系に変換された座標が入る
//			CComPtr<ICoordinateMapper> mapper;
//			ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));
//
//			mapper->MapDepthPointToCameraSpace(depth_point, depth_z, &camera_point);
//
//			if (in_area(camera_point) == true){
//				if (color > 255){
//					color = 255;
//				}
//				if (color < 0){
//					color = 0;
//				}
//				depthImage.data[i] = color;
//			}
//		}
//	}
//
//	/*
//	void drawBackground(cv::Mat campus){
//
//		if (background_tf){
//			//////////////////////////////////////////////////////////////////////////////////////////////////////////
//			// 背景を描画する
//			// 背景をMatに描画する関数にしたい．あとは張り付けるだけでオッケーにしたい
//
//			std::vector<ColorSpacePoint> colorSpace(depthWidth * depthHeight);
//
//			// Depth座標系に対応するColor座標系の一覧を取得する
//			CComPtr<ICoordinateMapper> mapper;
//			ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));
//			mapper->MapDepthFrameToColorSpace(background_depthBuffer.size(), &background_depthBuffer[0], colorSpace.size(), &colorSpace[0]);
//
//			for (int i = 0; i < depthHeight * depthWidth; i++){ // 背景をつける
//				if (i % 4 == 0){
//					DepthSpacePoint depth_point; //Depth座標系の点
//					depth_point.X = i % depthWidth; //Depth画像上のxを代入
//					depth_point.Y = i / depthWidth; //Depth画像上のyを代入
//
//					ColorSpacePoint color_point; //Color座標系の点
//					//mapper->MapDepthPointToColorSpace(depth_point, depthBuffer[i], &color_point);
//					mapper->MapDepthPointToColorSpace(depth_point, background_depthBuffer[i], &color_point);
//					// ↑mapperが知的．depthBufferの深度によってカラーのマッピング位置を決めている．単純な引き延ばしではない．
//
//					// colorSpaceのインデックスを求める
//					int colorX = (int)color_point.X;
//					int colorY = (int)color_point.Y;
//					if ((colorX < 0) || (colorWidth <= colorX) || (colorY < 0) || (colorHeight <= colorY)){
//						continue;
//					}
//
//					int colorIndex = (colorY * colorWidth) + colorX;
//
//					int colorBufferIndex = colorIndex * colorBytesPerPixel;
//
//					cv::Scalar color_point_color = cv::Scalar(backgroundBuffer[colorBufferIndex], backgroundBuffer[colorBufferIndex + 1], backgroundBuffer[colorBufferIndex + 2]);
//
//					int fullsc_x = full_layout.magnification*(i % depthWidth) + full_layout.margin_x;
//					int fullsc_y = full_layout.magnification*(i / depthWidth) + full_layout.margin_y;
//
//
//					campus.at<cv::Vec3b>(fullsc_y, fullsc_x)[0] = backgroundBuffer[colorBufferIndex];
//					campus.at<cv::Vec3b>(fullsc_y, fullsc_x)[1] = backgroundBuffer[colorBufferIndex + 1];
//					campus.at<cv::Vec3b>(fullsc_y, fullsc_x)[2] = backgroundBuffer[colorBufferIndex + 2];
//
//
//					// フルスクリーン座標に変換
//					cv::Point fullScreenPoint;
//					fullScreenPoint.x = fullsc_x;
//					fullScreenPoint.y = fullsc_y;
//
//					cv::circle(campus, fullScreenPoint, 5, color_point_color, -1);
//
//				}
//				//////////////////////////////////////////////////////////////////////////////////////////////////////////
//			}
//		}
//	}
//	*/
//
//	// 接触範囲の内外判定
//	bool in_area(CameraSpacePoint camera_point){
//		double left = -1.5;
//		double right = 1.5;
//		double near_ = 1.8;
//		double far_ = 2.2;
//		double high = appearLocation * 0.2;
//
//		//printf("camera_point %f,%f,%f\n", camera_point.X, camera_point.Y, camera_point.Z);
//
//		if (camera_point.X >= left && camera_point.X <= right){
//			if (camera_point.Y >= high - 0.1 && camera_point.Y <= high){
//				if (camera_point.Z >= near_ && camera_point.Z <= far_){
//					return true;
//				}
//			}
//		}
//		return false;
//	}
//
//	// オブジェクトの出現位置を変える
//	void ObjectAppearChange(){
//
//		appearLocation = rand() % 5;
//
//		// 現時刻を表示する
//		start = clock();
//		std::cout << "start = " << appearLocation << " -> " << start << "sec.\n";
//
//		return;
//	}
//	
//
//	//////////////////////////////////////////////////
//	//////////////////////////////////////////////////
//	//////////////////////////////////////////////////
//
//
//	// DepthImage上に円を描画 (DepthImage，Camera座標系の描画位置，描画半径，描画色)
//	void drawEllipse(cv::Mat &bodyImage, const Joint &joint, int r, const cv::Scalar &color)
//	{
//		// Camera座標系をDepth座標系に変換する
//		CComPtr<ICoordinateMapper> mapper;
//		ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));
//
//		DepthSpacePoint point;
//		mapper->MapCameraPointToDepthSpace(joint.Position, &point);
//
//		cv::circle(bodyImage, cv::Point(point.X, point.Y), r, color, -1);
//	}
//
//	// Eigen版：DepthImage上に円を描画 (DepthImage，Camera座標系の描画位置，描画半径，描画色)
//	void mydrawEllipse(cv::Mat &bodyImage, const Eigen::Vector4f &joint, int r, const cv::Scalar &color)
//	{
//		// Camera座標系をDepth座標系に変換する
//		CComPtr<ICoordinateMapper> mapper;
//		ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));
//
//		DepthSpacePoint point;
//		CameraSpacePoint joint3;
//		joint3.X = joint.x();
//		joint3.Y = joint.y();
//		joint3.Z = joint.z();
//		mapper->MapCameraPointToDepthSpace(joint3, &point);
//
//		cv::circle(bodyImage, cv::Point(point.X, point.Y), r, color, -1);
//	}
//	
//	// フルスクリーン用のdrawWllipse
//	void drawEllipse_fullScreen(
//		cv::Mat &bodyImage,
//		CameraSpacePoint &cam_point,
//		int r, const cv::Scalar &color,
//		double magnification,
//		int margin_x, int margin_y)
//	{
//		// Camera座標系をDepth座標系に変換する
//		CComPtr<ICoordinateMapper> mapper;
//		ERROR_CHECK(kinect->get_CoordinateMapper(&mapper));
//
//		DepthSpacePoint point;
//		mapper->MapCameraPointToDepthSpace(cam_point, &point);
//
//		// ↑ここまでmydrawEllipseと同様（変数名とか違うけど）
//
//		// フルスクリーン座標に変換
//		cv::Point fullScreenPoint;
//		fullScreenPoint.x = (int)((double)point.X*magnification + margin_x);
//		fullScreenPoint.y = point.Y*magnification + margin_y;
//
//		cv::circle(bodyImage, fullScreenPoint, r, color, -1);
//	}
//
//	void draw_circle(
//		cv::Mat &bodyImage,
//		DepthSpacePoint depth_point,
//		int r, const cv::Scalar &color,
//		double magnification,
//		int margin_x, int margin_y){
//
//		// フルスクリーン座標に変換
//		cv::Point fullScreenPoint;
//		fullScreenPoint.x = depth_point.X*magnification + margin_x;
//		fullScreenPoint.y = depth_point.Y*magnification + margin_y;
//
//		cv::circle(bodyImage, fullScreenPoint, r, color, -1);
//	}
//	
//
//	// 基本のポーズであるかをチェックする
//	/*bool check_base_posture(BONE_DATA *bone_data){
//
//		//(引数から)両の肩，肘，手首，左右の腰，膝，足首の関節を取得する
//		//またはボーンを取得
//
//		// チェック項目
//		//右腕
//		bool check_right_upper_arm = false;
//		bool check_right_lower_arm = false;
//
//		//右脚
//		bool check_right_upper_leg = false;
//		bool check_right_lower_leg = false;
//
//		//左腕
//		bool check_left_upper_arm = false;
//		bool check_left_lower_arm = false;
//
//		//左脚
//		bool check_left_upper_leg = false;
//		bool check_left_lower_leg = false;
//
//		bool all_check = false;
//
//		// ボーンのbottom→top方向から姿勢を判断する
//
//		// 右上腕，右前腕のb-t方向がx:0.4~0.6,y<0
//		if (bone_data[11].vector.x() >= 0.4 && bone_data[11].vector.x() <= 0.6 && bone_data[11].vector.y() < 0){
//			check_right_upper_arm = true;
//		}
//		if (bone_data[12].vector.x() >= 0.4 && bone_data[12].vector.x() <= 0.6 && bone_data[11].vector.y() < 0){
//			check_right_lower_arm = true;
//		}
//
//		//右太もも，右脛のb-t方向がx:0.4~0.6,y<0
//		if (bone_data[21].vector.x() >= 0.1 && bone_data[21].vector.x() <= 0.3 && bone_data[21].vector.y() < 0){
//			check_right_upper_leg = true;
//		}
//		if (bone_data[22].vector.x() >= 0.1 && bone_data[22].vector.x() <= 0.3 && bone_data[22].vector.y() < 0){
//			check_right_lower_leg = true;
//		}
//
//
//		//左上腕，左前腕のb-t方向がx:-0.4~-0.6,y<0
//		if (bone_data[5].vector.x() >= -0.6 && bone_data[5].vector.x() <= -0.4 && bone_data[5].vector.y() < 0){
//			check_left_upper_arm = true;
//		}
//		if (bone_data[6].vector.x() >= -0.6 && bone_data[6].vector.x() <= -0.4 && bone_data[6].vector.y() < 0){
//			check_left_lower_arm = true;
//		}
//
//
//		//左太もも，左脛のb-t方向がx:-0.4~-0.6
//		if (bone_data[17].vector.x() >= -0.2 && bone_data[17].vector.x() <= -0.1 && bone_data[17].vector.y() < 0){
//			check_left_upper_leg = true;
//		}
//		if (bone_data[18].vector.x() >= -0.2 && bone_data[18].vector.x() <= -0.1 && bone_data[18].vector.y() < 0){
//			check_left_lower_leg = true;
//		}
//
//
//		std::cout << check_right_upper_arm << " " << check_right_lower_arm << " ";
//		std::cout << check_right_upper_leg << " " << check_right_lower_leg << "  ";
//		std::cout << check_left_upper_arm << " " << check_left_lower_arm << " ";
//		std::cout << check_left_upper_leg << " " << check_left_lower_leg << "\n";
//
//		return all_check;
//
//	}*/
//
//};
//
//// Bluetoothとの接続処理
//void bluetooth_conect(){
//
//	printf("Bluetooth接続開始\n");
//
//	//1.ポートをオープン
//	wchar_t COM_[] = L"COM4";
//	arduino = CreateFile(COM_, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	//2014/01/22追記　これでつながらない場合には"\\\\.\\COM7"とするとつながるかもしれません。
//
//	if (arduino == INVALID_HANDLE_VALUE){
//		DWORD error = GetLastError();
//		std::cout << error << std::endl;
//		printf("PORT COULD NOT OPEN \n");
//		system("PAUSE");
//		exit(0); // 終了しない
//	}
//	//2.送受信バッファ初期化
//	Ret = SetupComm(arduino, 1024, 1024);
//	if (!Ret){
//		printf("SET UP FAILED\n");
//		CloseHandle(arduino);
//		system("PAUSE");
//		exit(0); // 終了しない
//	}
//	Ret = PurgeComm(arduino, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
//	if (!Ret){
//		printf("CLEAR FAILED\n");
//		CloseHandle(arduino);
//		exit(0); // 終了しない
//	}
//	//3.基本通信条件の設定
//	DCB dcb;
//	GetCommState(arduino, &dcb);
//	dcb.DCBlength = sizeof(DCB);
//	dcb.BaudRate = 9600;
//	dcb.fBinary = TRUE;
//	dcb.ByteSize = 8;
//	dcb.fParity = NOPARITY;
//	dcb.StopBits = ONESTOPBIT;
//
//	return;
//}
//
//int main(int argc, char *argv[])
//{
//	//bluetooth_conect();// bluetooth接続
//	std::cout << "main_tmpひらいてるよ";
//	try {
//		KinectApp app;		// インスタンスを生成して，
//		app.initialize();	// いろいろ初期化して，
//		//app.getInitData();	// 変換元のデータを取得して，
//		app.run();			// ひたすらループを回す
//	}
//	catch (std::exception& ex){
//		std::cout << ex.what() << std::endl;
//	}
//	return 0;
//}
//
