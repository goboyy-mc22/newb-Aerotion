#ifndef SKY_H
#define SKY_H

#include "detection.h"
#include "noise.h"

struct nl_skycolor {
  vec3 zenith;
  vec3 horizon;
  vec3 horizonEdge;
};

// rainbow spectrum
vec3 spectrum(float x) {
  vec3 s = vec3(x-0.5, x, x+0.5);
  s = smoothstep(1.0,0.0,abs(s));
  return s*s;
}

vec3 getUnderwaterCol(vec3 FOG_COLOR) {
  return 2.0*NL_UNDERWATER_TINT*FOG_COLOR*FOG_COLOR;
}

vec3 getEndZenithCol() {
  return NL_END_ZENITH_COL;
}

vec3 getEndHorizonCol() {
  return NL_END_HORIZON_COL;
}

nl_skycolor nlEndSkyColors(nl_environment env) {
  nl_skycolor s;
  s.zenith = getEndZenithCol();
  s.horizon = getEndHorizonCol();
  s.horizonEdge = s.horizon;
  return s;
}

nl_skycolor nlOverworldSkyColors(nl_environment env) {
  nl_skycolor s;
  float f = 1.0 + 2.0*(1.0-max(-env.dayFactor, 0.0));
  float nightFactor = step(env.dayFactor, 0.0);
  s.zenith = mix(NL_DAY_ZENITH_COL, NL_NIGHT_ZENITH_COL*f, nightFactor);
  s.horizon = mix(NL_DAY_HORIZON_COL, NL_NIGHT_HORIZON_COL*f, nightFactor);
  s.horizonEdge = mix(NL_DAY_EDGE_COL, NL_NIGHT_EDGE_COL*f, nightFactor);

  float dawnFactor = 1.0-env.dayFactor*env.dayFactor;
  dawnFactor *= dawnFactor*dawnFactor;
  dawnFactor *= mix(1.0, dawnFactor*dawnFactor, nightFactor);
  s.zenith = mix(s.zenith, NL_DAWN_ZENITH_COL, dawnFactor);
  s.horizon = mix(s.horizon, NL_DAWN_HORIZON_COL, dawnFactor);
  s.horizonEdge = mix(s.horizonEdge, NL_DAWN_EDGE_COL, dawnFactor);

  float zh = dot(s.zenith, vec3_splat(0.33));
  float hh = dot(s.horizon, vec3_splat(0.33));
  float rainMix = env.rainFactor*NL_SKY_RAIN_MIX_FACTOR;
  s.zenith = mix(s.zenith, NL_RAIN_ZENITH_COL*zh, rainMix);
  s.horizon = mix(s.horizon, NL_RAIN_HORIZON_COL*hh, rainMix);
  s.horizonEdge = mix(s.horizonEdge, s.horizon, env.rainFactor);

  if (env.underwater) {
    vec3 underwaterFog = env.fogCol*env.fogCol*NL_UNDERWATER_TINT;
    s.zenith = mix(2.0*underwaterFog, underwaterFog*zh, 0.8);
    s.horizon = mix(2.0*underwaterFog, underwaterFog*hh, 0.8);
    s.horizonEdge = s.horizon;
  }

  return s;
}

nl_skycolor nlSkyColors(nl_environment env) {
  if (env.end) {
    return nlEndSkyColors(env);
  }
  return nlOverworldSkyColors(env);
}

// square-ish white bloom around the sun/moon quad (vanilla texture).
// dir = sun/moon direction, hs = half size of the quad in tangent units (matches SunMoon/vertex.sc)
// dawnW = 0.0 at noon (bloom unchanged) ~ 1.0 at dawn/dusk (thinner bloom, fading out from the sun to its edge)
float sunMoonBloom(vec3 viewDir, vec3 dir, float hs, float dawnW) {
  float cd = dot(viewDir, dir);
  if (cd < 0.05) { return 0.0; }
  vec3 cr = cross(vec3(0.0, 1.0, 0.0), dir);
  vec3 r = length(cr) > 0.001 ? normalize(cr) : vec3(1.0, 0.0, 0.0);
  vec3 u = cross(dir, r);
  vec2 q = vec2(dot(viewDir, r), dot(viewDir, u))/(cd*hs);
  // p-norm distance: NL_BLOOM_SHAPE 2.0 = round, 6.0 = rounded square, higher = square
  // NL_BLOOM_RANGE stretches the bloom outward (1.0 = fitted to the reference)
  vec2 qa = pow(abs(q) + 1e-6, vec2_splat(NL_BLOOM_SHAPE));
  float d = pow(qa.x + qa.y, 1.0/NL_BLOOM_SHAPE)/NL_BLOOM_RANGE;
  float b = pow(max(d, 0.8), -1.67);
  // fade-out: noon keeps the original cut-off (2.7 ~ 5.45), dawn starts fading right next to the sun
  // and ends sooner (NL_DAWN_BLOOM_WIDTH), so the glow gets weaker the further it is from the sun
  float fadeStart = mix(2.7, 0.8, dawnW);
  float fadeEnd = mix(5.45, mix(1.8, 5.45, NL_DAWN_BLOOM_WIDTH), dawnW);
  return b*(1.0 - smoothstep(fadeStart, fadeEnd, d));
}

