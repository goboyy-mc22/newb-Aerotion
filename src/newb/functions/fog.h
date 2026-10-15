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

float nlRenderGodRayIntensity(vec3 cPos, vec3 worldPos, float t, vec2 uv1, float relativeDist, float dayFactor) {
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
  vol *= relativeDist; // was relativeDist^2: shafts also show closer to the camera

  vol = smoothstep(0.0, 0.06, vol); // soft shaft edges (the original was 0.1, 0.03 was too hard)

  // dawn/dusk mask from the sun height: fades in/out smoothly with time. (The vanilla fog color it replaces changes
  // abruptly around sunrise and, after the hard smoothstep above, made the godrays pop in like a switch)
  float dd = 1.0 - dayFactor*dayFactor;
  dd *= dd*dd;
  float fadeIn = smoothstep(-0.15, 0.12, dayFactor); // about 1 minute from first glimmer to full strength
  float dawnMask = dd*dd*fadeIn*fadeIn*(3.0 - 2.0*fadeIn); // eased twice: starts very gently

  return vol*dawnMask;
}

#endif
