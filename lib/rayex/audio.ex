defmodule Rayex.Audio do
  @moduledoc """
    Audio related functions
  """

  alias Rayex.Structs, as: S
  alias Rayex.Unifex.Raylib

  # Audio device management functions
  @spec init_audio_device() :: :ok
  defdelegate init_audio_device(), to: Raylib


  # Wave/Sound loading/unloading functions
@doc "Initialize window and OpenGL context"
@spec load_sound(String.t()) :: S.Sound.t()
defdelegate load_sound(fileName), to: Raylib

@spec play_sound(S.Sound.t()) :: :ok
defdelegate play_sound(sound), to: Raylib

  # Wave/Sound management functions

  # Music management functions

  # AudioStream management functions
end
