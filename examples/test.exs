defmodule TestSound.Resources do
  use Agent

  alias Rayex.Structs, as: S
  use Rayex

  def start_link(initial_state \\ %{}) do
    Agent.start_link(fn ->
      %{
        should_close: false,
        camera: %S.Camera{
          position: %S.Vector3{x: 10.0, y: 10.0, z: 10.0},
          target: %S.Vector3{x: 0.0, y: 0.0, z: 0.0},
          up: %S.Vector3{x: 0.0, y: 1.0, z: 0.0},
          fovy: 45.0,
          projection: 0
          },
        # sound: load_sound("sound_path_here/test.mp3"),
        tab: 0,
        slider: 0.0,
        progress: 0.0,
        toggled: false,
        model: nil,
        model_position: %S.Vector3{x: 0.0, y: 0.0, z: 0.0}
      }
    end, name: __MODULE__)
  end

  def get(), do: Agent.get(__MODULE__, &(&1))

  def get(keys) when is_list(keys), do: Agent.get(__MODULE__, fn state -> get_in(state, Enum.map(keys, &Access.key/1)) end)
  def get(key) when is_atom(key), do: Agent.get(__MODULE__, fn state -> Map.get(state, key) end)

  def set(keys, value) when is_list(keys), do: Agent.update(__MODULE__, fn state -> put_in(state, Enum.map(keys, &Access.key/1), value) end)
  def set(key, value) when is_atom(key), do: Agent.update(__MODULE__, fn state -> Map.put(state, key, value) end)

end

defmodule TestSound do

  alias TestSound.Resources, as: R
  alias Rayex.Structs, as: S
  use Rayex
  use Rayex.Keys

  def init() do
    dbg R.start_link()

    init_window(1280, 720, "This is mainly for testing all the new stuff.")

    # Sound does not work right now; have to rewrite it all once THIS is patched: https://github.com/membraneframework/membrane_core/issues/719
    # init_audio_device()

    init(window_ready?())
  end

  def init(isReady) do

    if (!isReady), do: __MODULE__.init(window_ready?())

    set_target_fps(60)

    gui_enable()
    gui_load_style_default()

    # Model does not work right now; have to rewrite it all once THIS is patched: https://github.com/membraneframework/membrane_core/issues/719
    # R.set(:model, load_model("examples/resources/models/robot.glb"))

    # Model does not work right now; have to rewrite it all once THIS is patched: https://github.com/membraneframework/membrane_core/issues/719
    # model_anims = load_model_animations("examples/resources/models/robot.glb")
    # Enum.each(model_anims, fn anim ->
    #   dbg anim
    # end)
    # R.set(:model_animations, load_model_animations("examples/resources/models/robot.glb"))

    # set_master_volume(50.0)

    update()
  end

  def update() do

    update_camera()

    clear_background(%S.Color{r: 0, g: 0, b: 0, a: 255})

    begin_drawing()

    rect = %S.Rectangle{x: 18.0, y: 18.0, width: 100.0, height: 18.0}
    # val = gui_tab_bar(rect, ["Tab1", "Tab2", "Tab3"], 3, R.get(:tab))
    # case val do
    #   {-1, 0} -> R.set(:tab, 0)
    #   {-1, 1} -> R.set(:tab, 1)
    #   {-1, 2} -> R.set(:tab, 2)
    #   {0, _} -> IO.puts("Closed tab 1")
    #   {1, _} -> IO.puts("Closed tab 2")
    #   {2, _} -> IO.puts("Closed tab 3")
    #   _ -> :ok
    # end

    # rect = %Rayex.Structs.Rectangle{x: 5.0, y: 5.0, width: 100.0, height: 20.0}
    # rect = rect |> S.Rectangle.add(0.0, 24.0)
    # val = gui_slider(rect, "", "Slider Value = #{R.get(:slider)}", R.get(:slider), 0.0, 1.0)

    # case val do
    #   {1, new_value} -> R.set(:slider, Float.round(new_value, 1))
    #   _ -> :ok
    # end

    rect = rect |> S.Rectangle.add(0.0, 24.0)
    val = gui_progress_bar(rect, "", "Progress Bar Value = #{R.get(:progress)}", R.get(:progress), 0.0, 1.0)

    if is_key_pressed?(key_q()) do
      R.set(:progress, if R.get(:progress) - 0.1 < 0 do 0.0 else Float.round(R.get(:progress) - 0.1, 1) end)
    end

    if is_key_pressed?(key_e()) do
      R.set(:progress, if R.get(:progress) + 0.1 > 1.0 do 1.0 else Float.round(R.get(:progress) + 0.1, 1) end)
    end

    rect = rect |> S.Rectangle.add(0.0, 24.0)
    if gui_button(rect, "Play") do
      IO.puts("Button pressed")
    end

    # rect = rect |> S.Rectangle.add(0.0, 24.0)
    # gui_label(rect, "This is a label")

    # rect = rect |> S.Rectangle.add(0.0, 24.0)
    # if gui_label_button(rect, "Label Button!") do
    #   IO.puts("Label button pressed")
    # end

    rect = rect |> S.Rectangle.add(0.0, 24.0)
    case gui_toggle(rect, "This is a toggle", R.get(:toggled)) do
      {1, _} -> IO.puts("Toggled"); R.set(:toggled, !R.get(:toggled))
      _ -> :ok
    end

    begin_mode_3d(R.get(:camera))
    draw_grid(10, 1.0)

    # Model does not work right now; have to rewrite it all once THIS is patched: https://github.com/membraneframework/membrane_core/issues/719
    # if (is_model_ready?(R.get(:model))) do
    #   draw_model(R.get(:model), R.get(:model_position), 1.0, %S.Color{r: 255, g: 255, b: 255, a: 255})
    # end

    end_mode_3d()

    end_drawing()

    if is_key_down?(key_up()) do
      R.set(:model_position, R.get(:model_position) |> S.Vector3.add(0.0, 0.0, 0.1))
    end

    if is_key_down?(key_down()) do
      R.set(:model_position, R.get(:model_position) |> S.Vector3.add(0.0, 0.0, -0.1))
    end

    if is_key_pressed?(key_space()) do
      play_sound(R.get(:sound))
    end

    if is_key_pressed?(key_escape()) do
      close_window()
      R.set(:should_close, true)
    end

    if(R.get(:should_close)) do
      gui_disable()
      {:ok, "Closing."}
    else
      __MODULE__.update()
    end
  end

  def update_camera() do

    if is_key_down?(key_left_shift()) do
      R.set([:camera, :position], S.Vector3.add(R.get([:camera, :position]), 0.0, 0.1, 0.0))
    end

    if is_key_down?(key_left_control()) do
      R.set([:camera, :position], S.Vector3.add(R.get([:camera, :position]), 0.0, -0.1, 0.0))
    end

    if is_key_down?(key_w()) do
      R.set([:camera, :position], S.Vector3.add(R.get([:camera, :position]), -0.1, 0.0, -0.1))
    end

    if is_key_down?(key_s()) do
      R.set([:camera, :position], S.Vector3.add(R.get([:camera, :position]), 0.1, 0.0, 0.1))
    end

  end

end

TestSound.init()
