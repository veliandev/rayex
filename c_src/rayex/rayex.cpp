#include "rayex.h"
#include "raylib.h"
#include <erl_nif.h>

#include <iostream>
#include <list>
#include <algorithm>
#include <array>

#ifdef __cplusplus
extern "C" {
#endif

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "miniaudio.h"

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

#ifdef __cplusplus
}
#endif

// Variables Cache
struct AudioBufferCache{
  int id;
  rAudioBuffer* buffer;
};
std::list<AudioBufferCache> audioBufferCache;

struct AudioProcessorCache{
  int id;
  rAudioProcessor* processor;
};
std::list<AudioProcessorCache> audioProcessorCache;

struct ModelCache{
  unsigned int id;
  Model model;
};
std::list<ModelCache> modelCache;

// NOTE: "E_" when converting back to NIF struct

#define VECTOR2(v) ((Vector2){.x = v.x, .y = v.y})
#define E_VECTOR2(v) ((vector2){.x = v.x, .y = v.y})

#define VECTOR3(v) ((Vector3){.x = v.x, .y = v.y, .z = v.z})
#define E_VECTOR3(v) ((vector3){.x = v.x, .y = v.y, .z = v.z})

#define VECTOR4(v) ((Vector4){.x = v.x, .y = v.y, .z = v.z, .w = v.w})
#define E_VECTOR4(v) ((vector4){.x = v.x, .y = v.y, .z = v.z, .w = v.w})

#define QUATERNION(q) ((Quaternion){.x = q.x, .y = q.y, .z = q.z, .w = q.w})
#define E_QUATERNION(q) ((quaternion){.x = q.x, .y = q.y, .z = q.z, .w = q.w})

Matrix MATRIX(matrix m){
  Matrix result;
  result.m0 = m.m0;
  result.m1 = m.m1;
  result.m2 = m.m2;
  result.m3 = m.m3;
  result.m4 = m.m4;
  result.m5 = m.m5;
  result.m6 = m.m6;
  result.m7 = m.m7;
  result.m8 = m.m8;
  result.m9 = m.m9;
  result.m10 = m.m10;
  result.m11 = m.m11;
  result.m12 = m.m12;
  result.m13 = m.m13;
  result.m14 = m.m14;
  result.m15 = m.m15;
  return result;
}
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

// Shader SHADER(shader s) {
//   Shader result;
//   result.id = s.id;
//   result.locs = s.locs;
//   return result;
// }

shader E_SHADER(Shader s) {
  shader result;
  result.id = s.id;
  result.locs = s.locs;
  return result;
}

// MaterialMap* MATERIAL_MAP(material_map* m) {
//   MaterialMap* result = new MaterialMap();
//   result->texture = TEXTURE_2D(m->texture);
//   result->color = COLOR(m->color);
//   result->value = (float)m->value;
//   return result;
// }

material_map* E_MATERIAL_MAP(MaterialMap* m) {
  material_map* result = new material_map();
  result->texture = E_TEXTURE_2D(m->texture);
  result->color = E_COLOR(m->color);
  result->value = (double)m->value;
  return result;
}

#define E_MESH(m)                           \
  ((mesh){                                \
    .vertex_count = m.vertexCount,        \
    .triangle_count = m.triangleCount,    \
    .vertices = m.vertices,               \
    .texcoords = m.texcoords,             \
    .texcoords2 = m.texcoords2,           \
    .normals = m.normals,                 \
    .tangents = m.tangents,               \
    .colors = m.colors,                   \
    .indices = m.indices,                 \
    .anim_vertices = m.animVertexData,    \
    .anim_normals = m.animNormals,        \
    .bone_ids = m.boneIds,                \
    .bone_weights = m.boneWeights,         \
    .vao_id = m.vaoId,                     \
    .vbo_id = m.vboId                     \
    })

// Mesh* MESH_POINTER(mesh* m) {
//   Mesh* result = new Mesh();
//   result->vertexCount = m->vertex_count;
//   result->triangleCount = m->triangle_count;
//   result->vertices = (float*)m->vertices;
//   result->texcoords = (float*)m->texcoords;
//   result->texcoords2 = (float*)m->texcoords2;
//   result->normals = (float*)m->normals;
//   result->tangents = (float*)m->tangents;
//   result->colors = (unsigned char*)m->colors;
//   result->indices = (short unsigned int*)m->indices;
//   result->animVertices = (float*)m->anim_vertices;
//   result->animNormals = (float*)m->anim_normals;
//   result->boneIds = (unsigned char*)m->bone_ids;
//   result->boneWeights = (float*)m->bone_weights;
//   result->vaoId = m->vao_id;
//   result->vboId = m->vbo_id;
//   return result;
// }

