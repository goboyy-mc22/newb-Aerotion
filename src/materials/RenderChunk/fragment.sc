$input v_color0, v_color1, v_fog, v_refl, v_texcoord0, v_lightmapUV, v_extra, v_position, v_wpos, v_cpos

#include <bgfx_shader.sh>
#include <newb/main.sh>

SAMPLER2D_AUTOREG(s_MatTexture);
SAMPLER2D_AUTOREG(s_SeasonsTexture);
SAMPLER2D_AUTOREG(s_LightMapTexture);
SAMPLER2D_AUTOREG(s_caustic);

uniform vec4 TimeOfDay;
uniform vec4 FogColor;
uniform vec4 FogAndDistanceControl;
uniform vec4 ViewPositionAndTime;
uniform vec4 CameraPosition;

// smooth puddle shapes and rain ripples (per pixel, world position wrapped to keep sin() precise)
float puddleHash(vec2 p) {
  return fract(37.45*sin(dot(p, vec2(4.36, 8.28))));
}

float puddleNoise(vec2 p) {
  vec2 i = floor(p);
  vec2 f = fract(p);
  f = f*f*(3.0 - 2.0*f);
  return mix(mix(puddleHash(i), puddleHash(i + vec2(1.0, 0.0)), f.x),
             mix(puddleHash(i + vec2(0.0, 1.0)), puddleHash(i + vec2(1.0, 1.0)), f.x), f.y);
}

