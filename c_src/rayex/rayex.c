#include "rayex.h"
#include "miniaudio.h"
#include <raylib.h>

/***********
 * STRUCTS *
 ***********/
// Audio buffer struct
struct rAudioBuffer {
    ma_data_converter converter;    // Audio data converter

    AudioCallback callback;         // Audio buffer callback for buffer filling on audio threads
    rAudioProcessor *processor;     // Audio processor

    float volume;                   // Audio buffer volume
    float pitch;                    // Audio buffer pitch
    float pan;                      // Audio buffer pan (0.0f to 1.0f)

    bool playing;                   // Audio buffer state: AUDIO_PLAYING
    bool paused;                    // Audio buffer state: AUDIO_PAUSED
    bool looping;                   // Audio buffer looping, default to true for AudioStreams
    int usage;                      // Audio buffer usage mode: STATIC or STREAM

    bool isSubBufferProcessed[2];   // SubBuffer processed (virtual double buffer)
    unsigned int sizeInFrames;      // Total buffer size in frames
    unsigned int frameCursorPos;    // Frame cursor position
    unsigned int framesProcessed;   // Total frames processed in this buffer (required for play timing)

    unsigned char *data;            // Data buffer, on music stream keeps filling

    rAudioBuffer *next;             // Next audio buffer on the list
    rAudioBuffer *prev;             // Previous audio buffer on the list
};

// Audio processor struct
// NOTE: Useful to apply effects to an AudioBuffer
struct rAudioProcessor {
    AudioCallback process;          // Processor callback function
    rAudioProcessor *next;          // Next audio processor on the list
    rAudioProcessor *prev;          // Previous audio processor on the list
};

// NOTE: "E_" when converting back to NIF struct

#define VECTOR2(v) ((Vector2){.x = v.x, .y = v.y})
#define E_VECTOR2(v) ((vector2){.x = v.x, .y = v.y})

#define VECTOR3(v) ((Vector3){.x = v.x, .y = v.y, .z = v.z})
#define E_VECTOR3(v) ((vector3){.x = v.x, .y = v.y, .z = v.z})

#define VECTOR4(v) ((Vector4){.x = v.x, .y = v.y, .z = v.z, .w = v.w})
#define E_VECTOR4(v) ((vector4){.x = v.x, .y = v.y, .z = v.z, .w = v.w})

#define QUATERNION(q) ((Quaternion){.x = q.x, .y = q.y, .z = q.z, .w = q.w})
#define E_QUATERNION(q) ((quaternion){.x = q.x, .y = q.y, .z = q.z, .w = q.w})

#define MATRIX(m)                                                              \
  ((Matrix){.m0 = m.m0,                                                        \
            .m1 = m.m1,                                                        \
            .m2 = m.m2,                                                        \
            .m3 = m.m3,                                                        \
            .m4 = m.m4,                                                        \
            .m5 = m.m5,                                                        \
            .m6 = m.m6,                                                        \
            .m7 = m.m7,                                                        \
            .m8 = m.m8,                                                        \
            .m9 = m.m9,                                                        \
            .m10 = m.m10,                                                      \
            .m11 = m.m11,                                                      \
            .m12 = m.m12,                                                      \
            .m13 = m.m13,                                                      \
            .m14 = m.m14,                                                      \
            .m15 = m.m15})
#define E_MATRIX(m)                                                            \
  ((matrix){.m0 = m.m0,                                                        \
            .m1 = m.m1,                                                        \
            .m2 = m.m2,                                                        \
            .m3 = m.m3,                                                        \
            .m4 = m.m4,                                                        \
            .m5 = m.m5,                                                        \
            .m6 = m.m6,                                                        \
            .m7 = m.m7,                                                        \
            .m8 = m.m8,                                                        \
            .m9 = m.m9,                                                        \
            .m10 = m.m10,                                                      \
            .m11 = m.m11,                                                      \
            .m12 = m.m12,                                                      \
            .m13 = m.m13,                                                      \
            .m14 = m.m14,                                                      \
            .m15 = m.m15})

#define COLOR(c) ((Color){.r = c.r, .g = c.g, .b = c.b, .a = c.a})
#define E_COLOR(c) ((color){.r = c.r, .g = c.g, .b = c.b, .a = c.a})

#define RECTANGLE(r)                                                           \
  ((Rectangle){.x = r.x, .y = r.y, .width = r.width, .height = r.height})
#define E_RECTANGLE(r)                                                         \
  ((rectangle){.x = r.x, .y = r.y, .width = r.width, .height = r.height})

e_ma_channel_converter_weights E_MA_CHANNEL_CONVERTER_WEIGHTS(float** f32, int** s16)
{
  e_ma_channel_converter_weights result;// = e_ma_channel_converter_weights();
  result.f32 = f32;
  result.s16 = s16;
  return result;
}

