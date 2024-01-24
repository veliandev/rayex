# Follows this refsheet:
# https://www.raylib.com/cheatsheet/cheatsheet.html

module Rayex.Unifex.Raylib

interface [NIF, CNode]

########
# CORE #
########

# Window-related functions
spec init_window(width :: int, height :: int, title :: string) :: :ok :: label
dirty :cpu, init_window: 3

spec window_should_close() :: result :: bool
dirty :cpu, window_should_close: 0

spec close_window() :: :ok :: label
dirty :cpu, close_window: 0

spec is_window_ready() :: result :: bool
dirty :cpu, is_window_ready: 0

spec is_window_fullscreen() :: result :: bool
dirty :cpu, is_window_fullscreen: 0

spec is_window_hidden() :: result :: bool
dirty :cpu, is_window_hidden: 0

spec is_window_minimized() :: result :: bool
dirty :cpu, is_window_minimized: 0

spec is_window_maximized() :: result :: bool
dirty :cpu, is_window_maximized: 0

spec is_window_focused() :: result :: bool
dirty :cpu, is_window_focused: 0

spec is_window_resized() :: result :: bool
dirty :cpu, is_window_resized: 0

spec is_window_state(flag :: int) :: result :: bool
dirty :cpu, is_window_state: 1

spec set_window_state(flag :: int) :: :ok :: label
dirty :cpu, set_window_state: 1

spec clear_window_state(flag :: int) :: :ok :: label
dirty :cpu, clear_window_state: 1

spec toggle_fullscreen() :: :ok :: label
dirty :cpu, toggle_fullscreen: 0

# Cursor-related functions

# Drawing-related functions
spec clear_background(color :: color) :: :ok :: label
dirty :cpu, clear_background: 1

spec begin_drawing() :: :ok :: label
dirty :cpu, begin_drawing: 0

spec end_drawing() :: :ok :: label
dirty :cpu, end_drawing: 0

spec begin_mode_2d(camera_2d :: camera_2d) :: :ok :: label
dirty :cpu, begin_mode_2d: 1

spec end_mode_2d() :: :ok :: label
dirty :cpu, end_mode_2d: 0

spec begin_mode_3d(camera :: camera) :: :ok :: label
dirty :cpu, begin_mode_3d: 1

spec end_mode_3d() :: :ok :: label
dirty :cpu, end_mode_3d: 0

# VR stereo config functions for VR simulator

# Shader management functions
# NOTE: Shader functionality is not available on OpenGL 1.1

# Screen-space-related functions
# spec get_mouse_ray(mouse_position :: vector2, camera :: camera) :: ray :: ray

# Timing-related functions
spec set_target_fps(fps :: int) :: :ok :: label
spec get_fps() :: fps :: int
spec get_frame_time() :: delta :: float
spec get_time() :: time_from_start :: float

# Misc. functions

# Set custom callbacks
# WARNING: Callbacks setup is intended for advance users

# Files management functions

# Compression/Encoding functionality

# Persistent storage management

# Misc.

# Input-related functions: keyboard
spec is_key_pressed(key :: int) :: result :: bool
dirty :cpu, is_key_pressed: 1

spec is_key_pressed_repeat(key :: int) :: result :: bool
dirty :cpu, is_key_pressed_repeat: 1

spec is_key_down(key :: int) :: result :: bool
dirty :cpu, is_key_down: 1

spec is_key_released(key :: int) :: result :: bool
dirty :cpu, is_key_released: 1

spec is_key_up(key :: int) :: result :: bool
dirty :cpu, is_key_up: 1

spec get_key_pressed() :: key :: int
dirty :cpu, get_key_pressed: 0

spec get_char_pressed() :: character :: int
dirty :cpu, get_char_pressed: 0

spec set_exit_key(key :: int) :: :ok :: label
dirty :cpu, set_exit_key: 1

# Input-related functions: gamepads

# Input-related functions: mouse
# spec is_mouse_button_pressed(button :: int) :: result :: bool
# spec is_mouse_button_down(button :: int) :: result :: bool
# spec is_mouse_button_released(button :: int) :: result :: bool
# spec is_mouse_button_up(button :: int) :: result :: bool
# spec get_mouse_position() :: xy :: vector2

# Input-related functions: touch

