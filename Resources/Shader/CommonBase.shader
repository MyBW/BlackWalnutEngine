vertex_shader  CommnonBaseVertexShader glsl
{
    Source  UBOTest.vert
}

fragment_shader  CommnonBaseFragmentShader glsl
{
   Source  UBOTest.frag
}

vertex_shader  DefferCommnonBaseVertexShader glsl
{
    Source  DefferTest.vert
}

fragment_shader  DefferCommnonBaseFragmentShader glsl
{
   Source  DefferTest.frag
}

vertex_shader  DefferRenderingVertexShader glsl
{
    Source  DefferRendering.vert
}

fragment_shader  DefferRenderingFragmentShader glsl
{
   Source  DefferRendering.frag
}

vertex_shader  PointLightDefferLightingVertexShader glsl
{
    Source  PointLightDefferLighting.vert
}

fragment_shader  PointLightDefferLightingFragmentShader glsl
{
   Source  PointLightDefferLighting.frag
}

vertex_shader  DirectionalLightShadowMapVertexShader glsl
{
    Source  DirectionalLightShadowMapVertexShader.vert
}

fragment_shader  DirectionalLightShadowMapFragmentShader glsl
{
   Source  DirectionalLightShadowMapFragmentShader.frag
}




vertex_shader  SkyBoxVertexShader glsl
{
    Source  SkyBoxVertexShader.vert
}

fragment_shader  SkyBoxFragmentShader glsl
{
   Source  SkyBoxFragmentShader.frag
}




vertex_shader  EnvironmentMapVertexShader glsl
{
    Source  EnvironmentMapVertexShader.vert
}

fragment_shader  EnvironmentMapFragmentShader glsl
{
   Source  EnvironmentMapFragmentShader.frag
}


vertex_shader  DefferDataProcessVertex glsl
{
    Source  DefferDataProcess.vert
}

fragment_shader  DefferDataProcessFragment glsl
{
   Source  DefferDataProcess.frag
}
fragment_shader  ClearDefferBufferDataFragment glsl
{
   Source  ClearDefferBufferData.frag
}

vertex_shader  DefferDirectLightingVertex glsl
{
    Source  DefferDirectLighting.vert
}

fragment_shader  DefferDirectLightingFragment glsl
{
   Source  DefferDirectLighting.frag
}

vertex_shader PreprocessEvnMapVertex glsl
{
  Source PreprocessEvnMap.vert
}

fragment_shader PreprocessEvnMapFragment glsl
{
  Source PreprocessEvnMap.frag
}

vertex_shader PreprocessEvnMapForSpecularVertex glsl
{
  Source PreprocessEvnMap.vert
}

fragment_shader PreprocessEvnMapForSpecularFragment glsl
{
  Source PreprocessEvnMapForSpecular.frag
}

vertex_shader PreprocessEvnMapLUTVertex glsl
{
  Source DefferDirectLighting.vert
}

fragment_shader PreprocessEvnMapLUTFragment glsl
{
  Source PerprocessEvnMapForLUT.frag
}

fragment_shader DefferDataProcessFragmentWithTexture glsl
{
  Source DefferDataProcessWithTextureInput.frag
}

vertex_shader ConvertEquirectangularToCubeMapVertex glsl
{
  Source ConvertEquirectangularToCubeMap.vert
}

fragment_shader ConvertEquirectangularToCubeMapFragment glsl
{
  Source ConvertEquirectangularToCubeMap.frag
}

vertex_shader SHConvolutionVertex glsl
{
  Source SHConvolution.vert
}

fragment_shader SHConvolutionFragment glsl
{
  Source SHConvolution.frag
}


vertex_shader AccumulateDiffuseVertex glsl
{
  Source AccumulateDiffuse.vert
}

fragment_shader AccumulateDiffuseFragment glsl
{
  Source AccumulateDiffuse.frag
}


vertex_shader AccumulateReadCubeMapVertex glsl
{
  Source AccumulateReadCubeMap.vert
}

fragment_shader AccumulateReadCubeMapFragment glsl
{
  Source AccumulateReadCubeMap.frag
}


vertex_shader ImageBaseLightingVertex glsl
{
  Source ImageBaseLighting.vert
}

fragment_shader ImageBaseLightingFragment glsl
{
  Source ImageBaseLighting.frag
}


fragment_shader AmibientOcclusionFragment glsl
{
  Source PostProcessAO.frag
}

fragment_shader AmibientOcclusionFilterFragment glsl
{
  Source PostProcessAOFilter.frag
}

fragment_shader DownSampleTextureFragment glsl
{
  Source DownSampleTexture.frag
}

fragment_shader ComputeLumFragment glsl
{
  Source ComputeLumValue.frag
}

fragment_shader ComputeBloomFragment glsl
{
  Source ComputeBloomValue.frag
}

fragment_shader ToneMapFragment glsl
{
  Source ToneMap.frag
}

fragment_shader FilterImageCussiXFragment glsl
{
  Source FilterImageGussiX.frag
}

fragment_shader FilterImageCussiYFragment glsl
{
  Source FilterImageGussiY.frag
}

fragment_shader TemporalAntiAliasingFragment glsl
{
  Source TemporalAntiAliasing.frag
}

fragment_shader MotionBlurFragment glsl
{
  Source MotionBlur.frag
}

fragment_shader ScreenSpaceRayTrackFragment glsl
{
  Source ScreenSpaceRayTrack.frag
}

fragment_shader ScreenSpaceReflectionFragment glsl
{
  Source ScreenSpaceReflection.frag
}

fragment_shader TestForGeometryVertex glsl
{
  Source TestForGeometry.vert
}

fragment_shader TestForGeometryFragment glsl
{
  Source TestForGeometry.frag
}

geometry_shader TestForGeometryGeometry glsl
{
  Source TestForGeometry.geom
}

compute_shader TestForComputerShader glsl
{
  Source TestForComputer.comp
}

vertex_shader VoxelizationSceneVertex glsl
{
  Source VoxelizationScene.vert
}

fragment_shader VoxelizationSceneFragment glsl
{
  Source VoxelizationScene.frag
}

fragment_shader VoxelizationSceneWithoutTextureFragment glsl
{
  Source VoxelizationSceneWithoutTexture.frag
}


geometry_shader VoxelizationSceneGeometry glsl
{
  Source VoxelizationScene.geom
}

vertex_shader RenderVoxelVertex glsl
{
  Source RenderVoxel.vert
}

fragment_shader RenderVoxelFragment glsl
{
  Source RenderVoxel.frag
}

geometry_shader RenderVoxelGeometry glsl
{
  Source RenderVoxel.geom
}

vertex_shader SponezaBaseProgramVertex glsl
{
  Source SponezaBaseShading.vert
}

fragment_shader SponezaBaseProgramFragment glsl
{
  Source SponezaBaseShading.frag
}


fragment_shader ShadingSceneWithVoxelFragment glsl
{
  Source ShadingSceneWithVoxel.frag
}


fragment_shader  Cornell_r_DefferDataProcessFragment glsl
{
   Source  Cornell_r_DefferDataProcess.frag
}

fragment_shader  Cornell_l_DefferDataProcessFragment glsl
{
   Source  Cornell_l_DefferDataProcess.frag
}