mesh* E_MESH_POINTER(Mesh* m) {
  mesh* result = new mesh();
  result->vertex_count = m->vertexCount;
  result->triangle_count = m->triangleCount;
  result->vertices = (double*)m->vertices;
  result->texcoords = (double*)m->texcoords;
  result->texcoords2 = (double*)m->texcoords2;
  result->normals = (double*)m->normals;
  result->tangents = (double*)m->tangents;
  result->colors = (unsigned int*)m->colors;
  result->indices = (unsigned int*)m->indices;
  result->anim_vertices = (double*)m->animVertices;
  result->anim_normals = (double*)m->animNormals;
  result->bone_ids = (unsigned int*)m->boneIds;
  result->bone_weights = (double*)m->boneWeights;
  result->vao_id = m->vaoId;
  result->vbo_id = m->vboId;
  return result;
}

material* E_MATERIAL_POINTER(Material* m) {
  material* result = new material();
  result->shader = E_SHADER(m->shader);
  result->maps = E_MATERIAL_MAP(m->maps);
  result->params = (double*)m->params;
  return result;
}

// Material* MATERIAL_POINTER(material* m) {
//   Material* result = new Material();
//   result->shader = SHADER(m->shader);
//   result->maps = MATERIAL_MAP(m->maps);
//   // std::memcpy(m, r.getFloat(), sizeof(m));
//   float newParams[4] = {m->params[0], m->params[1], m->params[2], m->params[3]};
//   std::copy(result->params, result->params + 4, newParams);
//   // result->params = newParams;
//   // result->params = m->params;
//   // result->params[1] = m->params[1];
//   // result->params[2] = m->params[2];
//   // result->params[3] = m->params[3];
//   return result;
// }

BoneInfo* BONE_INFO(bone_info* b) {
  BoneInfo* result = new BoneInfo();
  for(int i = 0; i < sizeof(b->name) / sizeof(char); i++){
    result->name[i] = b->name[i];
  }
  result->parent = b->parent;
  return result;
}

bone_info* E_BONE_INFO(BoneInfo* b, int boneCount) {
  bone_info* result = new bone_info();

  for(int i = 0; i < boneCount; i++)
  {
    result[i].name = b[i].name;
    result[i].parent = b[i].parent;
  }

  return result;
}

// BoneInfo* BONE_INFO(bone_info* b) {
//   BoneInfo* result = new BoneInfo();
//   result->name[0] = b->name[0];
//   result->name[1] = b->name[1];
//   result->name[2] = b->name[2];
//   result->name[3] = b->name[3];
//   result->parent = b->parent;
//   return result;
// }

transform_type* E_TRANSFORM_POINTER(Transform* t) {
  transform_type* result = new transform_type();
  result->translation = E_VECTOR3(t->translation);
  result->rotation = E_QUATERNION(t->rotation);
  result->scale = E_VECTOR3(t->scale);
  return result;
}

Transform* TRANSFORM_POINTER(transform_type* t) {
  Transform* result = new Transform();
  std::cout << "VECTOR3" << std::endl;
  result->translation = VECTOR3(t->translation);
  std::cout << "QUATERNION" << std::endl;
  result->rotation = QUATERNION(t->rotation);
  std::cout << "VECTOR3" << std::endl;
  result->scale = VECTOR3(t->scale);
  return result;
}

