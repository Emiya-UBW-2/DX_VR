set NowDir=%CD%
cd %~dp0

ShaderCompiler.exe /Tvs_4_0 VS_SSAO.hlsl
ShaderCompiler.exe /Tps_4_0 PS_SSAO.hlsl
ShaderCompiler.exe /Tps_4_0 PS_BilateralBlur.hlsl

pause

cd %NowDir%