# Gestures and Touch Handling Functions (Module: rgestures)

# Camera System Functions (Module: rcamera)
# spec update_camera(camera :: camera, mode :: int) :: :ok :: label
# spec update_camera_pro(camera :: camera, movement :: vector3, rotation :: vector3, zoom :: float) :: :ok :: label

# spec set_camera_pan_control(key_pan :: int) :: :ok :: label
# spec set_camera_alt_control(key_alt :: int) :: :ok :: label
# spec set_camera_smooth_zoom_control(key_smooth_zoom :: int) :: :ok :: label

# spec set_camera_move_controls(
#        key_front :: int,
#        key_back :: int,
#        key_right :: int,
#        key_left :: int,
#        key_up :: int,
#        key_down :: int
#      ) :: :ok :: label

##########
# SHAPES #
##########

# Set texture and rectangle to be used on shapes drawing
# NOTE: It can be useful when using basic shapes and one single font,
# defining a font char white rectangle would allow drawing everything in a single draw call
# spec set_shapes_texture(texture :: texture_2d, source :: rectangle) :: :ok :: label

# Basic shapes drawing functions
# spec draw_pixel(x :: int, y :: int, color :: color) :: :ok :: label

# spec draw_line(start_x :: int, start_y :: int, end_x :: int, end_y :: int, color :: color) ::
#        :ok :: label

# spec draw_rectangle_rec(rec :: rectangle, color :: color) :: :ok :: label
# spec draw_rectangle_lines_ex(rec :: rectangle, line_thick :: int, color :: color) :: :ok :: label
# spec draw_triangle(v1 :: vector2, v2 :: vector2, v3 :: vector2, color :: color) :: :ok :: label

# Basic shapes collision detection functions
# spec check_collision_point_rec(point :: vector2, rec :: rectangle) :: result :: bool
# spec get_ray_collision_box(ray :: ray, box :: bounding_box) :: ray_collision :: ray_collision

############
# TEXTURES #
############

# Image loading functions
# NOTE: This functions do not require GPU access

# Image generation functions

# Image manipulation functions

# Image drawing functions
# NOTE: Image software-rendering functions (CPU)

# Texture loading functions
# NOTE: These functions require GPU access

# Texture configuration functions

# Texture drawing functionsc

# Color/pixel related functions

########
# TEXT #
########

# Font loading/unloading functions

# Text drawing functions
# spec draw_fps(pos_x :: int, pos_y :: int) :: :ok :: label

# spec draw_text(text :: string, pos_x :: int, pos_y :: int, font_size :: int, color :: color) ::
#        :ok :: label

# Text misc. functions

# Text codepoints management functions (unicode characters)

# Text strings management functions (no utf8 strings, only byte chars)
# NOTE: Some strings allocate memory internally for returned strings, just be careful!

##########
# MODELS #
##########

# Basic geometric 3D shapes drawing functions
# spec draw_cube(
#        position :: vector3,
#        width :: float,
#        height :: float,
#        length :: float,
#        color :: color
#      ) :: :ok :: label

# spec draw_cube_wires(
#        position :: vector3,
#        width :: float,
#        height :: float,
#        length :: float,
#        color :: color
#      ) :: :ok :: label

# spec draw_ray(ray :: ray, color :: color) :: :ok :: label
spec draw_grid(slices :: int, spacing :: float) :: :ok :: label
dirty :cpu, draw_grid: 2

# // Model management functions
# Model LoadModel(const char *fileName);                                                // Load model from files (meshes and materials)
spec load_model(filename :: string) :: model :: model
dirty :cpu, load_model: 1

spec draw_model(model :: model, position :: vector3, scale :: float, tint :: color) :: :ok :: label
dirty :cpu, draw_model: 4

spec is_model_ready(model) :: result :: bool

spec load_model_animations(filename :: string) :: result :: [model_animation]
dirty :io, load_model_animations: 1

# void UpdateModelAnimation(Model model, ModelAnimation anim, int frame);               // Update model animation pose
spec update_model_animation(model :: model, anim :: model_animation, frame :: int) :: :ok :: label
dirty :cpu, update_model_animation: 3

