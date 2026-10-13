$input v_texcoord0, v_isSun

#include <bgfx_shader.sh>

#ifndef INSTANCING
  #include <newb/config.h>
  #include <newb/functions/tonemap.h>

  uniform vec4 SunMoonColor;

  SAMPLER2D_AUTOREG(s_SunMoonTexture);
#endif

void main() {
  #ifndef INSTANCING
    vec4 color = texture2D(s_SunMoonTexture, v_texcoord0);

    // sun only: blur the hard pixel edge of the texture so it fades into the bloom
    // (outside the texture counts as transparent, the moon is left untouched)
    if (v_isSun > 0.5) {
      vec4 acc = vec4(0.0, 0.0, 0.0, 0.0);
      float wsum = 0.0;
      for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
          vec2 o = vec2(float(i), float(j));
          vec2 uv = v_texcoord0 + o*(0.5*NL_SUN_EDGE_RADIUS);
          float inside = step(0.0, min(uv.x, uv.y))*step(max(uv.x, uv.y), 1.0);
          float w = exp(-0.35*dot(o, o));
          vec4 s = texture2D(s_SunMoonTexture, clamp(uv, 0.0, 1.0));
          acc += vec4(s.rgb*s.a, s.a)*(inside*w);
          wsum += w;
        }
      }
      vec4 soft = vec4(acc.rgb/max(acc.a, 0.0001), acc.a/wsum);
      color = mix(color, soft, NL_SUN_EDGE_SOFT);
    }

    color.rgb *= SunMoonColor.rgb;
    color.rgb *= 4.4*color.rgb;
    float tr = 1.0 - SunMoonColor.a;
    color.a *= 1.0 - tr*tr;
    color.rgb = colorCorrection(color.rgb);
    gl_FragColor = color;
  #else
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
  #endif
}
