//風
int wind = 0;

//砲煙発生ポイント
int spoint = 80;

//砲煙発生ポイント移動
int mspoint = 250;

//砲火炎発生ポイント
int fpoint  = 550;

//砲火柱発生ポイント
int fwpoint =100;

//広域煙調整用
int wsmpoint =100; 

//広域煙拡大速度調整
int ssmpoint = 10;

//砲口残煙発生ポイント
int gbrspoint =100;

//砲口残煙調整
int brspoint =100;


//砲煙サイズ調整
int smokesize = 1;

//---複製数---//

//フラッシュ
int FlashNum =4;
//煙
int SmokeNum =256;
//爆炎
int FireNum = 32;
//火柱
int FireWallNum = 32;
//パーティクル
int ParticleNum =128;

//パーティクル2
int ParticleNum2 =512;

//パーティクル3
int ParticleNum3 =32;

// 全体の大きさ
float TotalScale = 10;

float DefAlpha = 1;

//深度マップ保存テクスチャ
shared texture2D SPE_DepthTex : RENDERCOLORTARGET;
sampler2D SPE_DepthSamp = sampler_state {
    texture = <SPE_DepthTex>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
    AddressU  = CLAMP;
    AddressV = CLAMP;
};



//ソフトパーティクルエンジン使用フラグ
bool use_spe : CONTROLOBJECT < string name = "SoftParticleEngine.x"; >;

float4x4 world_view_proj_matrix : WorldViewProjection;
float4x4 world_view_trans_matrix : WorldViewTranspose;
float4x4 inv_view_matrix : WORLDVIEWINVERSE;
float4x4 world_matrix : World;
float3   CameraPosition    : POSITION  < string Object = "Camera"; >;
static float3 billboard_vec_x = normalize(world_view_trans_matrix[0].xyz);
static float3 billboard_vec_y = normalize(world_view_trans_matrix[1].xyz);

float time_0_X : Time;
float particleSystemShape = float( 1.00 );
float particleSpread = float( 18.00 );
float particleSpeed = float(1);
float particleSystemHeight = float( 10.00 );
float particleSize = float( 5 );
//回転速度
float RotateSpd = float(1);
//縮小速度（0：縮小しない）
float ScaleSpd = float(-2);


// The model for the particle system consists of a hundred quads.
// These quads are simple (-1,-1) to (1,1) quads where each quad
// has a z ranging from 0 to 1. The z will be used to differenciate
// between different particles

//カウント用変数
int index = 0;

texture2D rndtex <
    string ResourceName = "random1024.bmp";
>;
sampler rnd = sampler_state {
    texture = <rndtex>;
    Filter = NONE;
};

float4   MaterialDiffuse   : DIFFUSE  < string Object = "Geometry"; >;

struct VS_OUTPUT {
   float4 Pos: POSITION;
   float2 Tex: TEXCOORD0;
   float color: TEXCOORD1;
   float3 Eye	: TEXCOORD2;
   float3 WPos: TEXCOORD3;
   float4 LastPos: TEXCOORD4;
};

	
struct VS_OUTPUT2 {
   float4 Pos: POSITION;
   float2 Tex: TEXCOORD0;
   float color: TEXCOORD1;
   float3 Eye	: TEXCOORD2;
   float3 WPos: TEXCOORD3;
   float4 LastPos: TEXCOORD4;	
};

VS_OUTPUT2 AllFunc2(int num,float4 Pos,float Scale,float ParticleLen,float AlphaSpd,float rndadd,float up)
{ VS_OUTPUT2 Out;
   //フラッシュ2&煙用
	
	//砲煙サイズ調整

spoint    = spoint*smokesize;
mspoint   = mspoint*smokesize;
fpoint    = fpoint*smokesize;
fwpoint   = fwpoint*smokesize;
wsmpoint  = wsmpoint*smokesize; 
ssmpoint  = ssmpoint*smokesize;
gbrspoint = gbrspoint*smokesize;
brspoint  = brspoint*smokesize;
	

   Out.Tex = Pos.xy*0.5+0.5;
   Out.Tex *= 0.5;

   float fi = index;
   fi = fi/num;

   float t = 1 - frac(fi + particleSpeed);
   float s = pow(t, particleSystemShape);

   float r = tex2Dlod(rnd, float4(fi+rndadd,0,0,1));
   float3 pos;
   float len = (MaterialDiffuse.a);
   len = 1-len * len;
   len *= (2 + t);

   len *= ParticleLen;
   
   float sqlen = sqrt(len);
   pos.x = (particleSpread * cos(48 * r) * sqlen * TotalScale)+(len*wind);
   pos.y = particleSpread * sin(48 * r) * sqlen * TotalScale;
   pos.z = -(particleSpread * cos(r) * sqlen * r) * up * TotalScale;
   pos.z -= TotalScale+spoint+(len*mspoint);

   float3 w = (particleSize * float3(Pos.xy - float2(0,0),0)) * max(0,(1-t * ScaleSpd));
   //通常回転
   //回転行列の作成
   float rad = time_0_X * RotateSpd*(0.5-r) + t + cos(62 * r);
   float4x4 matRot;
   matRot[0] = float4(cos(rad),sin(rad),0,0); 
   matRot[1] = float4(-sin(rad),cos(rad),0,0); 
   matRot[2] = float4(0,0,1,0); 
   matRot[3] = float4(0,0,0,1); 
   w = mul(w,matRot);
   //ビルボード回転
   w = mul(w,inv_view_matrix);
   //拡大
   w = mul(w,length(world_matrix[0])*2*Scale*TotalScale);

   // Billboard the quads.
   // The view matrix gives us our right and up vectors.
   //pos += (Pos.x * view_trans_matrix[0] + Pos.y * view_trans_matrix[1]);
   pos += w;
   pos /= 10;
   
   
   Out.Pos = mul(float4(pos, 1), world_view_proj_matrix);

   Out.color = MaterialDiffuse.a*(1-AlphaSpd) - t*0.25;
   Out.color = lerp(0,Out.color,min(1,(1-MaterialDiffuse.a)*32));

	pos = mul(float4(pos,1),world_matrix);

	Out.LastPos = Out.Pos;
	Out.WPos = pos;
	Out.Eye = pos - CameraPosition;
   return Out;}

