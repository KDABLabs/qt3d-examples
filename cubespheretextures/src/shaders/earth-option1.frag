#version 430 core

#pragma include light.inc.frag

in FragData {
  vec3 color;
  vec3 triPosition;
  vec2 texCoords;
  vec4 patchDistance;
  vec3 eyePosition;
  vec3 normal;
  vec3 worldPosition;
  flat int idxIntoSatelliteTextureInfo;
} fragIn;

uniform sampler2DArray satelliteImagery;
uniform bool wireframeEnabled;
uniform bool showImagery;

struct TextureInfo
{
  vec4 layerScaleOffsets[4];
  vec4 extentMinMax[4];
};
// sizeof(TextureInfo) is 128 bytes with std140

// UBO minimum max size should be 16384 bytes, we've assumed 65536
layout(std140) uniform ImageryMappingBlock
{
    TextureInfo info[512]; // At most we can have 65536 / 128 == 512 (16384 / 32 == 128 if we were to strictly follow the GL specs) different textureInfo
} satelliteUniformBuffer;


out vec4 fragColor;


const vec3 lightPosition = vec3(1.0, 1.0, 1.0) * 1000000.0;
const float shininess = 15000.0;

float amplify(float d, float scale, float offset)
{
  d = scale * d + offset;
  d = clamp(d, 0, 1);
  d = 1 - exp2(-2*d*d);
  return d;
}

vec3 adsModel(const in vec3 pos, const in vec3 n, const in vec3 diffuseColor)
{
    // Calculate the vector from the fragment to the light
    vec3 s = normalize(lightPosition - pos);

    // Calculate the vector from the fragment to the eye position
    // (origin since this is in "eye" or "camera" space)
    vec3 v = normalize(-pos);

    // Reflect the light beam using the normal at this fragment
    vec3 r = reflect(-s, n);

    // Calculate the diffuse component
    float diffuse = max(dot(s, n), 0.0);

    // Calculate the specular component
    float specular = 0.0;
    if (dot(s, n) > 0.0)
        specular = pow(max(dot(r, v), 0.0), shininess) * (shininess + 2.0) / 2.0;

    // Combine the ambient, diffuse and specular contributions
    vec3 specularColor = vec3(1.0);
    vec3 ambiantColor = vec3(0.1);
    return ambiantColor + diffuseColor * diffuse + specularColor * specular;
}

void main()
{
  if (showImagery) {
      fragColor = vec4(0.0);
      TextureInfo satelliteInfo = satelliteUniformBuffer.info[fragIn.idxIntoSatelliteTextureInfo];

        for (int i = 0; i < 4; ++i) {
            // Satellite
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

              fragColor += vec4(color.rgb, color.a * alpha);
            }
          // Add some lighting
//          fragColor = vec4(adsModel(fragIn.eyePosition, normalize(fragIn.normal), fragColor.rgb), 1.0);
      }
  }
  else {
      fragColor = vec4(fragIn.color, 1.0);
  }

  if (wireframeEnabled) {
    float d1 = min(min(fragIn.triPosition.x, fragIn.triPosition.y), fragIn.triPosition.z);
    float d2 = min(min(min(fragIn.patchDistance.x, fragIn.patchDistance.y), fragIn.patchDistance.z), fragIn.patchDistance.w);
    d1 = 1 - amplify(d1, 50, -0.5);
    d2 = amplify(d2, 50, -0.5);
    fragColor = vec4(d2 * fragColor.xyz + d1 * d2 * -fragColor.xzy, 1.0);
  }

}
