#pragma once

constexpr int const CHUNK_KEY_DINGS_CHEL_F = 0;
constexpr int const CHUNK_KEY_SFX_PROJECTILEDECAY = 1;
constexpr int const CHUNK_KEY_SPLASH = 2;
constexpr int const CHUNK_KEY_FONT_DIALOG = 3;
constexpr int const CHUNK_KEY_FONT_DIALOG_FAT = 4;
constexpr int const CHUNK_KEY_FONT_FANCY = 5;
constexpr int const CHUNK_KEY_FONT_ALIEN = 6;
constexpr int const CHUNK_KEY_DINGS_BARREL_INDIGO = 7;
constexpr int const CHUNK_KEY_DINGS_BARREL_WOODEN = 8;
constexpr int const CHUNK_KEY_DINGS_FLOORROCKTILE_CRACKED = 9;
constexpr int const CHUNK_KEY_DINGS_FLOORROCKTILE = 10;
constexpr int const CHUNK_KEY_DINGS_FLOORSTONETILE_OCHRE = 11;
constexpr int const CHUNK_KEY_DINGS_FLOORSTONETILE_PURPLE = 12;
constexpr int const CHUNK_KEY_DINGS_FLOORSTONETILE_SLATE = 13;
constexpr int const CHUNK_KEY_DINGS_GRASS = 14;
constexpr int const CHUNK_KEY_DINGS_LETTUCE = 15;
constexpr int const CHUNK_KEY_DINGS_SCHAED = 16;
constexpr int const CHUNK_KEY_DINGS_SNOW = 17;
constexpr int const CHUNK_KEY_DINGS_BARREL_LOADED = 18;
constexpr int const CHUNK_KEY_DINGS_ROOOFTILE_MESH = 19;
constexpr int const CHUNK_KEY_GPU_CHAN = 20;
constexpr int const CHUNK_KEY_SFX_BUMP = 21;
constexpr int const CHUNK_KEY_SFX_SELCHG = 22;
constexpr int const CHUNK_KEY_SFX_SELCONF = 23;
constexpr int const CHUNK_KEY_SFX_ASTERISK = 24;
constexpr int const CHUNK_KEY_SFX_FLOOR = 25;
constexpr int const CHUNK_KEY_SFX_WALLS = 26;
constexpr int const CHUNK_KEY_SFX_ROOOF = 27;
constexpr int const CHUNK_KEY_UI_ICON_MODEX = 28;
constexpr int const CHUNK_KEY_UI_ICON_DIVINE = 29;
constexpr int const CHUNK_KEY_UI_ICON_HERC = 30;
constexpr int const CHUNK_KEY_UI_ICON_HD = 31;
constexpr int const CHUNK_KEY_UI_MENU_EMPTY = 32;
constexpr int const CHUNK_KEY_SFX_CRICKET_FRITZ = 33;
constexpr int const CHUNK_KEY_SFX_CRICKET_FRANZ = 34;
constexpr int const CHUNK_KEY_MOBS_PLAYER1 = 35;
constexpr int const CHUNK_KEY_MOBS_PLAYER2 = 36;
constexpr int const CHUNK_KEY_MOBS_PLAYER3 = 37;
constexpr int const CHUNK_KEY_MOBS_PLAYER4 = 38;
constexpr int const CHUNK_KEY_DLG_MANUFACTURER = 39;
constexpr int const CHUNK_KEY_DING_DESCRIPTIONS_1 = 40;
constexpr int const CHUNK_KEY_CONVO_1 = 41;
constexpr int const CHUNK_KEY_SAMPLE_MAP = 42;
constexpr int const CHUNK_KEY_GAME_CONTROLLER_DB_TXT = 43;
constexpr int const CHUNK_KEY_LANG_FLAG_AM_PNG = 44;
constexpr int const CHUNK_KEY_LANG_FLAG_DE_PNG = 45;
constexpr int const CHUNK_KEY_LANG_FLAG_FI_PNG = 46;
constexpr int const CHUNK_KEY_LANG_FLAG_UA_PNG = 47;
constexpr int const CHUNK_KEY_DINGS_SNURCH_SNAVIOR_HEAD_ROOOF = 48;
constexpr int const CHUNK_KEY_DINGS_SNURCH_SNAVIOR_FLOOR = 49;
constexpr int const CHUNK_KEY_DINGS_SNURCH_SNAVIOR_COLLISION = 50;
constexpr int const CHUNK_KEY_SHADER_RETRO_VERTEX = 51;
constexpr int const CHUNK_KEY_SHADER_RETRO_FRAGMENT = 52;
constexpr int const CHUNK_KEY_SHADER_LIFECURTAIN_VERTEX = 53;
constexpr int const CHUNK_KEY_SHADER_LIFECURTAIN_FRAGMENT = 54;
constexpr int const CHUNK_KEY_SHADER_RAIN_VERTEX = 55;
constexpr int const CHUNK_KEY_SHADER_RAIN_FRAGMENT = 56;