VS_OUTPUT2 AllFunc3(int num,float4 Pos,float Scale,float ParticleLen,float AlphaSpd,float rndadd,float up)
{ VS_OUTPUT2 Out;
   //火炎用
   Out.Tex = Pos.xy*0.5+0.5;
   Out.Tex *= 0.5;

	//砲煙サイズ調整

spoint    = spoint*smokesize;
mspoint   = mspoint*smokesize;
fpoint    = fpoint*smokesize;
fwpoint   = fwpoint*smokesize;
wsmpoint  = wsmpoint*smokesize; 
ssmpoint  = ssmpoint*smokesize;
gbrspoint = gbrspoint*smokesize;
brspoint  = brspoint*smokesize;
	
   float fi = index;
   fi = fi/num;

   float t = 1 - frac(fi + particleSpeed);
   float s = pow(t, particleSystemShape);

   float r = tex2Dlod(rnd, float4(fi+rndadd,0,0,1));
   float3 pos;
   float len = (MaterialDiffuse.a);
   len = 1-len * len;
   len *= (2 + t);

   len *= ParticleLen;
   
   float sqlen = sqrt(len);
   pos.x = (particleSpread * cos(48 * r) * sqlen * TotalScale)+(len*wind);
   pos.y = particleSpread * sin(48 * r) * sqlen * TotalScale;
   pos.z = -(particleSpread * cos(r) * sqlen * r) * up * TotalScale;
   pos.z -= TotalScale*(len*(mspoint-50));

   float3 w = (particleSize * float3(Pos.xy - float2(0,0),0)) * max(0,(1-t * ScaleSpd));
   //通常回転
   //回転行列の作成
   float rad = time_0_X * RotateSpd*(0.5-r) + t + cos(62 * r);
   float4x4 matRot;
   matRot[0] = float4(cos(rad),sin(rad),0,0); 
   matRot[1] = float4(-sin(rad),cos(rad),0,0); 
   matRot[2] = float4(0,0,1,0); 
   matRot[3] = float4(0,0,0,1); 
   w = mul(w,matRot);
   //ビルボード回転
   w = mul(w,inv_view_matrix);
   //拡大
   w = mul(w,length(world_matrix[0])*1*Scale*TotalScale);

   // Billboard the quads.
   // The view matrix gives us our right and up vectors.
   //pos += (Pos.x * view_trans_matrix[0] + Pos.y * view_trans_matrix[1]);
   pos += w;
   pos /= 10;
   
   
   Out.Pos = mul(float4(pos, 1), world_view_proj_matrix);

   Out.color = MaterialDiffuse.a*(1-AlphaSpd) - t*0.25;
   Out.color = lerp(0,Out.color,min(1,(1-MaterialDiffuse.a)*32))*(len);

	pos = mul(float4(pos,1),world_matrix);

	Out.LastPos = Out.Pos;
	Out.WPos = pos;
	Out.Eye = pos - CameraPosition;
   return Out;}
VS_OUTPUT2 AllFunc4(int num,float4 Pos,float Scale,float ParticleLen,float AlphaSpd,float rndadd,float up)
{ VS_OUTPUT2 Out;
   //メイン火炎用
   Out.Tex = Pos.xy*0.5+0.5;
   Out.Tex *= 0.5;
	
//砲煙サイズ調整

spoint    = spoint*smokesize;
mspoint   = mspoint*smokesize;
fpoint    = fpoint*smokesize;
fwpoint   = fwpoint*smokesize;
wsmpoint  = wsmpoint*smokesize; 
ssmpoint  = ssmpoint*smokesize;
gbrspoint = gbrspoint*smokesize;
brspoint  = brspoint*smokesize;
	
	
   float fi = index;
   fi = fi/num;

   float t = 1 - frac(fi + particleSpeed);
   float s = pow(t, particleSystemShape);

   float r = tex2Dlod(rnd, float4(fi+rndadd,0,0,1));
   float3 pos;
   float len = (MaterialDiffuse.a);
   len = 1-len * len;
   len *= (2 + t);

   len *= ParticleLen;
   
   float sqlen = sqrt(len);
   pos.x = (particleSpread * cos(48 * r) * sqlen * TotalScale)+(len*wind);
   pos.y = particleSpread * sin(48 * r) * sqlen * TotalScale;
   pos.z = -(particleSpread * cos(r) * sqlen * r) * up * TotalScale;
   pos.z -= TotalScale+(len*fpoint);

   float3 w = (particleSize * float3(Pos.xy - float2(0,0),0)) * max(0,(1-t * ScaleSpd));
   //通常回転
   //回転行列の作成
   float rad = time_0_X * RotateSpd*(0.5-r) + t + cos(62 * r);
   float4x4 matRot;
   matRot[0] = float4(cos(rad),sin(rad),0,0); 
   matRot[1] = float4(-sin(rad),cos(rad),0,0); 
   matRot[2] = float4(0,0,1,0); 
   matRot[3] = float4(0,0,0,1); 
   w = mul(w,matRot);
   //ビルボード回転
   w = mul(w,inv_view_matrix);
   //拡大
   w = mul(w,length(world_matrix[0])*4*Scale*TotalScale);

   // Billboard the quads.
   // The view matrix gives us our right and up vectors.
   //pos += (Pos.x * view_trans_matrix[0] + Pos.y * view_trans_matrix[1]);
   pos += w;
   pos /= 10;
   
   
   Out.Pos = mul(float4(pos, 1), world_view_proj_matrix);

   Out.color = MaterialDiffuse.a*(1-AlphaSpd) - t*0.25;
   Out.color = lerp(0,Out.color,min(1,(1-MaterialDiffuse.a)*32));

	pos = mul(float4(pos,1),world_matrix);

	Out.LastPos = Out.Pos;
	Out.WPos = pos;
	Out.Eye = pos - CameraPosition;
   return Out;}