model* E_MODEL(Model m) {
  uint modelId = modelCache.size();
  modelCache.push_back((ModelCache){modelId, m});

  model* result = new model();
  result->transform = E_MATRIX(m.transform);
  std::cout << "MESH COUNT : " << m.meshCount << std::endl;
  result->mesh_count = m.meshCount;  
  result->material_count = m.materialCount; 
  result->meshes = E_MESH_POINTER(m.meshes);
  result->materials = E_MATERIAL_POINTER(m.materials);
  result->mesh_material = m.meshMaterial;
  result->bone_count = m.boneCount;

  for(int i = 0; i < m.boneCount; i++) {
    std::cout << "Bone " << i << " has values: " << m.bones[i].name << " " << m.bones[i].parent << std::endl;
  }

  result->bones = E_BONE_INFO(m.bones, m.boneCount);

  for(int i = 0; i < m.boneCount; i++) {
    std::cout << "RESULT Bone " << i << " has values: " << result->bones[i].name << " " << result->bones[i].parent << std::endl;
  }

  result->bind_pose = E_TRANSFORM_POINTER(m.bindPose);
  result->c_id = modelId;
  std::cout << "Should be solid E_MODEL" << std::endl;
  return result;
}

Model MODEL(model m) {
  // Model* result = new Model();

  auto mcm = std::find_if(modelCache.cbegin(), modelCache.cend(), [&m] (const ModelCache& mc) {
    return mc.id == m.c_id;
  });

  Model result = mcm->model;

  // std::cout << result.meshCount << std::endl;

  // std::cout << "MATRIX" << std::endl;
  // result->transform = MATRIX(m.transform);
  // result->meshCount = m.mesh_count;  
  // result->materialCount = m.material_count; 
  // std::cout << "MESH_POINTER" << std::endl;
  // result->meshes = mcm->model->meshes;
  // std::cout << "MATERIAL_POINTER" << std::endl;
  // result->materials = mcm->model->materials;
  // result->meshMaterial = m.mesh_material;
  // result->boneCount = m.bone_count;
  // result->bones = mcm->model->bones;
  // std::cout << "TRANSFORM_POINTER" << std::endl;
  // result->bindPose = modelCacheMatch->model->bindPose;

  // std::cout << "Set all the model stuff." << std::endl;
  return result;
}

ModelAnimation MODEL_ANIMATION(model_animation m){
  // model_animation* result = new model_animation();
  // for(int i = 0; i < animsCount; i++) {
  //   result[i].bone_count = m[i].boneCount;
  //   result[i].frame_count = m[i].frameCount;
  //   result[i].bones = E_BONE_INFO(m[i].bones);
  //   result[i].name = m[i].name;
  //   result[i].c_id = 0;
  //   std::cout << result[i].name << std::endl;
  //   // std::cout << "Size of E: " << sizeof(result) << "; Size of C: " << sizeof(m) << std::endl;
  // }
  // std::cout << "Should be a working E Anim" << std::endl;
  
  ModelAnimation result;
  result.boneCount = m.bone_count;
  result.frameCount = m.frame_count;
  result.bones = BONE_INFO(m.bones);

  for(int i = 0; i < sizeof(m.name) / sizeof(char); i++){
    result.name[i] = m.name[i];
  }
  //result.framePoses = something?

  std::cout << result.name << std::endl;

  return result;
}

model_animation* E_MODEL_ANIMATION(ModelAnimation* m, int animsCount){
  model_animation* result = new model_animation[animsCount];
  for(int i = 0; i < animsCount; i++) {
    result[i].bone_count = m[i].boneCount;
    result[i].frame_count = m[i].frameCount;
    result[i].bones = E_BONE_INFO(m[i].bones, m[i].boneCount);
    result[i].name = m[i].name;
  }
  std::cout << "Should be a working E Anim" << std::endl;
  
  return result;
}

// Model MODEL(model m) {
//   Model result;                      
//   result.transform = MATRIX(m.transform);
//   result.meshCount = m.mesh_count;
//   result.materialCount = m.material_count;
//   result.meshes = m.meshes;
//   result.materials = m.materials;
//   result.meshMaterial = m.meshMaterial;
//   result.boneCount = m.boneCount;
//   result.bones = m.bones;
//   result.bindPose = m.bindPose;
//   return result;
// }

// Wave WAVE(wave w)
// {
//   Wave* result = waveEntryGet(w.id);
//   return *result;
// }

// wave E_WAVE(char* id, Wave w)
// {
//   wave result;
//   int index = waveEntryInsert(id, &w);
//   result.id = id;
//   return result;
// }

