#pragma once

constexpr int CHUNK_KEY_DINGS_CHEL_F = 0;
constexpr int CHUNK_KEY_SFX_PROJECTILEDECAY = 1;
constexpr int CHUNK_KEY_SPLASH = 2;
constexpr int CHUNK_KEY_FONT_DIALOG = 3;
constexpr int CHUNK_KEY_FONT_DIALOG_FAT = 4;
constexpr int CHUNK_KEY_FONT_FANCY = 5;
constexpr int CHUNK_KEY_FONT_ALIEN = 6;
constexpr int CHUNK_KEY_DINGS_BARREL_INDIGO = 7;
constexpr int CHUNK_KEY_DINGS_BARREL_WOODEN = 8;
constexpr int CHUNK_KEY_DINGS_FLOORROCKTILE_CRACKED = 9;
constexpr int CHUNK_KEY_DINGS_FLOORROCKTILE = 10;
constexpr int CHUNK_KEY_DINGS_FLOORSTONETILE_OCHRE = 11;
constexpr int CHUNK_KEY_DINGS_FLOORSTONETILE_PURPLE = 12;
constexpr int CHUNK_KEY_DINGS_FLOORSTONETILE_SLATE = 13;
constexpr int CHUNK_KEY_DINGS_GRASS = 14;
constexpr int CHUNK_KEY_DINGS_LETTUCE = 15;
constexpr int CHUNK_KEY_DINGS_SCHAED = 16;
constexpr int CHUNK_KEY_DINGS_SNOW = 17;
constexpr int CHUNK_KEY_DINGS_BARREL_LOADED = 18;
constexpr int CHUNK_KEY_DINGS_ROOOFTILE_MESH = 19;
constexpr int CHUNK_KEY_GPU_CHAN = 20;
constexpr int CHUNK_KEY_SFX_BUMP = 21;
constexpr int CHUNK_KEY_SFX_SELCHG = 22;
constexpr int CHUNK_KEY_SFX_SELCONF = 23;
constexpr int CHUNK_KEY_SFX_ASTERISK = 24;
constexpr int CHUNK_KEY_SFX_FLOOR = 25;
constexpr int CHUNK_KEY_SFX_WALLS = 26;
constexpr int CHUNK_KEY_SFX_ROOOF = 27;
constexpr int CHUNK_KEY_UI_ICON_MODEX = 28;
constexpr int CHUNK_KEY_UI_ICON_DIVINE = 29;
constexpr int CHUNK_KEY_UI_ICON_HERC = 30;
constexpr int CHUNK_KEY_UI_ICON_HD = 31;
constexpr int CHUNK_KEY_UI_MENU_EMPTY = 32;
constexpr int CHUNK_KEY_SFX_CRICKET_FRITZ = 33;
constexpr int CHUNK_KEY_SFX_CRICKET_FRANZ = 34;
constexpr int CHUNK_KEY_MOBS_PLAYER1 = 35;
constexpr int CHUNK_KEY_MOBS_PLAYER2 = 36;
constexpr int CHUNK_KEY_MOBS_PLAYER3 = 37;
constexpr int CHUNK_KEY_MOBS_PLAYER4 = 38;
constexpr int CHUNK_KEY_DLG_MANUFACTURER = 39;
constexpr int CHUNK_KEY_DING_DESCRIPTIONS_1 = 40;
constexpr int CHUNK_KEY_CONVO_1 = 41;
constexpr int CHUNK_KEY_SAMPLE_MAP = 42;
constexpr int CHUNK_KEY_GAME_CONTROLLER_DB_TXT = 43;
constexpr int CHUNK_KEY_LANG_FLAG_AM_PNG = 44;
constexpr int CHUNK_KEY_LANG_FLAG_DE_PNG = 45;
constexpr int CHUNK_KEY_LANG_FLAG_FI_PNG = 46;
constexpr int CHUNK_KEY_LANG_FLAG_UA_PNG = 47;
constexpr int CHUNK_KEY_DINGS_SNURCH_SNAVIOR_HEAD_ROOOF = 48;
constexpr int CHUNK_KEY_DINGS_SNURCH_SNAVIOR_FLOOR = 49;
constexpr int CHUNK_KEY_DINGS_SNURCH_SNAVIOR_COLLISION = 50;
constexpr int CHUNK_KEY_SHADER_RETRO_VERTEX = 51;
constexpr int CHUNK_KEY_SHADER_RETRO_FRAGMENT = 52;
constexpr int CHUNK_KEY_SHADER_LIFECURTAIN_VERTEX = 53;
constexpr int CHUNK_KEY_SHADER_LIFECURTAIN_FRAGMENT = 54;
constexpr int CHUNK_KEY_SHADER_RAIN_VERTEX = 55;
constexpr int CHUNK_KEY_SHADER_RAIN_FRAGMENT = 56;
constexpr int CHUNK_KEY_SHADER_VORTEX_VERTEX = 57;
constexpr int CHUNK_KEY_SHADER_VORTEX_FRAGMENT = 58;

