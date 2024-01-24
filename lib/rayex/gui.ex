defmodule Rayex.GUI do
  @moduledoc """
    Raylib GUI functions
  """

  alias Rayex.Structs, as: S
  alias Rayex.Unifex.Raylib

  @spec gui_enable() :: :ok
  defdelegate gui_enable(), to: Raylib

  @spec gui_disable() :: :ok
  defdelegate gui_disable(), to: Raylib

  @spec gui_lock() :: :ok
  defdelegate gui_lock(), to: Raylib

  # @spec gui_show_cursor() :: :ok
  # defdelegate gui_show_cursor(), to: Raylib

  @spec gui_unlock() :: :ok
  defdelegate gui_unlock(), to: Raylib

  @spec gui_is_locked?() :: boolean()
  defdelegate gui_is_locked?(), to: Raylib, as: :gui_is_locked

  @spec gui_set_alpha(float()) :: :ok
  defdelegate gui_set_alpha(alpha), to: Raylib

  @spec gui_set_state(integer()) :: :ok
  defdelegate gui_set_state(state), to: Raylib

  @spec gui_get_state() :: integer()
  defdelegate gui_get_state(), to: Raylib

  @spec gui_load_style_default() :: :ok
  defdelegate gui_load_style_default(), to: Raylib

  # Container/separator controls, useful for controls organization
  @spec gui_window_box(S.Rectangle.t(), String.t()) :: integer()
  defdelegate gui_window_box(bounds, title), to: Raylib

  @spec gui_group_box(S.Rectangle.t(), String.t()) :: integer()
  defdelegate gui_group_box(bounds, title), to: Raylib

  @spec gui_line(S.Rectangle.t(), String.t()) :: integer()
  defdelegate gui_line(bounds, text), to: Raylib

  @spec gui_panel(S.Rectangle.t(), String.t()) :: integer()
  defdelegate gui_panel(bounds, text), to: Raylib

  @spec gui_tab_bar(S.Rectangle.t(), [String.t()], integer(), integer()) :: {integer(), integer()}
  defdelegate gui_tab_bar(bounds, text, count, active), to: Raylib

  @spec gui_label(S.Rectangle.t(), String.t()) :: integer()
  defdelegate gui_label(bounds, text), to: Raylib

  @spec gui_button(S.Rectangle.t(), String.t()) :: boolean()
  defdelegate gui_button(bounds, text), to: Raylib

  @spec gui_label_button(S.Rectangle.t(), String.t()) :: boolean()
  defdelegate gui_label_button(bounds, text), to: Raylib

  @spec gui_toggle(S.Rectangle.t(), String.t(), boolean()) :: {integer(), boolean()}
  defdelegate gui_toggle(bounds, text, active), to: Raylib

  @spec gui_slider(S.Rectangle.t(), String.t(), String.t(), float(), float(), float()) :: {integer(), float()}
  defdelegate gui_slider(bounds, text_left, text_right, value, min_value, max_value), to: Raylib

  @spec gui_slider_bar(S.Rectangle.t(), String.t(), String.t(), float(), float(), float()) :: {integer(), float()}
  defdelegate gui_slider_bar(bounds, text_left, text_right, value, min_value, max_value), to: Raylib

  @spec gui_progress_bar(S.Rectangle.t(), String.t(), String.t(), float(), float(), float()) :: {integer(), float()}
  defdelegate gui_progress_bar(bounds, text_left, text_right, value, min_value, max_value), to: Raylib

  @spec gui_status_bar(S.Rectangle.t(), String.t()) :: integer()
  defdelegate gui_status_bar(bounds, text), to: Raylib

  @spec gui_message_box(S.Rectangle.t(), String.t(), String.t(), String.t()) :: integer()
  defdelegate gui_message_box(bounds, title, message, buttons), to: Raylib

end
