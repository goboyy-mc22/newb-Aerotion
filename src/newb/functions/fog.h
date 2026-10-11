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

// Soft, wide, dark shafts radiating from the sun's position (straight lines through the sun on screen).
// Returns shaft strength 0..1. viewDir must be normalized.
float nlRenderGodRayShafts(vec3 viewDir, vec3 sunDir, float t, vec3 FOG_COLOR) {
  float sd = dot(viewDir, sunDir);
  float toward = smoothstep(NL_GODRAY_SPREAD, NL_GODRAY_SPREAD + 0.5, sd);

  // angle around the sun axis: constant angle = straight line through the sun on screen
  vec3 rt = normalize(cross(vec3(0.0, 1.0, 0.0), sunDir));
  vec3 tp = cross(sunDir, rt);
  vec3 q = viewDir - sunDir*sd;
  float a = atan2(dot(q, tp), dot(q, rt));
  float downward = smoothstep(0.15, 0.65, -sin(a)); // keep the fan pointing down, like shadows cast on the land

  float p = NL_GODRAY_SPACING*a;
  float v = 0.75*cos(p + 0.01*t) + 0.25*cos(0.37*p + 1.3 - 0.007*t);
  float shaft = smoothstep(NL_GODRAY_EDGE, NL_GODRAY_EDGE + 0.5, v);

  float nearSun = smoothstep(0.02, 0.2, length(q)); // melt into the glow at the sun itself
  float belowSun = smoothstep(0.0, 0.08, sunDir.y - viewDir.y); // shafts fall from the sun downward
  float dawn = clamp(3.0*(FOG_COLOR.r - FOG_COLOR.b), 0.0, 1.0);
  return shaft*toward*nearSun*belowSun*downward*dawn;
}

// multiplier applied to the final color inside a shaft: darkens and saturates the fog/dawn color
vec3 nlGodRayTint(vec3 fogColor) {
  vec3 n = fogColor/max(max(max(fogColor.r, fogColor.g), fogColor.b), 0.001);
  return NL_GODRAY_DARK*mix(vec3(1.0, 1.0, 1.0), n, 0.75);
}

#endif