#ifndef NDEBUG
inline const char* GetUncookedRelPath(int chunkIndex)
{
	switch(chunkIndex) {
	case 0: return "prefab/chel-f.png"; // CHUNK_KEY_DINGS_CHEL_F
	case 1: return "sfx/projectile-decay.wav"; // CHUNK_KEY_SFX_PROJECTILEDECAY
	case 2: return "ui/nn-splash-scene.png"; // CHUNK_KEY_SPLASH
	case 3: return "font/clear-sans-normie.ttf"; // CHUNK_KEY_FONT_DIALOG
	case 4: return "font/clear-sans-fat.ttf"; // CHUNK_KEY_FONT_DIALOG_FAT
	case 5: return "font/yesevaone-regular-show.otf"; // CHUNK_KEY_FONT_FANCY
	case 6: return "font/esl-gothic-shavian.ttf"; // CHUNK_KEY_FONT_ALIEN
	case 7: return "prefab/barrel-indigo.png"; // CHUNK_KEY_DINGS_BARREL_INDIGO
	case 8: return "prefab/barrel-wooden.png"; // CHUNK_KEY_DINGS_BARREL_WOODEN
	case 9: return "prefab/floorrocktile-cracked.png"; // CHUNK_KEY_DINGS_FLOORROCKTILE_CRACKED
	case 10: return "prefab/floorrocktile.png"; // CHUNK_KEY_DINGS_FLOORROCKTILE
	case 11: return "prefab/floorstonetile-ochre.png"; // CHUNK_KEY_DINGS_FLOORSTONETILE_OCHRE
	case 12: return "prefab/floorstonetile-purple.png"; // CHUNK_KEY_DINGS_FLOORSTONETILE_PURPLE
	case 13: return "prefab/floorstonetile-slate.png"; // CHUNK_KEY_DINGS_FLOORSTONETILE_SLATE
	case 14: return "prefab/grass.png"; // CHUNK_KEY_DINGS_GRASS
	case 15: return "prefab/lettuce.png"; // CHUNK_KEY_DINGS_LETTUCE
	case 16: return "prefab/schaedel.png"; // CHUNK_KEY_DINGS_SCHAED
	case 17: return "prefab/snowblock.png"; // CHUNK_KEY_DINGS_SNOW
	case 18: return "prefab/barrel-loaded.png"; // CHUNK_KEY_DINGS_BARREL_LOADED
	case 19: return "prefab/rooof-meshtile.png"; // CHUNK_KEY_DINGS_ROOOFTILE_MESH
	case 20: return "ui/gpu-chan.png"; // CHUNK_KEY_GPU_CHAN
	case 21: return "sfx/bump.wav"; // CHUNK_KEY_SFX_BUMP
	case 22: return "sfx/selchg.wav"; // CHUNK_KEY_SFX_SELCHG
	case 23: return "sfx/selconf.wav"; // CHUNK_KEY_SFX_SELCONF
	case 24: return "sfx/asterisk.wav"; // CHUNK_KEY_SFX_ASTERISK
	case 25: return "sfx/floor.wav"; // CHUNK_KEY_SFX_FLOOR
	case 26: return "sfx/walls.wav"; // CHUNK_KEY_SFX_WALLS
	case 27: return "sfx/rooof.wav"; // CHUNK_KEY_SFX_ROOOF
	case 28: return "ui/icon-modex.png"; // CHUNK_KEY_UI_ICON_MODEX
	case 29: return "ui/icon-divine.png"; // CHUNK_KEY_UI_ICON_DIVINE
	case 30: return "ui/icon-herc.png"; // CHUNK_KEY_UI_ICON_HERC
	case 31: return "ui/icon-hd.png"; // CHUNK_KEY_UI_ICON_HD
	case 32: return "ui/menu-emptyness.png"; // CHUNK_KEY_UI_MENU_EMPTY
	case 33: return "sfx/cricket-fritz.wav"; // CHUNK_KEY_SFX_CRICKET_FRITZ
	case 34: return "sfx/cricket-franz.wav"; // CHUNK_KEY_SFX_CRICKET_FRANZ
	case 35: return "prefab/player1.png"; // CHUNK_KEY_MOBS_PLAYER1
	case 36: return "prefab/player2.png"; // CHUNK_KEY_MOBS_PLAYER2
	case 37: return "prefab/player3.png"; // CHUNK_KEY_MOBS_PLAYER3
	case 38: return "prefab/player4.png"; // CHUNK_KEY_MOBS_PLAYER4
	case 39: return "ui/manufacturer.dlg"; // CHUNK_KEY_DLG_MANUFACTURER
	case 40: return "ding/dingdescriptions.1.bml"; // CHUNK_KEY_DING_DESCRIPTIONS_1
	case 41: return "dialog/convo.1.bml"; // CHUNK_KEY_CONVO_1
	case 42: return "ui/sample-map.png"; // CHUNK_KEY_SAMPLE_MAP
	case 43: return "misc/gamecontrollerdb.txt"; // CHUNK_KEY_GAME_CONTROLLER_DB_TXT
	case 44: return "ui/langflag-am.png"; // CHUNK_KEY_LANG_FLAG_AM_PNG
	case 45: return "ui/langflag-de.png"; // CHUNK_KEY_LANG_FLAG_DE_PNG
	case 46: return "ui/langflag-fi.png"; // CHUNK_KEY_LANG_FLAG_FI_PNG
	case 47: return "ui/langflag-ua.png"; // CHUNK_KEY_LANG_FLAG_UA_PNG
	case 48: return "prefab/snurch-snavior-head-rooof.png"; // CHUNK_KEY_DINGS_SNURCH_SNAVIOR_HEAD_ROOOF
	case 49: return "prefab/snurch-snavior-floor.png"; // CHUNK_KEY_DINGS_SNURCH_SNAVIOR_FLOOR
	case 50: return "ding/snurch-snavior-collision.csv"; // CHUNK_KEY_DINGS_SNURCH_SNAVIOR_COLLISION
	case 51: return "shaders/retro-vertex.glsl"; // CHUNK_KEY_SHADER_RETRO_VERTEX
	case 52: return "shaders/retro-fragment.glsl"; // CHUNK_KEY_SHADER_RETRO_FRAGMENT
	case 53: return "shaders/lifecurtain-vertex.glsl"; // CHUNK_KEY_SHADER_LIFECURTAIN_VERTEX
	case 54: return "shaders/lifecurtain-fragment.glsl"; // CHUNK_KEY_SHADER_LIFECURTAIN_FRAGMENT
	case 55: return "shaders/rain-vertex.glsl"; // CHUNK_KEY_SHADER_RAIN_VERTEX
	case 56: return "shaders/rain-fragment.glsl"; // CHUNK_KEY_SHADER_RAIN_FRAGMENT
	case 57: return "shaders/vortex-vertex.glsl"; // CHUNK_KEY_SHADER_VORTEX_VERTEX
	case 58: return "shaders/vortex-fragment.glsl"; // CHUNK_KEY_SHADER_VORTEX_FRAGMENT
	default: return "~MISSING~";
	}
}
#endif