e_ma_channel_converter E_MA_CHANNEL_CONVERTER(ma_channel_converter c)
{
  e_ma_channel_converter result;// = e_ma_channel_converter();
  result.format = c.format;
  result.channels_in = c.channelsIn;
  result.channels_out = c.channelsOut;
  result.mixing_mode = c.mixingMode;
  result.conversion_path = c.conversionPath;
  result.channel_map_in = c.pChannelMapIn;
  result.channel_map_out = c.pChannelMapOut;
  result.shuffle_table = c.pShuffleTable;
  result.weights = E_MA_CHANNEL_CONVERTER_WEIGHTS(c.weights.f32, c.weights.s16); // I fully expect this to error.
  result.weights.s16 = c.weights.s16;
  result.owns_heap = c._ownsHeap;
  result.heap = c._pHeap;
  return result;
}

e_ma_linear_resampler_config E_MA_LINEAR_RESAMPLER_CONFIG(ma_linear_resampler_config c)
{
  e_ma_linear_resampler_config result;// = e_ma_linear_resampler_config();
  result.format = c.format;
  result.channels = c.channels;
  result.sample_rate_in = c.sampleRateIn;
  result.sample_rate_out = c.sampleRateOut;
  result.lpf_order = c.lpfOrder;
  result.lpf_nyquist_factor = c.lpfNyquistFactor;
  return result;
}

e_ma_biquad_coefficient E_MA_BIQUAD_COEFFICIENT(ma_biquad_coefficient bqc)
{
  e_ma_biquad_coefficient result;// = e_ma_biquad_coefficient();
  result.f32 = bqc.f32;
  result.s32 = bqc.s32;
  return result;
}

e_ma_biquad_coefficient* E_MA_BIQUAD_COEFFICIENT_POINTER(ma_biquad_coefficient* bqc)
{
  e_ma_biquad_coefficient* result = (e_ma_biquad_coefficient*)malloc(sizeof(e_ma_biquad_coefficient));
  result->f32 = bqc->f32;
  result->s32 = bqc->s32;
  return result;
}

e_ma_biquad* E_MA_BIQUAD(ma_biquad* bq)
{
  e_ma_biquad* result = (e_ma_biquad*)malloc(sizeof(e_ma_biquad));
  result->format = bq->format;
  result->channels = bq->channels;
  result->b0 = E_MA_BIQUAD_COEFFICIENT(bq->b0);
  result->b1 = E_MA_BIQUAD_COEFFICIENT(bq->b1);
  result->b2 = E_MA_BIQUAD_COEFFICIENT(bq->b2);
  result->a1 = E_MA_BIQUAD_COEFFICIENT(bq->a1);
  result->a2 = E_MA_BIQUAD_COEFFICIENT(bq->a2);
  result->pR1 = E_MA_BIQUAD_COEFFICIENT_POINTER(bq->pR1);
  result->pR2 = E_MA_BIQUAD_COEFFICIENT_POINTER(bq->pR2);
  result->heap = bq->_pHeap;
  result->owns_heap = bq->_ownsHeap;
  return result;
}


// #define E_MA_LPF1(lpf1) \
// ((e_ma_lpf1){ \
//   .format = lpf1->format,\
//   .channels = lpf1->channels,\
//   .a = E_MA_BIQUAD_COEFFICIENT(lpf1->a),\
//   .pR1 = E_MA_BIQUAD_COEFFICIENT_POINTER(lpf1->pR1), \
//   .heap = lpf1->_pHeap,\
//   .owns_heap = lpf1->_ownsHeap,\
// })

e_ma_lpf1* E_MA_LPF1(ma_lpf1* lpf1)
{
  e_ma_lpf1* result = (e_ma_lpf1*)malloc(sizeof(e_ma_lpf1));
  result->format = lpf1->format;
  result->channels = lpf1->channels;
  result->a = E_MA_BIQUAD_COEFFICIENT(lpf1->a);
  result->pR1 = E_MA_BIQUAD_COEFFICIENT_POINTER(lpf1->pR1);
  result->heap = lpf1->_pHeap;
  result->owns_heap = lpf1->_ownsHeap;
  return result;
}

// #define E_MA_LPF(lpf) \
// ((e_ma_lpf){  \
//   .format = lpf.format, \
//   .channels = lpf.channels, \
//   .sample_rate = lpf.sampleRate, \
//   .lpf1_count = lpf.lpf1Count, \
//   .lpf2_count = lpf.lpf2Count, \
//   .lpf1 = E_MA_LPF1(lpf.pLPF1), \
//   .lpf2 = E_MA_BIQUAD(lpf.pLPF2->bq), \
//   .heap = lpf._pHeap, \
//   .owns_heap = lpf._ownsHeap \
// })