AudioStream AUDIO_STREAM(audio_stream s) {
  AudioStream result;

  auto bufferCacheMatch = std::find_if(audioBufferCache.cbegin(), audioBufferCache.cend(), [&s] (const AudioBufferCache& abc) {
    return abc.id == s.buffer;
  });

  auto processorCacheMatch = std::find_if(audioProcessorCache.cbegin(), audioProcessorCache.cend(), [&s] (const AudioProcessorCache& apc) {
    return apc.id == s.processor;
  });

  result.buffer = bufferCacheMatch->buffer;
  result.processor = processorCacheMatch->processor;

  result.sampleRate = s.sample_rate;
  result.sampleSize = s.sample_size;
  result.channels = s.channels;
  return result;
}

audio_stream E_AUDIO_STREAM(AudioStream s) {
  audio_stream result;

  uint bufferId = audioBufferCache.size();
  audioBufferCache.push_back((AudioBufferCache){bufferId, s.buffer});

  uint processorId = audioProcessorCache.size();
  audioProcessorCache.push_back((AudioProcessorCache){processorId, s.processor});

  // std::cout << &result.buffer << std::endl;

  // std::cout << std::addressof(s.buffer) << std::endl;

  // std::cout << "Callback : "          << s.buffer->callback << std::endl;         // Audio buffer callback for buffer filling on audio threads
  // std::cout << "Proc : "              << s.buffer->processor << std::endl;     // Audio processor
  // std::cout << "Volume : "            << s.buffer->volume << std::endl;                   // Audio buffer volume
  // std::cout << "Pitch : "             << s.buffer->pitch << std::endl;                    // Audio buffer pitch
  // std::cout << "Pan : "               << s.buffer->pan << std::endl;                      // Audio buffer pan (0.0f to 1.0f)
  // std::cout << "Playing : "           << s.buffer->playing << std::endl;                   // Audio buffer state: AUDIO_PLAYING
  // std::cout << "Paused : "            << s.buffer->paused << std::endl;                    // Audio buffer state: AUDIO_PAUSED
  // std::cout << "Looping : "           << s.buffer->looping << std::endl;                   // Audio buffer looping, default to true for AudioStreams
  // std::cout << "Usage : "             << s.buffer->usage << std::endl;                      // Audio buffer usage mode: STATIC or STREAM
  // std::cout << "Size in frames : "    << s.buffer->sizeInFrames << std::endl;      // Total buffer size in frames
  // std::cout << "Frame cursor pos : "  << s.buffer->frameCursorPos << std::endl;    // Frame cursor position
  // std::cout << "Frames Processed : "  << s.buffer->framesProcessed << std::endl;   // Total frames processed in this buffer (required for play timing)

  result.buffer = bufferId;
  result.processor = processorId;
  result.sample_rate = s.sampleRate;
  result.sample_size = s.sampleSize;
  result.channels = s.channels;
  return result;
}

Sound SOUND(sound s) {
  Sound result;
  result.stream = AUDIO_STREAM(s.stream);
  result.frameCount = s.frame_count;
  return result;
}