VS_OUTPUT AllFunc(int num,float4 Pos,float Scale,float ParticleLen,float AlphaSpd,float rndadd,float up)
{
  VS_OUTPUT Out;
   //火柱&火炎用
   Out.Tex = Pos.xy*0.5+0.5;
   Out.Tex *= 0.5;
	
	//砲煙サイズ調整

spoint    = spoint*smokesize;
mspoint   = mspoint*smokesize;
fpoint    = fpoint*smokesize;
fwpoint   = fwpoint*smokesize;
wsmpoint  = wsmpoint*smokesize; 
ssmpoint  = ssmpoint*smokesize;
gbrspoint = gbrspoint*smokesize;
brspoint  = brspoint*smokesize;

   float fi = index;
   fi = fi/num;

   float t = 1 - frac(fi + particleSpeed);
   float s = pow(t, particleSystemShape);

   float r = tex2Dlod(rnd, float4(fi+rndadd,0,0,1));
   float3 pos;
   float len = (MaterialDiffuse.a);
   len = 1-len * len;
   len *= (2 + t);

   len *= ParticleLen;
   
   float sqlen = sqrt(len);
   pos.x = (particleSpread * cos(48 * r) * sqlen * TotalScale)+(len*wind);
   pos.y = particleSpread * sin(48 * r) * sqlen * TotalScale;
   pos.z = -(particleSpread * cos(r) * sqlen * r) * up * TotalScale;
   pos.z -= TotalScale+(len*fwpoint);

   float3 w = (particleSize * float3(Pos.xy - float2(0,0),0)) * max(0,(1-t * ScaleSpd));
   //通常回転
   //回転行列の作成
   float rad = time_0_X * RotateSpd*(0.5-r) + t + cos(62 * r);
   float4x4 matRot;
   matRot[0] = float4(cos(rad),sin(rad),0,0); 
   matRot[1] = float4(-sin(rad),cos(rad),0,0); 
   matRot[2] = float4(0,0,1,0); 
   matRot[3] = float4(0,0,0,1); 
   w = mul(w,matRot);
   //ビルボード回転
   w = mul(w,inv_view_matrix);
   //拡大
   w = mul(w,length(world_matrix[0])*2*Scale*TotalScale);

   // Billboard the quads.
   // The view matrix gives us our right and up vectors.
   //pos += (Pos.x * view_trans_matrix[0] + Pos.y * view_trans_matrix[1]);
   pos += w;
   pos /= 10;
   
   
   Out.Pos = mul(float4(pos, 1), world_view_proj_matrix);

   Out.color = MaterialDiffuse.a*(1-AlphaSpd) - t*0.25;
   Out.color = lerp(0,Out.color,min(1,(1-MaterialDiffuse.a)*32));

	pos = mul(float4(pos,1),world_matrix);

	Out.LastPos = Out.Pos;
	Out.WPos = pos;
	Out.Eye = pos - CameraPosition;
   return Out;
}
VS_OUTPUT2 AllFunc5(int num,float4 Pos,float Scale,float ParticleLen,float AlphaSpd,float rndadd,float up)
{ VS_OUTPUT2 Out;
   //フラッシュ専用
   Out.Tex = Pos.xy*0.5+0.5;
   Out.Tex *= 0.5;
//砲煙サイズ調整

spoint    = spoint*smokesize;
mspoint   = mspoint*smokesize;
fpoint    = fpoint*smokesize;
fwpoint   = fwpoint*smokesize;
wsmpoint  = wsmpoint*smokesize; 
ssmpoint  = ssmpoint*smokesize;
gbrspoint = gbrspoint*smokesize;
brspoint  = brspoint*smokesize;
	
	
   float fi = index;
   fi = fi/num;

   float t = 1 - frac(fi + particleSpeed);
   float s = pow(t, particleSystemShape);

   float r = tex2Dlod(rnd, float4(fi+rndadd,0,0,1));
   float3 pos;
   float len = (MaterialDiffuse.a);
   len = 1-len * len;
   len *= (2 + t);

   len *= ParticleLen;
   
   float sqlen = sqrt(len);
   pos.x = (particleSpread * cos(48 * r) * sqlen * TotalScale)+(len*wind);
   pos.y = particleSpread * sin(48 * r) * sqlen * TotalScale;
   pos.z = -(particleSpread * cos(r) * sqlen * r) * up * TotalScale;
   pos.z -= TotalScale;

   float3 w = (particleSize * float3(Pos.xy - float2(0,0),0)) * max(0,(1-t * ScaleSpd));
   //通常回転
   //回転行列の作成
   float rad = time_0_X * RotateSpd*(0.5-r) + t + cos(62 * r);
   float4x4 matRot;
   matRot[0] = float4(cos(rad),sin(rad),0,0); 
   matRot[1] = float4(-sin(rad),cos(rad),0,0); 
   matRot[2] = float4(0,0,1,0); 
   matRot[3] = float4(0,0,0,1); 
   w = mul(w,matRot);
   //ビルボード回転
   w = mul(w,inv_view_matrix);
   //拡大
   w = mul(w,length(world_matrix[0])*1*Scale*TotalScale)/(len*5000);

   // Billboard the quads.
   // The view matrix gives us our right and up vectors.
   //pos += (Pos.x * view_trans_matrix[0] + Pos.y * view_trans_matrix[1]);
   pos += w;
   pos /= 10;
   
   
   Out.Pos = mul(float4(pos, 1), world_view_proj_matrix);

   Out.color = MaterialDiffuse.a*(1-AlphaSpd) - t*0.25;
   Out.color = lerp(0,Out.color,min(1,(1-MaterialDiffuse.a))*64)/(1+len*6000);

	pos = mul(float4(pos,1),world_matrix);

	Out.LastPos = Out.Pos;
	Out.WPos = pos;
	Out.Eye = pos - CameraPosition;
   return Out;}

