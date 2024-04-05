set NowDir=%CD%
cd %~dp0
@rem ShaderCompiler.exe /Tvs_4_0 GetDepthVS.fx
@rem ShaderCompiler.exe /Tps_4_0 GetDepthPS.fx

ShaderCompiler.exe /Tvs_4_0 SkinMesh4_DirLight_DepthShadow_Step2VS.hlsl
ShaderCompiler.exe /Tvs_4_0 NormalMesh_DirLight_DepthShadow_Step2VS.hlsl
ShaderCompiler.exe /Tps_4_0 DirLight_DepthShadow_Step2PS.hlsl

pause

cd %NowDir%