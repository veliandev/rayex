defmodule TestSound do

  alias Rayex.Structs.Vector3
  use Rayex
  use Rayex.Keys

  def init() do
    init_window(1280, 720, "Test Sound")
    set_target_fps(60)
    init_audio_device()

    resources = %{
      camera: %Rayex.Structs.Camera{
        position: %Rayex.Structs.Vector3{x: 10.0, y: 10.0, z: 10.0},
        target: %Rayex.Structs.Vector3{x: 0.0, y: 0.0, z: 0.0},
        up: %Rayex.Structs.Vector3{x: 0.0, y: 1.0, z: 0.0},
        fovy: 45.0,
        projection: 0
        },
      sound: load_sound("/home/fatigue/downloads/test.mp3")
    }

    update(resources)
  end

  def update(resources) do
    clear_background(%Rayex.Structs.Color{r: 0, g: 0, b: 0, a: 255})

    resources = camera_controller(resources)

    begin_drawing()
    begin_mode_3d(resources.camera)

    draw_grid(10, 1.0)

    end_mode_3d()
    end_drawing()

    if is_key_pressed?(key_escape()) do
      close_window()
    end

    if is_key_pressed?(key_space()) do
      play_sound(resources.sound)
    end

    update(resources)
  end

  def camera_controller(resources) do

    resources = if is_key_down?(key_left_shift()) do
      update_in(resources, [Access.key(:camera), Access.key(:position)], fn(position) -> %{position | y: position.y + 0.1} end)
    else
      resources
    end

    resources = if is_key_down?(key_left_control()) do
       update_in(resources, [Access.key(:camera), Access.key(:position)], fn(position) -> %{position | y: position.y - 0.1} end)
    else
      resources
    end

    resources = if is_key_down?(key_w()) do
      update_in(resources, [Access.key(:camera), Access.key(:position)], fn(position) -> %{position | x: position.x - 0.1, z: position.z - 0.1} end)
    else
      resources
    end

    resources = if is_key_down?(key_s()) do
      update_in(resources, [Access.key(:camera), Access.key(:position)], fn(position) -> %{position | x: position.x + 0.1, z: position.z + 0.1} end)
    else
      resources
    end

    resources

  end

end

TestSound.init()