vec3 renderOverworldSky(nl_skycolor skyCol, nl_environment env, vec3 viewDir, bool isSkyPlane) {
  float avy = abs(viewDir.y);
  float mask = 0.5 + (0.5*viewDir.y/(0.4 + avy));

  vec2 g = clamp(0.5 - 0.5*vec2(dot(env.sunDir, viewDir), dot(env.moonDir, viewDir)), 0.0, 1.0);
  vec2 g1 = 1.0-mix(sqrt(g), g, env.rainFactor);
  vec2 g2 = g1*g1;
  vec2 g4 = g2*g2;
  vec2 g8 = g4*g4;
  float mg8 = (g8.x+g8.y)*mask*(1.0-0.9*env.rainFactor);
  #ifdef NL_SUN_BLOOM
    // sky plane: the sun uses the square bloom below, only the moon keeps the round glow
    if (isSkyPlane) { mg8 = g8.y*mask*(1.0-0.9*env.rainFactor); }
  #endif

  float vh = 1.0 - viewDir.y*viewDir.y;
  float vh2 = vh*vh;
  vh2 = mix(vh2, mix(1.0, vh2*vh2, NL_SKY_VOID_FACTOR), step(viewDir.y, 0.0));
  vh2 = mix(vh2, 1.0, mg8);
  float vh4 = vh2*vh2;

  float rainMask = 1.0 - 0.6*env.rainFactor;
  float gradient1 = vh4*vh4;
  float gradient2 = 0.8*gradient1 + 0.2*vh2;
  gradient1 *= gradient1;
  gradient1 = mix(gradient1*gradient1, 1.0, mg8)*rainMask;
  gradient2 = mix(gradient2, 1.0, mg8)*rainMask;

  float dawnFactor = 1.0-env.dayFactor*env.dayFactor;
  
  // dawn spread: same weight as the dawn color blend in nlOverworldSkyColors
  float dw = dawnFactor*dawnFactor*dawnFactor;
  dw *= mix(1.0, dw*dw, step(env.dayFactor, 0.0));
  float sw = clamp(NL_DAWN_SPREAD*dw, 0.0, 1.0);
  float ew = clamp(NL_DAWN_EDGE_SPREAD*dw, 0.0, 1.0);

  // angular falloff around the sun: exponent 2.0 = original, lower = wider
  float sunFall = mix(g2.x, pow(g1.x, NL_DAWN_WIDTH), sw);
  float df = mix(1.0, sunFall, dawnFactor*dawnFactor);

  // height falloff: exponent on (1 - y^2), 2.0 = tight, lower = reaches higher
  float dawnGradient1 = mix(gradient1, pow(vh, NL_DAWN_HEIGHT), sw);
  float dawnGradient2 = mix(gradient2, vh, ew);

  vec3 sky = mix(skyCol.horizon, skyCol.horizonEdge, dawnGradient1*df*df);
  float skyT = dawnGradient2*df; // 0.0 = zenith ~ 1.0 = horizon/sun side
  vec3 skyPlain = mix(skyCol.zenith, sky, skyT);

  #ifdef NL_DAWN_LAVENDER
    // dawn only (dw is 0 at noon, so noon is unchanged): zenith -> pink lavender -> golden horizon,
    // the zenith color itself stays the same
    vec3 skyLav = mix(skyCol.zenith, NL_DAWN_LAVENDER_COL, smoothstep(0.0, NL_DAWN_LAVENDER_POS, skyT));
    skyLav = mix(skyLav, sky, smoothstep(NL_DAWN_LAVENDER_POS, 1.0, skyT));
    sky = mix(skyPlain, skyLav, clamp(NL_DAWN_LAVENDER*dw, 0.0, 1.0)*(1.0-env.rainFactor));
  #else
    sky = skyPlain;
  #endif

  sky *= 0.7+0.7*gradient2;
  sky *= (1.0 + (2.0*mg8 + 7.0*mg8*mg8)*mask)*mix(1.0, mask, NL_SKY_VOID_DARKNESS);

  if (!isSkyPlane) {
    float source = max(0.0, (mg8-0.22)/0.78);
    source *= source;
    source *= source;
    sky *= 1.0 + 15.0*source*(1.0-env.rainFactor);
  }

  #ifdef NL_SUN_BLOOM
    if (isSkyPlane) {
      float bloom = NL_SUN_BLOOM*sunMoonBloom(viewDir, env.sunDir, NL_SUN_SIZE*NL_SUNMOON_UNIT, dawnFactor*dawnFactor);
      // noon is the same as always, at dawn/dusk the color follows the sky hue and the bloom is thinner
      vec3 hc = max(skyCol.horizon, vec3_splat(0.001));
      vec3 hue = min(hc/min(hc.r, min(hc.g, hc.b)), vec3_splat(2.5)); // every channel >= 1, so the core stays as bright as white
      vec3 bloomCol = mix(vec3_splat(1.0), hue, clamp(dawnFactor*dawnFactor*NL_SUN_BLOOM_TINT, 0.0, 1.0));
      sky += bloomCol*bloom*mask*(1.0-0.9*env.rainFactor);
    }
  #endif

  #ifdef NL_RAINBOW
    if (!env.underwater) {
      float rainbowFade = 0.5 + 0.5*viewDir.y;
      rainbowFade *= rainbowFade;
      rainbowFade *= mix(NL_RAINBOW_CLEAR, NL_RAINBOW_RAIN, env.rainFactor);
      rainbowFade *= 0.5+0.5*env.dayFactor;
      sky += spectrum(24.2*(0.85-g.x))*rainbowFade*skyCol.horizon;
    }
  #endif

  return sky;
}

