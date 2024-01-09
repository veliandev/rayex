defmodule TestSound do
  use Rayex;

  def init() do
    init_audio_device()
    is_audio_ready(is_audio_device_ready?())
  end

  def is_audio_ready(result) do
    if result do
      dbg sound = load_sound("/home/fatigue/downloads/test.mp3")
      dbg play_sound(sound)
    else
     is_audio_ready(is_audio_device_ready?())
    end
  end

end

TestSound.init()
