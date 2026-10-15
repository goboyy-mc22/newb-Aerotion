#ifndef NL_CONFIG_H
#define NL_CONFIG_H

/* Color correction */
#define NL_TONEMAP_TYPE 3              // 1:Exponential, 2:Reinhard, 3:Extended Reinhard, 4:ACES
#define NL_GAMMA 1.30                  // 0.3 low ~ 2.0 high
#define NL_EXPOSURE 1.10               // [toggle] 0.5 dark ~ 3.0 bright
#define NL_SATURATION 1.20             // [toggle] 0.0 grayscale ~ 4.0 super saturated
//#define NL_TINT                      // [toggle] enable light/dark tone tinting
#define NL_TINT_LOW  vec3(1.0,1.0,1.0) // color tint for dark tone
#define NL_TINT_HIGH vec3(1.0,1.0,1.0) // color tint for light tone

/* Lighting */
#define NL_SUNLIGHT_INTENSITY   3.9  // 1.0 weak ~ 5.0 bright
#define NL_TORCHLIGHT_INTENSITY 1.5 // 0.5 weak ~ 3.0 bright
#define NL_SHADOW_INTENSITY     1.0  // 0.0 no shadow ~ 1.0 strong shadow
#define NL_MIN_LIGHTING_BOOST   0.1  // 1.0 minimal lighting boost for dark areas ~ 3.0 brighter dark areas
//#define NL_BLINKING_TORCH  // [toggle] flickering light
//#define NL_CLOUD_SHADOW      // [toggle] cloud shadow (simple clouds only)

/* The End detection: must equal the air fog_color in assets/fogs/the_end_fog_setting.json (#302044 = 48,32,68) */
#define NL_END_FOG_COL vec3(48.0,32.0,68.0) // 0-255 per channel

/* Ambient light for nether/end */
#define NL_NETHER_AMBIENT vec3(3.0,2.16,1.89)
#define NL_END_AMBIENT    vec3(1.98,1.25,2.3)

/* Sun/moon light color */
#define NL_DAWN_SUNLIGHT_COL   vec3(1.0,0.7,0.15)
#define NL_NOON_SUNLIGHT_COL   vec3(1.0,0.75,0.68)
#define NL_NIGHT_MOONLIGHT_COL vec3(0.002,0.0045,0.052)

/* Torch colors */
#define NL_OVERWORLD_TORCH_COL  vec3(1.0,0.58,0.35)
#define NL_UNDERWATER_TORCH_COL vec3(1.0,0.58,0.35)
#define NL_NETHER_TORCH_COL     vec3(1.0,0.58,0.35)
#define NL_END_TORCH_COL        vec3(1.0,0.58,0.35)

/* Fog */
#define NL_FOG 1.0                // [toggle] 0.1 subtle ~ 1.0 blend with sky completely
#define NL_MIST_DENSITY 1.0      // 0.0 no mist ~ 1.0 misty
#define NL_RAIN_MIST_OPACITY 1.0 // [toggle] 0.04 very subtle ~ 0.5 thick rain mist blow
#define NL_CLOUDY_FOG 0.4         // [toggle] 0.0 subtle - 0.8 dense fog clouds

/* Sky */
#define NL_SKY_VOID_FACTOR     0.5
#define NL_SKY_VOID_DARKNESS   0.0
#define NL_SKY_RAIN_MIX_FACTOR 1.0

/* Sky colors - zenith=top, horizon=bottom */
#define NL_DAWN_ZENITH_COL   vec3(0.12,0.38,0.90)  // blue at the top
#define NL_DAWN_HORIZON_COL  vec3(4.5,1.90,0.20)   // orange-red band at the horizon
#define NL_DAWN_EDGE_COL     vec3(4.6,3.00,0.50)    // yellow-orange core near the sun
#define NL_DAWN_SPREAD 1.0       // 0.0 off ~ 1.0 full spread
#define NL_DAWN_EDGE_SPREAD 1.0  // 0.0 zenith stays blue ~ 1.0 whole sky tinted
#define NL_DAWN_WIDTH 1.0       // sideways reach: 2.0 tight ~ 0.4 very wide
#define NL_DAWN_HEIGHT 0.9      // upward reach: 2.0 low ~ 0.7 very high

