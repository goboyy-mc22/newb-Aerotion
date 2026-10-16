#ifndef INSTANCING
  $input v_worldPos, v_underwaterRainTimeDay
#endif

#include <bgfx_shader.sh>

#ifndef INSTANCING
  #include <newb/main.sh>
  uniform vec4 TimeOfDay;
  uniform vec4 FogColor;
  uniform vec4 FogAndDistanceControl;
#endif

void main() {
  #ifndef INSTANCING
    vec3 viewDir = normalize(v_worldPos);

    nl_environment env;
    env.end = false;
    env.nether = false;
    env.underwater = v_underwaterRainTimeDay.x > 0.5;
    env.rainFactor = v_underwaterRainTimeDay.y;
    env.dayFactor = v_underwaterRainTimeDay.w;
    env.fogCol = FogColor.rgb;
    env = calculateSunParams(env, TimeOfDay.x);

    nl_skycolor skycol = nlOverworldSkyColors(env);

    vec3 skyColor = nlRenderSky(skycol, env, -viewDir, v_underwaterRainTimeDay.z, true);

    skyColor = colorCorrection(skyColor);

    #ifdef NL_DEBUG_DAWN
      // DIAGNOSTIC (do not merge): the whole sky becomes one flat color. Watch which channel JUMPS at sunrise:
      //   R = raw rain detection (from the fog distance control)       -> should stay 0 in clear weather
      //   G = vanilla fog color  clamp(3*(r - b))                      -> vanilla input
      //   B = 0.5 + 0.5*dayFactor (sun height, always smooth)           -> reference, never jumps
      skyColor = vec3(detectRain(FogAndDistanceControl.xyz), clamp(3.0*(FogColor.r - FogColor.b), 0.0, 1.0), 0.5 + 0.5*env.dayFactor);
    #endif

    gl_FragColor = vec4(skyColor, 1.0);
  #else
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
  #endif
}
