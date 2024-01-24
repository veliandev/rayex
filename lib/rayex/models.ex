defmodule Rayex.Models do
  @moduledoc """
    Models related functions
  """

  alias Rayex.Structs, as: S
  alias Rayex.Unifex.Raylib

  # Basic geometric 3D shapes drawing functions

  @doc "Draw cube"
  @spec draw_cube(S.Vector3.t(), float(), float(), float(), S.Color.t()) :: :ok
  defdelegate draw_cube(position, width, height, length, color), to: Raylib

  @doc "Draw cube wires"
  @spec draw_cube_wires(S.Vector3.t(), float(), float(), float(), S.Color.t()) :: :ok
  defdelegate draw_cube_wires(position, width, height, length, color), to: Raylib

  @doc "Draw a ray line"
  @spec draw_ray(S.Ray.t(), S.Color.t()) :: :ok
  defdelegate draw_ray(ray, color), to: Raylib

  @doc "Draw a grid (centered at (0, 0, 0))"
  @spec draw_grid(non_neg_integer(), float()) :: :ok
  defdelegate draw_grid(slices, spacing), to: Raylib

  @spec load_model(String.t()) :: S.Model.t()
  defdelegate load_model(path), to: Raylib

  @spec is_model_ready?(S.Model.t()) :: boolean()
  defdelegate is_model_ready?(model), to: Raylib, as: :is_model_ready

  @spec draw_model(S.Model.t(), S.Vector3.t(), float(), S.Color.t()) :: :ok
  defdelegate draw_model(model, position, scale, tint), to: Raylib

  @spec load_model_animations(String.t()) :: [S.ModelAnimation.t()]
  defdelegate load_model_animations(filename), to: Raylib

end