#define NL_DAY_ZENITH_COL    vec3(0.2,1.2,2.8)
#define NL_DAY_HORIZON_COL   vec3(0.25,1.6,2.4)
#define NL_DAY_EDGE_COL      vec3(1.2,2.0,2.5)

#define NL_NIGHT_ZENITH_COL  vec3(0.004,0.040,0.250)
#define NL_NIGHT_HORIZON_COL vec3(0.004,0.095,0.340)
#define NL_NIGHT_EDGE_COL    vec3(0.006,0.160,0.520)

#define NL_RAIN_ZENITH_COL   vec3(0.175,0.175,0.175)
#define NL_RAIN_HORIZON_COL  vec3(0.35,0.35,0.35)

#define NL_END_ZENITH_COL    vec3(0.0,0.0,0.0)
#define NL_END_HORIZON_COL   vec3(0.0,0.0,0.0)

/* blackhole at the end */
#define NL_BLACKHOLE                         // [toggle] enable blackhole in endsky
#define NL_BH_COL_LOW vec3(0.0,0.0,0.0)      // hole color
#define NL_BH_COL_HIGH vec3(0.25,0.21,0.4)   // spiral ring color
#define NL_BH_DIST 1.9                     // blackhole size: 5.0 tiny - 0.5 massive
#define NL_BH_SPEED 0.6                      // spiral rotation speed

/* Rainbow */
//#define NL_RAINBOW           // [toggle] enable rainbow in sky
#define NL_RAINBOW_CLEAR 0.0 // 0.3 subtle ~ 1.0 bright during clear weather
#define NL_RAINBOW_RAIN  0.4 // 0.3 subtle ~ 1.0 bright during rain weather

/* Ore glow intensity */
#define NL_GLOW_TEX 3.5          // 0.4 weak ~ 8.0 bright
#define NL_GLOW_SHIMMER 1.0      // [toggle] 0.1 subtle ~ 1.0 100% shimmer
#define NL_GLOW_SHIMMER_SPEED 1.0 // 0.5 slow - 2.0 fast
//#define NL_GLOW_LEAK 0.6        // [toggle] 0.08 subtle ~ 1.0 100% brightness of NL_GLOW_TEX

/* Waving */
#define NL_PLANTS_WAVE 0.04    // [toggle] 0.02 gentle ~ 0.4 violent
#define NL_LANTERN_WAVE 0.15   // [toggle] 0.05 subtle ~ 0.4 large swing
#define NL_WAVE_SPEED 2.6      // 0.5 slow wave ~ 5.0 very fast wave
//#define NL_EXTRA_PLANTS_WAVE // [toggle] !dont use! wave using texture coords (1.21.0 vanilla 2048x1024)
#define NL_WAVE_RANGE 13.0     // 6.0 upto 6 blocks ~ 32.0 upto 32 blocks away (slower)

/* Water */
#define NL_WATER_TRANSPARENCY 0.5 // 0.0 transparent ~ 1.0 normal
#define NL_WATER_BUMP 0.05        // 0.001 plain ~ 0.2 bumpy water
#define NL_WATER_WAVE_SPEED  0.6  // 0.2 calm ~ 2.0 turbulent
#define NL_WATER_TEX_OPACITY 0.0  // 0.0 plain water ~ 1.0 vanilla water texture
#define NL_WATER_WAVE             // [toggle] wave effect
//#define NL_WATER_REFL_MASK      // [toggle] fake water reflection mask
#define NL_WATER_TINT vec3(0.0,0.1,0.9)

/* Underwater */
#define NL_UNDERWATER_BRIGHTNESS 2.0        // 0.0 dark ~ 3.0 bright
#define NL_CAUSTIC_INTENSITY 1.9             // 0.5 weak ~ 5.0 bright
#define NL_UNDERWATER_WAVE 0.05               // [toggle] 0.02 subtle ~ 0.6 trippy
#define NL_UNDERWATER_STREAKS 2.0            // [toggle] 0.8 subtle - 2.0 bright streaks from top
#define NL_UNDERWATER_TINT vec3(0.4,0.7,1.0) // fog tint color when underwater

/* Cloud type */
#define NL_CLOUD_TYPE 0 // 0:vanilla, 1:soft, 2:rounded, 3:realistic