# Model LoadModelFromMesh(Mesh mesh);                                                   // Load model from generated mesh (default material)
# bool IsModelReady(Model model);                                                       // Check if a model is ready
# void UnloadModel(Model model);                                                        // Unload model (including meshes) from memory (RAM and/or VRAM)
# BoundingBox GetModelBoundingBox(Model model);                                         // Compute model bounding box limits (considers all meshes)

# // Model drawing functions
# void DrawModel(Model model, Vector3 position, float scale, Color tint);               // Draw a model (with texture if set)
# void DrawModelEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint); // Draw a model with extended parameters
# void DrawModelWires(Model model, Vector3 position, float scale, Color tint);          // Draw a model wires (with texture if set)
# void DrawModelWiresEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint); // Draw a model wires (with texture if set) with extended parameters
# void DrawBoundingBox(BoundingBox box, Color color);                                   // Draw bounding box (wires)
# void DrawBillboard(Camera camera, Texture2D texture, Vector3 position, float size, Color tint);   // Draw a billboard texture
# void DrawBillboardRec(Camera camera, Texture2D texture, Rectangle source, Vector3 position, Vector2 size, Color tint); // Draw a billboard texture defined by source
# void DrawBillboardPro(Camera camera, Texture2D texture, Rectangle source, Vector3 position, Vector3 up, Vector2 size, Vector2 origin, float rotation, Color tint); // Draw a billboard texture defined by source and rotation

# // Mesh management functions
# void UploadMesh(Mesh *mesh, bool dynamic);                                            // Upload mesh vertex data in GPU and provide VAO/VBO ids
# void UpdateMeshBuffer(Mesh mesh, int index, const void *data, int dataSize, int offset); // Update mesh vertex data in GPU for a specific buffer index
# void UnloadMesh(Mesh mesh);                                                           // Unload mesh data from CPU and GPU
# void DrawMesh(Mesh mesh, Material material, Matrix transform);                        // Draw a 3d mesh with material and transform
# void DrawMeshInstanced(Mesh mesh, Material material, const Matrix *transforms, int instances); // Draw multiple mesh instances with material and different transforms
# bool ExportMesh(Mesh mesh, const char *fileName);                                     // Export mesh data to file, returns true on success
# BoundingBox GetMeshBoundingBox(Mesh mesh);                                            // Compute mesh bounding box limits
# void GenMeshTangents(Mesh *mesh);                                                     // Compute mesh tangents

# // Mesh generation functions
# Mesh GenMeshPoly(int sides, float radius);                                            // Generate polygonal mesh
# Mesh GenMeshPlane(float width, float length, int resX, int resZ);                     // Generate plane mesh (with subdivisions)
# Mesh GenMeshCube(float width, float height, float length);                            // Generate cuboid mesh
# Mesh GenMeshSphere(float radius, int rings, int slices);                              // Generate sphere mesh (standard sphere)
# Mesh GenMeshHemiSphere(float radius, int rings, int slices);                          // Generate half-sphere mesh (no bottom cap)
# Mesh GenMeshCylinder(float radius, float height, int slices);                         // Generate cylinder mesh
# Mesh GenMeshCone(float radius, float height, int slices);                             // Generate cone/pyramid mesh
# Mesh GenMeshTorus(float radius, float size, int radSeg, int sides);                   // Generate torus mesh
# Mesh GenMeshKnot(float radius, float size, int radSeg, int sides);                    // Generate trefoil knot mesh
# Mesh GenMeshHeightmap(Image heightmap, Vector3 size);                                 // Generate heightmap mesh from image data
# Mesh GenMeshCubicmap(Image cubicmap, Vector3 cubeSize);                               // Generate cubes-based map mesh from image data

# // Material loading/unloading functions
# Material *LoadMaterials(const char *fileName, int *materialCount);                    // Load materials from model file
# Material LoadMaterialDefault(void);                                                   // Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps)
# bool IsMaterialReady(Material material);                                              // Check if a material is ready
# void UnloadMaterial(Material material);                                               // Unload material from GPU memory (VRAM)
# void SetMaterialTexture(Material *material, int mapType, Texture2D texture);          // Set texture for a material map type (MATERIAL_MAP_DIFFUSE, MATERIAL_MAP_SPECULAR...)
# void SetModelMeshMaterial(Model *model, int meshId, int materialId);                  // Set material for a mesh

