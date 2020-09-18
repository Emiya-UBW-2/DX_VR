#define FRAME_RATE 90.f
#include"DXLib_ref/DXLib_ref.h"

class MAPS {
private:
	GraphHandle sky_sun;			/*sunpic*/
	int grasss = 5000;				/*grassの数*/
	std::vector<VERTEX3D> grassver; /*grass*/
	std::vector<DWORD> grassind;    /*grass*/
	int VerBuf, IndexBuf;			/*grass*/
	MV1 grass;						/*grassモデル*/
	GraphHandle graph;				/*画像ハンドル*/
	int IndexNum, VerNum;			/*grass*/
	int vnum, pnum;					/*grass*/
	MV1_REF_POLYGONLIST RefMesh;	/*grass*/
	VECTOR_ref map_max = VGet(10, 10, 10);
	VECTOR_ref map_min = VGet(-10, -10, -10);
public:
	MAPS(void) {
		//map
		SetUseASyncLoadFlag(TRUE);
		sky_sun = GraphHandle::Load("data/sun.png");	  /*太陽*/
		SetUseASyncLoadFlag(FALSE);
	}
	void set_map_readyb(void) {
		SetUseASyncLoadFlag(FALSE);
		graph = GraphHandle::Load("data/map/grass/grass.png");		 /*grass*/
		MV1::Load("data/map/grass/grass.mv1", &grass, false);		/*grass*/
		SetUseASyncLoadFlag(FALSE);
		return;
	}
	void set_map_cancelb(void) {
		SetASyncLoadFinishDeleteFlag(graph.get());										 /*grass*/
		SetASyncLoadFinishDeleteFlag(grass.get());		/*grass*/
	}
	bool set_map_ready(void) {
		/*grass*/
		vnum = 0;
		pnum = 0;
		MV1SetupReferenceMesh(grass.get(), -1, TRUE); /*参照用メッシュの作成*/
		RefMesh = MV1GetReferenceMesh(grass.get(), -1, TRUE); /*参照用メッシュの取得*/
		IndexNum = RefMesh.PolygonNum * 3 * grasss; /*インデックスの数を取得*/
		VerNum = RefMesh.VertexNum * grasss;	/*頂点の数を取得*/
		grassver.resize(VerNum);   /*頂点データとインデックスデータを格納するメモリ領域の確保*/
		grassind.resize(IndexNum); /*頂点データとインデックスデータを格納するメモリ領域の確保*/
		for (int i = 0; i < grasss; ++i) {
			VECTOR_ref tmpvect = VGet((float)(-(map_max - map_min).x() * 5 + GetRand(int((map_max - map_min).x()) * 10)) / 10.0f, 0.0f, (float)(-(map_max - map_min).z() * 5 + GetRand(int((map_max - map_min).z()) * 10)) / 10.0f);
			//
			MV1SetMatrix(grass.get(), MMult(MMult(MGetRotY(deg2rad(GetRand(360))), MGetRotVec2(VGet(0, 1.f, 0), VGet(0, 1.f, 0))), MGetTranslate(tmpvect.get())));
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
	void delete_map(void) {
		graph.Dispose();
		grass.Dispose();
		grassver.clear();
		grassind.clear();
	}

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	auto Drawparts = std::make_unique<DXDraw>("FPS_0", FRAME_RATE, true);	/*汎用クラス*/
	auto Debugparts = std::make_unique<DeBuG>(FRAME_RATE);	/*デバッグクラス*/

	auto grassparts = std::make_unique<MAPS>();	/*草クラス*/
	//
	VECTOR_ref pos_HMD;
	MATRIX_ref mat_HMD;
	VECTOR_ref pos_HAND1;
	MATRIX_ref mat_HAND1;
	VECTOR_ref pos_HAND2;
	MATRIX_ref mat_HAND2;
	DXDraw::cam_info cams;

	MV1 map_mod;
	MV1::Load("data/map/model.mv1", &map_mod, false);
	map_mod.SetMatrix(MGetIdent());

	MV1 sky_mod;
	MV1::Load("data/map/sky.mqo", &sky_mod, false);
	sky_mod.SetMatrix(MGetIdent());

	grassparts->set_map_readyb();

	grassparts->set_map_ready();
	//
	bool startp = false;
	cams.set_cam_info(VGet(0, 1.f, 0), VGet(0, 0, -1.f), VGet(0, 1.f, 0), deg2rad(90), 0.1f, 100.f);//カメラ
	{
		Drawparts->Set_Light_Shadow(VGet(10.f, 10.f, 10.f), VGet(-10.f, -10.f, -10.f), VGet(-0.5f, -0.5f, -0.5f), [&] {map_mod.DrawModel(); });	//ライト、影
		//main_loop
		while (ProcessMessage() == 0) {
			const auto waits = GetNowHiPerformanceCount();
			Debugparts->put_way();
			//座標取得
			Drawparts->GetDevicePositionVR(Drawparts->get_hmd_num(), &pos_HMD, &mat_HMD);
			Drawparts->GetDevicePositionVR(Drawparts->get_hand1_num(), &pos_HAND1, &mat_HAND1);
			Drawparts->GetDevicePositionVR(Drawparts->get_hand2_num(), &pos_HAND2, &mat_HAND2);
			//操作
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
			//VR空間に適用
			Drawparts->Move_Player();
			//カメラ指定
			if (Drawparts->use_vr) {
				cams.campos = pos_HMD;
				cams.camvec = mat_HMD.zvec()*-1.f;
				cams.camup = mat_HMD.yvec();
			}
			else {
				cams.campos = VGet(0, 1.f, 0);
				cams.camvec = VGet(0, 0, -1.f);
				cams.camup = VGet(0, 1.f, 0);
			}
			//描画
			Drawparts->draw_VR(
				[&] {
					sky_mod.DrawModel();
					Drawparts->Draw_by_Shadow(
						[&] {
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						map_mod.DrawModel();
						//hand1
						DrawCone3D((pos_HAND1 + mat_HAND1.zvec()*0.1f).get(), (pos_HAND1 - mat_HAND1.zvec()*0.1f).get(), 0.1f, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);
						//hand2
						DrawCone3D((pos_HAND2 + mat_HAND2.zvec()*0.1f).get(), (pos_HAND2 - mat_HAND2.zvec()*0.1f).get(), 0.1f, 8, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);
						grassparts->draw_grass();
					});
				},
				cams);
			//debug
			GraphHandle::SetDraw_Screen(int(DX_SCREEN_BACK), false);
			Debugparts->end_way();
			Debugparts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
			//画面更新
			Drawparts->Screen_Flip(waits);
			if ((CheckHitKey(KEY_INPUT_ESCAPE) != 0) | startp) {
				break;
			}
		}
	}
	grassparts->delete_map();

	return 0;
}