e_ma_lpf E_MA_LPF(ma_lpf lpf)
{
  e_ma_lpf result;// = e_ma_lpf();
  result.format = lpf.format;
  result.channels = lpf.channels;
  result.sample_rate = lpf.sampleRate;
  result.lpf1_count = lpf.lpf1Count;
  result.lpf2_count = lpf.lpf2Count;
  result.lpf1 = E_MA_LPF1(lpf.pLPF1);
  result.lpf2 = E_MA_BIQUAD(&lpf.pLPF2->bq);
  result.heap = lpf._pHeap;
  result.owns_heap = lpf._ownsHeap;
  return result;
}

// #define E_MA_LINEAR_RESAMPLER(r) \
//   ((e_ma_linear_resampler){ \
//     .config = E_MA_LINEAR_RESAMPLER_CONFIG(r.config), \
//     .in_advance_int = r.inAdvanceInt, \
//     .in_advance_frac = r.inAdvanceFrac, \
//     .in_time_int = r.inTimeInt, \
//     .in_time_frac = r.inTimeFrac, \
//     .x0 = E_MA_CHANNEL_CONVERTER_WEIGHTS(r.x0), \
//     .x1 = E_MA_CHANNEL_CONVERTER_WEIGHTS(r.x1), \
//     .lpf = E_MA_LPF(r.lpf), \
//     .owns_heap = r._ownsHeap, \
//     .heap = r._pHeap, \
//   })

e_ma_linear_resampler E_MA_LINEAR_RESAMPLER(ma_linear_resampler r)
{
  e_ma_linear_resampler result;// = e_ma_linear_resampler();
  result.config = E_MA_LINEAR_RESAMPLER_CONFIG(r.config);
  result.in_advance_int = r.inAdvanceInt;
  result.in_advance_frac = r.inAdvanceFrac;
  result.in_time_int = r.inTimeInt;
  result.in_time_frac = r.inTimeFrac;
  result.x0 = E_MA_CHANNEL_CONVERTER_WEIGHTS(&r.x0.f32, &r.x0.s16);
  result.x1 = E_MA_CHANNEL_CONVERTER_WEIGHTS(&r.x1.f32, &r.x1.s16);
  result.lpf = E_MA_LPF(r.lpf);
  result.owns_heap = r._ownsHeap;
  result.heap = r._pHeap;
  return result;
}

// #define E_MA_RESAMPLER(r) \
//   ((e_ma_resampler){      \
//     .resampling_backend = r.pBackend,\
//     .resampling_backend_vtable = r.pBackendVTable, \
//     .backend_user_data = r.pBackendUserData, \
//     .format = r.format, \
//     .channels = r.channels, \
//     .sample_rate_in = r.sampleRateIn, \
//     .sample_rate_out = r.sampleRateOut,\
//     .linear = E_MA_LINEAR_RESAMPLER(r.state.linear), \
//     .owns_heap = r._ownsHeap, \
//     .heap = r._pHeap, \
//   })

e_ma_resampler E_MA_RESAMPLER(ma_resampler r)
{
  e_ma_resampler result;// = e_ma_resampler();
  result.resampling_backend = r.pBackend;
  result.resampling_backend_vtable = r.pBackendVTable;
  result.backend_user_data = r.pBackendUserData;
  result.format = r.format;
  result.channels = r.channels;
  result.sample_rate_in = r.sampleRateIn;
  result.sample_rate_out = r.sampleRateOut;
  result.linear = E_MA_LINEAR_RESAMPLER(r.state.linear);
  result.owns_heap = r._ownsHeap;
  result.heap = r._pHeap;
  return result;
}

// #define E_MA_DATA_CONVERTER(c)  \
//   ((e_ma_data_converter){       \
//   .format_in = c.formatIn,      \
//   .format_out = c.formatOut,    \
//   .channels_in = c.channelsIn,   \
//   .channels_out = c.channelsOut,            \
//   .sample_rate_in = c.sampleRateIn,          \
//   .sample_rate_out = c.sampleRateOut, \
//   .dither_mode = c.ditherMode, \
//   .execution_path = c.executionPath,\
//   .channel_converter = E_MA_CHANNEL_CONVERTER(c.channelConverter),\
//   .resampler = E_MA_RESAMPLER(c.resampler),\
//   .has_pre_format_conversion = c.hasPreFormatConversion,\
//   .has_post_format_conversion = c.hasPostFormatConversion,\
//   .has_channel_converter = c.hasChannelConverter,\
//   .has_resampler = c.hasResampler,\
//   .is_passthrough = c.isPassthrough,\
//   .owns_heap = c._ownsHeap,\
//   .heap = c._pHeap\
//   })

