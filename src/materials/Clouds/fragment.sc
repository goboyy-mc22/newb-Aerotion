$input v_color0
#include <newb/config.h>
#if NL_CLOUD_TYPE >= 2
  $input v_color1, v_color2, v_dayFactor
#endif

#include <bgfx_shader.sh>
#include <newb/main.sh>

uniform vec4 CameraPosition;

#define NL_CLOUD_PARAMS(x) NL_CLOUD2##x##STEPS, NL_CLOUD2##x##THICKNESS, NL_CLOUD2##x##RAIN_THICKNESS, NL_CLOUD2##x##VELOCITY, NL_CLOUD2##x##SCALE, NL_CLOUD2##x##DENSITY, NL_CLOUD2##x##SHAPE

void main() {
  vec4 color = v_color0;

  #if NL_CLOUD_TYPE >= 2
    vec3 vDir = normalize(v_color0.xyz);
    vec3 cloudPos = v_color0.xyz;
    cloudPos.xz += CameraPosition.xz;

    #if NL_CLOUD_TYPE == 2
      color = renderCloudsRounded(vDir, cloudPos, v_color1.w, v_color2.w, v_color2.rgb, v_color1.rgb, NL_CLOUD_PARAMS(_));

      #ifdef NL_CLOUD2_LAYER2
        vec2 parallax = vDir.xz / abs(vDir.y) * NL_CLOUD2_LAYER2_OFFSET;
        vec3 offsetPos = cloudPos;
        offsetPos.xz += parallax;
        vec4 color2 = renderCloudsRounded(vDir, offsetPos, v_color1.a, v_color2.a*2.0, v_color2.rgb, v_color1.rgb, NL_CLOUD_PARAMS(_LAYER2_));
        color = mix(color2, color, 0.2 + 0.8*color.a);
      #endif

      #ifdef NL_AURORA
        color += renderAurora(cloudPos, v_color2.a, v_color1.a, v_dayFactor)*(1.0-0.95*color.a);
      #endif

      color.a *= v_color0.a;
    #elif NL_CLOUD_TYPE == 3
      vDir.xz *= 0.3 + v_color0.w; // height parallax

      vec2 p = (vDir.xz)/(0.015 + 0.035*abs(vDir.y));
      p += 0.035*CameraPosition.xz;

      vec4 clouds = renderClouds(p, v_color2.w, v_color1.w, v_color2.rgb, v_color1.rgb, NL_CLOUD3_SCALE, NL_CLOUD3_SPEED, NL_CLOUD3_SHADOW);
      color = clouds;

      #ifdef NL_AURORA
        p.xy *= 34.7;
        color += renderAurora(p.xyy, v_color2.w, v_color1.w, v_dayFactor)*(1.0-0.95*color.a);
      #endif

      color.a *= smoothstep(0.0, 0.7, vDir.y);
    #elif NL_CLOUD_TYPE == 4
      // coloring same as type 0 (zenith + horizon)
      vec3 baseCol = v_color1.rgb + v_color2.rgb;
      vec3 sideCol = baseCol + dot(baseCol, vec3(0.3,0.4,0.3)); // type 0 top (y=1)
      vec3 bottomCol = baseCol;                                 // type 0 bottom (y=0)
      sideCol *= 1.0 - 0.8*v_color1.w;
      bottomCol *= 1.0 - 0.8*v_color1.w;

      vec3 p = vDir;                // view direction
      p.y = max(p.y, 0.001);
      // fixed position: where the view ray hits the cloud layer in the world
      // (v_color0.w = cloud layer height relative to the camera, camera xz = world position)
      float cloudH = v_color0.w;
      vec2 cp = (CameraPosition.xz + p.xz/p.y*cloudH)/NL_CLOUD4_CELL;
      float b = smoothstep(0.2, 0.9, p.y)*step(0.0, cloudH); // fade (no clouds when above the layer)

      vec4 vanilla = fastvanillaClouds(cp, vec3_splat(0.0), b, v_color2.w, sideCol, bottomCol);
      color.rgb = vanilla.rgb/max(vanilla.a, 0.0001); // back to straight cloud color
      color.a = vanilla.a*NL_CLOUD4_VISIBLE;
    #endif

    color.rgb = colorCorrection(color.rgb);
  #endif

  gl_FragColor = color;
}