VS_OUTPUT ParticleFunc(int num,float4 Pos,float Scale,float ParticleLen,float AlphaSpd,float rndadd)
{
  VS_OUTPUT Out;
   
   Out.Tex = Pos.xy*0.48+0.5;
   Out.Tex *= 0.5;
   Out.Tex+=0.01;
   float fi = index;
   fi = fi/num;
   float r = tex2Dlod(rnd, float4(fi+rndadd,0,0,1));
   float t = 1- frac(fi + particleSpeed);
   float s = pow(t, particleSystemShape);

   float3 pos;
   float len = 1 * (MaterialDiffuse.a);
   len = 1-len * len;
   len *= (2 + t);

   len *= ParticleLen;
   
   pos.x = particleSpread * cos(64 * r) * len;
   pos.z = particleSpread * cos(32 * r) * len;
   pos.y = particleSpread * cos(12 * r) * len;

   float3 w = (particleSize * float3(Pos.xy - float2(0,0),0)) * max(0,(1-t * ScaleSpd));
   //通常回転
   //回転行列の作成
   float rad = t * RotateSpd + cos(62 * fi);
   float4x4 matRot;
   matRot[0] = float4(cos(rad),sin(rad),0,0); 
   matRot[1] = float4(-sin(rad),cos(rad),0,0); 
   matRot[2] = float4(0,0,1,0); 
   matRot[3] = float4(0,0,0,1); 
   w = mul(w,matRot);
   //ビルボード回転
   w = mul(w,inv_view_matrix);
   //拡大
   w = mul(w,length(world_matrix[0])*2*Scale) * (MaterialDiffuse.a*(1-AlphaSpd)*0.25 - t*0.25);

   // Billboard the quads.
   // The view matrix gives us our right and up vectors.
   //pos += (Pos.x * view_trans_matrix[0] + Pos.y * view_trans_matrix[1]);
   pos += w;
   pos /= 10;
   
   
   Out.Pos = mul(float4(pos, 1), world_view_proj_matrix);

	pos = mul(float4(pos,1),world_matrix);

	Out.LastPos = Out.Pos;
	Out.WPos = pos;
	Out.Eye = pos - CameraPosition;
   Out.color = 1;	
   return Out;
}
VS_OUTPUT AllFunc6(int num,float4 Pos,float Scale,float ParticleLen,float AlphaSpd,float rndadd,float up)
{
  VS_OUTPUT Out;
   //広域煙用
   Out.Tex = Pos.xy*0.5+0.5;
   Out.Tex *= 0.5;
	
	//砲煙サイズ調整

spoint    = spoint*smokesize;
mspoint   = mspoint*smokesize;
fpoint    = fpoint*smokesize;
fwpoint   = fwpoint*smokesize;
wsmpoint  = wsmpoint*smokesize; 
ssmpoint  = ssmpoint*smokesize;
gbrspoint = gbrspoint*smokesize;
brspoint  = brspoint*smokesize;

   float fi = index;
   fi = fi/num;

   float t = 1 - frac(fi + particleSpeed);
   float s = pow(t, particleSystemShape);

   float r = tex2Dlod(rnd, float4(fi+rndadd,0,0,1));
   float3 pos;
   float len = (MaterialDiffuse.a);
   len = 1-len * len;
   len *= (2 + t);

   len *= ParticleLen;
   
   float sqlen = sqrt(len);
   pos.x = (particleSpread * cos(48 * r) * sqlen * TotalScale)+(len*wind);
   pos.y = particleSpread * sin(48 * r) * sqlen * TotalScale;
   pos.z = -(particleSpread * cos(r) * sqlen * r) * up * TotalScale;
pos.z -= TotalScale+(len*wsmpoint);

   float3 w = (particleSize * float3(Pos.xy - float2(0,0),0)) * max(0,(1-t * ScaleSpd));
   //通常回転
   //回転行列の作成
   float rad = time_0_X * RotateSpd*(0.5-r) + t + cos(62 * r);
   float4x4 matRot;
   matRot[0] = float4(cos(rad),sin(rad),0,0); 
   matRot[1] = float4(-sin(rad),cos(rad),0,0); 
   matRot[2] = float4(0,0,1,0); 
   matRot[3] = float4(0,0,0,1); 
   w = mul(w,matRot);
   //ビルボード回転
   w = mul(w,inv_view_matrix);
   //拡大
   w = mul(w,length(world_matrix[0])*20*Scale*TotalScale)*(len*(wsmpoint/2))*(len*(wsmpoint/10));

   // Billboard the quads.
   // The view matrix gives us our right and up vectors.
   //pos += (Pos.x * view_trans_matrix[0] + Pos.y * view_trans_matrix[1]);
   pos += w;
   pos /= 10;
   
   
   Out.Pos = mul(float4(pos, 1), world_view_proj_matrix);

   Out.color = MaterialDiffuse.a*(1-AlphaSpd) - t*0.25;
   Out.color = lerp(0,Out.color,min(1,(1-MaterialDiffuse.a)*32));

	pos = mul(float4(pos,1),world_matrix);

	Out.LastPos = Out.Pos;
	Out.WPos = pos;
	Out.Eye = pos - CameraPosition;
   return Out;
}

VS_OUTPUT AllFunc7(int num,float4 Pos,float Scale,float ParticleLen,float AlphaSpd,float rndadd,float up)
{
  VS_OUTPUT Out;
   //追加フラッシュ
   Out.Tex = Pos.xy*0.5+0.5;
   Out.Tex *= 0.5;
	
	//砲煙サイズ調整

spoint    = spoint*smokesize;
mspoint   = mspoint*smokesize;
fpoint    = fpoint*smokesize;
fwpoint   = fwpoint*smokesize;
wsmpoint  = wsmpoint*smokesize; 
ssmpoint  = ssmpoint*smokesize;
gbrspoint = gbrspoint*smokesize;
brspoint  = brspoint*smokesize;

   float fi = index;
   fi = fi/num;

   float t = 1 - frac(fi + particleSpeed);
   float s = pow(t, particleSystemShape);

   float r = tex2Dlod(rnd, float4(fi+rndadd,0,0,1));
   float3 pos;
   float len = (MaterialDiffuse.a);
   len = 1-len * len;
   len *= (2 + t);

   len *= ParticleLen;
   
   float sqlen = sqrt(len);
   pos.x = (particleSpread * cos(48 * r) * sqlen * TotalScale);
   pos.y = particleSpread * sin(48 * r) * sqlen * TotalScale;
   pos.z = -(particleSpread * cos(r) * sqlen * r) * up * TotalScale;
   pos.z -= TotalScale;

   float3 w = (particleSize * float3(Pos.xy - float2(0,0),0)) * max(0,(1-t * ScaleSpd));
   //通常回転
   //回転行列の作成
   float rad = time_0_X * RotateSpd*(0.5-r) + t + cos(62 * r);
   float4x4 matRot;
   matRot[0] = float4(cos(rad),sin(rad),0,0); 
   matRot[1] = float4(-sin(rad),cos(rad),0,0); 
   matRot[2] = float4(0,0,1,0); 
   matRot[3] = float4(0,0,0,1); 
   w = mul(w,matRot);
   //ビルボード回転
   w = mul(w,inv_view_matrix);
   //拡大
   w = mul(w,length(world_matrix[0])*2*Scale*TotalScale);

   // Billboard the quads.
   // The view matrix gives us our right and up vectors.
   //pos += (Pos.x * view_trans_matrix[0] + Pos.y * view_trans_matrix[1]);
   pos += w;
   pos /= 10;
   
   
   Out.Pos = mul(float4(pos, 1), world_view_proj_matrix);

   Out.color = MaterialDiffuse.a*(1-AlphaSpd) - t*0.85;
   Out.color = lerp(0,Out.color,min(1,(1-MaterialDiffuse.a)*64));

	pos = mul(float4(pos,1),world_matrix);

	Out.LastPos = Out.Pos;
	Out.WPos = pos;
	Out.Eye = pos - CameraPosition;
   return Out;
}