# // Model animations loading/unloading functions
# ModelAnimation *LoadModelAnimations(const char *fileName, int *animCount);            // Load model animations from file
# void UpdateModelAnimation(Model model, ModelAnimation anim, int frame);               // Update model animation pose
# void UnloadModelAnimation(ModelAnimation anim);                                       // Unload animation data
# void UnloadModelAnimations(ModelAnimation *animations, int animCount);                // Unload animation array data
# bool IsModelAnimationValid(Model model, ModelAnimation anim);                         // Check model animation skeleton match

# // Collision detection functions
# bool CheckCollisionSpheres(Vector3 center1, float radius1, Vector3 center2, float radius2);   // Check collision between two spheres
# bool CheckCollisionBoxes(BoundingBox box1, BoundingBox box2);                                 // Check collision between two bounding boxes
# bool CheckCollisionBoxSphere(BoundingBox box, Vector3 center, float radius);                  // Check collision between box and sphere
# RayCollision GetRayCollisionSphere(Ray ray, Vector3 center, float radius);                    // Get collision info between ray and sphere
# RayCollision GetRayCollisionBox(Ray ray, BoundingBox box);                                    // Get collision info between ray and box
# RayCollision GetRayCollisionMesh(Ray ray, Mesh mesh, Matrix transform);                       // Get collision info between ray and mesh
# RayCollision GetRayCollisionTriangle(Ray ray, Vector3 p1, Vector3 p2, Vector3 p3);            // Get collision info between ray and triangle
# RayCollision GetRayCollisionQuad(Ray ray, Vector3 p1, Vector3 p2, Vector3 p3, Vector3 p4);    // Get collision info between ray and quad

#########
# AUDIO #
#########

# Audio device management functions
spec init_audio_device() :: :ok :: label
spec close_audio_device() :: :ok :: label
spec is_audio_device_ready() :: result :: bool
spec get_master_volume() :: volume :: float
spec set_master_volume(volume :: float) :: :ok :: label

# Wave/Sound loading/unloading functions
# spec load_wave(file_name :: string, atom_id :: string) :: wave :: wave
spec load_sound(file_name :: string) :: sound :: sound
dirty :io, load_sound: 1

# Wave/Sound management functions
# spec play_wave(wave :: wave) :: :ok :: label
spec play_sound(sound :: sound) :: :ok :: label
spec is_sound_ready(sound :: sound) :: result :: bool

# Music management functions

# AudioStream management functions

# Raygui functions
# Global gui state control functions
spec gui_enable() :: :ok :: label
spec gui_disable() :: :ok :: label
spec gui_lock() :: :ok :: label
spec gui_unlock() :: :ok :: label
spec gui_is_locked() :: result :: bool
spec gui_set_alpha(alpha :: float) :: :ok :: label
spec gui_set_state(state :: int) :: :ok :: label
spec gui_get_state() :: result :: int

# # Font set/get functions
# RAYGUIAPI void GuiSetFont(Font font);                           // Set gui custom font (global state)
# RAYGUIAPI Font GuiGetFont(void);                                // Get gui custom font (global state)

# # Style set/get functions
# RAYGUIAPI void GuiSetStyle(int control, int property, int value); // Set one style property
# RAYGUIAPI int GuiGetStyle(int control, int property);           // Get one style property

# # Styles loading functions
# RAYGUIAPI void GuiLoadStyle(const char *fileName);              // Load style file over global style variable (.rgs)
spec gui_load_style_default() :: :ok :: label

# # Tooltips management functions
# RAYGUIAPI void GuiEnableTooltip(void);                          // Enable gui tooltips (global state)
# RAYGUIAPI void GuiDisableTooltip(void);                         // Disable gui tooltips (global state)
# RAYGUIAPI void GuiSetTooltip(const char *tooltip);              // Set tooltip string

# # Icons functionality
# RAYGUIAPI const char *GuiIconText(int iconId, const char *text); // Get text with icon id prepended (if supported)
# # #if !defined(RAYGUI_NO_ICONS)
# # RAYGUIAPI void GuiSetIconScale(int scale);                      // Set default icon drawing size
# # RAYGUIAPI unsigned int *GuiGetIcons(void);                      // Get raygui icons data pointer
# # RAYGUIAPI char **GuiLoadIcons(const char *fileName, bool loadIconsName); // Load raygui icons file (.rgi) into internal icons data
# # RAYGUIAPI void GuiDrawIcon(int iconId, int posX, int posY, int pixelSize, Color color); // Draw icon using pixel size at specified position
# # #endif


