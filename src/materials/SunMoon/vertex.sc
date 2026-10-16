$input a_position, a_texcoord0
$output v_texcoord0, v_sunVis

#include <bgfx_shader.sh>

#ifndef INSTANCING
  #include <newb/config.h>
  #include <newb/functions/utils.h>
  #include <newb/functions/detection.h>

  uniform vec4 TimeOfDay;
  uniform vec4 FogColor;
  uniform vec4 FogAndDistanceControl;

  // world position of a point (p = offset in the quad plane) on the sun/moon path
  vec3 sunMoonPos(vec2 p, float dist, float st, float tilt, float yaw) {
    vec3 w = vec3(dist, 70.0*p.x, 70.0*p.y);
    w.xy = mul(rmat2(st), -w.xy);
    w.yz = mul(rmat2(tilt), w.yz);
    w.xz = mul(rmat2(yaw), w.xz);
    return w;
  }
#endif

void main() {
  v_texcoord0 = a_texcoord0;
  v_sunVis = 1.0; // 1.0 = fully visible, only the sun is ever hidden below
  #ifndef INSTANCING
    vec3 pos = a_position;
    pos.x = -pos.x;

    vec2 dir = mul(u_model[0], vec4(0.0, 0.0, 0.0, 1.0)).xy;
    float st = 2.0*PI*TimeOfDay.x + 0.5*PI;
    bool isSun = dot(vec2(cos(st), sin(st)), dir) > 0.0;

    float dist = 300.0;
    float angle = 0.0;
    float tilt = 0.0;
    float yaw = 0.0;
    if (isSun) {
      dist = -dist;
      pos.x = -pos.x;
      pos.xz *= NL_SUN_SIZE;
      angle = degToRad(NL_SUN_TILT);
      tilt = degToRad( NL_SUN_PATH_TILT);
      yaw = degToRad(NL_SUN_PATH_YAW);
    } else {
      pos.xz *= NL_MOON_SIZE;
      angle = degToRad(NL_MOON_TILT);
      tilt = degToRad( NL_MOON_PATH_TILT);
      yaw = degToRad(NL_MOON_PATH_YAW);
    }
    // NL_SUN_TILT / NL_MOON_TILT: 0.0 = upright
    pos.xz = mul(rmat2(angle), pos.xz);

    // keep the quad upright (edges parallel to the horizon / vertical) at any time of day
    // and any path tilt/yaw, while keeping the original mirroring
    vec3 c  = sunMoonPos(vec2(0.0, 0.0), dist, st, tilt, yaw);
    vec3 E1 = sunMoonPos(vec2(1.0, 0.0), dist, st, tilt, yaw) - c;
    vec3 E2 = sunMoonPos(vec2(0.0, 1.0), dist, st, tilt, yaw) - c;
    vec3 d  = normalize(c);

    // the sun is only visible when it is really there: hidden when it is below the horizon (void),
    // underwater, and in rain. Clear day and dawn are unchanged (factor 1.0), the moon is untouched.
    if (isSun) {
      float sunVis = smoothstep(-0.10, 0.0, d.y);
      sunVis *= 1.0 - smoothstep(0.0, 0.6, detectRainChecked(FogAndDistanceControl.xyz, FogColor.rgb));
      sunVis *= detectUnderwater(FogColor.rgb, FogAndDistanceControl.xy) ? 0.0 : 1.0;
      v_sunVis = sunVis;
    }

    vec3 cr = cross(vec3(0.0, 1.0, 0.0), d);
    vec3 r  = length(cr) > 0.001 ? normalize(cr) : vec3(1.0, 0.0, 0.0); // screen right
    vec3 u  = cross(d, r);                                              // screen up
    float h = dot(cross(E1, E2), d) >= 0.0 ? 1.0 : -1.0;
    vec4 wpos = vec4(c + 70.0*(pos.x*r + pos.z*h*u), 1.0);

    gl_Position = mul(u_viewProj, wpos);
  #else
    gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
  #endif
}