VS_OUTPUT2 AllFunc8(int num,float4 Pos,float Scale,float ParticleLen,float AlphaSpd,float rndadd,float up)
{ VS_OUTPUT2 Out;
   //追加煙柱
   Out.Tex = Pos.xy*0.5+0.5;
   Out.Tex *= 0.5;
	
	//砲煙サイズ調整

spoint    = spoint*smokesize;
mspoint   = mspoint*smokesize;
fpoint    = fpoint*smokesize;
fwpoint   = fwpoint*smokesize;
wsmpoint  = wsmpoint*smokesize; 
ssmpoint  = ssmpoint*smokesize;
gbrspoint = gbrspoint*smokesize;
brspoint  = brspoint*smokesize;

   float fi = index;
   fi = fi/num;

   float t = 1 - frac(fi + particleSpeed);
   float s = pow(t, particleSystemShape);

   float r = tex2Dlod(rnd, float4(fi+rndadd,0,0,1));
   float3 pos;
   float len = (MaterialDiffuse.a);
   len = 1-len * len;
   len *= (2 + t);

   len *= ParticleLen;
   
   float sqlen = sqrt(len);
   pos.x = (particleSpread * cos(48 * r) * sqlen * TotalScale)+(len*wind);
   pos.y = particleSpread * sin(48 * r) * sqlen * TotalScale;
   pos.z = -(particleSpread * cos(r) * sqlen * r) * up * TotalScale;
   pos.z -= TotalScale+spoint+(len*(mspoint*3));

   float3 w = (particleSize * float3(Pos.xy - float2(0,0),0)) * max(0,(1-t * ScaleSpd));
   //通常回転
   //回転行列の作成
   float rad = time_0_X * RotateSpd*(0.5-r) + t + cos(62 * r);
   float4x4 matRot;
   matRot[0] = float4(cos(rad),sin(rad),0,0); 
   matRot[1] = float4(-sin(rad),cos(rad),0,0); 
   matRot[2] = float4(0,0,1,0); 
   matRot[3] = float4(0,0,0,1); 
   w = mul(w,matRot);
   //ビルボード回転
   w = mul(w,inv_view_matrix);
   //拡大
   w = mul(w,length(world_matrix[0])*4*Scale*TotalScale)*(len*(ssmpoint*2));

   // Billboard the quads.
   // The view matrix gives us our right and up vectors.
   //pos += (Pos.x * view_trans_matrix[0] + Pos.y * view_trans_matrix[1]);
   pos += w;
   pos /= 10;
   
   
   Out.Pos = mul(float4(pos, 1), world_view_proj_matrix);

   Out.color = MaterialDiffuse.a*(1-AlphaSpd) - t*0.15;
   Out.color = lerp(0,Out.color,min(1,(1-MaterialDiffuse.a)*32));

	pos = mul(float4(pos,1),world_matrix);

	Out.LastPos = Out.Pos;
	Out.WPos = pos;
	Out.Eye = pos - CameraPosition;
   return Out;}