# # Controls
# #----------------------------------------------------------------------------------------------------------
# # Container/separator controls, useful for controls organization
spec gui_window_box(bounds :: rectangle, title :: string) :: result :: int
spec gui_group_box(bounds :: rectangle, title :: string) :: result :: int
spec gui_line(bounds :: rectangle, text :: string) :: result :: int
spec gui_panel(bounds :: rectangle, text :: string) :: result :: int
# RAYGUIAPI int GuiTabBar(Rectangle bounds, const char **text, int count, int *active);                  // Tab Bar control, returns TAB to be closed or -1
spec gui_tab_bar(bounds :: rectangle, text :: [string], count :: int, active :: int) :: {exit_pressed :: int, tab_pressed :: int}
# RAYGUIAPI int GuiScrollPanel(Rectangle bounds, const char *text, Rectangle content, Vector2 *scroll, Rectangle *view); // Scroll Panel control

# # Basic controls set
spec gui_label(bounds :: rectangle, text :: string) :: result :: int
spec gui_button(bounds :: rectangle, text :: string) :: result :: bool
spec gui_label_button(bounds :: rectangle, text :: string) :: result :: bool
spec gui_toggle(bounds :: rectangle, text :: string, active :: bool) :: {result :: int, active :: bool}
# RAYGUIAPI int GuiToggleGroup(Rectangle bounds, const char *text, int *active);                         // Toggle Group control, returns active toggle index

# RAYGUIAPI int GuiToggleSlider(Rectangle bounds, const char *text, int *active);                        // Toggle Slider control, returns true when clicked
# RAYGUIAPI int GuiCheckBox(Rectangle bounds, const char *text, bool *checked);                          // Check Box control, returns true when active
# RAYGUIAPI int GuiComboBox(Rectangle bounds, const char *text, int *active);                            // Combo Box control, returns selected item index

# RAYGUIAPI int GuiDropdownBox(Rectangle bounds, const char *text, int *active, bool editMode);          // Dropdown Box control, returns selected item
# RAYGUIAPI int GuiSpinner(Rectangle bounds, const char *text, int *value, int minValue, int maxValue, bool editMode); // Spinner control, returns selected value
# RAYGUIAPI int GuiValueBox(Rectangle bounds, const char *text, int *value, int minValue, int maxValue, bool editMode); // Value Box control, updates input text with numbers
# RAYGUIAPI int GuiTextBox(Rectangle bounds, char *text, int textSize, bool editMode);                   // Text Box control, updates input text

# RAYGUIAPI int GuiSlider(Rectangle bounds, const char *textLeft, const char *textRight, float *value, float minValue, float maxValue); // Slider control, returns selected value
spec gui_slider(bounds :: rectangle, text_left :: string, text_right :: string, value :: float, min_value :: float, max_value :: float) :: {result :: int, value :: float}
spec gui_slider_bar(bounds :: rectangle, text_left :: string, text_right :: string, value :: float, min_value :: float, max_value :: float) :: {result :: int, value :: float}
# RAYGUIAPI int GuiProgressBar(Rectangle bounds, const char *textLeft, const char *textRight, float *value, float minValue, float maxValue); // Progress Bar control, shows current progress value
spec gui_progress_bar(bounds :: rectangle, text_left :: string, text_right :: string, value :: float, min_value :: float, max_value :: float) :: {result :: int, value :: float}
# RAYGUIAPI int GuiStatusBar(Rectangle bounds, const char *text);                                        // Status Bar control, shows info text
spec gui_status_bar(bounds :: rectangle, text :: string) :: result :: int
# RAYGUIAPI int GuiDummyRec(Rectangle bounds, const char *text);                                         // Dummy control for placeholders
# RAYGUIAPI int GuiGrid(Rectangle bounds, const char *text, float spacing, int subdivs, Vector2 *mouseCell); // Grid control, returns mouse cell position