sound E_SOUND(Sound s) {
  sound result;
  result.stream = E_AUDIO_STREAM(s.stream);
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

// Camera* CAMERA(camera* c) {
//   Camera* result = (Camera*)malloc(sizeof(Camera));
//   result->position = VECTOR3(c->position);
//   result->target = VECTOR3(c->target);
//   result->up = VECTOR3(c->up);
//   result->fovy = c->fovy;
//   result->projection = c->projection;
//   return result;
// }

#define CAMERA(c)                                                            \
  ((Camera){.position = VECTOR3(c.position),                                 \
              .target = VECTOR3(c.target),                                     \
              .up = VECTOR3(c.up),                                             \
              .fovy = c.fovy,                                                  \
              .projection = c.projection})
#define E_CAMERA(c)                                                          \
  ((camera){.position = E_VECTOR3(c.position),                              \
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

UNIFEX_TERM clear_background(UnifexEnv *env, color c) {
  ClearBackground(COLOR(c));
  return clear_background_result_ok(env);
}

UNIFEX_TERM begin_drawing(UnifexEnv *env) {
  BeginDrawing();
  return begin_drawing_result_ok(env);
}

UNIFEX_TERM end_drawing(UnifexEnv *env) {
  EndDrawing();
  return end_drawing_result_ok(env);
}

UNIFEX_TERM begin_mode_2d(UnifexEnv *env, camera_2d c) {
  BeginMode2D(CAMERA2D(c));
  return begin_mode_2d_result_ok(env);
}

UNIFEX_TERM end_mode_2d(UnifexEnv *env) {
  EndMode2D();
  return end_mode_2d_result_ok(env);
}

UNIFEX_TERM begin_mode_3d(UnifexEnv *env, camera c) {
  BeginMode3D(CAMERA(c));
  return begin_mode_3d_result_ok(env);
}

UNIFEX_TERM end_mode_3d(UnifexEnv *env) {
  EndMode3D();
  return end_mode_3d_result_ok(env);
}

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

UNIFEX_TERM set_target_fps(UnifexEnv *env, int fps) {
  SetTargetFPS(fps);
  return set_target_fps_result_ok(env);
}

UNIFEX_TERM get_fps(UnifexEnv *env) {
  int res = GetFPS();
  return get_fps_result(env, res);
}

UNIFEX_TERM get_frame_time(UnifexEnv *env) {
  float res = GetFrameTime();
  return get_frame_time_result(env, res);
}

UNIFEX_TERM get_time(UnifexEnv *env) {
  double res = GetTime();
  return get_time_result(env, res);
}

// Misc. functions

// Set custom callbacks
// WARNING: Callbacks setup is intended for advance users

// Files management functions

// Compression/Encoding functionality

// Persistent storage management

// Misc.

// Input-related functions: keyboard
UNIFEX_TERM is_key_pressed(UnifexEnv *env, int key) {
  bool res = IsKeyPressed(key);
  return is_key_pressed_result(env, res);
}

UNIFEX_TERM is_key_pressed_repeat(UnifexEnv *env, int key) {
  bool res = IsKeyPressedRepeat(key);
  return is_key_pressed_repeat_result(env, res);
}

UNIFEX_TERM is_key_down(UnifexEnv *env, int key) {
  bool res = IsKeyDown(key);
  return is_key_down_result(env, res);
}

UNIFEX_TERM is_key_released(UnifexEnv *env, int key) {
  bool res = IsKeyReleased(key);
  return is_key_released_result(env, res);
}

UNIFEX_TERM is_key_up(UnifexEnv *env, int key) {
  bool res = IsKeyUp(key);
  return is_key_up_result(env, res);
}

UNIFEX_TERM get_key_pressed(UnifexEnv *env) {
  int res = GetKeyPressed();
  return get_key_pressed_result(env, res);
}

UNIFEX_TERM get_char_pressed(UnifexEnv *env) {
  int res = GetCharPressed();
  return get_char_pressed_result(env, res);
}

UNIFEX_TERM set_exit_key(UnifexEnv *env, int key) {
  SetExitKey(key);
  return set_exit_key_result_ok(env);
}

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
// UNIFEX_TERM update_camera(UnifexEnv *env, camera* c, int mode) {
//   Camera* camera = CAMERA(c);
//   UpdateCamera(camera, mode);
//   return update_camera_result_ok(env);
// }

// UNIFEX_TERM update_camera_pro(UnifexEnv *env, camera* c, vector3 movement, vector3 rotation, double zoom) {
//   Camera* camera = CAMERA(c);
//   UpdateCameraPro(camera, VECTOR3(movement), VECTOR3(rotation), zoom);
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

UNIFEX_TERM draw_grid(UnifexEnv *env, int slices, double spacing) {
  DrawGrid(slices, spacing);
  return draw_grid_result_ok(env);
}

UNIFEX_TERM load_model(UnifexEnv *env, char *fileName) {
  return load_model_result(env, *E_MODEL(LoadModel(fileName)));
}

UNIFEX_TERM is_model_ready(UnifexEnv *env, model m) {
  // std::cout << "Is model ready called" << std::endl;
  bool res = IsModelReady(MODEL(m));
  // std::cout << "Is model ready passed with " << res << std::endl;
  return is_model_ready_result(env, res);
}

UNIFEX_TERM draw_model(UnifexEnv *env, model m, vector3 pos, double scale, color tint) {
  Model cModel = MODEL(m);
  DrawModel(cModel, VECTOR3(pos), (float)scale, COLOR(tint));
  return draw_model_result_ok(env);
}

UNIFEX_TERM load_model_animations(UnifexEnv *env, char *file_name) {
  int animsCount = 0;
  std::cout << "Loading model animations" << std::endl;
  ModelAnimation* anims = LoadModelAnimations(file_name, &animsCount);
  std::cout << "Loaded " << animsCount << " animations" << std::endl;
  model_animation* result = E_MODEL_ANIMATION(anims, animsCount);

  for(int i = 0; i < animsCount; i++){
    std::cout << "Anim " << i << " bone count " << result[i].bone_count << std::endl;
    std::cout << "Anim " << i << " frame count " << result[i].frame_count << std::endl;
    
    for(int j = 0; j < result[i].bone_count; j++){
      std::cout << "Anim " << i << " bone " << j << " name " << result[i].bones[j].name << std::endl;
      std::cout << "Anim " << i << " bone " << j << " parent " << result[i].bones[j].parent << std::endl;
    }

    std::cout << "Anim " << i << " name " << result[i].name << std::endl;
  }

  std::cout << "Returning " << animsCount << " animations" << std::endl;
  std::cout << "I think the amount of bytes is " << sizeof(result) / sizeof(model_animation) << std::endl;

  return load_model_animations_result(env, result, sizeof(result) / sizeof(model_animation));
}

UNIFEX_TERM update_model_animation(UnifexEnv *env, model model, model_animation anim, int frame) {
  UpdateModelAnimation(MODEL(model), MODEL_ANIMATION(anim), frame);
  return update_model_animation_result_ok(env);
}

/*********
 * AUDIO *
 *********/

// Audio device management functions
UNIFEX_TERM init_audio_device(UnifexEnv *env) {
  InitAudioDevice();
  
  while(!IsAudioDeviceReady()){
    // Waiting
  }
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
  SetMasterVolume((float)volume);
  return set_master_volume_result_ok(env);
}

UNIFEX_TERM get_master_volume(UnifexEnv *env) {
  float res = GetMasterVolume();
  return get_master_volume_result(env, res);
}

// Wave/Sound loading/unloading functions

// UNIFEX_TERM load_wave(UnifexEnv *env, char *fileName, char *atom_id) {
//   wave result = E_WAVE(atom_id, LoadWave(fileName));
//   return load_wave_result(env, result);
// }

UNIFEX_TERM load_sound(UnifexEnv *env, char *fileName) {
  sound res = E_SOUND(LoadSound(fileName));
  return load_sound_result(env, res);
}

// Wave/Sound management functions

// UNIFEX_TERM play_wave(UnifexEnv *env, wave w) {
//   Wave result = WAVE(w);
//   PlayWave(result);
//   return play_wave_result_ok(env);
// }

UNIFEX_TERM is_sound_ready(UnifexEnv *env, sound s) {
  return is_sound_ready_result(env, IsSoundReady(SOUND(s)));
}

UNIFEX_TERM play_sound(UnifexEnv *env, sound s) {
  Sound result = SOUND(s);
  PlaySound(result);
  return play_sound_result_ok(env);
}

// Music management functions

// AudioStream management functions


// Ray GUI
UNIFEX_TERM gui_enable(UnifexEnv *env) {
  GuiEnable();
  return gui_enable_result_ok(env);
}

UNIFEX_TERM gui_disable(UnifexEnv *env) {
  GuiDisable();
  return gui_disable_result_ok(env);
}

UNIFEX_TERM gui_lock(UnifexEnv *env) {
  GuiLock();
  return gui_lock_result_ok(env);
}

UNIFEX_TERM gui_unlock(UnifexEnv *env) {
  GuiUnlock();
  return gui_unlock_result_ok(env);
}

UNIFEX_TERM gui_is_locked(UnifexEnv *env) {
  bool res = GuiIsLocked();
  return gui_is_locked_result(env, res);
}

UNIFEX_TERM gui_set_alpha(UnifexEnv *env, double alpha) {
  GuiSetAlpha((float)alpha);
  return gui_set_alpha_result_ok(env);
}

UNIFEX_TERM gui_set_state(UnifexEnv *env, int state) {
  GuiSetState(state);
  return gui_set_state_result_ok(env);
}

UNIFEX_TERM gui_get_state(UnifexEnv *env) {
  int res = GuiGetState();
  return gui_get_state_result(env, res);
}

UNIFEX_TERM gui_load_style_default(UnifexEnv *env) {
  GuiLoadStyleDefault();
  return gui_load_style_default_result_ok(env);
}

UNIFEX_TERM gui_window_box(UnifexEnv *env, rectangle bounds, char *text) {
  int res = GuiWindowBox(RECTANGLE(bounds), text);
  return gui_window_box_result(env, res);
}

UNIFEX_TERM gui_line(UnifexEnv *env, rectangle bounds, char *text) {
  int res = GuiLine(RECTANGLE(bounds), text);
  return gui_line_result(env, res);
}

UNIFEX_TERM gui_panel(UnifexEnv *env, rectangle bounds, char *text) {
  int res = GuiPanel(RECTANGLE(bounds), text);
  return gui_panel_result(env, res);
}

UNIFEX_TERM gui_tab_bar(UnifexEnv *env, rectangle bounds, char **text, unsigned int text_length, int count, int active) {
  int res = GuiTabBar(RECTANGLE(bounds), text, count, &active);
  return gui_tab_bar_result(env, res, active);
}

UNIFEX_TERM gui_group_box(UnifexEnv *env, rectangle bounds, char *text) {
  int res = GuiGroupBox(RECTANGLE(bounds), text);
  return gui_group_box_result(env, res);
}

UNIFEX_TERM gui_label(UnifexEnv *env, rectangle bounds, char *text) {
  int res = GuiLabel(RECTANGLE(bounds), text);
  return gui_label_result(env, res);
}

UNIFEX_TERM gui_button(UnifexEnv *env, rectangle bounds, char *text) {
  bool res = GuiButton(RECTANGLE(bounds), text);
  return gui_button_result(env, res);
}

UNIFEX_TERM gui_label_button(UnifexEnv *env, rectangle bounds, char *text) {
  bool res = (GuiLabelButton(RECTANGLE(bounds), text) == 1);
  return gui_label_button_result(env, res);
}

UNIFEX_TERM gui_toggle(UnifexEnv *env, rectangle bounds, char *text, int active) {
  bool active_boolean = active == 1 ? true : false;
  int res = GuiToggle(RECTANGLE(bounds), text, &active_boolean);
  return gui_toggle_result(env, res, active_boolean);
}

// UNIFEX_TERM gui_toggle_test(UnifexEnv *env, rectangle bounds, char *text, bool* active) {
//   Rectangle rect = RECTANGLE(bounds);
//   int res = GuiToggle(rect, text, active);
//   return gui_toggle_test_result(env, res);
// }

UNIFEX_TERM gui_slider(UnifexEnv *env, rectangle bounds, char *textLeft, char *textRight, double value, double minValue, double maxValue) {
  float valAsFloat = (float)value;
  float *valAsFloatPtr = &valAsFloat;
  int res = GuiSlider(RECTANGLE(bounds), textLeft, textRight, valAsFloatPtr, (float)minValue, (float)maxValue);
  return gui_slider_result(env, res, *valAsFloatPtr);
}

UNIFEX_TERM gui_slider_bar(UnifexEnv *env, rectangle bounds, char *textLeft, char *textRight, double value, double minValue, double maxValue) {
  float valAsFloat = (float)value;
  float *valAsFloatPtr = &valAsFloat;
  int res = GuiSlider(RECTANGLE(bounds), textLeft, textRight, valAsFloatPtr, (float)minValue, (float)maxValue);
  return gui_slider_bar_result(env, res, *valAsFloatPtr);
}

UNIFEX_TERM gui_progress_bar(UnifexEnv *env, rectangle bounds, char *textLeft, char *textRight, double value, double minValue, double maxValue) {
  float valAsFloat = (float)value;
  float *valAsFloatPtr = &valAsFloat;
  int res = GuiProgressBar(RECTANGLE(bounds), textLeft, textRight, valAsFloatPtr, (float)minValue, (float)maxValue);
  return gui_progress_bar_result(env, res, *valAsFloatPtr);
}

UNIFEX_TERM gui_status_bar(UnifexEnv *env, rectangle bounds, char *text) {
  int res = GuiStatusBar(RECTANGLE(bounds), text);
  return gui_status_bar_result(env, res);
}

UNIFEX_TERM gui_message_box(UnifexEnv *env, rectangle bounds, char *title, char *message, char *buttons) {
  int res = GuiMessageBox(RECTANGLE(bounds), title, message, buttons);
  return gui_message_box_result(env, res);
}