VS_OUTPUT AllFunc9(int num,float4 Pos,float Scale,float ParticleLen,float AlphaSpd,float rndadd,float up)
{
   VS_OUTPUT Out;
   //発射残煙演出1
   Out.Tex = Pos.xy*0.5+0.5;
   Out.Tex *= 0.5;
	
	
	//砲煙サイズ調整

spoint    = spoint*smokesize;
mspoint   = mspoint*smokesize;
fpoint    = fpoint*smokesize;
fwpoint   = fwpoint*smokesize;
wsmpoint  = wsmpoint*smokesize; 
ssmpoint  = ssmpoint*smokesize;
gbrspoint = gbrspoint*smokesize;
brspoint  = brspoint*smokesize;

   float fi = index;
   fi = fi/num;

   float t = 1 - frac(fi + particleSpeed);
   float s = pow(t, particleSystemShape);

   float r = tex2Dlod(rnd, float4(fi+rndadd,0,0,1));
   float3 pos;
   float len = (MaterialDiffuse.a);
   len = 1-len * len;
   len *= (1 + t);

   len *= ParticleLen;

   float sqlen = sqrt(len);
   pos.x = particleSpread * cos(48 * r) * sqlen      ;
   pos.y = particleSpread * sin(48 * r) * sqlen    ;
   pos.z = -(particleSpread * cos(r) * sqlen * r) * up;
	pos.z -= TotalScale -(gbrspoint+20)+(len*(brspoint*10))+(len*len*(brspoint*3));
	
   float3 w = (particleSize * float3(Pos.xy - float2(0,0),0)) * max(0,(1-t * ScaleSpd));
   //通常回転
    //回転行列の作成
   float rad = time_0_X * RotateSpd*(0.5-r) + t + cos(62 * r);
   float4x4 matRot;
   matRot[0] = float4(cos(rad),sin(rad),0,0); 
   matRot[1] = float4(-sin(rad),cos(rad),0,0); 
   matRot[2] = float4(0,0,1,0); 
   matRot[3] = float4(0,0,0,1); 
   w = mul(w,matRot);
   //ビルボード回転
   w = mul(w,inv_view_matrix);
   //拡大
   w = mul(w,length(world_matrix[0])*0.02*Scale*TotalScale)*1*(len*len*(brspoint*15))*(num/(brspoint*0.9))*((1-len)*(num/(brspoint*2)));

   // Billboard the quads.
   // The view matrix gives us our right and up vectors.
   //pos += (Pos.x * view_trans_matrix[0] + Pos.y * view_trans_matrix[1]);
   pos += w;
   pos /= 10;
   
   
   Out.Pos = mul(float4(pos, 1), world_view_proj_matrix);

	Out.color = (MaterialDiffuse.a*(1-AlphaSpd)/1.85) - ((t/2.5)*0.15);
   Out.color = lerp(0,Out.color,min(1,(1-MaterialDiffuse.a)*32))*(len*80);

	pos = mul(float4(pos,1),world_matrix);

	Out.LastPos = Out.Pos;
	Out.WPos = pos;
	Out.Eye = pos - CameraPosition;
   return Out;
}
VS_OUTPUT AllFunc10(int num,float4 Pos,float Scale,float ParticleLen,float AlphaSpd,float rndadd,float up)
{
   VS_OUTPUT Out;
   
   Out.Tex = Pos.xy*0.5+0.5;
   Out.Tex *= 0.5;
	
	//砲煙サイズ調整

spoint    = spoint*smokesize;
mspoint   = mspoint*smokesize;
fpoint    = fpoint*smokesize;
fwpoint   = fwpoint*smokesize;
wsmpoint  = wsmpoint*smokesize; 
ssmpoint  = ssmpoint*smokesize;
gbrspoint = gbrspoint*smokesize;
brspoint  = brspoint*smokesize;

   float fi = index;
   fi = fi/num;

   float t = 1 - frac(fi + particleSpeed);
   float s = pow(t, particleSystemShape);

   float r = tex2Dlod(rnd, float4(fi+rndadd,0,0,1));
   float3 pos;
   float len = (MaterialDiffuse.a);
   len = 1-len * len;
   len *= (1 + t);

   len *= ParticleLen;
   
 
   //pos.x = particleSpread * cos(r)*len;
	//pos.z = -(particleSpread * cos(r) * len* r) *up*9;
   //pos.y = (particleSpread * cos( r) * len * r);

   float sqlen = sqrt(len);
   pos.x = particleSpread * cos(48 * r) * sqlen      ;
   pos.y = particleSpread * sin(48 * r) * sqlen +10      ;
   pos.z = -(particleSpread * cos(r) * sqlen * r) * up;
	pos.z -= TotalScale -(gbrspoint) +(len*(brspoint))+(len*len*(brspoint*150));
	   //pos.x = particleSpread * cos(32 * r) * len;
   //pos.z = particleSpread * sin(32 * r) * len;
   //pos.y = (particleSpread * cos(r) * len * r) * up *20;

	
   float3 w = (particleSize * float3(Pos.xy - float2(0,0),0)) * max(0,(1-t * ScaleSpd));
   //通常回転
    //回転行列の作成
   float rad = time_0_X * RotateSpd*(0.5-r) + t + cos(62 * r);
   float4x4 matRot;
   matRot[0] = float4(cos(rad),sin(rad),0,0); 
   matRot[1] = float4(-sin(rad),cos(rad),0,0); 
   matRot[2] = float4(0,0,1,0); 
   matRot[3] = float4(0,0,0,1); 
   w = mul(w,matRot);
   //ビルボード回転
   w = mul(w,inv_view_matrix);
   //拡大
   w = mul(w,length(world_matrix[0])*0.1*Scale*TotalScale)*(len*20);

   // Billboard the quads.
   // The view matrix gives us our right and up vectors.
   //pos += (Pos.x * view_trans_matrix[0] + Pos.y * view_trans_matrix[1]);
   pos += w;
   pos /= 10;
   
   
   Out.Pos = mul(float4(pos, 1), world_view_proj_matrix);

	Out.color = (MaterialDiffuse.a*(1-AlphaSpd)/1.85) - ((t/2.5)*0.15);
   Out.color = lerp(0,Out.color,min(1,(1-MaterialDiffuse.a)*32))*(len*80);

	pos = mul(float4(pos,1),world_matrix);

	Out.LastPos = Out.Pos;
	Out.WPos = pos;
	Out.Eye = pos - CameraPosition;
   return Out;
}
VS_OUTPUT AllFunc11(int num,float4 Pos,float Scale,float ParticleLen,float AlphaSpd,float rndadd,float up)
{
   VS_OUTPUT Out;
   
   Out.Tex = Pos.xy*0.5+0.5;
   Out.Tex *= 0.5;
	
	//砲煙サイズ調整

spoint    = spoint*smokesize;
mspoint   = mspoint*smokesize;
fpoint    = fpoint*smokesize;
fwpoint   = fwpoint*smokesize;
wsmpoint  = wsmpoint*smokesize; 
ssmpoint  = ssmpoint*smokesize;
gbrspoint = gbrspoint*smokesize;
brspoint  = brspoint*smokesize;

   float fi = index;
   fi = fi/num;

   float t = 1 - frac(fi + particleSpeed);
   float s = pow(t, particleSystemShape);

   float r = tex2Dlod(rnd, float4(fi+rndadd,0,0,1));
   float3 pos;
   float len = (MaterialDiffuse.a);
   len = 1-len * len;
   len *= (1 + t);

   len *= ParticleLen;
   
 
   //pos.x = particleSpread * cos(r)*len;
	//pos.z = -(particleSpread * cos(r) * len* r) *up*9;
   //pos.y = (particleSpread * cos( r) * len * r);

   float sqlen = sqrt(len);
   pos.x = particleSpread * cos(48 * r) * sqlen      ;
   pos.y = particleSpread * sin(48 * r) * sqlen      ;
   pos.z = -(particleSpread * cos(r) * sqlen * r) * up;
	pos.z -= TotalScale  *(len*(fwpoint*10));
	   //pos.x = particleSpread * cos(32 * r) * len;
   //pos.z = particleSpread * sin(32 * r) * len;
   //pos.y = (particleSpread * cos(r) * len * r) * up *20;

	
   float3 w = (particleSize * float3(Pos.xy - float2(0,0),0)) * max(0,(1-t * ScaleSpd));
   //通常回転
    //回転行列の作成
   float rad = time_0_X * RotateSpd*(0.1-r) + t + cos(62 * r);
   float4x4 matRot;
   matRot[0] = float4(cos(rad),sin(rad),0,0); 
   matRot[1] = float4(-sin(rad),cos(rad),0,0); 
   matRot[2] = float4(0,0,1,0); 
   matRot[3] = float4(0,0,0,1); 
   w = mul(w,matRot);
   //ビルボード回転
   w = mul(w,inv_view_matrix);
   //拡大
   w = mul(w,length(world_matrix[0])*3*Scale*TotalScale)*(len*16);

   // Billboard the quads.
   // The view matrix gives us our right and up vectors.
   //pos += (Pos.x * view_trans_matrix[0] + Pos.y * view_trans_matrix[1]);
   pos += w;
   pos /= 10;
   
   
   Out.Pos = mul(float4(pos, 1), world_view_proj_matrix);

	Out.color = (MaterialDiffuse.a*(1-AlphaSpd)/1.85) - ((t/2.5)*0.25);
   Out.color = lerp(0,Out.color,min(1,(1-MaterialDiffuse.a)*32))*(len*10);

	pos = mul(float4(pos,1),world_matrix);

	Out.LastPos = Out.Pos;
	Out.WPos = pos;
	Out.Eye = pos - CameraPosition;
   return Out;
}
VS_OUTPUT FlashVS(float4 Pos: POSITION){
   VS_OUTPUT Out = (VS_OUTPUT)0;
   if(MaterialDiffuse.a != 0 && MaterialDiffuse.a != 1)
   {
   		Out = AllFunc5(SmokeNum,Pos, 2, 0.01, 0.05, 12, 12);
	    Out.Tex.y += 0.5;
   }
   return Out;
}
VS_OUTPUT FlashVS2(float4 Pos: POSITION){
   VS_OUTPUT Out = (VS_OUTPUT)0;
   if(MaterialDiffuse.a != 0 && MaterialDiffuse.a != 1)
   {
   		Out = AllFunc2(SmokeNum,Pos, 0.4,  0.5,  0.15, 0, 6);
	    
   }
   return Out;
}
VS_OUTPUT FlashVS3(float4 Pos: POSITION){
   VS_OUTPUT Out = (VS_OUTPUT)0;
   if(MaterialDiffuse.a != 0 && MaterialDiffuse.a != 1)
   {
   		Out = AllFunc7(SmokeNum,Pos, 0.2,  0.01,  0.01, 0, 0);
	    Out.Tex.y += 0.5;
   }
   return Out;
}
VS_OUTPUT2 SmokeVS(float4 Pos: POSITION){
   VS_OUTPUT Out = (VS_OUTPUT)0;
   if(MaterialDiffuse.a != 0 && MaterialDiffuse.a != 1)
   {
   		Out = AllFunc2(SmokeNum,Pos, 0.4,  0.5,  0.35, 0, 4);
   }
   return Out;
}
VS_OUTPUT SmokeHiSpeedVS(float4 Pos: POSITION){
   VS_OUTPUT Out = (VS_OUTPUT)0;
   if(MaterialDiffuse.a != 0 && MaterialDiffuse.a != 1)
   {
   		Out = AllFunc2(SmokeNum,Pos, 0.3, 0.1, 0.35 ,0 ,8);
   }
   return Out;
}
VS_OUTPUT FireVS(float4 Pos: POSITION){
   VS_OUTPUT Out = (VS_OUTPUT)0;
   if(MaterialDiffuse.a != 0 && MaterialDiffuse.a != 1)
   {
   		Out = AllFunc4(SmokeNum,Pos, 0.3, 0.4, 0.65 ,-20 ,6.5);
   	
	    Out.Tex.xy += 0.5;
   }
   return Out;
}
VS_OUTPUT FireVS2(float4 Pos: POSITION){
   VS_OUTPUT Out = (VS_OUTPUT)0;
   if(MaterialDiffuse.a != 0 && MaterialDiffuse.a != 1)
   {
   		Out = AllFunc3(SmokeNum,Pos,0.5, 0.1, 0.15 ,0 ,1);
   	
   }
   return Out;
}
VS_OUTPUT FireWallVS(float4 Pos: POSITION){
   VS_OUTPUT Out = (VS_OUTPUT)0;
   if(MaterialDiffuse.a != 0 && MaterialDiffuse.a != 1)
   {
   		Out = AllFunc(FireWallNum,Pos,0.8,0.05, 0.8, 0 ,24);
	    Out.Tex.xy += 0.5;
   }
   return Out;
}

