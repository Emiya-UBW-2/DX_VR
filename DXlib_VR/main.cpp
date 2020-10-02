#define FRAME_RATE 90.f
#include"DXLib_ref/DXLib_ref.h"

class GRASS {
private:
	int grasss = 50;				/*grassの数*/
	std::vector<VERTEX3D> grassver; /*grass*/
	std::vector<DWORD> grassind;    /*grass*/
	int VerBuf, IndexBuf;			/*grass*/
	MV1 grass;						/*grassモデル*/
	GraphHandle graph;				/*画像ハンドル*/
	int IndexNum, VerNum;			/*grass*/
	int vnum, pnum;					/*grass*/
	MV1_REF_POLYGONLIST RefMesh;	/*grass*/
public:
	GRASS(void) {
	}
	void set_grass_ready_before(void) {
		SetUseASyncLoadFlag(FALSE);
		graph = GraphHandle::Load("data/map/grass/grass.png");		 /*grass*/
		MV1::Load("data/map/grass/grass.mv1", &grass, false);		/*grass*/
		SetUseASyncLoadFlag(FALSE);
		return;
	}
	void set_grass_cancel(void) {
		SetASyncLoadFinishDeleteFlag(graph.get());										 /*grass*/
		SetASyncLoadFinishDeleteFlag(grass.get());		/*grass*/
	}
	bool set_grass_ready(const char* buf,const float x_max=10.f,const float z_max = 10.f, const float x_min = -10.f, const float z_min = -10.f) {
		/*grass*/
		MV1SetupReferenceMesh(grass.get(), -1, TRUE); /*参照用メッシュの作成*/
		RefMesh = MV1GetReferenceMesh(grass.get(), -1, TRUE); /*参照用メッシュの取得*/
		IndexNum = RefMesh.PolygonNum * 3 * grasss; /*インデックスの数を取得*/
		VerNum = RefMesh.VertexNum * grasss;	/*頂点の数を取得*/
		grassver.resize(VerNum);   /*頂点データとインデックスデータを格納するメモリ領域の確保*/
		grassind.resize(IndexNum); /*頂点データとインデックスデータを格納するメモリ領域の確保*/

		vnum = 0;
		pnum = 0;

		int grass_pos = LoadSoftImage(buf);
		int xs = 0,ys = 0;
		GetSoftImageSize(grass_pos, &xs, &ys);

		for (int i = 0; i < grasss; ++i) {

			float x_t = (float)(GetRand(int((x_max - x_min)) * 100) - int(x_max - x_min) * 50) / 100.0f;
			float z_t = (float)(GetRand(int((z_max - z_min)) * 100) - int(z_max - z_min) * 50) / 100.0f;
			int _r_, _g_, _b_, _a_;
			while (1) {
				GetPixelSoftImage(grass_pos, int((x_t - x_min)/ (x_max - x_min)*float(xs)), int((z_t - z_min) / (z_max - z_min)*float(ys)), &_r_, &_g_, &_b_, &_a_);
				if (_r_ <= 128) {
					break;
				}
				else {
					x_t = (float)(GetRand(int((x_max - x_min)) * 100) - int(x_max - x_min) * 50) / 100.0f;
					z_t = (float)(GetRand(int((z_max - z_min)) * 100) - int(z_max - z_min) * 50) / 100.0f;
				}
			}
			VECTOR_ref tmpvect = VGet(x_t, 0.0f, z_t);
			//
			MV1SetMatrix(grass.get(), MMult(MMult(MGetRotY(deg2rad(GetRand(90))), MGetRotVec2(VGet(0, 1.f, 0), VGet(0, 1.f, 0))), MGetTranslate(tmpvect.get())));
			//上省
			MV1RefreshReferenceMesh(grass.get(), -1, TRUE);       /*参照用メッシュの更新*/
			RefMesh = MV1GetReferenceMesh(grass.get(), -1, TRUE); /*参照用メッシュの取得*/
			for (int j = 0; j < RefMesh.VertexNum; ++j) {
				auto& g = grassver[j + vnum];
				g.pos = RefMesh.Vertexs[j].Position;
				g.norm = RefMesh.Vertexs[j].Normal;
				g.dif = RefMesh.Vertexs[j].DiffuseColor;
				g.spc = RefMesh.Vertexs[j].SpecularColor;
				g.u = RefMesh.Vertexs[j].TexCoord[0].u;
				g.v = RefMesh.Vertexs[j].TexCoord[0].v;
				g.su = RefMesh.Vertexs[j].TexCoord[1].u;
				g.sv = RefMesh.Vertexs[j].TexCoord[1].v;
			}
			for (size_t j = 0; j < size_t(RefMesh.PolygonNum); ++j) {
				for (size_t k = 0; k < std::size(RefMesh.Polygons[j].VIndex); ++k)
					grassind[j * 3 + k + pnum] = WORD(RefMesh.Polygons[j].VIndex[k] + vnum);
			}
			vnum += RefMesh.VertexNum;
			pnum += RefMesh.PolygonNum * 3;
		}
		VerBuf = CreateVertexBuffer(VerNum, DX_VERTEX_TYPE_NORMAL_3D);
		IndexBuf = CreateIndexBuffer(IndexNum, DX_INDEX_TYPE_32BIT);
		SetVertexBufferData(0, grassver.data(), VerNum, VerBuf);
		SetIndexBufferData(0, grassind.data(), IndexNum, IndexBuf);
		return true;
	}
	void draw_grass(void) {
		DrawPolygonIndexed3D_UseVertexBuffer(VerBuf, IndexBuf, graph.get(), TRUE);
	}
	void delete_grass(void) {
		graph.Dispose();
		grass.Dispose();
		grassver.clear();
		grassind.clear();
	}

};

