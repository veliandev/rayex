defmodule Rayex.Audio do
  @moduledoc """
    Audio related functions
  """

  alias Rayex.Structs, as: S
  alias Rayex.Unifex.Raylib

  # Audio device management functions
  @spec init_audio_device() :: :ok
  defdelegate init_audio_device(), to: Raylib

  @spec is_audio_device_ready?() :: boolean()
  defdelegate is_audio_device_ready?(), to: Raylib, as: :is_audio_device_ready


  # Wave/Sound loading/unloading functions
  @doc "Initialize window and OpenGL context"

  # @spec load_wave(Wave.t(), atom()) :: S.Wave.t()
  # defdelegate load_wave(fileName, atomId), to: RayLib

  @spec is_sound_ready?(S.Sound.t()) :: boolean()
  defdelegate is_sound_ready?(sound), to: Raylib, as: :is_sound_ready

  @spec load_sound(String.t()) :: S.Sound.t()
  defdelegate load_sound(fileName), to: Raylib

  # @spec play_wave(S.Wave.t()) :: :ok
  # defdelegate play_wave(wave), to: Raylib

  @spec play_sound(S.Sound.t()) :: :ok
  defdelegate play_sound(sound), to: Raylib

  @spec set_master_volume(float()) :: :ok
  defdelegate set_master_volume(volume), to: Raylib

  # Wave/Sound management functions

  # Music management functions

  # AudioStream management functions
end