#ifndef NDEBUG
inline const char* GetUncookedRelPath(int chunkIndex)
{
	switch(chunkIndex)
	{
	case 0: return "chel-f.png"; // CHUNK_KEY_DINGS_CHEL_F
	case 1: return "projectile-decay.wav"; // CHUNK_KEY_SFX_PROJECTILEDECAY
	case 2: return "nn-splash-scene.png"; // CHUNK_KEY_SPLASH
	case 3: return "clear-sans-normie.ttf"; // CHUNK_KEY_FONT_DIALOG
	case 4: return "clear-sans-fat.ttf"; // CHUNK_KEY_FONT_DIALOG_FAT
	case 5: return "yesevaone-regular-show.otf"; // CHUNK_KEY_FONT_FANCY
	case 6: return "esl-gothic-shavian.ttf"; // CHUNK_KEY_FONT_ALIEN
	case 7: return "barrel-indigo.png"; // CHUNK_KEY_DINGS_BARREL_INDIGO
	case 8: return "barrel-wooden.png"; // CHUNK_KEY_DINGS_BARREL_WOODEN
	case 9: return "floorrocktile-cracked.png"; // CHUNK_KEY_DINGS_FLOORROCKTILE_CRACKED
	case 10: return "floorrocktile.png"; // CHUNK_KEY_DINGS_FLOORROCKTILE
	case 11: return "floorstonetile-ochre.png"; // CHUNK_KEY_DINGS_FLOORSTONETILE_OCHRE
	case 12: return "floorstonetile-purple.png"; // CHUNK_KEY_DINGS_FLOORSTONETILE_PURPLE
	case 13: return "floorstonetile-slate.png"; // CHUNK_KEY_DINGS_FLOORSTONETILE_SLATE
	case 14: return "grass.png"; // CHUNK_KEY_DINGS_GRASS
	case 15: return "lettuce.png"; // CHUNK_KEY_DINGS_LETTUCE
	case 16: return "schaedel.png"; // CHUNK_KEY_DINGS_SCHAED
	case 17: return "snowblock.png"; // CHUNK_KEY_DINGS_SNOW
	case 18: return "barrel-loaded.png"; // CHUNK_KEY_DINGS_BARREL_LOADED
	case 19: return "rooof-meshtile.png"; // CHUNK_KEY_DINGS_ROOOFTILE_MESH
	case 20: return "gpu-chan.png"; // CHUNK_KEY_GPU_CHAN
	case 21: return "bump.wav"; // CHUNK_KEY_SFX_BUMP
	case 22: return "selchg.wav"; // CHUNK_KEY_SFX_SELCHG
	case 23: return "selconf.wav"; // CHUNK_KEY_SFX_SELCONF
	case 24: return "asterisk.wav"; // CHUNK_KEY_SFX_ASTERISK
	case 25: return "floor.wav"; // CHUNK_KEY_SFX_FLOOR
	case 26: return "walls.wav"; // CHUNK_KEY_SFX_WALLS
	case 27: return "rooof.wav"; // CHUNK_KEY_SFX_ROOOF
	case 28: return "icon-modex.png"; // CHUNK_KEY_UI_ICON_MODEX
	case 29: return "icon-divine.png"; // CHUNK_KEY_UI_ICON_DIVINE
	case 30: return "icon-herc.png"; // CHUNK_KEY_UI_ICON_HERC
	case 31: return "icon-hd.png"; // CHUNK_KEY_UI_ICON_HD
	case 32: return "menu-emptyness.png"; // CHUNK_KEY_UI_MENU_EMPTY
	case 33: return "cricket-fritz.wav"; // CHUNK_KEY_SFX_CRICKET_FRITZ
	case 34: return "cricket-franz.wav"; // CHUNK_KEY_SFX_CRICKET_FRANZ
	case 35: return "player1.png"; // CHUNK_KEY_MOBS_PLAYER1
	case 36: return "player2.png"; // CHUNK_KEY_MOBS_PLAYER2
	case 37: return "player3.png"; // CHUNK_KEY_MOBS_PLAYER3
	case 38: return "player4.png"; // CHUNK_KEY_MOBS_PLAYER4
	case 39: return "manufacturer.dlg"; // CHUNK_KEY_DLG_MANUFACTURER
	case 40: return "dingdescriptions.1.bml"; // CHUNK_KEY_DING_DESCRIPTIONS_1
	case 41: return "convo.1.bml"; // CHUNK_KEY_CONVO_1
	case 42: return "sample-map.png"; // CHUNK_KEY_SAMPLE_MAP
	case 43: return "gamecontrollerdb.txt"; // CHUNK_KEY_GAME_CONTROLLER_DB_TXT
	case 44: return "langflag-am.png"; // CHUNK_KEY_LANG_FLAG_AM_PNG
	case 45: return "langflag-de.png"; // CHUNK_KEY_LANG_FLAG_DE_PNG
	case 46: return "langflag-fi.png"; // CHUNK_KEY_LANG_FLAG_FI_PNG
	case 47: return "langflag-ua.png"; // CHUNK_KEY_LANG_FLAG_UA_PNG
	case 48: return "snurch-snavior-head-rooof.png"; // CHUNK_KEY_DINGS_SNURCH_SNAVIOR_HEAD_ROOOF
	case 49: return "snurch-snavior-floor.png"; // CHUNK_KEY_DINGS_SNURCH_SNAVIOR_FLOOR
	case 50: return "snurch-snavior-collision.csv"; // CHUNK_KEY_DINGS_SNURCH_SNAVIOR_COLLISION
	case 51: return "shaders/retro-vertex.glsl"; // CHUNK_KEY_SHADER_RETRO_VERTEX
	case 52: return "shaders/retro-fragment.glsl"; // CHUNK_KEY_SHADER_RETRO_FRAGMENT
	case 53: return "shaders/lifecurtain-vertex.glsl"; // CHUNK_KEY_SHADER_LIFECURTAIN_VERTEX
	case 54: return "shaders/lifecurtain-fragment.glsl"; // CHUNK_KEY_SHADER_LIFECURTAIN_FRAGMENT
	case 55: return "shaders/rain-vertex.glsl"; // CHUNK_KEY_SHADER_RAIN_VERTEX
	case 56: return "shaders/rain-fragment.glsl"; // CHUNK_KEY_SHADER_RAIN_FRAGMENT
	}

	return nullptr;
}
#endif