VS_OUTPUT SmokeWallVS(float4 Pos: POSITION){
   VS_OUTPUT Out = (VS_OUTPUT)0;
   if(MaterialDiffuse.a != 0 && MaterialDiffuse.a != 1)
   {
   		Out = AllFunc2(FireWallNum,Pos, 0.5, 0.5, 0.3, 0 , 1);
   }
   return Out;
}

VS_OUTPUT SmokeWallVS2(float4 Pos: POSITION){
   VS_OUTPUT Out = (VS_OUTPUT)0;
   if(MaterialDiffuse.a != 0 && MaterialDiffuse.a != 1)
   {
   		Out = AllFunc8(FireWallNum,Pos, 0.1, 0.051, 0.3, 0 , 24);
   }
   return Out;
}

VS_OUTPUT ParticleVS(float4 Pos: POSITION){
   VS_OUTPUT Out = (VS_OUTPUT)0;
   if(MaterialDiffuse.a != 0 && MaterialDiffuse.a != 1)
   {
   		Out =AllFunc(ParticleNum,Pos, 0.2, 0.01, 0.85, 0 , 32);
   		Out.Tex.xy += 0.5;
   }
   return Out;
}
VS_OUTPUT SmokeVS2(float4 Pos: POSITION){
   VS_OUTPUT Out = (VS_OUTPUT)0;
   if(MaterialDiffuse.a != 0 && MaterialDiffuse.a != 1)
   {
   		Out =AllFunc6(ParticleNum,Pos, 0.1, 0.01, 0.75, 0 , 32);
   
   }
   return Out;
}
VS_OUTPUT SmokeVS3(float4 Pos: POSITION){
   VS_OUTPUT Out = (VS_OUTPUT)0;
   if(MaterialDiffuse.a != 0 && MaterialDiffuse.a != 1)
   {
   		Out =AllFunc9(ParticleNum2,Pos, 0.05, 0.1, 0.83, 0 ,36);
   
   }
   return Out;
}
VS_OUTPUT SmokeVS4(float4 Pos: POSITION){
   VS_OUTPUT Out = (VS_OUTPUT)0;
   if(MaterialDiffuse.a != 0 && MaterialDiffuse.a != 1)
   {
   		Out =AllFunc9(ParticleNum2,Pos, 0.05, 0.1, 0.85, 0, 36);
   
   }
   return Out;
}
VS_OUTPUT SmokeVS5(float4 Pos: POSITION){
   VS_OUTPUT Out = (VS_OUTPUT)0;
   if(MaterialDiffuse.a != 0 && MaterialDiffuse.a != 1)
   {
   		Out =AllFunc10(ParticleNum2,Pos,1, 0.1, 0.85, 0, 64);
   
   }
   return Out;
}
VS_OUTPUT SmokeVS6(float4 Pos: POSITION){
   VS_OUTPUT Out = (VS_OUTPUT)0;
   if(MaterialDiffuse.a != 0 && MaterialDiffuse.a != 1)
   {
   		Out =AllFunc11(ParticleNum3,Pos,1, 0.1, 0.85, 0, 12);
   
   }
   return Out;
}
float particleShape
<
   string UIName = "particleShape";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 1.00;
> = float( 0.37 );
texture Particle_Tex
<
   string ResourceName = "particle.png";