e_ma_data_converter E_MA_DATA_CONVERTER(ma_data_converter c)
{
  e_ma_data_converter result;// = e_ma_data_converter();
  result.format_in = c.formatIn;    
  result.format_out = c.formatOut;  
  result.channels_in = c.channelsIn; 
  result.channels_out = c.channelsOut;         
  result.sample_rate_in = c.sampleRateIn;        
  result.sample_rate_out = c.sampleRateOut;
  result.dither_mode = c.ditherMode;
  result.execution_path = c.executionPath;
  result.channel_converter = E_MA_CHANNEL_CONVERTER(c.channelConverter);
  result.resampler = E_MA_RESAMPLER(c.resampler);
  result.has_pre_format_conversion = c.hasPreFormatConversion;
  result.has_post_format_conversion = c.hasPostFormatConversion;
  result.has_channel_converter = c.hasChannelConverter;
  result.has_resampler = c.hasResampler;
  result.is_passthrough = c.isPassthrough;
  result.owns_heap = c._ownsHeap;
  result.heap = c._pHeap;
  return result;
}

// rAudioBuffer* R_AUDIO_BUFFER(r_audio_buffer* b)
// {
//   rAudioBuffer* rab = (rAudioBuffer*)malloc(sizeof(rAudioBuffer));
//   rab->converter = b->converter;
//   rab->callback = b->callback;
//   rab->processor = b->processor;
//   rab->volume = b->volume;
//   rab->pitch = b->pitch;
//   rab->pan = b->pan;
//   rab->playing = b->playing;
//   rab->paused = b->paused;
//   rab->looping = b->looping;
//   rab->usage = b->usage;
//   rab->isSubBufferProcessed[0] = false;
//   rab->isSubBufferProcessed[1] = false;
//   rab->sizeInFrames = b->size_in_frames;
//   rab->frameCursorPos = b->frame_cursor_pos;
//   rab->framesProcessed = b->frames_processed;
//   rab->data = b->data;
//   rab->next = b->next;
//   rab->prev = b->prev;
//   return rab;
// }

r_audio_buffer* E_R_AUDIO_BUFFER(rAudioBuffer* b)
{
  r_audio_buffer* result = (r_audio_buffer*)malloc(sizeof(r_audio_buffer));
  result->converter = E_MA_DATA_CONVERTER(b->converter);
  result->callback = b->callback;
  result->processor = b->processor;
  result->volume = b->volume;
  result->pitch = b->pitch;
  result->pan = b->pan;
  result->playing = b->playing;
  result->paused = b->paused;
  result->looping = b->looping;
  result->usage = b->usage;
  result->is_sub_buffer_processed = b->isSubBufferProcessed;
  result->size_in_frames = b->sizeInFrames;
  result->frame_cursor_pos = b->frameCursorPos;
  result->frames_processed = b->framesProcessed;
  result->data = b->data;
  result->next = b->next;
  result->prev = b->prev;
  return result;
}

// rAudioProcessor* R_AUDIO_PROCESSOR(r_audio_processor* p)
// {
//   rAudioProcessor* rap = (rAudioProcessor*)malloc(sizeof(rAudioProcessor));
//   rap->process = p->process;
//   rap->next = p->next;
//   rap->prev = p->prev;
//   return rap;
// }
r_audio_processor* E_R_AUDIO_PROCESSOR(rAudioProcessor* p) 
{ 
  r_audio_processor* result = (r_audio_processor*)malloc(sizeof(r_audio_processor));
  result->process = p->process;  
  result->next = p->next;
  result->prev = p->prev;
  return result;
}

// #define E_R_AUDIO_BUFFER(b) \
//   ((r_audio_buffer*){     \
//     .converter = &E_MA_DATA_CONVERTER(b->converter), \
//     .callback = &b->callback,  \
//     .processor = &b->processor,  \
//     .volume = &b->volume,  \
//     .pitch = &b->pitch,  \
//     .pan = &b->pan,  \
//     .playing = &b->playing,  \
//     .paused = &b->paused,  \
//     .looping = &b->looping,  \
//     .usage = &b->usage,  \
//     .is_sub_buffer_processed = &b->isSubBufferProcessed, \
//     .size_in_frames = &b->sizeInFrames,  \
//     .frame_cursor_pos = &b->frameCursorPos,  \
//     .frames_processed = &b->framesProcessed, \
//     .data = &b->data,  \
//     .next = &b->next,  \
//     .prev = &b->prev  \
//   })

// #define E_R_AUDIO_PROCESSOR(p) \
//   ((r_audio_processor){ \
//     .process = p->process, \
//     .next = p->next,      \
//     .prev = p->prev       \
//   })

