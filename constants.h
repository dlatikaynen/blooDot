#pragma once

/* we won't do too many popups here.
 * https://github.com/cia-foundation/TempleOS/blob/c26482bb6ad3f80106d28504ec5db3c6a360732c/Adam/God/HSNotes.DD#L210 */
constexpr int const GodsPreferredWidth = 640;
constexpr int const GodsPreferredHight = 480;

constexpr int const LayoutMetricButtonLabelOffsetY = 3;

constexpr float const MillisecondsPerFrame = 16.666f;

constexpr SDL_Color const AlienTextColor = { 250,200,200,222 };
constexpr SDL_Color const ButtonTextColor = { 250, 230, 230, 245 };
constexpr SDL_Color const ChapterTextColor = { 25, 23, 23, 245 };
constexpr SDL_Color const DialogTextColor = { 250, 250, 250, 255 };
constexpr SDL_Color const DialogTextShadowColor = { 65, 56, 56, 200 };

constexpr char const* SettingsFileName = "blooDot.ligma";
constexpr char const* SavegameFileName = "blooDot.$i.bofa";