>;
sampler Particle = sampler_state
{
   Texture = (Particle_Tex);
   ADDRESSU = CLAMP;
   ADDRESSV = CLAMP;
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
};
float4 AllPS(VS_OUTPUT IN) : COLOR {
   float4 col = tex2D(Particle,IN.Tex-0.005);
   col.a *= IN.color * DefAlpha;
   
	if(use_spe)
	{
		float2 ScTex = IN.LastPos.xyz/IN.LastPos.w;
		ScTex.y *= -1;
		ScTex.xy += 1;
		ScTex.xy *= 0.5;
	    // 深度
	    float dep = length(CameraPosition - IN.WPos);
	    float scrdep = tex2D(SPE_DepthSamp,ScTex).r;

	    //return float4(smoothstep(0,59,scrdep),0,0,1);
	    //return float4(smoothstep(0,59,dep),0,0,1);
	    
	    float adddep = 1-saturate(length(abs(frac(IN.Tex*4)-0.5)));
	    dep = length(dep-scrdep);
	    dep = smoothstep(0,10,dep);
	    col.a *= dep;
    }
    
   return col;
}
//合成方法の設定
//
//半透明合成：
//BLENDMODE_SRC SRCALPHA
//BLENDMODE_DEST INVSRCALPHA
//
//加算合成：
//
//BLENDMODE_SRC SRCALPHA
//BLENDMODE_DEST ONE

#define BLENDMODE_SRC SRCALPHA
#define BLENDMODE_DEST INVSRCALPHA
//--------------------------------------------------------------//
// Technique Section for Effect Workspace.Particle Effects.FireParticleSystem
//--------------------------------------------------------------//
technique FireParticleSystem <
    string Script = 
    
		"LoopByCount=SmokeNum;"
        "LoopGetIndex=index;"
	    "Pass=SmokePass;"
        "LoopEnd=;"
       
		"LoopByCount=SmokeNum;"
        "LoopGetIndex=index;"
	    "Pass=SmokePass_HiSpeed;"
        "LoopEnd=;"
        
		"LoopByCount=FireWallNum;"
        "LoopGetIndex=index;"
	    "Pass=SmokeWallPass;"
        "LoopEnd=;"
 
 		"LoopByCount=FireWallNum;"
        "LoopGetIndex=index;"
	    "Pass=SmokeWallPass2;"
        "LoopEnd=;"
 
		"LoopByCount=FlashNum;"
        "LoopGetIndex=index;"
	    "Pass=FlashPass;"
        "LoopEnd=;"
        
        "LoopByCount=FlashNum;"
        "LoopGetIndex=index;"
	    "Pass=FlashPass2;"
        "LoopEnd=;"
        
           
        "LoopByCount=FlashNum;"
        "LoopGetIndex=index;"
	    "Pass=FlashPass3;"
        "LoopEnd=;"
        
        
		"LoopByCount=FireNum;"
        "LoopGetIndex=index;"
	    "Pass=FirePass;"
        "LoopEnd=;"

	"LoopByCount=FireNum;"
        "LoopGetIndex=index;"
	    "Pass=FirePass2;"
        "LoopEnd=;"

		"LoopByCount=FireWallNum;"
        "LoopGetIndex=index;"
	    "Pass=FireWallPass;"
        "LoopEnd=;"
        
		"LoopByCount=ParticleNum;"
        "LoopGetIndex=index;"
	    "Pass=ParticlePass;"
        "LoopEnd=;"
        
        "LoopByCount=ParticleNum;"
        "LoopGetIndex=index;"
	    "Pass=SmokePass2;"
        "LoopEnd=;"
        
                "LoopByCount=ParticleNum;"
        "LoopGetIndex=index;"
	    "Pass=SmokePass3;"
        "LoopEnd=;"
        
                        "LoopByCount=ParticleNum;"
        "LoopGetIndex=index;"
	    "Pass=SmokePass4;"
        "LoopEnd=;"
        
                                "LoopByCount=ParticleNum;"
        "LoopGetIndex=index;"
	    "Pass=SmokePass5;"
        "LoopEnd=;"
        
                                        "LoopByCount=ParticleNum;"
        "LoopGetIndex=index;"
	    "Pass=SmokePass6;"
        "LoopEnd=;"
    ;
> {
   pass FlashPass
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = ONE;

      VertexShader = compile vs_3_0 FlashVS();
      PixelShader = compile ps_3_0 AllPS();
   }
	
	   pass FlashPass2
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = ONE;

      VertexShader = compile vs_3_0 FlashVS2();
      PixelShader = compile ps_3_0 AllPS();
   }
	
		   pass FlashPass3
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = ONE;

      VertexShader = compile vs_3_0 FlashVS3();
      PixelShader = compile ps_3_0 AllPS();
   }
   pass SmokePass
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = INVSRCALPHA;

      VertexShader = compile vs_3_0 SmokeVS();
      PixelShader = compile ps_3_0 AllPS();
   }
	

	   pass SmokePass2
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = ONE;

      VertexShader = compile vs_3_0 SmokeVS2();
      PixelShader = compile ps_3_0 AllPS();
   }
	
	
	   pass SmokePass3
   {
   	
   	
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = ONE;

   	
   	
      VertexShader = compile vs_3_0 SmokeVS3();
      PixelShader = compile ps_3_0 AllPS();
   }
	
		   pass SmokePass4
   {
   	
   	
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = INVSRCALPHA;

   	
   	
      VertexShader = compile vs_3_0 SmokeVS4();
      PixelShader = compile ps_3_0 AllPS();
   }
	
			   pass SmokePass5
   {
   	
   	
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = ONE;

   	
   	
      VertexShader = compile vs_3_0 SmokeVS5();
      PixelShader = compile ps_3_0 AllPS();
   }
	
	 pass SmokePass6
   {
   	
   	
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = ONE;

   	
   	
      VertexShader = compile vs_3_0 SmokeVS6();
      PixelShader = compile ps_3_0 AllPS();
   }
   pass SmokePass_HiSpeed
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = INVSRCALPHA;

      VertexShader = compile vs_3_0 SmokeHiSpeedVS();
      PixelShader = compile ps_3_0 AllPS();
   }
   pass FirePass
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = ONE;

      VertexShader = compile vs_3_0 FireVS();
      PixelShader = compile ps_3_0 AllPS();
   }
	   pass FirePass2
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = ONE;

      VertexShader = compile vs_3_0 FireVS2();
      PixelShader = compile ps_3_0 AllPS();
   }
   pass FireWallPass
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = ONE;

      VertexShader = compile vs_3_0 FireWallVS();
      PixelShader = compile ps_3_0 AllPS();
   }
   pass SmokeWallPass
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = INVSRCALPHA;

      VertexShader = compile vs_3_0 SmokeWallVS();
      PixelShader = compile ps_3_0 AllPS();
   }
	
	   pass SmokeWallPass2
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = INVSRCALPHA;

      VertexShader = compile vs_3_0 SmokeWallVS2();
      PixelShader = compile ps_3_0 AllPS();
   }
   pass ParticlePass
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = ONE;

      VertexShader = compile vs_3_0 ParticleVS();
      PixelShader = compile ps_3_0 AllPS();
   }
}