# # Advance controls set
# RAYGUIAPI int GuiListView(Rectangle bounds, const char *text, int *scrollIndex, int *active);          // List View control, returns selected list item index
# RAYGUIAPI int GuiListViewEx(Rectangle bounds, const char **text, int count, int *scrollIndex, int *active, int *focus); // List View with extended parameters
# RAYGUIAPI int GuiMessageBox(Rectangle bounds, const char *title, const char *message, const char *buttons); // Message Box control, displays a message
spec gui_message_box(bounds :: rectangle, title :: string, message :: string, buttons :: string) :: result :: int
# RAYGUIAPI int GuiTextInputBox(Rectangle bounds, const char *title, const char *message, const char *buttons, char *text, int textMaxSize, bool *secretViewActive); // Text Input Box control, ask for text, supports secret
# RAYGUIAPI int GuiColorPicker(Rectangle bounds, const char *text, Color *color);                        // Color Picker control (multiple color controls)
# RAYGUIAPI int GuiColorPanel(Rectangle bounds, const char *text, Color *color);                         // Color Panel control
# RAYGUIAPI int GuiColorBarAlpha(Rectangle bounds, const char *text, float *alpha);                      // Color Bar Alpha control
# RAYGUIAPI int GuiColorBarHue(Rectangle bounds, const char *text, float *value);                        // Color Bar Hue control
# RAYGUIAPI int GuiColorPickerHSV(Rectangle bounds, const char *text, Vector3 *colorHsv);                // Color Picker control that avoids conversion to RGB on each call (multiple color controls)
# RAYGUIAPI int GuiColorPanelHSV(Rectangle bounds, const char *text, Vector3 *colorHsv);                 // Color Panel control that returns HSV color value, used by GuiColorPickerHSV()


###########
# STRUCTS #
###########
# https://github.com/raysan5/raylib/blob/master/src/raylib.h

type string_array :: %Rayex.Structs.StringArray{
  strings: [string]
}

type vector2 :: %Rayex.Structs.Vector2{
       x: float,
       y: float
     }

type vector3 :: %Rayex.Structs.Vector3{
       x: float,
       y: float,
       z: float
     }

type vector4 :: %Rayex.Structs.Vector4{
       x: float,
       y: float,
       z: float,
       w: float
     }

# same as vector4
type quaternion :: %Rayex.Structs.Quaternion{
       x: float,
       y: float,
       z: float,
       w: float
     }

type matrix :: %Rayex.Structs.Matrix{
       m0: float,
       m1: float,
       m2: float,
       m3: float,
       #
       m4: float,
       m5: float,
       m6: float,
       m7: float,
       #
       m8: float,
       m9: float,
       m10: float,
       m11: float,
       #
       m12: float,
       m13: float,
       m14: float,
       m15: float
     }

type color :: %Rayex.Structs.Color{
       r: unsigned,
       g: unsigned,
       b: unsigned,
       a: unsigned
     }

type rectangle :: %Rayex.Structs.Rectangle{
       x: float,
       y: float,
       width: float,
       height: float
     }

type image :: %Rayex.Structs.Image{
       # XXX: verify if payload works as a void*
       # take a look at https://hexdocs.pm/unifex/Unifex.Specs.DSL.html#spec/1-parameters
       data: payload,
       width: int,
       height: int,
       mipmaps: int,
       format: int
     }

type texture_2d :: %Rayex.Structs.Texture{
       id: unsigned,
       width: float,
       height: float,
       mipmaps: int,
       format: int
     }

type render_texture :: %Rayex.Structs.RenderTexture{
       id: unsigned,
       texture: texture_2d,
       depth: texture_2d
     }

type n_patch_info :: %Rayex.Structs.NPatchInfo{
       source: rectangle,
       left: int,
       top: int,
       right: int,
       bottom: int,
       layout: int
     }

type glyph_info :: %Rayex.Structs.GlyphInfo{
       value: int,
       offset_x: int,
       offset_y: int,
       advance_x: int,
       image: image
     }

type font :: %Rayex.Structs.Font{
       base_size: int,
       glyph_count: int,
       glyph_padding: int,
       texture: texture_2d,
       recs: [rectangle],
       glyphs: [glyph_info]
     }

type camera :: %Rayex.Structs.Camera{
       position: vector3,
       target: vector3,
       up: vector3,
       fovy: float,
       projection: int
     }

