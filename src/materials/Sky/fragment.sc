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
      // DIAGNOSTIC (do not merge): the whole sky becomes one flat color, watch how it changes around sunrise
      //   R = OLD godray mask, from the vanilla fog color:   clamp(3*(FogColor.r - FogColor.b), 0, 1)
      //   G = NEW godray mask, from the sun height (smooth)
      //   B = vanilla fog brightness
      // If R (or B) jumps within a second while G stays smooth, the vanilla fog color is the switch.
      float dbgDd = 1.0 - env.dayFactor*env.dayFactor;
      dbgDd *= dbgDd*dbgDd;
      float dbgFade = smoothstep(-0.15, 0.12, env.dayFactor);
      float dbgNew = dbgDd*dbgDd*dbgFade*dbgFade*(3.0 - 2.0*dbgFade);
      float dbgOld = clamp(3.0*(FogColor.r - FogColor.b), 0.0, 1.0);
      skyColor = vec3(dbgOld, dbgNew, dot(FogColor.rgb, vec3_splat(0.33)));
    #endif

    gl_FragColor = vec4(skyColor, 1.0);
  #else
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
  #endif
}
