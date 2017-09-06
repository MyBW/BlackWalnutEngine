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