// #define R_AUDIO_STREAM(s) \
//   ((AudioStream){.buffer = R_AUDIO_BUFFER(s.buffer), \
//   .processor = R_AUDIO_PROCESSOR(s.processor), \
//   .sampleRate = s.sample_rate, \
//   .sampleSize = s.sample_size, \
//   .channels = s.channels})
// #define E_R_AUDIO_STREAM(s) \
//   ((audio_stream){.buffer = E_R_AUDIO_BUFFER(s.buffer), \
//   .processor = E_R_AUDIO_PROCESSOR(s.processor), \
//   .sample_rate = s.sampleRate, \
//   .sample_size = s.sampleSize, \
//   .channels = s.channels})

audio_stream E_R_AUDIO_STREAM(AudioStream s)
{
  audio_stream result;// = audio_stream();
  result.buffer = E_R_AUDIO_BUFFER(s.buffer);
  result.processor = E_R_AUDIO_PROCESSOR(s.processor);
  result.sample_rate = s.sampleRate;
  result.sample_size = s.sampleSize;
  result.channels = s.channels;
  return result;
}

// #define SOUND(s) ((Sound){.stream = R_AUDIO_STREAM(s.stream), .frameCount = s.frame_count})
// #define E_SOUND(s) ((sound){.stream = E_R_AUDIO_STREAM(s.stream), .frame_count = s.frameCount })

sound E_SOUND(Sound s) {
  sound result;// = sound();
  result.stream = E_R_AUDIO_STREAM(s.stream);
  result.frame_count = s.frameCount;
  return result;
}

// XXX: maybe data will not work because is a (void *) and elixir sends a
// payload
#define IMAGE(i)                                                               \
  ((Image){.data = i.data,                                                     \
           .width = i.width,                                                   \
           .height = i.height,                                                 \
           .mipmaps = i.mipmaps,                                               \
           .format = i.format})
#define E_IMAGE(i)                                                             \
  ((image){.data = i.data,                                                     \
           .width = i.width,                                                   \
           .height = i.height,                                                 \
           .mipmaps = i.mipmaps,                                               \
           .format = i.format})

#define TEXTURE(t)                                                             \
  ((Texture){.id = t.id,                                                       \
             .width = t.width,                                                 \
             .height = t.height,                                               \
             .mipmaps = t.mipmaps,                                             \
             .format = t.format})
#define E_TEXTURE(t)                                                           \
  ((texture){.id = t.id,                                                       \
             .width = t.width,                                                 \
             .height = t.height,                                               \
             .mipmaps = t.mipmaps,                                             \
             .format = t.format})

#define TEXTURE_2D(t)                                                          \
  ((Texture2D){.id = t.id,                                                     \
               .width = t.width,                                               \
               .height = t.height,                                             \
               .mipmaps = t.mipmaps,                                           \
               .format = t.format})
#define E_TEXTURE_2D(t)                                                        \
  ((texture_2d){.id = t.id,                                                    \
                .width = t.width,                                              \
                .height = t.height,                                            \
                .mipmaps = t.mipmaps,                                          \
                .format = t.format})

#define CAMERA3D(c)                                                            \
  ((Camera3D){.position = VECTOR3(c.position),                                 \
              .target = VECTOR3(c.target),                                     \
              .up = VECTOR3(c.up),                                             \
              .fovy = c.fovy,                                                  \
              .projection = c.projection})
#define E_CAMERA3D(c)                                                          \
  ((camera_3d){.position = E_VECTOR3(c.position),                              \
               .target = E_VECTOR3(c.target),                                  \
               .up = E_VECTOR3(c.up),                                          \
               .fovy = c.fovy,                                                 \
               .projection = c.projection})

#define CAMERA2D(c)                                                            \
  ((Camera2D){.offset = VECTOR2(c.offset),                                     \
              .target = VECTOR2(c.target),                                     \
              .rotation = c.rotation,                                          \
              .zoom = c.zoom})
#define E_CAMERA2D(c)                                                          \
  ((camera_2d){.offset = E_VECTOR2(c.offset),                                  \
               .target = E_VECTOR2(c.target),                                  \
               .rotation = c.rotation,                                         \
               .zoom = c.zoom})

#define RAY(v)                                                                 \
  ((Ray){.position = VECTOR3(v.position), .direction = VECTOR3(v.direction)})
#define E_RAY(v)                                                               \
  ((ray){.position = E_VECTOR3(v.position),                                    \
         .direction = E_VECTOR3(v.direction)})

#define RAY_COLLISION(v)                                                       \
  ((RayCollision){.hit = v.hit,                                                \
                  .distance = v.distance,                                      \
                  .point = VECTOR3(v.point),                                   \
                  .normal = VECTOR3(v.normal)})
#define E_RAY_COLLISION(v)                                                     \
  ((ray_collision){.hit = v.hit,                                               \
                   .distance = v.distance,                                     \
                   .point = E_VECTOR3(v.point),                                \
                   .normal = E_VECTOR3(v.normal)})