void main() {
  #if defined(DEPTH_ONLY_OPAQUE) || defined(DEPTH_ONLY) || defined(INSTANCING)
    gl_FragColor = vec4(1.0,1.0,1.0,1.0);
    return;
  #endif

  vec4 diffuse = texture2D(s_MatTexture, v_texcoord0);
  vec4 color = v_color0;

  #ifdef ALPHA_TEST
    if (diffuse.a < 0.6) {
      discard;
    }
  #endif

  nl_environment env = nlDetectEnvironment(TimeOfDay.x,FogColor.rgb,FogAndDistanceControl.xyz);
  
  vec3 N;
     N = normalize(cross(dFdx(v_position), dFdy(v_position)));

  bool blockUnderWater = !env.end &&
    v_lightmapUV.y < 0.9 &&
    abs((2.0 * v_position.y - 15.0) / 16.0 - v_lightmapUV.y) < 0.00002;

  if(env.underwater || blockUnderWater){
      vec2 uv = v_position.xz * 0.15; 
      uv += vec2(ViewPositionAndTime.w * 0.02, ViewPositionAndTime.w * 0.02); 
      vec3 caustic = texture2D(s_caustic, uv).rgb;

      float ndotl = max(N.y, 0.0); 
      caustic *= ndotl * 0.5;

      vec3 watercol = vec3(0.0, 0.3, 0.5);
      vec3 finalColor = watercol + caustic;

      diffuse.rgb *= finalColor;
      diffuse.rgb *= 2.7;
  }

  #if defined(SEASONS) && (defined(OPAQUE) || defined(ALPHA_TEST))
    diffuse.rgb *= mix(vec3(1.0,1.0,1.0), texture2D(s_SeasonsTexture, v_color1.xy).rgb * 2.0, v_color1.z);
  #endif

  vec3 glow = nlGlow(s_MatTexture, v_texcoord0, v_extra.a);

  diffuse.rgb *= diffuse.rgb;

  #if defined(TRANSPARENT) && !(defined(SEASONS) || defined(RENDER_AS_BILLBOARDS))
    if (v_extra.b > 0.9) {
      diffuse.rgb = vec3_splat(1.0 - NL_WATER_TEX_OPACITY*(1.0 - diffuse.b*1.8));
      diffuse.a = color.a;
    }
  #else
    diffuse.a = 1.0;
  #endif

  diffuse.rgb *= color.rgb;

  float rainShadowMask = mix(1.0, 0.25, env.rainFactor);
  float nightShadowMask = mix(1.0, 0.25, step(env.dayFactor, 0.0)); // same night switch as the sky
  float shadow = smoothstep(0.875, 0.860, pow(v_lightmapUV.y, 2.0));
  shadow = mix(shadow, 0.0, pow(v_lightmapUV.x * 1.2, 6.0));
  diffuse.rgb *= 1.0 - 0.68 * shadow * rainShadowMask * nightShadowMask;

  if (v_extra.b <= 0.9) {
    float sideshadow = smoothstep(0.64, 0.62, v_color1.g);
    diffuse.rgb *= 1.0 - 0.62 * sideshadow * rainShadowMask * nightShadowMask;
  }

  diffuse.rgb += glow;

  //Water Foam Detection
  float wcPos = v_wpos.y + CameraPosition.y;
  float dy_w = abs(dFdy(v_wpos.y));

  bool isOneBlockUnderwater = ((wcPos <= 62.89 && wcPos > 62.0) || (wcPos <= -60.11 && wcPos > -61.0)) 
    && v_extra.b < 0.9 
    && !(dy_w < 0.0002) 
    && step(0.85, v_lightmapUV.y) * step(v_lightmapUV.y, 0.927) == 1.0; 

  if (isOneBlockUnderwater) {
    float d = 0.865;
    float bpy = fract(v_cpos.y);
    vec3 viewdir = normalize(-v_wpos);

    // Proyección de rayos 3D
    vec2 offset = (d - bpy) * viewdir.xz / viewdir.y;
    vec2 watpos = v_cpos.xz + offset;

    float fade = clamp((d - bpy) / abs(viewdir.y), 0.0, 1.0);
    fade = 1.0 - fade;

    float t = 0.3 * ViewPositionAndTime.w;
    vec2 st = watpos;
    float n = sin(st.x + 3.0*sin(0.5*st.y) + t) * sin(st.y + 3.0*sin(0.5*st.x) - t);
    n = 0.5 + 0.5 * sin(st.x + 2.0*n + 0.2*t) * sin(st.y - 4.0*n + 0.1*t);
    fade = pow(fade, 1.0 + 2.5 * n);

    // Textura procedural de espuma
    float foam = fract(238.084 * sin(dot(floor(16.0 * watpos), vec2(1.32, 141.3))));
    foam *= fade;
    foam = mix(foam, 1.0, fade * fade);

    diffuse.rgb = mix(diffuse.rgb, vec3(1.0, 1.0, 1.1)*3.0, foam);
  }

  if (v_extra.b > 0.9) {
    diffuse.rgb += v_refl.rgb*v_refl.a;
  } else if (v_refl.a > 0.0) {
    // reflective effect - only on xz plane
    float dy = abs(dFdy(v_extra.g));
    if (dy < 0.0002) {
      float mask = v_refl.a*(clamp(v_extra.r*10.0,8.2,8.8)-7.8);
      #ifdef NL_GROUND_RAIN_PUDDLES
        // smooth puddles: strong mirror + darker floor inside, faint sheen outside (0.0 = uniform wet ground)
        vec2 pp = mod(v_position.xz, 256.0);
        float pn = 0.65*puddleNoise(pp*0.6) + 0.35*puddleNoise(pp*1.5 + 17.0);
        float pud = smoothstep(0.42, 0.58, pn);
        mask *= mix(1.0, 0.4 + 3.0*pud, NL_GROUND_RAIN_PUDDLES);

        // rain ripples inside puddles: one expanding ring per cell
        vec2 rc = pp*2.2;
        vec2 rcell = floor(rc);
        vec2 rf = fract(rc) - 0.5 + (vec2(puddleHash(rcell + 3.0), puddleHash(rcell + 7.0)) - 0.5)*0.4;
        float rph = fract(0.6*ViewPositionAndTime.w + puddleHash(rcell));
        float ring = clamp(1.0 - abs(length(rf) - 0.5*rph)/0.07, 0.0, 1.0)*(1.0 - rph);
        mask += 0.2*ring*pud*env.rainFactor*NL_GROUND_RAIN_PUDDLES;

        diffuse.rgb *= 1.0 - 0.35*pud*env.rainFactor*NL_GROUND_RAIN_PUDDLES;
      #endif
      mask = min(mask, 1.0);
      diffuse.rgb *= 1.0 - 0.6*mask;
      diffuse.rgb += v_refl.rgb*mask;
    }
  }

  diffuse.rgb = mix(diffuse.rgb, v_fog.rgb, v_fog.a);

  diffuse.rgb = colorCorrection(diffuse.rgb);

  gl_FragColor = diffuse;
}