vec3 renderEndSky(vec3 horizonCol,vec3 zenithCol,vec3 viewDir,float t) {
  vec3 dir = normalize(viewDir);
  float grad = 0.5+0.5*dir.y;
  vec3 sky = mix(zenithCol,horizonCol,pow(1.0-grad,1.35));

  vec3 starCell = floor(dir*185.0);
  vec3 rnd = hash33(starCell);

  float starChance = step(0.980,rnd.x);
  float starDist = length(fract(dir*185.0)-0.5);
  float starRadius = mix(0.105,0.18,rnd.y);
  float starPoint = 1.0-smoothstep(starRadius,starRadius*1.45,starDist);

  vec3 starColor = vec3(1.0,0.93,0.82);

  if (rnd.x > 0.978) {
      starColor = vec3(0.55,0.72,1.0);
  }
  if (rnd.x > 0.988) {
      starColor = vec3(0.72,0.55,1.0);
  }
  if (rnd.x > 0.995) {
      starColor = vec3(0.55,0.9,1.0);
  }
    
  float star = starPoint*starChance;
  sky += starColor*star*1.45;

  return sky;
}

vec3 nlRenderEndFog(nl_skycolor skycol, vec3 viewDir) {
  viewDir.y = -viewDir.y;

  vec3 dir = normalize(viewDir);
  float grad = 0.5 + 0.5*dir.y;

  return mix(skycol.zenith, skycol.horizon, pow(1.0 - grad, 1.35));
}

vec3 nlRenderSky(nl_skycolor skycol, nl_environment env, vec3 viewDir, float t, bool isSkyPlane) {
  vec3 sky;
  viewDir.y = -viewDir.y;

  if (env.end) {
    sky = renderEndSky(skycol.horizon, skycol.zenith, viewDir, t);
  } else {
    sky = renderOverworldSky(skycol, env, viewDir, isSkyPlane);
    #ifdef NL_UNDERWATER_STREAKS
       if (env.underwater) {
         float a = atan2(viewDir.x, viewDir.z);
         float grad = 0.5 + 0.5*viewDir.y;
         grad *= grad;
         float spread = (0.5 + 0.5*sin(3.0*a + 0.2*t + 2.0*sin(5.0*a - 0.4*t)));
         spread *= (0.5 + 0.5*sin(3.0*a - sin(0.5*t)))*grad;
         spread += (1.0-spread)*grad;
         float streaks = spread*spread;
         streaks *= streaks;
         streaks = (spread + 3.0*grad*grad + 4.0*streaks*streaks);
         sky += 2.0*streaks*skycol.horizon;
       }
    #endif
  }

  return sky;
}

