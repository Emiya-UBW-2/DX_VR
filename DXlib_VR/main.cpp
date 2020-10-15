#define FRAME_RATE 90.f
#include"DXLib_ref/DXLib_ref.h"
#include"classes.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	auto Drawparts = std::make_unique<DXDraw>("FPS_0", FRAME_RATE, true);	/*�ėp�N���X*/
	auto Debugparts = std::make_unique<DeBuG>(FRAME_RATE);	/*�f�o�b�O�N���X*/
	auto grassparts = std::make_unique<GRASS>();	/*���N���X*/
	GraphHandle screen = GraphHandle::Make(1080, 1200);
	//map
	MV1 map_mod;
	MV1::Load("data/map/model.mv1", &map_mod, false);
	map_mod.SetMatrix(MGetIdent());
	//sky
	MV1 sky_mod;
	MV1::Load("data/map/sky/model.mv1", &sky_mod, false);
	sky_mod.SetMatrix(MGetIdent());
	//�v���C���[����ϐ��Q
	bool oldv=true;
	VECTOR_ref pos_track0;
	MATRIX_ref mat_track0;
	VECTOR_ref pos_track1;
	MATRIX_ref mat_track1;
	VECTOR_ref pos_track2;
	MATRIX_ref mat_track2;
	VECTOR_ref gunpos_TPS = VGet(-0.1f, -0.05f, -0.3f);
	VECTOR_ref rec_HMD;
	Chara chara;
	MV1 body_obj;												//�g�̃��f��
	DXDraw::cam_info cams;
	switchs TPS, ads, chgun, delgun, usegun, change_gun;		//����X�C�b�`
	float xrad_p = 0.f;											//�}�E�X�G�C���p�ϐ��m��
	switchs squat;
	VECTOR_ref add_pos, add_pos_buf;							//�ړ�
	MV1::Load("data/body/model.mv1", &body_obj, false);				/*�g��*/
	chara.Ready_chara(body_obj);
	chara.Set_chara_Position(VGet(0.0f, 0.0f, 0.f), MGetIdent(), MATRIX_ref::RotY(0));
	chara.start_c = true;
	//grass
	grassparts->set_grass_ready_before();
	grassparts->set_grass_ready("data/grassput.bmp", 5.f, 5.f, -5.f, -5.f);
	//
	cams.set_cam_info(VGet(0, 1.f, 0), VGet(0, 0, -1.f), VGet(0, 1.f, 0), deg2rad(90), 0.1f, 50.f);//�J����
	{
		Drawparts->Set_Light_Shadow(VGet(10.f, 10.f, 10.f), VGet(-10.f, -10.f, -10.f), VGet(-0.5f, -0.5f, -0.5f), [&] {/*map_mod.DrawModel();*/ });	//���C�g�A�e
		//main_loop
		while (ProcessMessage() == 0) {
			const float fps = GetFPS();
			const auto waits = GetNowHiPerformanceCount();
			Debugparts->put_way();
			//���W�擾
			if (Drawparts->tracker_num.size() > 0) {
				Drawparts->GetDevicePositionVR(Drawparts->tracker_num[0], &chara.pos_WAIST, &chara.mat_WAIST);
			}
			if (Drawparts->tracker_num.size() > 1) {
				Drawparts->GetDevicePositionVR(Drawparts->tracker_num[1], &pos_track1, &mat_track1);
			}
			if (Drawparts->tracker_num.size() > 2) {
				Drawparts->GetDevicePositionVR(Drawparts->tracker_num[2], &pos_track2, &mat_track2);
			}
			//�v���C���[����
			{
				//HMD_mat
				if (Drawparts->use_vr) {
					//+���_�擾
					auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hmd_num()];
					Drawparts->GetDevicePositionVR(Drawparts->get_hmd_num(), &chara.pos_HMD, &chara.mat_HMD);
					if (chara.start_c || (ptr_.turn && ptr_.now)!=oldv) {
						rec_HMD = VGet(chara.pos_HMD.x(),0.f, chara.pos_HMD.z());
					}
					oldv = ptr_.turn && ptr_.now;
					chara.pos_HMD = chara.pos_HMD - rec_HMD;
				}
				else {
					int x_m, y_m;
					GetMousePoint(&x_m, &y_m);
					x_m = std::clamp(x_m - deskx / 2, -120, 120);
					y_m = std::clamp(y_m - desky / 2, -120, 120);
					//���s��
					{
						//���[��
						{
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
						}
						//�}�E�X�G�C��
						chara.mat_HMD = MATRIX_ref::RotX(-xrad_p)*chara.mat_HMD;
						xrad_p = std::clamp(xrad_p - deg2rad(y_m)*100.f / 1000.f, deg2rad(-45), deg2rad(45));
						chara.mat_HMD *= MATRIX_ref::RotY(deg2rad(x_m)*100.f / 1000.f);
						chara.mat_HMD = MATRIX_ref::RotX(xrad_p)*chara.mat_HMD;
					}
					SetMousePoint(deskx / 2, desky / 2);
					SetMouseDispFlag(FALSE);
				}
				//�ړ�
				if (Drawparts->use_vr) {
					if (Drawparts->get_hand2_num() != -1) {
						auto& ptr_ = (*Drawparts->get_device())[Drawparts->get_hand2_num()];
						//���s��
						{
							if (ptr_.turn && ptr_.now) {
								if ((ptr_.on[1] & BUTTON_TOUCHPAD) != 0) {
									auto speed = 2.f;
									easing_set(&add_pos_buf, (chara.mat_HMD.zvec()*-1.f*ptr_.touch.y() + chara.mat_HMD.xvec()*-1.f*ptr_.touch.x())*-speed / fps, 0.95f, fps);
								}
								else {
									easing_set(&add_pos_buf, VGet(0, 0, 0), 0.95f, fps);
								}
								if (chara.add_ypos == 0.f) {
									//chara.add_ypos = 0.05f;/*�W�����v*/
									add_pos = add_pos_buf;
								}
								else {
									easing_set(&add_pos, VGet(0, 0, 0), 0.995f, fps);
								}
							}
						}
						//
					}
				}
				else {
					{
						auto wkey = (CheckHitKey(KEY_INPUT_W) != 0);
						auto skey = (CheckHitKey(KEY_INPUT_S) != 0);
						auto akey = (CheckHitKey(KEY_INPUT_A) != 0);
						auto dkey = (CheckHitKey(KEY_INPUT_D) != 0);
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
							//chara.add_ypos = 0.05f;/*�W�����v*/
							add_pos = add_pos_buf;
						}
						else {
							easing_set(&add_pos, VGet(0, 0, 0), 0.995f, fps);
						}
					}
				}

				//���s���[�h���̕ǂ��̑��̔���
				{
					VECTOR_ref pos_t = chara.pos;
					pos_t += add_pos;
					//��
					{
						//mapparts->map_col_wall(chara.pos, &pos_t);
						if ((add_pos - (pos_t - chara.pos)).size() != 0.f) {
							add_pos = pos_t - chara.pos;
							if (chara.add_ypos == 0.f) {
								add_pos_buf = add_pos;
							}
						}
					}
					//����
					{
						//auto pp = mapparts->map_col_line(pos_t + VGet(0, 1.6f, 0), pos_t, 0);
						if (chara.add_ypos <= 0.f && true) {
							if (VECTOR_ref(VGet(0, 1.f, 0.f)).dot(VGet(0, 1, 0)) >= cos(deg2rad(30))) {
								//pos_t = pp.HitPosition;
							}
							else {
								//�u���b�N����x�N�g��
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
							//���A
							if (pos_t.y() <= -5.f) {
								pos_t = VGet(0.f, 9.f, 0.f);
								chara.add_ypos = 0.f;
								chara.body.SetMatrix(chara.mat*MATRIX_ref::Mtrans(pos_t));
								chara.body.PhysicsResetState();
							}
						}
					}
					//���f
					chara.pos = pos_t;
				}
				//pos
				if (Drawparts->use_vr) {
					//
					{
						VECTOR_ref v_ = chara.mat_WAIST.zvec()*-1.f;
						float x_1 = -sinf(chara.body_yrad);
						float y_1 = cosf(chara.body_yrad);
						float x_2 = v_.x();
						float y_2 = -v_.z();
						float r_ = std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
						chara.body_yrad += r_ * FRAME_RATE / fps / 10.f;
					}
					MATRIX_ref t_inv = MATRIX_ref::RotY(chara.body_yrad);
					//�g��,����,��
					MATRIX_ref m_inv = t_inv;
					chara.body.SetMatrix(chara.mat*m_inv);

					if (Drawparts->tracker_num.size() > 0) {
						//��
						chara.body.SetFrameLocalMatrix(chara.bodyb_f.first, (chara.mat_WAIST*m_inv.Inverse())*MATRIX_ref::Mtrans(chara.bodyb_f.second));
						//����
						chara.body.SetFrameLocalMatrix(chara.head_f.first, (MATRIX_ref::Axis1(chara.mat_HMD.xvec()*-1.f, chara.mat_HMD.yvec(), chara.mat_HMD.zvec()*-1.f) *m_inv.Inverse()*(chara.mat_WAIST*m_inv.Inverse()).Inverse())*MATRIX_ref::Mtrans(chara.head_f.second));
					}
					else {
						//����
						chara.body.SetFrameLocalMatrix(chara.head_f.first,(MATRIX_ref::Axis1(chara.mat_HMD.xvec()*-1.f, chara.mat_HMD.yvec(), chara.mat_HMD.zvec()*-1.f) *m_inv.Inverse())*MATRIX_ref::Mtrans(chara.head_f.second));
					}

					chara.body.SetMatrix(chara.mat*m_inv
						*MATRIX_ref::Mtrans(chara.pos + chara.pos_HMD - (chara.body.frame(chara.RIGHTeye_f.first) + (chara.body.frame(chara.LEFTeye_f.first) - chara.body.frame(chara.RIGHTeye_f.first))*0.5f))
					);
					//��
					{

					}
					//��
					{
						chara.body.get_anime(3).per = 0.f;
						chara.body.get_anime(3).time = 0.f;
						//�E��
						{
							Drawparts->GetDevicePositionVR(Drawparts->get_hand1_num(), &chara.pos_RIGHTHAND, &chara.mat_RIGHTHAND);
							chara.mat_RIGHTHAND = chara.mat_RIGHTHAND*MATRIX_ref::RotAxis(chara.mat_RIGHTHAND.xvec(), deg2rad(-60));
							chara.mat_RIGHTHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), chara.vecadd_RIGHTHAND)*chara.mat_RIGHTHAND;//���R�C��
							//�e��
							VECTOR_ref tgt_pt = chara.pos_RIGHTHAND + chara.pos;
							//
							VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - chara.body.frame(chara.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());//�
							VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
							float cos_t = getcos_tri((chara.body.frame(chara.RIGHThand_f.first) - chara.body.frame(chara.RIGHTarm2_f.first)).size(), (chara.body.frame(chara.RIGHTarm2_f.first) - chara.body.frame(chara.RIGHTarm1_f.first)).size(), (chara.body.frame(chara.RIGHTarm1_f.first) - tgt_pt).size());
							VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
							//��r
							chara.body.SetFrameLocalMatrix(chara.RIGHTarm1_f.first, MATRIX_ref::Mtrans(chara.RIGHTarm1_f.second));
							MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(chara.body.frame(chara.RIGHTarm2_f.first) - chara.body.frame(chara.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
							chara.body.SetFrameLocalMatrix(chara.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(chara.RIGHTarm1_f.second));
							//���r
							chara.body.SetFrameLocalMatrix(chara.RIGHTarm2_f.first, MATRIX_ref::Mtrans(chara.RIGHTarm2_f.second));
							MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(chara.body.frame(chara.RIGHThand_f.first) - chara.body.frame(chara.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - chara.body.frame(chara.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
							chara.body.SetFrameLocalMatrix(chara.RIGHTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(chara.RIGHTarm2_f.second));
							//��
							chara.body.SetFrameLocalMatrix(chara.RIGHThand_f.first,
								MATRIX_ref::RotY(deg2rad(-10))*
								MATRIX_ref::RotZ(deg2rad(50))*
								MATRIX_ref::RotX(deg2rad(90))*
								chara.mat_RIGHTHAND*
								m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(chara.RIGHThand_f.second));
							//�E�l�����w
							chara.body.get_anime(0).per = 1.f;
							chara.body.get_anime(5).per = 1.f;
						}
						//����
						{
							Drawparts->GetDevicePositionVR(Drawparts->get_hand2_num(), &chara.pos_LEFTHAND, &chara.mat_LEFTHAND);
							chara.mat_LEFTHAND = chara.mat_LEFTHAND*MATRIX_ref::RotAxis(chara.mat_LEFTHAND.xvec(), deg2rad(-60));
							{
								VECTOR_ref vec_a1 = MATRIX_ref::Vtrans(((chara.pos + chara.pos_LEFTHAND) - chara.body.frame(chara.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//�
								VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
								float cos_t = getcos_tri((chara.body.frame(chara.LEFThand_f.first) - chara.body.frame(chara.LEFTarm2_f.first)).size(), (chara.body.frame(chara.LEFTarm2_f.first) - chara.body.frame(chara.LEFTarm1_f.first)).size(), (chara.body.frame(chara.LEFTarm1_f.first) - (chara.pos + chara.pos_LEFTHAND)).size());
								VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
								//��r
								chara.body.SetFrameLocalMatrix(chara.LEFTarm1_f.first, MATRIX_ref::Mtrans(chara.LEFTarm1_f.second));
								MATRIX_ref a1_inv = MATRIX_ref::RotVec2(
									MATRIX_ref::Vtrans(chara.body.frame(chara.LEFTarm2_f.first) - chara.body.frame(chara.LEFTarm1_f.first), m_inv.Inverse()),
									vec_t
								);
								chara.body.SetFrameLocalMatrix(chara.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(chara.LEFTarm1_f.second));
								//���r
								chara.body.SetFrameLocalMatrix(chara.LEFTarm2_f.first, MATRIX_ref::Mtrans(chara.LEFTarm2_f.second));
								MATRIX_ref a2_inv = MATRIX_ref::RotVec2(
									MATRIX_ref::Vtrans(chara.body.frame(chara.LEFThand_f.first) - chara.body.frame(chara.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()),
									MATRIX_ref::Vtrans((chara.pos + chara.pos_LEFTHAND) - chara.body.frame(chara.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse())
								);
								chara.body.SetFrameLocalMatrix(chara.LEFTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(chara.LEFTarm2_f.second));
								//��
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
					//
					{
						VECTOR_ref v_ = chara.mat_HMD.zvec();
						float x_1 = -sinf(chara.body_yrad);
						float y_1 = cosf(chara.body_yrad);
						float x_2 = v_.x();
						float y_2 = -v_.z();
						float r_ = std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
						chara.body_yrad += r_ * FRAME_RATE / fps / 10.f;
					}
					MATRIX_ref t_inv = MATRIX_ref::RotY(chara.body_yrad);
					{
						VECTOR_ref v_ = chara.mat_HMD.zvec();
						float x_1 = sinf(chara.body_xrad);
						float y_1 = -cosf(chara.body_xrad);
						float x_2 = -v_.y();
						float y_2 = -std::hypotf(v_.x(), v_.z());
						chara.body_xrad += std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
					}
					//�g��
					MATRIX_ref m_inv = MATRIX_ref::RotY(deg2rad(30))*MATRIX_ref::RotZ(chara.body_zrad)*MATRIX_ref::RotX(chara.body_xrad)*MATRIX_ref::RotY(DX_PI_F)*t_inv;
					MATRIX_ref mb_inv = MATRIX_ref::RotY(deg2rad(15))*MATRIX_ref::RotY(DX_PI_F)*t_inv;
					MATRIX_ref mg_inv = MATRIX_ref::RotY(DX_PI_F)*t_inv;
					{
						chara.body.SetMatrix(chara.mat*MATRIX_ref::Mtrans(chara.pos));
						//
						chara.body.SetFrameLocalMatrix(chara.bodyg_f.first, mg_inv*MATRIX_ref::Mtrans(chara.bodyg_f.second));
						chara.body.SetFrameLocalMatrix(chara.bodyb_f.first, mb_inv*mg_inv.Inverse()*MATRIX_ref::Mtrans(chara.bodyb_f.second));
						chara.body.SetFrameLocalMatrix(chara.body_f.first, m_inv*mb_inv.Inverse()*MATRIX_ref::Mtrans(chara.body_f.second));
					}
					//����
					chara.body.SetFrameLocalMatrix(chara.head_f.first, chara.mat_HMD*m_inv.Inverse()*MATRIX_ref::Mtrans(chara.head_f.second));
					//��
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
						chara.body.get_anime(8).time += 30.f / fps;
						if (chara.body.get_anime(8).time >= chara.body.get_anime(8).alltime) {
							chara.body.get_anime(8).time = 0.f;
						}
					}
					//���_
					//��
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
								//�E��
								{
									//���_���ꎞ�擾
									VECTOR_ref tmp_pHMD=(chara.body.frame(chara.RIGHTeye_f.first) + (chara.body.frame(chara.LEFTeye_f.first) - chara.body.frame(chara.RIGHTeye_f.first))*0.5f) - chara.pos;
									//���W
									chara.mat_RIGHTHAND = chara.mat_HMD;//���R�C��
									chara.pos_RIGHTHAND = tmp_pHMD + MATRIX_ref::Vtrans(gunpos_TPS, chara.mat_RIGHTHAND);
									//�E������������W
									VECTOR_ref tgt_pt = chara.pos_RIGHTHAND + chara.pos;
									VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - chara.body.frame(chara.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());
									VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
									float cos_t = getcos_tri((chara.body.frame(chara.RIGHThand_f.first) - chara.body.frame(chara.RIGHTarm2_f.first)).size(), (chara.body.frame(chara.RIGHTarm2_f.first) - chara.body.frame(chara.RIGHTarm1_f.first)).size(), (chara.body.frame(chara.RIGHTarm1_f.first) - tgt_pt).size());
									VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
									//��r
									chara.body.SetFrameLocalMatrix(chara.RIGHTarm1_f.first, MATRIX_ref::Mtrans(chara.RIGHTarm1_f.second));
									MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(chara.body.frame(chara.RIGHTarm2_f.first) - chara.body.frame(chara.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
									chara.body.SetFrameLocalMatrix(chara.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(chara.RIGHTarm1_f.second));
									//���r
									chara.body.SetFrameLocalMatrix(chara.RIGHTarm2_f.first, MATRIX_ref::Mtrans(chara.RIGHTarm2_f.second));
									MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(chara.body.frame(chara.RIGHThand_f.first) - chara.body.frame(chara.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - chara.body.frame(chara.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
									chara.body.SetFrameLocalMatrix(chara.RIGHTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(chara.RIGHTarm2_f.second));
									//��
									chara.body.SetFrameLocalMatrix(chara.RIGHThand_f.first, MATRIX_ref::RotY(deg2rad(-10))* MATRIX_ref::RotZ(deg2rad(50))* MATRIX_ref::RotX(deg2rad(90))* chara.mat_RIGHTHAND* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(chara.RIGHThand_f.second));
								}
								//����
								{
									//���W
									chara.pos_LEFTHAND = chara.pos_RIGHTHAND;//
									chara.mat_LEFTHAND = chara.mat_HMD;
									{
										VECTOR_ref vec_a1 = MATRIX_ref::Vtrans(((chara.pos + chara.pos_LEFTHAND) - chara.body.frame(chara.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//�
										VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
										float cos_t = getcos_tri((chara.body.frame(chara.LEFThand_f.first) - chara.body.frame(chara.LEFTarm2_f.first)).size(), (chara.body.frame(chara.LEFTarm2_f.first) - chara.body.frame(chara.LEFTarm1_f.first)).size(), (chara.body.frame(chara.LEFTarm1_f.first) - (chara.pos + chara.pos_LEFTHAND)).size());
										VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
										//��r
										chara.body.SetFrameLocalMatrix(chara.LEFTarm1_f.first, MATRIX_ref::Mtrans(chara.LEFTarm1_f.second));
										MATRIX_ref a1_inv = MATRIX_ref::RotVec2(
											MATRIX_ref::Vtrans(chara.body.frame(chara.LEFTarm2_f.first) - chara.body.frame(chara.LEFTarm1_f.first), m_inv.Inverse()),
											vec_t
										);
										chara.body.SetFrameLocalMatrix(chara.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(chara.LEFTarm1_f.second));
										//���r
										chara.body.SetFrameLocalMatrix(chara.LEFTarm2_f.first, MATRIX_ref::Mtrans(chara.LEFTarm2_f.second));
										MATRIX_ref a2_inv = MATRIX_ref::RotVec2(
											MATRIX_ref::Vtrans(chara.body.frame(chara.LEFThand_f.first) - chara.body.frame(chara.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()),
											MATRIX_ref::Vtrans((chara.pos + chara.pos_LEFTHAND) - chara.body.frame(chara.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse())
										);
										chara.body.SetFrameLocalMatrix(chara.LEFTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(chara.LEFTarm2_f.second));
										//��
										chara.body.SetFrameLocalMatrix(chara.LEFThand_f.first,
											MATRIX_ref::RotZ(deg2rad(-60))*
											MATRIX_ref::RotX(deg2rad(80))*
											chara.mat_LEFTHAND*
											m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(chara.LEFThand_f.second));
									}
								}
							}
						}
						//�E�l�����w
						{
							chara.body.get_anime(0).per = 1.f;
							chara.body.get_anime(5).per = 1.f;
						}
					}
					//���f
					{
						//���_
						chara.pos_HMD = (chara.body.frame(chara.RIGHTeye_f.first) + (chara.body.frame(chara.LEFTeye_f.first) - chara.body.frame(chara.RIGHTeye_f.first))*0.5f) - chara.pos;
						//�E��
						chara.pos_RIGHTHAND = chara.pos_HMD + MATRIX_ref::Vtrans(gunpos_TPS, chara.mat_RIGHTHAND);
					}
				}

				//����
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
			//����
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
			//VR��ԂɓK�p
			Drawparts->Move_Player();
			//�J�����w��
			if (Drawparts->use_vr) {
				cams.campos = chara.pos + chara.pos_HMD;
				cams.camvec = (chara.mat_HMD*chara.mat).zvec();
				cams.camup = (chara.mat_HMD*chara.mat).yvec();
			}
			else {
				cams.campos = chara.pos + VGet(0, chara.head_hight, 0);
				cams.camvec = chara.mat.zvec();
				cams.camup = chara.mat.yvec();
			}
			//�`��
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
			screen.SetDraw_Screen(chara.pos + chara.pos_HMD + VGet(0, 0.3f, 2.f), chara.pos + chara.pos_HMD, VGet(0, 1, 0), cams.fov, cams.near_, cams.far_);
			map_mod.DrawModel();
			chara.Draw_chara();
			grassparts->draw_grass();
			GraphHandle::SetDraw_Screen(int(DX_SCREEN_BACK),false);
			screen.DrawGraph(0, 0, false);


			Debugparts->end_way();
			Debugparts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
			//��ʍX�V
			Drawparts->Screen_Flip(waits);
			if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
				break;
			}
		}
	}
	grassparts->delete_grass();
	chara.Delete_chara();
	return 0;
}