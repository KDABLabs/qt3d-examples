#version 430

out vec4 satelliteColor;
out vec4 elevationColor;

in FragData {
  flat int indexIntoUBO;
  vec2 texCoords;
} fragIn;

uniform sampler2DArray satelliteImagery;
uniform sampler2DArray elevation;

struct TextureInfo
{
  vec4 layerScaleOffsets[4];
  vec4 extentMinMax[4];
};
// sizeof(TextureInfo) is 128 bytes with std140

// UBO minimum max size should be 16384 bytes, we've assumed 65536
layout(std140) uniform ElevationMappingBlock
{
  TextureInfo info[512]; // At most we can have 65536 / 128 == 512 (16384 / 128) == 128 if we were to strictly follow the GL specs) different textureInfo
} elevationUniformBuffer;

// UBO minimum max size should be 16384 bytes, we've assumed 65536
layout(std140) uniform ImageryMappingBlock
{
  TextureInfo info[512]; // At most we can have 65536 / 128 == 512 (16384 / 128) == 128 if we were to strictly follow the GL specs) different textureInfo
} satelliteUniformBuffer;

void main()
{
  TextureInfo satelliteInfo = satelliteUniformBuffer.info[fragIn.indexIntoUBO];
  TextureInfo elevationInfo = elevationUniformBuffer.info[fragIn.indexIntoUBO];

  satelliteColor = vec4(0.0);
  elevationColor = vec4(0.0);

  for (int i = 0; i < 4; ++i) {
    // Satellite
    {
      float layer = satelliteInfo.layerScaleOffsets[i].x;
      if (layer >= 0.0) {
        float scale = satelliteInfo.layerScaleOffsets[i].y;
        vec2 offset = satelliteInfo.layerScaleOffsets[i].zw;
        vec3 texCoords = vec3(fragIn.texCoords * scale + offset, layer);
        vec4 color = texture(satelliteImagery, texCoords);

        // We need alpha to be 0 when the texture coordinates are beyond
        // or below the extent of the image
        // step -> 0.0 is returned if x[i] < edge[i], and 1.0 is returned otherwise
        vec2 stepMin = step(satelliteInfo.extentMinMax[i].xy, texCoords.xy);
        vec2 stepMax = step(vec2(0, 0), texCoords.xy - satelliteInfo.extentMinMax[i].zw);
        float alpha = stepMin.x * stepMin.y * stepMax.x * stepMax.y;

        satelliteColor += vec4(color.rgb, color.a * alpha);
      }
    }
    // Elevation
    {
      float layer = elevationInfo.layerScaleOffsets[i].x;
      if (layer >= 0.0) {
        float scale = elevationInfo.layerScaleOffsets[i].y;
        vec2 offset = elevationInfo.layerScaleOffsets[i].zw;
        vec3 texCoords = vec3(fragIn.texCoords * scale + offset, layer);
        vec4 color = texture(elevation, texCoords);

        // We need alpha to be 0 when the texture coordinates are beyond
        // or below the extent of the image
        // step -> 0.0 is returned if x[i] < edge[i], and 1.0 is returned otherwise
        vec2 stepMin = step(elevationInfo.extentMinMax[i].xy, texCoords.xy);
        vec2 stepMax = step(vec2(0, 0), texCoords.xy - elevationInfo.extentMinMax[i].zw);
        float alpha = stepMin.x * stepMin.y * stepMax.x * stepMax.y;

        elevationColor += vec4(color.rgb, color.a * alpha);
      }
    }
  }
}