vec4 renderBlackhole(vec3 viewdir,float t) {
    t *= NL_BH_SPEED;
    
    float r = 2.4;
    
    vec3 vr = viewdir;
    vr.xy = vec2(vr.x * cos(r) - vr.y * sin(r),vr.x * sin(r) + vr.y * cos(r));
    vec3 viewd = vr - vec3(0.0,-1.0,0.0);
    
    float nl = sin(15.0 * viewd.x + t) * sin(15.0 * viewd.y - t) * sin(15.0 * viewd.z + t);
    float a = atan2(viewd.x,viewd.z);
    float d = NL_BH_DIST * length(viewd + 0.003 * nl);
    float d0 = (0.6 - d) / 0.6;
    float dm0 = 1.0 - max(d0,0.0);
    float gl = 1.0 - clamp(-0.3 * d0,0.0,1.0);
    float gla = pow(1.0 - min(abs(d0),1.0),8.0);
    float gl8 = pow(gl,8.0);
    float hole = 0.9 * pow(dm0,32.0) + 0.1 * pow(dm0,3.0);
    float bh = (gla + 0.8 * gl8 + 0.2 * gl8 * gl8) * hole;
    float spiralPhase = 3.0 * a - 4.0 * d + 24.0 * pow(max(1.4 - d,0.0),4.0) + t;
    float df = sin(spiralPhase);
    
    df *= 1.2 + 0.05 * sin(4.0 * a + d + 4.0 * t - 4.0 * df);
    
    float df2 = sin(7.0 * a - 8.0 * d + 16.0 * pow(max(1.25 - d,0.0),3.0) - t * 1.35);
 
    df += 0.08 * df2;
    bh *= 1.0 + pow(df,4.0) * hole * max(1.0 - bh,0.0);
    
    float spiralWave = 0.4 + 0.35 * sin(spiralPhase);
    float spiralShape = pow(spiralWave,2.0);
    float innerSpiral = smoothstep(0.18,0.62,d) * spiralShape;
    float innerBreakup = 0.72 + 0.28 * spiralShape;
    float detail = 0.8 + 0.04 * sin(12.0 * a - 15.0 * d + t * 1.5);
    
    bh *= innerBreakup * detail;
    
    float edge = pow(max(1.0 - smoothstep(0.34,0.62,d),0.0),3.0);
    float halo = pow(max(1.0 - smoothstep(0.45,1.15,d),0.0),4.0);
    float spiralLight = 0.72 + 0.28 * spiralShape;
    float light = bh * 4.0;
    
    light += edge * bh * 0.20;
    light += innerSpiral * bh * 0.16;
    light += halo * bh * 0.10;
    light *= spiralLight;
    
    float colorMix = clamp(bh * (0.72 + 0.28 * spiralShape),0.0,1.0);
    
    vec3 col = light * mix(NL_BH_COL_LOW,NL_BH_COL_HIGH,colorMix);
    
    return vec4(col,hole);
}

vec3 endstreaks(vec3 horizonCol,vec3 zenithCol,vec3 viewDir,float t) {
  float skyTime = t*0.2;

  float a = atan2(viewDir.x,viewDir.z);
  float y = viewDir.y;

  float n1 = 1.0+1.0*sin(14.0*a+skyTime+0.0*viewDir.x*y);
  float n2 = 1.0+0.5*sin(8.0*a+0.0*skyTime+0.0*n1+0.5*sin(50.0*a-4.0*skyTime));
  float n3 = 1.0+1.0*sin(10.0*a-0.5*skyTime+5.0*n2+10.0*viewDir.z*y);

  float waves = 0.1*n2*n1+0.5*n1+0.1*n3;
  waves = smoothstep(-1.0,1.0,waves);

  float grad = 0.7+0.3*y;

  float streaks = waves*(1.0-grad*grad*grad);
  streaks += (1.0-streaks)*smoothstep(1.0-waves,-1.0,y);

  float f = 0.3*streaks+0.3*smoothstep(1.0,0.0,y);
  float h = streaks*streaks*streaks;
  float g = h*h;
  g *= g;

  vec3 sky = mix(zenithCol,horizonCol,f*f);

  float body = streaks*(1.0-2.0*streaks);
  sky += (0.2*body+0.0*g*g+h*h)*vec3(1.0,0.2,1.0);

  float bloom = smoothstep(0.0,1.0,streaks);
  bloom = pow(bloom,2.0);

  sky += vec3(1.0,0.2,1.0)*bloom*0.5;

  float light = smoothstep(0.0,1.0,streaks);
  //sky += vec3(0.5,0.5,0.5)*light*0.2;

  sky += 0.1*body*spectrum(sin(1.0*viewDir.x*viewDir.y+skyTime));

  return sky;
}

#endif