//player
class Chara {
	class sendstat {

	public:
		MV1::ani anime[9];
		MATRIX_ref bodys_f;

		MATRIX_ref head_f;
		MATRIX_ref RIGHTarm1_f;
		MATRIX_ref RIGHTarm2_f;
		MATRIX_ref RIGHThand_f;
		MATRIX_ref LEFTarm1_f;
		MATRIX_ref LEFTarm2_f;
		MATRIX_ref LEFThand_f;
		MATRIX_ref bodyg_f;
		MATRIX_ref bodyb_f;
		MATRIX_ref body_f;
		bool start_c = true;

		void get_data(Chara& data) {
			for (int i = 0; i < 9; i++) {
				this->anime[i].per = data.body.get_anime(i).per;
				this->anime[i].time = data.body.get_anime(i).time;
			}
			this->bodys_f = data.body.GetMatrix();
			this->head_f = data.body.GetFrameLocalMatrix(data.head_f.first);
			this->RIGHTarm1_f = data.body.GetFrameLocalMatrix(data.RIGHTarm1_f.first);
			this->RIGHTarm2_f = data.body.GetFrameLocalMatrix(data.RIGHTarm2_f.first);
			this->RIGHThand_f = data.body.GetFrameLocalMatrix(data.RIGHThand_f.first);
			this->LEFTarm1_f = data.body.GetFrameLocalMatrix(data.LEFTarm1_f.first);
			this->LEFTarm2_f = data.body.GetFrameLocalMatrix(data.LEFTarm2_f.first);
			this->LEFThand_f = data.body.GetFrameLocalMatrix(data.LEFThand_f.first);
			this->bodyg_f = data.body.GetFrameLocalMatrix(data.bodyg_f.first);
			this->bodyb_f = data.body.GetFrameLocalMatrix(data.bodyb_f.first);
			this->body_f = data.body.GetFrameLocalMatrix(data.body_f.first);
		}
		void put_data(Chara& data) {
			for (int i = 0; i < 9; i++) {
				data.body.get_anime(i).per = this->anime[i].per;
				data.body.get_anime(i).time = this->anime[i].time;
			}
			data.body.SetMatrix(this->bodys_f*MATRIX_ref::Mtrans(VGet(0, 0, 1.f)));
			data.body.SetFrameLocalMatrix(data.head_f.first, this->head_f);
			data.body.SetFrameLocalMatrix(data.RIGHTarm1_f.first, this->RIGHTarm1_f);
			data.body.SetFrameLocalMatrix(data.RIGHTarm2_f.first, this->RIGHTarm2_f);
			data.body.SetFrameLocalMatrix(data.RIGHThand_f.first, this->RIGHThand_f);
			data.body.SetFrameLocalMatrix(data.LEFTarm1_f.first, this->LEFTarm1_f);
			data.body.SetFrameLocalMatrix(data.LEFTarm2_f.first, this->LEFTarm2_f);
			data.body.SetFrameLocalMatrix(data.LEFThand_f.first, this->LEFThand_f);
			data.body.SetFrameLocalMatrix(data.bodyg_f.first, this->bodyg_f);
			data.body.SetFrameLocalMatrix(data.bodyb_f.first, this->bodyb_f);
			data.body.SetFrameLocalMatrix(data.body_f.first, this->body_f);
		}
	};
public:
	sendstat senddata;
	/*モデル、音声*/
	MV1 body;
	/**/
	float reload_cnt = 0.f;//リロード開始までのカウント
	switchs trigger;//トリガー
	bool gunf = false;//射撃フラグ
	switchs selkey;//トリガー
	bool LEFT_hand = false;//左手を添えているか
	bool reloadf = false;
	bool down_mag = false;
	//プレイヤー座標系
	VECTOR_ref pos;
	MATRIX_ref mat;
	//マガジン座標系
	VECTOR_ref pos_mag;
	MATRIX_ref mat_mag;
	//頭部座標系
	VECTOR_ref pos_HMD;
	MATRIX_ref mat_HMD;
	float add_ypos = 0.f;//垂直加速度
	float body_xrad = 0.f;//胴体角度
	float body_yrad = 0.f;//胴体角度
	float body_zrad = 0.f;//胴体角度
	frames head_f;
	float head_hight = 0.f;
	frames LEFTeye_f;
	frames RIGHTeye_f;
	//
	frames bodyg_f;
	frames bodyc_f;
	frames bodyb_f;
	frames body_f;
	//右手座標系
	VECTOR_ref pos_RIGHTHAND;
	MATRIX_ref mat_RIGHTHAND;
	VECTOR_ref vecadd_RIGHTHAND, vecadd_RIGHTHAND_p;//
	frames RIGHThand2_f;
	frames RIGHThand_f;
	frames RIGHTarm2_f;
	frames RIGHTarm1_f;
	//左手座標系
	VECTOR_ref pos_LEFTHAND;
	MATRIX_ref mat_LEFTHAND;
	frames LEFThand2_f;
	frames LEFThand_f;
	frames LEFTarm2_f;
	frames LEFTarm1_f;
	//
	bool start_c = true;