#define BOUNDING_BOX(v)                                                        \
  ((BoundingBox){.min = VECTOR3(v.min), .max = VECTOR3(v.max)})
#define E_BOUNDING_BOX(v)                                                      \
  ((bounding_box){.min = E_VECTOR3(v.min), .max = E_VECTOR3(v.max)})

/********
 * CORE *
 ********/

// Window-related functions

UNIFEX_TERM init_window(UnifexEnv *env, int width, int height, char *title) {
  InitWindow(width, height, title);
  return init_window_result_ok(env);
}

UNIFEX_TERM window_should_close(UnifexEnv *env) {
  bool res = WindowShouldClose();
  return window_should_close_result(env, res);
}

UNIFEX_TERM close_window(UnifexEnv *env) {
  CloseWindow();
  return close_window_result_ok(env);
}

UNIFEX_TERM is_window_ready(UnifexEnv *env) {
  bool res = IsWindowReady();
  return is_window_ready_result(env, res);
}

UNIFEX_TERM is_window_fullscreen(UnifexEnv *env) {
  bool res = IsWindowFullscreen();
  return is_window_fullscreen_result(env, res);
}

UNIFEX_TERM is_window_hidden(UnifexEnv *env) {
  bool res = IsWindowHidden();
  return is_window_hidden_result(env, res);
}

UNIFEX_TERM is_window_minimized(UnifexEnv *env) {
  bool res = IsWindowMinimized();
  return is_window_minimized_result(env, res);
}

UNIFEX_TERM is_window_maximized(UnifexEnv *env) {
  bool res = IsWindowMaximized();
  return is_window_maximized_result(env, res);
}

UNIFEX_TERM is_window_focused(UnifexEnv *env) {
  bool res = IsWindowFocused();
  return is_window_focused_result(env, res);
}

UNIFEX_TERM is_window_resized(UnifexEnv *env) {
  bool res = IsWindowResized();
  return is_window_resized_result(env, res);
}

UNIFEX_TERM is_window_state(UnifexEnv *env, int flag) {
  bool res = IsWindowState(flag);
  return is_window_state_result(env, res);
}

UNIFEX_TERM set_window_state(UnifexEnv *env, int flags) {
  SetWindowState(flags);
  return set_window_state_result_ok(env);
}

UNIFEX_TERM clear_window_state(UnifexEnv *env, int flags) {
  ClearWindowState(flags);
  return clear_window_state_result_ok(env);
}

UNIFEX_TERM toggle_fullscreen(UnifexEnv *env) {
  ToggleFullscreen();
  return toggle_fullscreen_result_ok(env);
}

// Cursor-related functions

// Drawing-related functions

// UNIFEX_TERM clear_background(UnifexEnv *env, color c) {
//   ClearBackground(COLOR(c));
//   return clear_background_result_ok(env);
// }

// UNIFEX_TERM begin_drawing(UnifexEnv *env) {
//   BeginDrawing();
//   return begin_drawing_result_ok(env);
// }

// UNIFEX_TERM end_drawing(UnifexEnv *env) {
//   EndDrawing();
//   return end_drawing_result_ok(env);
// }

// UNIFEX_TERM begin_mode_2d(UnifexEnv *env, camera_2d c) {
//   BeginMode2D(CAMERA2D(c));
//   return begin_mode_2d_result_ok(env);
// }

// UNIFEX_TERM end_mode_2d(UnifexEnv *env) {
//   EndMode2D();
//   return end_mode_2d_result_ok(env);
// }

// UNIFEX_TERM begin_mode_3d(UnifexEnv *env, camera_3d c) {
//   BeginMode3D(CAMERA3D(c));
//   return begin_mode_3d_result_ok(env);
// }

// UNIFEX_TERM end_mode_3d(UnifexEnv *env) {
//   EndMode3D();
//   return end_mode_3d_result_ok(env);
// }

// VR stereo config functions for VR simulator

// Shader management functions
// NOTE: Shader functionality is not available on OpenGL 1.1

// Screen-space-related functions
// UNIFEX_TERM get_mouse_ray(UnifexEnv *env, vector2 mouse_position,
//                           camera_3d camera) {
//   Ray r = GetMouseRay(VECTOR2(mouse_position), CAMERA3D(camera));
//   return get_mouse_ray_result(env, E_RAY(r));
// }

// Timing-related functions

// UNIFEX_TERM set_target_fps(UnifexEnv *env, int fps) {
//   SetTargetFPS(fps);
//   return set_target_fps_result_ok(env);
// }

// UNIFEX_TERM get_fps(UnifexEnv *env) {
//   int res = GetFPS();
//   return get_fps_result(env, res);
// }

