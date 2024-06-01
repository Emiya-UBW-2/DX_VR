set NowDir=%CD%
cd %~dp0

ShaderCompiler.exe /Tvs_4_0 VS_lens.hlsl
ShaderCompiler.exe /Tps_4_0 PS_lens.hlsl

ShaderCompiler.exe /Tvs_4_0 VS_SSR.hlsl
ShaderCompiler.exe /Tps_4_0 PS_SSR.hlsl

ShaderCompiler.exe /Tvs_4_0 VS_DoF.hlsl
ShaderCompiler.exe /Tps_4_0 PS_DoF.hlsl

ShaderCompiler.exe /Tvs_4_0 DepthVS.fx
ShaderCompiler.exe /Tps_4_0 DepthPS.fx

ShaderCompiler.exe /Tvs_4_0 FXAA_VS.hlsl
ShaderCompiler.exe /Tps_4_0 FXAA_PS.hlsl

ShaderCompiler.exe /Tvs_4_0 GodRay_VS.hlsl
ShaderCompiler.exe /Tps_4_0 GodRay_PS.hlsl

pause

cd %NowDir%