	void Ready_chara(MV1& hand_) {
		//手
		hand_.DuplicateonAnime(&this->body);
		for (int i = 0; i < int(this->body.frame_num()); i++) {
			std::string p = this->body.frame_name(i);
			if (p == std::string("グルーブ")) {
				bodyg_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p == std::string("下半身")) {
				bodyc_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("左足") != std::string::npos) {

			}
			else if (p.find("左ひざ") != std::string::npos) {

			}
			else if (p.find("左足首") != std::string::npos) {

			}
			else if (p.find("左足首先") != std::string::npos) {

			}
			else if (p.find("右足") != std::string::npos) {

			}
			else if (p.find("右ひざ") != std::string::npos) {

			}
			else if (p.find("右足首") != std::string::npos) {

			}
			else if (p.find("右足首先") != std::string::npos) {

			}
			else if (p.find("下半身先") != std::string::npos) {

			}
			else if (p.find("上半身") != std::string::npos && p.find("上半身2") == std::string::npos) {
				bodyb_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("上半身2") != std::string::npos) {
				body_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("首") != std::string::npos && p.find("手") == std::string::npos) {

			}
			else if (p.find("頭") != std::string::npos && p.find("先") == std::string::npos) {
				head_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				head_hight = this->body.frame(head_f.first).y();
			}
			else if (p.find("右目先") != std::string::npos) {
				RIGHTeye_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("左目先") != std::string::npos) {
				LEFTeye_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("頭先") != std::string::npos) {

			}
			else if (p.find("右肩") != std::string::npos) {

			}
			else if (p.find("右腕") != std::string::npos && p.find("捩") == std::string::npos) {
				RIGHTarm1_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("右腕捩") != std::string::npos) {
			}
			else if (p.find("右ひじ") != std::string::npos) {
				RIGHTarm2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("右手捩") != std::string::npos) {
			}
			else if (p == std::string("右手首")) {
				RIGHThand_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("右手首先") != std::string::npos) {
				RIGHThand2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("左肩") != std::string::npos) {

			}
			else if (p.find("左腕") != std::string::npos && p.find("捩") == std::string::npos) {
				LEFTarm1_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("左腕捩") != std::string::npos) {

			}
			else if (p.find("左ひじ") != std::string::npos) {
				LEFTarm2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("左手捩") != std::string::npos) {

			}
			else if (p == std::string("左手首")) {
				LEFThand_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
			else if (p.find("左手首先") != std::string::npos) {
				LEFThand2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
			}
		}
	}
	void Set_chara_Position(const VECTOR_ref& pos_, const MATRIX_ref& mat_, const MATRIX_ref& mat_H) {
		this->pos = pos_;
		this->mat = mat_;
		this->mat_HMD = mat_H;
	}
	void Draw_chara() {
		this->body.DrawModel();
	}
	void Delete_chara() {
	}
	void Init_chara() {
		this->body.Dispose();
		Delete_chara();
	}
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	auto Drawparts = std::make_unique<DXDraw>("FPS_0", FRAME_RATE, true);	/*汎用クラス*/
	auto Debugparts = std::make_unique<DeBuG>(FRAME_RATE);	/*デバッグクラス*/
	auto grassparts = std::make_unique<GRASS>();	/*草クラス*/
	//
	VECTOR_ref pos_track0;
	MATRIX_ref mat_track0;
	VECTOR_ref pos_track1;
	MATRIX_ref mat_track1;
	VECTOR_ref pos_track2;
	MATRIX_ref mat_track2;

	VECTOR_ref gunpos_TPS = VGet(-0.1f, -0.1f, -0.25f);

	Chara chara;
	MV1 body_obj;												//身体モデル

	DXDraw::cam_info cams;

	//プレイヤー操作変数群
	switchs TPS, ads, chgun, delgun, usegun, change_gun;		//操作スイッチ
	float xrad_p = 0.f;											//マウスエイム用変数確保
	switchs squat;
	VECTOR_ref add_pos, add_pos_buf;							//移動

	MV1 map_mod;
	MV1::Load("data/map/model.mv1", &map_mod, false);
	map_mod.SetMatrix(MGetIdent());

	MV1 sky_mod;
	MV1::Load("data/map/sky/model.mv1", &sky_mod, false);
	sky_mod.SetMatrix(MGetIdent());

	MV1::Load("data/body/model.mv1", &body_obj, false);				/*身体*/

	//自機セット
	chara.Ready_chara(body_obj);
	chara.Set_chara_Position(VGet(0.0f, 0.0f, 0.f), MGetIdent(), MATRIX_ref::RotY(0));
	chara.start_c = true;

	grassparts->set_grass_ready_before();

	grassparts->set_grass_ready("data/grassput.bmp",5.f,5.f,-5.f,-5.f);
	//
	bool startp = false;
	cams.set_cam_info(VGet(0, 1.f, 0), VGet(0, 0, -1.f), VGet(0, 1.f, 0), deg2rad(90), 0.1f, 5000.f);//カメラ
	{
		Drawparts->Set_Light_Shadow(VGet(10.f, 10.f, 10.f), VGet(-10.f, -10.f, -10.f), VGet(-0.5f, -0.5f, -0.5f), [&] {/*map_mod.DrawModel();*/ });	//ライト、影
		//main_loop
		while (ProcessMessage() == 0) {
			const float fps = GetFPS();
			const auto waits = GetNowHiPerformanceCount();
			Debugparts->put_way();
			//座標取得
			//Drawparts->GetDevicePositionVR(Drawparts->tracker_num[0], &pos_track0, &mat_track0);
			//Drawparts->GetDevicePositionVR(Drawparts->tracker_num[1], &pos_track1, &mat_track1);
			//Drawparts->GetDevicePositionVR(Drawparts->tracker_num[2], &pos_track2, &mat_track2);
			//プレイヤー操作
			{
				//HMD_mat

				if (Drawparts->use_vr) {
					//+視点取得
					Drawparts->GetDevicePositionVR(Drawparts->get_hmd_num(), &chara.pos_HMD, &chara.mat_HMD);
				}
				else {
					auto qkey = (CheckHitKey(KEY_INPUT_Q) != 0);
					auto ekey = (CheckHitKey(KEY_INPUT_E) != 0);
					chara.mat_HMD *= MATRIX_ref::RotAxis(chara.mat_HMD.zvec(), chara.body_zrad).Inverse();
					if (qkey) {
						easing_set(&chara.body_zrad, deg2rad(-30), 0.9f, fps);
					}
					else if (ekey) {
						easing_set(&chara.body_zrad, deg2rad(30), 0.9f, fps);
					}
					else {
						easing_set(&chara.body_zrad, 0.f, 0.9f, fps);
					}
					chara.mat_HMD *= MATRIX_ref::RotAxis(chara.mat_HMD.zvec(), chara.body_zrad);
					int x_m, y_m;
					GetMousePoint(&x_m, &y_m);
					chara.mat_HMD = MATRIX_ref::RotX(-xrad_p)*chara.mat_HMD;
					xrad_p = std::clamp(xrad_p - deg2rad(std::clamp(y_m - desky / 2, -120, 120))*0.1f, deg2rad(-45), deg2rad(45));
					chara.mat_HMD *= MATRIX_ref::RotY(deg2rad(std::clamp(x_m - deskx / 2, -120, 120))*0.1f);
					chara.mat_HMD = MATRIX_ref::RotX(xrad_p)*chara.mat_HMD;
					SetMousePoint(deskx / 2, desky / 2);
					SetMouseDispFlag(FALSE);
				}
				//移動
				if (Drawparts->use_vr) {
					if (Drawparts->get_hand2_num() != -1) {
						auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hand2_num()];
						if (ptr_.turn && ptr_.now) {
							if ((ptr_.on[1] & BUTTON_TOUCHPAD) != 0) {
								auto speed = 2.f;
								easing_set(&add_pos_buf, (chara.mat_HMD.zvec()*ptr_.touch.y() + chara.mat_HMD.xvec()*ptr_.touch.x())*-speed / fps, 0.95f, fps);
							}
							else {
								easing_set(&add_pos_buf, VGet(0, 0, 0), 0.95f, fps);
							}
							if (chara.add_ypos == 0.f) {
								if ((ptr_.on[0] & BUTTON_SIDE) != 0) {
									chara.add_ypos = 0.05f;
								}
								add_pos = add_pos_buf;
							}
							else {
								easing_set(&add_pos, VGet(0, 0, 0), 0.995f, fps);
							}
						}
					}
				}
				else {
					auto wkey = (CheckHitKey(KEY_INPUT_W) != 0);
					auto skey = (CheckHitKey(KEY_INPUT_S) != 0);
					auto akey = (CheckHitKey(KEY_INPUT_A) != 0);
					auto dkey = (CheckHitKey(KEY_INPUT_D) != 0);
					auto jampkey = (CheckHitKey(KEY_INPUT_SPACE) != 0);
					squat.get_in(CheckHitKey(KEY_INPUT_C) != 0);
					auto speed = ((ads.first ? 2.f : 4.f)*(squat.first ? 0.4f : 1.f));
					VECTOR_ref zv_t = chara.mat_HMD.zvec();
					zv_t.y(0.f);
					zv_t = zv_t.Norm();

					VECTOR_ref xv_t = chara.mat_HMD.xvec();
					xv_t.y(0.f);
					xv_t = xv_t.Norm();

					if (wkey) {
						easing_set(&add_pos_buf, zv_t*-speed / fps, 0.95f, fps);
					}
					if (skey) {
						easing_set(&add_pos_buf, zv_t*speed / fps, 0.95f, fps);
					}
					if (akey) {
						easing_set(&add_pos_buf, xv_t*speed / fps, 0.95f, fps);
					}
					if (dkey) {
						easing_set(&add_pos_buf, xv_t*-speed / fps, 0.95f, fps);
					}
					if (!wkey && !skey && !akey && !dkey) {
						easing_set(&add_pos_buf, VGet(0, 0, 0), 0.95f, fps);
					}
					if (chara.add_ypos == 0.f) {
						if (jampkey && !squat.first) {
							chara.add_ypos = 0.05f;
						}
						add_pos = add_pos_buf;
					}
					else {
						easing_set(&add_pos, VGet(0, 0, 0), 0.995f, fps);
					}
				}
				//壁その他の判定
				{
					VECTOR_ref pos_t = chara.pos;
					pos_t += add_pos;
					//壁
					{
						//mapparts->map_col_wall(chara.pos, &pos_t);
						if ((add_pos - (pos_t - chara.pos)).size() != 0.f) {
							add_pos = pos_t - chara.pos;
							if (chara.add_ypos == 0.f) {
								add_pos_buf = add_pos;
							}
						}
					}
					//落下
					{
						//auto pp = mapparts->map_col_line(pos_t + VGet(0, 1.6f, 0), pos_t, 0);
						if (chara.add_ypos <= 0.f && true) {
							if (VECTOR_ref(VGet(0, 1.f, 0.f)).dot(VGet(0,1,0)) >= cos(deg2rad(30))) {
								//pos_t = pp.HitPosition;
							}
							else {
								//ブロックするベクトル
								auto v_t = VECTOR_ref(VGet(0, 1, 0));
								v_t.y(0);
								v_t = v_t.Norm();
								//
								pos_t -= add_pos;
								add_pos += v_t * add_pos.cross(v_t.cross(add_pos).Norm()).size();
								if (chara.add_ypos == 0.f) {
									add_pos_buf = add_pos;
								}
								pos_t += add_pos;
							}
							chara.add_ypos = 0.f;
						}
						else {
							pos_t.yadd(chara.add_ypos);
							chara.add_ypos -= 9.8f / std::powf(fps, 2.f);
							//復帰
							if (pos_t.y() <= -5.f) {
								pos_t = VGet(0.f, 9.f, 0.f);
								chara.add_ypos = 0.f;
								chara.body.SetMatrix(chara.mat*MATRIX_ref::Mtrans(pos_t));
								chara.body.PhysicsResetState();
							}
						}
					}
					//反映
					chara.pos = pos_t;
				}
				//pos
				{
					VECTOR_ref v_ = chara.mat_HMD.zvec();
					float x_1 = -sinf(chara.body_yrad);
					float y_1 = cosf(chara.body_yrad);
					float x_2 = v_.x();
					float y_2 = -v_.z();
					float r_ = std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
					chara.body_yrad += r_ * FRAME_RATE / fps / 2.f;
				}
				{
					VECTOR_ref v_ = chara.mat_HMD.zvec();
					float x_1 = sinf(chara.body_xrad);
					float y_1 = -cosf(chara.body_xrad);
					float x_2 = -v_.y();
					float y_2 = -std::hypotf(v_.x(), v_.z());
					chara.body_xrad += std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
				}
				MATRIX_ref t_inv = MATRIX_ref::RotY(chara.body_yrad);
				if (Drawparts->use_vr) {
					//身体
					MATRIX_ref m_inv = t_inv;
					{
						chara.body.SetMatrix(chara.mat*m_inv  *MATRIX_ref::Mtrans(chara.pos + chara.pos_HMD));
						chara.body.SetMatrix(chara.mat*m_inv *MATRIX_ref::Mtrans((chara.body.frame(chara.RIGHTeye_f.first) + (chara.body.frame(chara.LEFTeye_f.first) - chara.body.frame(chara.RIGHTeye_f.first))*0.5f)));
						//chara.body.SetFrameLocalMatrix(chara.bodyc_f.first, m_inv*MATRIX_ref::Mtrans(chara.bodyc_f.second));
						//chara.body.SetFrameLocalMatrix(chara.body_f.first, m_inv*MATRIX_ref::Mtrans(chara.body_f.second));
					}
					//頭部
					chara.body.SetFrameLocalMatrix(chara.head_f.first, chara.mat_HMD*m_inv.Inverse()*MATRIX_ref::Mtrans(chara.head_f.second));
					//足
					{

					}
					//手
					{
						chara.body.get_anime(3).per = 0.f;
						chara.body.get_anime(3).time = 0.f;
						//右手
						{
							Drawparts->GetDevicePositionVR(Drawparts->get_hand1_num(), &chara.pos_RIGHTHAND, &chara.mat_RIGHTHAND);
							chara.mat_RIGHTHAND = chara.mat_RIGHTHAND*MATRIX_ref::RotAxis(chara.mat_RIGHTHAND.xvec(), deg2rad(-60));
							chara.mat_RIGHTHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), chara.vecadd_RIGHTHAND)*chara.mat_RIGHTHAND;//リコイル
							//銃器
							VECTOR_ref tgt_pt = chara.pos_RIGHTHAND + chara.pos;
							//
							VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - chara.body.frame(chara.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());//基準
							VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0とする
							float cos_t = getcos_tri((chara.body.frame(chara.RIGHThand_f.first) - chara.body.frame(chara.RIGHTarm2_f.first)).size(), (chara.body.frame(chara.RIGHTarm2_f.first) - chara.body.frame(chara.RIGHTarm1_f.first)).size(), (chara.body.frame(chara.RIGHTarm1_f.first) - tgt_pt).size());
							VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
							//上腕
							chara.body.SetFrameLocalMatrix(chara.RIGHTarm1_f.first, MATRIX_ref::Mtrans(chara.RIGHTarm1_f.second));
							MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(chara.body.frame(chara.RIGHTarm2_f.first) - chara.body.frame(chara.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
							chara.body.SetFrameLocalMatrix(chara.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(chara.RIGHTarm1_f.second));
							//下腕
							chara.body.SetFrameLocalMatrix(chara.RIGHTarm2_f.first, MATRIX_ref::Mtrans(chara.RIGHTarm2_f.second));
							MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(chara.body.frame(chara.RIGHThand_f.first) - chara.body.frame(chara.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - chara.body.frame(chara.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
							chara.body.SetFrameLocalMatrix(chara.RIGHTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(chara.RIGHTarm2_f.second));
							//手
							chara.body.SetFrameLocalMatrix(chara.RIGHThand_f.first,
								MATRIX_ref::RotY(deg2rad(-10))*
								MATRIX_ref::RotZ(deg2rad(50))*
								MATRIX_ref::RotX(deg2rad(90))*
								chara.mat_RIGHTHAND*
								m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(chara.RIGHThand_f.second));
							//右人差し指
							chara.body.get_anime(0).per = 1.f;
							chara.body.get_anime(5).per = 1.f;
						}
						//左手
						{
							Drawparts->GetDevicePositionVR(Drawparts->get_hand2_num(), &chara.pos_LEFTHAND, &chara.mat_LEFTHAND);
							chara.mat_LEFTHAND = chara.mat_LEFTHAND*MATRIX_ref::RotAxis(chara.mat_LEFTHAND.xvec(), deg2rad(-60));
							chara.LEFT_hand = false;
							{
								VECTOR_ref vec_a1 = MATRIX_ref::Vtrans(((chara.pos + chara.pos_LEFTHAND) - chara.body.frame(chara.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//基準
								VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0とする
								float cos_t = getcos_tri((chara.body.frame(chara.LEFThand_f.first) - chara.body.frame(chara.LEFTarm2_f.first)).size(), (chara.body.frame(chara.LEFTarm2_f.first) - chara.body.frame(chara.LEFTarm1_f.first)).size(), (chara.body.frame(chara.LEFTarm1_f.first) - (chara.pos + chara.pos_LEFTHAND)).size());
								VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
								//上腕
								chara.body.SetFrameLocalMatrix(chara.LEFTarm1_f.first, MATRIX_ref::Mtrans(chara.LEFTarm1_f.second));
								MATRIX_ref a1_inv = MATRIX_ref::RotVec2(
									MATRIX_ref::Vtrans(chara.body.frame(chara.LEFTarm2_f.first) - chara.body.frame(chara.LEFTarm1_f.first), m_inv.Inverse()),
									vec_t
								);
								chara.body.SetFrameLocalMatrix(chara.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(chara.LEFTarm1_f.second));
								//下腕
								chara.body.SetFrameLocalMatrix(chara.LEFTarm2_f.first, MATRIX_ref::Mtrans(chara.LEFTarm2_f.second));
								MATRIX_ref a2_inv = MATRIX_ref::RotVec2(
									MATRIX_ref::Vtrans(chara.body.frame(chara.LEFThand_f.first) - chara.body.frame(chara.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()),
									MATRIX_ref::Vtrans((chara.pos + chara.pos_LEFTHAND) - chara.body.frame(chara.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse())
								);
								chara.body.SetFrameLocalMatrix(chara.LEFTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(chara.LEFTarm2_f.second));
								//手
								chara.body.SetFrameLocalMatrix(chara.LEFThand_f.first,
									MATRIX_ref::RotZ(deg2rad(-60))*
									MATRIX_ref::RotX(deg2rad(80))*
									chara.mat_LEFTHAND*
									m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(chara.LEFThand_f.second));
							}
						}
					}
				}
				else {
					//身体
					MATRIX_ref m_inv = MATRIX_ref::RotY(deg2rad(30))*MATRIX_ref::RotZ(chara.body_zrad)*MATRIX_ref::RotX(chara.body_xrad)*t_inv;
					MATRIX_ref mb_inv = MATRIX_ref::RotY(deg2rad(15))*t_inv;
					MATRIX_ref mg_inv = t_inv;
					{
						if (chara.reloadf) {
							m_inv = MATRIX_ref::RotZ(chara.body_zrad)*MATRIX_ref::RotX(chara.body_xrad)*t_inv;
							mb_inv = t_inv;
						}
						chara.body.SetMatrix(chara.mat*MATRIX_ref::Mtrans(chara.pos));
						//
						chara.body.SetFrameLocalMatrix(chara.bodyg_f.first, mg_inv*MATRIX_ref::Mtrans(chara.bodyg_f.second));
						chara.body.SetFrameLocalMatrix(chara.bodyb_f.first, mb_inv*mg_inv.Inverse()*MATRIX_ref::Mtrans(chara.bodyb_f.second));
						chara.body.SetFrameLocalMatrix(chara.body_f.first, m_inv*mb_inv.Inverse()*MATRIX_ref::Mtrans(chara.body_f.second));
					}
					//頭部
					chara.body.SetFrameLocalMatrix(chara.head_f.first, chara.mat_HMD*m_inv.Inverse()*MATRIX_ref::Mtrans(chara.head_f.second));
					if (chara.reloadf) {
						chara.body.frame_reset(chara.head_f.first);
					}
					//足
					{
						auto speed = ((ads.first ? 2.f : 4.f)*(squat.first ? 0.4f : 1.f));
						auto ratio_t = add_pos.size() / (speed / fps);
						if (ads.first) {
							easing_set(&chara.body.get_anime(2).per, 0.f, 0.95f, fps);
							if (!squat.first) {
								easing_set(&chara.body.get_anime(1).per, 0.5f*ratio_t, 0.95f, fps);
								easing_set(&chara.body.get_anime(8).per, 0.f, 0.9f, fps);
								easing_set(&chara.body.get_anime(7).per, 0.f, 0.9f, fps);
							}
							else {
								easing_set(&chara.body.get_anime(1).per, 0.f, 0.95f, fps);
								easing_set(&chara.body.get_anime(8).per, 0.5f*ratio_t, 0.9f, fps);
								easing_set(&chara.body.get_anime(7).per, 1.f - 1.f*ratio_t, 0.9f, fps);
							}
						}
						else {
							easing_set(&chara.body.get_anime(2).per, 0.f, 0.95f, fps);
							if (!squat.first) {
								easing_set(&chara.body.get_anime(1).per, 1.f*ratio_t, 0.95f, fps);
								easing_set(&chara.body.get_anime(8).per, 0.f, 0.9f, fps);
								easing_set(&chara.body.get_anime(7).per, 0.f, 0.9f, fps);
							}
							else {
								easing_set(&chara.body.get_anime(1).per, 0.f, 0.95f, fps);
								easing_set(&chara.body.get_anime(8).per, 1.f*ratio_t, 0.9f, fps);
								easing_set(&chara.body.get_anime(7).per, 1.f - 1.f*ratio_t, 0.9f, fps);
							}
							easing_set(&chara.body.get_anime(0).per, 0.f, 0.95f, fps);
						}
						chara.body.get_anime(1).time += 30.f / fps;
						if (chara.body.get_anime(1).time >= chara.body.get_anime(1).alltime) {
							chara.body.get_anime(1).time = 0.f;
						}
						chara.body.get_anime(2).time += 30.f / fps;
						if (chara.body.get_anime(2).time >= chara.body.get_anime(2).alltime) {
							chara.body.get_anime(2).time = 0.f;
						}
					}
					//視点
					{
						easing_set(&gunpos_TPS, VGet(-0.1f, -0.05f, -0.3f), 0.75f, fps);
					}
					//手
					{
						chara.body.frame_reset(chara.RIGHTarm1_f.first);
						chara.body.frame_reset(chara.RIGHTarm2_f.first);
						chara.body.frame_reset(chara.RIGHThand_f.first);
						chara.body.frame_reset(chara.LEFTarm1_f.first);
						chara.body.frame_reset(chara.LEFTarm2_f.first);
						chara.body.frame_reset(chara.LEFThand_f.first);
						{
							chara.body.get_anime(6).per = 0.f;
							chara.body.get_anime(6).time = 0.f;
							{
								chara.body.get_anime(3).per = 0.f;
								chara.body.get_anime(3).time = 0.f;
								//右手
								{
									//視点を一時取得
									chara.pos_HMD = (chara.body.frame(chara.RIGHTeye_f.first) + (chara.body.frame(chara.LEFTeye_f.first) - chara.body.frame(chara.RIGHTeye_f.first))*0.5f) - chara.pos;
									//銃器
									chara.mat_RIGHTHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), chara.vecadd_RIGHTHAND)*chara.mat_HMD;//リコイル

									chara.pos_RIGHTHAND = chara.pos_HMD + MATRIX_ref::Vtrans( gunpos_TPS , chara.mat_RIGHTHAND);

									VECTOR_ref tgt_pt = chara.pos_RIGHTHAND + chara.pos;
									//基準
									VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - chara.body.frame(chara.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());
									VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0とする
									float cos_t = getcos_tri((chara.body.frame(chara.RIGHThand_f.first) - chara.body.frame(chara.RIGHTarm2_f.first)).size(), (chara.body.frame(chara.RIGHTarm2_f.first) - chara.body.frame(chara.RIGHTarm1_f.first)).size(), (chara.body.frame(chara.RIGHTarm1_f.first) - tgt_pt).size());
									VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
									//上腕
									chara.body.SetFrameLocalMatrix(chara.RIGHTarm1_f.first, MATRIX_ref::Mtrans(chara.RIGHTarm1_f.second));
									MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(chara.body.frame(chara.RIGHTarm2_f.first) - chara.body.frame(chara.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
									chara.body.SetFrameLocalMatrix(chara.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(chara.RIGHTarm1_f.second));
									//下腕
									chara.body.SetFrameLocalMatrix(chara.RIGHTarm2_f.first, MATRIX_ref::Mtrans(chara.RIGHTarm2_f.second));
									MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(chara.body.frame(chara.RIGHThand_f.first) - chara.body.frame(chara.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - chara.body.frame(chara.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
									chara.body.SetFrameLocalMatrix(chara.RIGHTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(chara.RIGHTarm2_f.second));
									//手
									chara.body.SetFrameLocalMatrix(chara.RIGHThand_f.first, MATRIX_ref::RotY(deg2rad(-10))* MATRIX_ref::RotZ(deg2rad(50))* MATRIX_ref::RotX(deg2rad(90))* chara.mat_RIGHTHAND* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(chara.RIGHThand_f.second));
								}
								//左手
								{
									chara.pos_LEFTHAND = chara.pos_RIGHTHAND + chara.pos;//
									chara.mat_LEFTHAND = chara.mat_HMD;
									chara.LEFT_hand = false;
									{
										VECTOR_ref vec_a1 = MATRIX_ref::Vtrans(((chara.pos + chara.pos_LEFTHAND) - chara.body.frame(chara.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//基準
										VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0とする
										float cos_t = getcos_tri((chara.body.frame(chara.LEFThand_f.first) - chara.body.frame(chara.LEFTarm2_f.first)).size(), (chara.body.frame(chara.LEFTarm2_f.first) - chara.body.frame(chara.LEFTarm1_f.first)).size(), (chara.body.frame(chara.LEFTarm1_f.first) - (chara.pos + chara.pos_LEFTHAND)).size());
										VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
										//上腕
										chara.body.SetFrameLocalMatrix(chara.LEFTarm1_f.first, MATRIX_ref::Mtrans(chara.LEFTarm1_f.second));
										MATRIX_ref a1_inv = MATRIX_ref::RotVec2(
											MATRIX_ref::Vtrans(chara.body.frame(chara.LEFTarm2_f.first) - chara.body.frame(chara.LEFTarm1_f.first), m_inv.Inverse()),
											vec_t
										);
										chara.body.SetFrameLocalMatrix(chara.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(chara.LEFTarm1_f.second));
										//下腕
										chara.body.SetFrameLocalMatrix(chara.LEFTarm2_f.first, MATRIX_ref::Mtrans(chara.LEFTarm2_f.second));
										MATRIX_ref a2_inv = MATRIX_ref::RotVec2(
											MATRIX_ref::Vtrans(chara.body.frame(chara.LEFThand_f.first) - chara.body.frame(chara.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()),
											MATRIX_ref::Vtrans((chara.pos + chara.pos_LEFTHAND) - chara.body.frame(chara.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse())
										);
										chara.body.SetFrameLocalMatrix(chara.LEFTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(chara.LEFTarm2_f.second));
										//手
										chara.body.SetFrameLocalMatrix(chara.LEFThand_f.first,
											MATRIX_ref::RotZ(deg2rad(-60))*
											MATRIX_ref::RotX(deg2rad(80))*
											chara.mat_LEFTHAND*
											m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(chara.LEFThand_f.second));
									}
								}
							}
						}
						//右人差し指
						{
							chara.body.get_anime(0).per = 1.f;
							chara.body.get_anime(5).per = 1.f;
						}
					}
				}
				chara.body.work_anime();
				if (!Drawparts->use_vr) {
					//視点取得
					chara.pos_HMD = (chara.body.frame(chara.RIGHTeye_f.first) + (chara.body.frame(chara.LEFTeye_f.first) - chara.body.frame(chara.RIGHTeye_f.first))*0.5f) - chara.pos;
					//
					chara.pos_RIGHTHAND = chara.pos_HMD + MATRIX_ref::Vtrans(gunpos_TPS, chara.mat_RIGHTHAND);
				}
				//操作
				if (Drawparts->use_vr) {
					if (Drawparts->get_hand1_num() != -1) {
						auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hand1_num()];
						if (ptr_.turn && ptr_.now) {
						}
					}
					if (Drawparts->get_hand2_num() != -1) {
						auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hand2_num()];
						if (ptr_.turn && ptr_.now) {
						}
					}
				}
				else {
				}
			}
			{
				//VR用
				if (Drawparts->get_hand1_num() != -1) {
					auto& ptr_ = *Drawparts->get_device_hand1();
					if (ptr_.turn && ptr_.now) {
						if ((ptr_.on[0] & BUTTON_TRIGGER) != 0) {
							startp = true;
						}
					}
				}
				//共通
				if (CheckHitKey(KEY_INPUT_SPACE) != 0) {
					startp = true;
				}
			}

			{
				{
					chara.body.work_anime();
					if (chara.start_c) {
						chara.body.PhysicsResetState();
						chara.start_c = false;
					}
					else {
						chara.body.PhysicsCalculation(1000.f / GetFPS());
					}
				}
			}
			//VR空間に適用
			Drawparts->Move_Player();
			//カメラ指定
			if (Drawparts->use_vr) {
				cams.campos = chara.pos + chara.pos_HMD;
				cams.camvec = (chara.mat_HMD*chara.mat).zvec();
				cams.camup = (chara.mat_HMD*chara.mat).yvec();
			}
			else {
				cams.campos = chara.pos + VGet(0, 1.6f, 0);
				cams.camvec = chara.mat.zvec();
				cams.camup = chara.mat.yvec();
			}
			//描画
			Drawparts->draw_VR(
				[&] {
					
					SetUseLighting(FALSE);
					sky_mod.DrawModel();
					SetUseLighting(TRUE);

					Drawparts->Draw_by_Shadow(
						[&] {
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						map_mod.DrawModel();

						chara.Draw_chara();

						DrawCone3D((chara.pos + MATRIX_ref::Vtrans((pos_track0 - chara.pos_HMD), chara.mat) + chara.pos_HMD + (mat_track0 * chara.mat).zvec()*0.1f).get(), (chara.pos + MATRIX_ref::Vtrans((pos_track0 - chara.pos_HMD), chara.mat) + chara.pos_HMD - (mat_track0 * chara.mat).zvec()*0.1f).get(), 0.1f, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);
						DrawCone3D((chara.pos + MATRIX_ref::Vtrans((pos_track1 - chara.pos_HMD), chara.mat) + chara.pos_HMD + (mat_track1 * chara.mat).zvec()*0.1f).get(), (chara.pos + MATRIX_ref::Vtrans((pos_track1 - chara.pos_HMD), chara.mat) + chara.pos_HMD - (mat_track1 * chara.mat).zvec()*0.1f).get(), 0.1f, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);
						DrawCone3D((chara.pos + MATRIX_ref::Vtrans((pos_track2 - chara.pos_HMD), chara.mat) + chara.pos_HMD + (mat_track2 * chara.mat).zvec()*0.1f).get(), (chara.pos + MATRIX_ref::Vtrans((pos_track2 - chara.pos_HMD), chara.mat) + chara.pos_HMD - (mat_track2 * chara.mat).zvec()*0.1f).get(), 0.1f, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);

						grassparts->draw_grass();
					});
				},
				cams);
			//debug
			GraphHandle::SetDraw_Screen(int(DX_SCREEN_BACK), chara.pos + chara.pos_HMD + VGet(0, 1.f, 2.f), chara.pos + chara.pos_HMD,VGet(0,1,0), cams.fov,cams.near_, cams.far_);
					map_mod.DrawModel();
					chara.Draw_chara();
					grassparts->draw_grass();



			Debugparts->end_way();
			Debugparts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
			//画面更新
			Drawparts->Screen_Flip(waits);
			if ((CheckHitKey(KEY_INPUT_ESCAPE) != 0) | startp) {
				break;
			}
		}
	}
	grassparts->delete_grass();
	chara.Delete_chara();
	return 0;
}