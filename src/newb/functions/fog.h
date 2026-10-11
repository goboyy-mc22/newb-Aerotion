#ifndef FOG_H
#define FOG_H

float nlRenderFogFade(float relativeDist, vec3 FOG_COLOR, vec2 FOG_CONTROL) {
  #ifdef NL_FOG
    float fade = smoothstep(FOG_CONTROL.x, FOG_CONTROL.y, relativeDist);

    // misty effect
    float density = NL_MIST_DENSITY*(19.0 - 18.0*FOG_COLOR.g);
    fade += (1.0-fade)*(0.3-0.3*exp(-relativeDist*relativeDist*density));

    return NL_FOG * fade;
  #else
    return 0.0;
  #endif
}

float nlRenderGodRayIntensity(vec3 cPos, vec3 worldPos, float t, vec2 uv1, float relativeDist, vec3 FOG_COLOR) {
  // offset wPos (only works upto 16 blocks)
  vec3 offset = cPos - 16.0*fract(worldPos*0.0625);
  offset = abs(2.0*fract(offset*0.0625)-1.0);
  offset = offset*offset*(3.0-2.0*offset);
  //offset = 0.5 + 0.5*cos(offset*0.392699082);

  //vec3 ofPos = wPos+offset;
  vec3 nrmof = normalize(worldPos);

  float u = nrmof.z/length(nrmof.zy);
  float diff = dot(offset,vec3(0.1,0.2,1.0)) + 0.07*t;
  float mask = nrmof.x*nrmof.x;

  float vol = sin(7.0*u + 1.5*diff)*sin(3.0*u + diff);
  vol *= vol*mask*uv1.y*(1.0-mask*mask);
  vol *= relativeDist*relativeDist;

  // dawn/dusk mask
  vol *= clamp(3.0*(FOG_COLOR.r-FOG_COLOR.b), 0.0, 1.0);

  vol = smoothstep(0.0, 0.1, vol);
  return vol;
}

float nlRenderGodRayShafts(vec3 worldPos, vec3 sunDir, float t, float relativeDist, vec3 FOG_COLOR) {
  // parallel shafts along the sun's azimuth; in perspective they converge exactly at the sun
  vec2 sd = normalize(sunDir.xz);
  vec2 perp = vec2(-sd.y, sd.x);
  float horizLen = max(length(worldPos.xz), 0.001);
  float s = dot(worldPos.xz, perp);
  float toward = smoothstep(0.0, 0.5, dot(worldPos.xz, sd)/horizLen);

  float a = sin(NL_GODRAY_SPACING*s + 0.35*sin(0.7*NL_GODRAY_SPACING*s + 0.02*t));
  float b = sin(2.3*NL_GODRAY_SPACING*s + 1.7 + 0.03*t);
  float v = a*0.7 + b*0.3 + 0.15;

  // bright core + soft glow
  float core = smoothstep(NL_GODRAY_WIDTH, NL_GODRAY_WIDTH + 0.06, v);
  float glow = 0.35*smoothstep(NL_GODRAY_WIDTH - 0.30, NL_GODRAY_WIDTH, v);
  float ray = clamp(core + glow, 0.0, 1.0)*toward*smoothstep(0.02, 0.25, relativeDist);

  // dawn/dusk mask
  ray *= clamp(3.0*(FOG_COLOR.r - FOG_COLOR.b), 0.0, 1.0);
  return ray;
}

#endif