// UNIFEX_TERM get_frame_time(UnifexEnv *env) {
//   float res = GetFrameTime();
//   return get_frame_time_result(env, res);
// }

// UNIFEX_TERM get_time(UnifexEnv *env) {
//   double res = GetTime();
//   return get_time_result(env, res);
// }

// Misc. functions

// Set custom callbacks
// WARNING: Callbacks setup is intended for advance users

// Files management functions

// Compression/Encoding functionality

// Persistent storage management

// Misc.

// Input-related functions: keyboard

// Input-related functions: gamepads

// Input-related functions: mouse

// UNIFEX_TERM is_mouse_button_pressed(UnifexEnv *env, int button) {
//   bool res = IsMouseButtonPressed(button);
//   return is_mouse_button_pressed_result(env, res);
// }

// UNIFEX_TERM is_mouse_button_down(UnifexEnv *env, int button) {
//   bool res = IsMouseButtonDown(button);
//   return is_mouse_button_down_result(env, res);
// }

// UNIFEX_TERM is_mouse_button_released(UnifexEnv *env, int button) {
//   bool res = IsMouseButtonReleased(button);
//   return is_mouse_button_released_result(env, res);
// }

// UNIFEX_TERM is_mouse_button_up(UnifexEnv *env, int button) {
//   bool res = IsMouseButtonUp(button);
//   return is_mouse_button_up_result(env, res);
// }

// UNIFEX_TERM get_mouse_position(UnifexEnv *env) {
//   Vector2 r = GetMousePosition();
//   return get_mouse_position_result(env, E_VECTOR2(r));
// }

// Input-related functions: touch

// Gestures and Touch Handling Functions (Module: rgestures)

// Camera System Functions (Module: rcamera)
// UNIFEX_TERM update_camera(UnifexEnv *env, camera_3d* c, int mode) {
//   Camera3D camera = CAMERA3D(*c);
//   UpdateCamera(&camera, mode);
//   return update_camera_result_ok(env);
// }

// UNIFEX_TERM update_camera_pro(UnifexEnv *env, camera_3d c*, vector3 movement, vector3 rotation, float zoom) {
//   Camera3D camera = CAMERA3D(*c);
//   UpdateCameraPro(&camera, VECTOR3(movement), VECTOR3(rotation), zoom);
//   return update_camera_pro_result_ok(env);
// }

// UNIFEX_TERM set_camera_pan_control(UnifexEnv *env, int key_pan) {
//   SetCameraPanControl(key_pan);
//   return set_camera_pan_control_result_ok(env);
// }

// UNIFEX_TERM set_camera_alt_control(UnifexEnv *env, int key_alt) {
//   SetCameraAltControl(key_alt);
//   return set_camera_alt_control_result_ok(env);
// }

// UNIFEX_TERM set_camera_smooth_zoom_control(UnifexEnv *env,
//                                            int key_smooth_zoom) {
//   SetCameraSmoothZoomControl(key_smooth_zoom);
//   return set_camera_smooth_zoom_control_result_ok(env);
// }

// UNIFEX_TERM set_camera_move_controls(UnifexEnv *env, int key_front,
//                                      int key_back, int key_right, int key_left,
//                                      int key_up, int key_down) {
//   SetCameraMoveControls(key_front, key_back, key_right, key_left, key_up,
//                         key_down);
//   return set_camera_move_controls_result_ok(env);
// }

/**********
 * SHAPES *
 **********/

// Set texture and rectangle to be used on shapes drawing
// NOTE: It can be useful when using basic shapes and one single font,
// defining a font char white rectangle would allow drawing everything in a
// single draw call

// UNIFEX_TERM set_shapes_texture(UnifexEnv *env, texture_2d t, rectangle s) {
//   SetShapesTexture(TEXTURE_2D(t), RECTANGLE(s));
//   return set_shapes_texture_result_ok(env);
// }

// // Basic shapes drawing functions

// UNIFEX_TERM draw_pixel(UnifexEnv *env, int x, int y, color c) {
//   DrawPixel(x, y, COLOR(c));
//   return draw_pixel_result_ok(env);
// }

// UNIFEX_TERM draw_line(UnifexEnv *env, int start_x, int start_y, int end_x,
//                       int end_y, color c) {
//   DrawLine(start_x, start_y, end_x, end_y, COLOR(c));
//   return draw_pixel_result_ok(env);
// }

// UNIFEX_TERM draw_rectangle_rec(UnifexEnv *env, rectangle r, color c) {
//   DrawRectangleRec(RECTANGLE(r), COLOR(c));
//   return draw_rectangle_rec_result_ok(env);
// }

// UNIFEX_TERM draw_rectangle_lines_ex(UnifexEnv *env, rectangle r, int line_thick,
//                                     color c) {
//   DrawRectangleLinesEx(RECTANGLE(r), line_thick, COLOR(c));
//   return draw_rectangle_lines_ex_result_ok(env);
// }