/* Vanilla cloud settings - make sure to remove clouds.png when using this */
#define NL_CLOUD0_THICKNESS 1.5     // 0.5 slim ~ 8.0 fat
#define NL_CLOUD0_RAIN_THICKNESS 3.3 // 0.5 slim ~ 8.0 fat
#define NL_CLOUD0_OPACITY 1.0       // 0.0 invisible ~ 1.0 opaque
#define NL_CLOUD0_MULTILAYER         // [toggle] extra cloud layer

/* Soft cloud settings */
#define NL_CLOUD1_SCALE vec2(0.016, 0.022) // 0.003 large ~ 0.2 tiny
#define NL_CLOUD1_DEPTH 1.3                // 0.0 no bump ~ 10.0 large bumps
#define NL_CLOUD1_SPEED 0.04               // 0.0 static ~ 0.4 fast moving
#define NL_CLOUD1_DENSITY 0.54             // 0.1 less clouds ~ 0.8 more clouds
#define NL_CLOUD1_OPACITY 0.9              // 0.0 invisible ~ 1.0 opaque

/* Rounded cloud settings */
#define NL_CLOUD2_THICKNESS 2.1            // 0.5 slim ~ 5.0 fat
#define NL_CLOUD2_RAIN_THICKNESS 2.5       // 0.5 slim ~ 5.0 fat
#define NL_CLOUD2_STEPS 5                  // 3 low quality ~ 16 high quality
#define NL_CLOUD2_SCALE vec2(0.033, 0.033) // 0.003 large ~ 0.3 tiny
#define NL_CLOUD2_SHAPE vec2(0.5, 0.4)     // 0.0 round ~ 1.0 box vec2(horizontal shape, vertical shape)
#define NL_CLOUD2_DENSITY 25.0             // 1.0 blurry ~ 100.0 sharp
#define NL_CLOUD2_VELOCITY 0.8             // 0.0 static ~ 4.0 very fast
//#define NL_CLOUD2_LAYER2                      // [toggle] extra cloud layer
#define NL_CLOUD2_LAYER2_OFFSET 143.0           // 30.0 near ~ 300.0 very high
#define NL_CLOUD2_LAYER2_THICKNESS 2.5          // 0.7 slim ~ 5.0 fat
#define NL_CLOUD2_LAYER2_RAIN_THICKNESS 3.0     // 0.7 slim ~ 5.0 fat
#define NL_CLOUD2_LAYER2_STEPS 3                // 3 low quality ~ 16 high quality
#define NL_CLOUD2_LAYER2_SCALE vec2(0.03, 0.03) // 0.003 large ~ 0.3 tiny
#define NL_CLOUD2_LAYER2_SHAPE vec2(0.5, 0.4)   // 0.0 round ~ 1.0 box vec2(horizontal shape, vertical shape)
#define NL_CLOUD2_LAYER2_DENSITY 25.0           // 1.0 blurry ~ 100.0 sharp
#define NL_CLOUD2_LAYER2_VELOCITY 0.8           // 0.0 static ~ 4.0 very fast

/* Realistic cloud settings */
#define NL_CLOUD3_SCALE vec2(0.03, 0.03) // 0.003 large ~ 1.0 tiny
#define NL_CLOUD3_SPEED 0.005            // 0.0 static ~ 4.0 fast moving
#define NL_CLOUD3_SHADOW 0.9             // 0.1 subtle ~ 1.0 dark
#define NL_CLOUD3_SHADOW_OFFSET 0.3      // 0.05 minimal ~ 1.0 large

/* Aurora settings */
//#define NL_AURORA 1.2           // [toggle] 0.4 dim ~ 4.0 very bright
#define NL_AURORA_VELOCITY 0.03 // 0.0 static ~ 0.3 very fast
#define NL_AURORA_SCALE 0.04    // 0.002 large ~ 0.4 tiny
#define NL_AURORA_WIDTH 0.18    // 0.04 thin line ~ 0.4 thick lines
#define NL_AURORA_COL1 vec3(0.1,1.0,0.0)
#define NL_AURORA_COL2 vec3(0.1,0.0,1.0)

//#define NL_CLOUD_AURORA_REFLECTION // [toggle] simple clouds/aurora reflection

/* Chunk loading slide in animation */
//#define NL_CHUNK_LOAD_ANIM 100.0 // [toggle] -600.0 fall from top ~ 600.0 rise from bottom