type camera_2d :: %Rayex.Structs.Camera2D{
       offset: vector2,
       target: vector2,
       rotation: float,
       zoom: float
     }

type mesh :: %Rayex.Structs.Mesh{
       vertex_count: int,
       triangle_count: int,

       # Vertex attributes data
       vertices: [float],
       texcoords: [float],
       texcoords2: [float],
       normals: [float],
       tangents: [float],
       colors: [unsigned],
       indices: [unsigned],

       # Animation vertex data
       anim_vertices: [float],
       anim_normals: [float],
       bone_ids: [unsigned],
       bone_weights: [float],

       # OpenGL identifiers
       vao_id: unsigned,
       vbo_id: [unsigned]
     }

type shader :: %Rayex.Structs.Shader{
       id: unsigned,
       locs: [int]
     }

type material_map :: %Rayex.Structs.MaterialMap{
       texture: texture_2d,
       color: color,
       value: float
     }

type material :: %Rayex.Structs.Material{
       shader: shader,
       maps: [material_map],
       params: [float]
     }

type transform_type :: %Rayex.Structs.Transform{
      translation: vector3,
      rotation: quaternion,
      scale: vector3
    }

type bone_info :: %Rayex.Structs.BoneInfo{
       name: string,
       parent: int
     }

type model :: %Rayex.Structs.Model{
       transform: matrix,
       mesh_count: int,
       material_count: int,
       meshes: [mesh],
       materials: [material],
       mesh_material: [int],
       bone_count: int,
       bones: [bone_info],
       bind_pose: [transform_type],
       c_id: unsigned
     }



type model_animation :: %Rayex.Structs.ModelAnimation{
        bone_count: int,
        frame_count: int,
        bones: [bone_info],
        #  frame_poses: [transform_type]
        name: string
     }

type ray :: %Rayex.Structs.Ray{
       position: vector3,
       direction: vector3
     }

type ray_collision :: %Rayex.Structs.RayCollision{
       hit: bool,
       distance: float,
       point: vector3,
       normal: vector3
     }

type bounding_box :: %Rayex.Structs.BoundingBox{
       min: vector3,
       max: vector3
     }

# type wave :: %Rayex.Structs.Wave{
#        id: atom
#      }

type audio_stream :: %Rayex.Structs.AudioStream{
  buffer: unsigned,#;       // Pointer to internal data used by the audio system
  processor: unsigned,# // Pointer to internal data processor, useful for audio effects
  sample_rate: unsigned,
  sample_size: unsigned,
  channels: unsigned
}

type sound :: %Rayex.Structs.Sound{
       stream: audio_stream,
       frame_count: unsigned
     }

type music :: %Rayex.Structs.Music{
       id: int
     }

type vr_device_info :: %Rayex.Structs.VrDeviceInfo{
       h_resolution: int,
       v_resolution: int,
       h_screen_size: float,
       v_screen_size: float,
       v_screen_center: float,
       eye_to_screen_distance: float,
       lensSeparation_distance: float,
       interpupillary_distance: float,
       lens_distortion_values: [float],
       chroma_ab_correction: [float]
     }

type vr_stereo_config :: %Rayex.Structs.VrStereoConfig{
       projection: [matrix],
       view_offset: [matrix],
       left_lens_center: [float],
       right_lens_center: [float],
       left_screen_center: [float],
       right_screen_center: [float],
       scale: [float],
       scale_in: [float]
     }


# typedef struct
# {
#     ma_format formatIn;
#     ma_format formatOut;
#     ma_uint32 channelsIn;
#     ma_uint32 channelsOut;
#     ma_uint32 sampleRateIn;
#     ma_uint32 sampleRateOut;
#     ma_dither_mode ditherMode;
#     ma_data_converter_execution_path executionPath; /* The execution path the data converter will follow when processing. */
#     ma_channel_converter channelConverter;
#     ma_resampler resampler;
#     ma_bool8 hasPreFormatConversion;
#     ma_bool8 hasPostFormatConversion;
#     ma_bool8 hasChannelConverter;
#     ma_bool8 hasResampler;
#     ma_bool8 isPassthrough;

#     /* Memory management. */
#     ma_bool8 _ownsHeap;
#     void* _pHeap;
# } ma_data_converter;