// UNIFEX_TERM draw_triangle(UnifexEnv *env, vector2 v1, vector2 v2, vector2 v3,
//                           color c) {
//   DrawTriangle(VECTOR2(v1), VECTOR2(v2), VECTOR2(v3), COLOR(c));
//   return draw_triangle_result_ok(env);
// }

// // Basic shapes collision detection functions

// UNIFEX_TERM check_collision_point_rec(UnifexEnv *env, vector2 p, rectangle r) {
//   bool res = CheckCollisionPointRec(VECTOR2(p), RECTANGLE(r));
//   return check_collision_point_rec_result(env, res);
// }

// UNIFEX_TERM get_ray_collision_box(UnifexEnv *env, ray r, bounding_box b) {
//   RayCollision result = GetRayCollisionBox(RAY(r), BOUNDING_BOX(b));
//   ray_collision res = E_RAY_COLLISION(result);
//   return get_ray_collision_box_result(env, res);
// }

/************
 * TEXTURES *
 ************/

// Image loading functions
// NOTE: This functions do not require GPU access

// Image generation functions

// Image manipulation functions

// Image drawing functions
// NOTE: Image software-rendering functions (CPU)

// Texture loading functions
// NOTE: These functions require GPU access

// Texture configuration functions

// Texture drawing functionsc

// Color/pixel related functions

/********
 * TEXT *
 ********/

// Font loading/unloading functions

// Text drawing functions

// UNIFEX_TERM draw_fps(UnifexEnv *env, int posX, int posY) {
//   DrawFPS(posX, posY);
//   return draw_fps_result_ok(env);
// }

// UNIFEX_TERM draw_text(UnifexEnv *env, char *text, int posX, int posY,
//                       int fontSize, color c) {
//   DrawText(text, posX, posY, fontSize, COLOR(c));
//   return draw_text_result_ok(env);
// }

// Text misc. functions

// Text codepoints management functions (unicode characters)

// Text strings management functions (no utf8 strings, only byte chars)
// NOTE: Some strings allocate memory internally for returned strings, just be
// careful!

/**********
 * MODELS *
 **********/

// Basic geometric 3D shapes drawing functions

// UNIFEX_TERM draw_cube(UnifexEnv *env, vector3 p, double width, double height,
//                       double length, color c) {
//   DrawCube(VECTOR3(p), width, height, length, COLOR(c));
//   return draw_cube_result_ok(env);
// }

// UNIFEX_TERM draw_cube_wires(UnifexEnv *env, vector3 p, double width,
//                             double height, double length, color c) {
//   DrawCubeWires(VECTOR3(p), width, height, length, COLOR(c));
//   return draw_cube_wires_result_ok(env);
// }

// UNIFEX_TERM draw_ray(UnifexEnv *env, ray r, color c) {
//   DrawRay(RAY(r), COLOR(c));
//   return draw_ray_result_ok(env);
// }

// UNIFEX_TERM draw_grid(UnifexEnv *env, int slices, double spacing) {
//   DrawGrid(slices, spacing);
//   return draw_grid_result_ok(env);
// }

// Model loading/unloading functions

// Model drawing functions

// Mesh management functions

// Mesh generation functions

// Material loading/unloading functions

// Model animations loading/unloading functions

// Collision detection functions

/*********
 * AUDIO *
 *********/

// Audio device management functions
UNIFEX_TERM init_audio_device(UnifexEnv *env) {
  InitAudioDevice();
  SetMasterVolume(100);
  return init_audio_device_result_ok(env);
}

UNIFEX_TERM close_audio_device(UnifexEnv *env) {
  CloseAudioDevice();
  return close_audio_device_result_ok(env);
}

UNIFEX_TERM is_audio_device_ready(UnifexEnv *env) {
  bool res = IsAudioDeviceReady();
  return is_audio_device_ready_result(env, res);
}

UNIFEX_TERM set_master_volume(UnifexEnv *env, double volume) {
  SetMasterVolume(volume);
  return set_master_volume_result_ok(env);
}

UNIFEX_TERM get_master_volume(UnifexEnv *env) {
  float res = GetMasterVolume();
  return get_master_volume_result(env, res);
}

// Wave/Sound loading/unloading functions
UNIFEX_TERM load_sound(UnifexEnv *env, char *fileName) {
  Sound newSound = LoadSound(fileName);

  sound result = E_SOUND(newSound);

  return load_sound_result(env, result);
}

// Wave/Sound management functions
UNIFEX_TERM play_sound(UnifexEnv *env, sound s) {
  // Sound newSound = SOUND(s);
  // PlaySound(newSound);
  return play_sound_result_ok(env);
}

// Music management functions

// AudioStream management functions