/* Sun/Moon */
#define NL_SUN_SIZE  3.2          // 0.3 tiny ~ 4.0 massive
#define NL_MOON_SIZE 3.0          // 0.3 tiny ~ 4.0 massive
#define NL_SUN_PATH_YAW    0.0 //
#define NL_MOON_PATH_YAW   0.0 //
#define NL_SUN_PATH_TILT   0.0 //
#define NL_MOON_PATH_TILT  0.0 //
#define NL_SUN_TILT        0.0 // 0.0 no tilt ~ 90.0 tilt of 90 degrees
#define NL_MOON_TILT       0.0 // 0.0 no tilt ~ 90.0 tilt of 90 degrees

/* Sun glow shape: the glow around the sun follows the (square) sun texture */
#define NL_SUN_GLOW_SHAPE 8.0 // [toggle] 2.0 round ~ 5.0 rounded square ~ 12.0 square. Sun only, the moon keeps the round glow

/* Fake godrays during sunrise/sunset */
#define NL_GODRAY 1.5     // [toggle] 0.1 low - 1.5 strength

/* Sky reflection */
//#define NL_GROUND_REFL 0.0       // [toggle] 0.2 slightly reflective ~ 1.0 fully reflect sky
#define NL_GROUND_RAIN_WETNESS 1.0 // 0.0 no wetness ~ 1.0 fully wet blocks when raining
#define NL_GROUND_RAIN_PUDDLES 1.0 // 0.0 no puddles ~ 1.0 puddles

/* Entity (Actor, ItemInHand) */
#define NL_ENTITY_BRIGHTNESS     0.1 // 0.1 dark ~ 1.6 bright
//#define NL_ENTITY_EDGE_HIGHLIGHT 0.3 // [toggle] 0.0 no highlight ~ 1.6 bright highlight

/* Weather particles */
//#define NL_WEATHER_SPECK 0.6         // [toggle] 0.0 vanilla texture ~ 1.0 soft speck
#define NL_WEATHER_RAIN_SLANT 1.0      // 1.0 minimal ~ 8.0 violent
#define NL_WEATHER_PARTICLE_SIZE 0.5   // 0.5 tiny ~ 4.0 large

/* Lava effects */
//#define NL_LAVA_NOISE            // [toggle] darken lava in certain regions
#define NL_LAVA_NOISE_BUMP 0.4 // [toggle] 0.1 subtle ~ 0.8 massive waves
#define NL_LAVA_NOISE_SPEED 0.2  // 0.0 still ~ 0.8 fast

/* LOW subpack: smaller values for small visuals + heavy functions switched off. Colors, lighting, sun glow shape, cloud type 0 and other main visuals are untouched */
#ifdef LOW
  // reduced values
  #undef NL_MIST_DENSITY
  #define NL_MIST_DENSITY 0.5

  #undef NL_RAIN_MIST_OPACITY
  #define NL_RAIN_MIST_OPACITY 0.5

  #undef NL_CLOUDY_FOG
  #define NL_CLOUDY_FOG 0.2

  #undef NL_GLOW_TEX
  #define NL_GLOW_TEX 2.8

  #undef NL_GLOW_SHIMMER
  #define NL_GLOW_SHIMMER 0.5

  #undef NL_PLANTS_WAVE
  #define NL_PLANTS_WAVE 0.025

  #undef NL_LANTERN_WAVE
  #define NL_LANTERN_WAVE 0.1

  #undef NL_WAVE_RANGE
  #define NL_WAVE_RANGE 6.0 // distance in blocks

  #undef NL_WATER_BUMP
  #define NL_WATER_BUMP 0.025

  #undef NL_UNDERWATER_WAVE
  #define NL_UNDERWATER_WAVE 0.025

  #undef NL_UNDERWATER_STREAKS
  #define NL_UNDERWATER_STREAKS 1.0

  #undef NL_GROUND_RAIN_WETNESS
  #define NL_GROUND_RAIN_WETNESS 0.5

  #undef NL_GROUND_RAIN_PUDDLES
  #define NL_GROUND_RAIN_PUDDLES 0.5

  #undef NL_LAVA_NOISE_BUMP
  #define NL_LAVA_NOISE_BUMP 0.2

  // heavy functions off (only the ones enabled in the main config)
  #undef NL_GODRAY
  #undef NL_WATER_WAVE
  #undef NL_CLOUD0_MULTILAYER // second layer of type 0 clouds
#endif

#endif
