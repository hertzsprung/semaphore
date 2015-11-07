#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sem_parser.h"
#include "sem_serialize_railpro.h"
#include "sem_strings.h"
#include "sem_game.h"

sem_success read_railpro_tile(sem_world* world, sem_tile* tile, uint8_t code);
sem_success parse_railpro_tile(sem_tile* tile, char* tile_description, sem_world* world);

sem_success sem_serialize_load_railpro(FILE* in, sem_game* game) {
	if (in == NULL) return sem_set_error("File does not exist");

	sem_world* world = &(game->world);
	world->max_x = 121;
	world->max_y = 41;
	sem_game_init_blank(game);

	fseek(in, -41*121*4, SEEK_END);

	for (uint32_t j=0; j<41; j++) {
		for (uint32_t i=0; i<121; i++) {
			uint32_t word;
			if (fread(&word, sizeof(uint32_t), 1, in) < 1) return sem_set_error("Unexpected end of file");
			word = (word & 0xffff0000) >> 16;
			unsigned char tile_hex[3];
			tile_hex[0] = (word & 0xff00) >> 8;
			tile_hex[1] = word & 0xff;
			tile_hex[2] = '\0';

			char* endptr = "";
			uint8_t tile_code = (uint8_t) strtol((const char*) tile_hex, &endptr, 16);

			// if the tile_code is '9', sometimes the other char is to be displayed
			// to annotate stations/tracks etc.  we just ditch those at the moment.
			if (*endptr != '\0' && tile_code == 0x9) tile_code = 0x0;

			sem_tile* tile = sem_tile_at(world, i, 40-j);
			if (read_railpro_tile(world, tile, tile_code) != SEM_OK) return SEM_ERROR;
		}
	}
	
	return SEM_OK;
}

sem_success read_railpro_tile(sem_world* world, sem_tile* tile, uint8_t code) {
	if (code == 0x01) {
		if (parse_railpro_tile(tile, "buffer W-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x02) {
		if (parse_railpro_tile(tile, "buffer N-S", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x03) {
		if (parse_railpro_tile(tile, "buffer NW-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x04) {
		if (parse_railpro_tile(tile, "buffer NE-SW", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x05) {
		if (parse_railpro_tile(tile, "exit W-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x06) {
		if (parse_railpro_tile(tile, "track W-E+N-S", world) != SEM_OK) return SEM_ERROR; // TODO: graded, N-S on top
	} else if (code == 0x08) {
		if (parse_railpro_tile(tile, "track NW-SE+NE-SW", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x09) {
		if (parse_railpro_tile(tile, "track W-E+NW-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x0a) {
		if (parse_railpro_tile(tile, "track W-E+NE-SW", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x0b || code == 0x0c || code == 0x0d || code == 0x0e) {
		if (parse_railpro_tile(tile, "entry W-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x10) {
		if (parse_railpro_tile(tile, "track W-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x11) {
		if (parse_railpro_tile(tile, "track N-S", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x12) {
		if (parse_railpro_tile(tile, "track NW-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x13) {
		if (parse_railpro_tile(tile, "track NE-SW", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x14) {
		if (parse_railpro_tile(tile, "track W-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x15) {
		if (parse_railpro_tile(tile, "track W-NE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x16) {
		if (parse_railpro_tile(tile, "track NW-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x17) {
		if (parse_railpro_tile(tile, "track SW-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x18) {
		if (parse_railpro_tile(tile, "track NW-S", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x19) {
		if (parse_railpro_tile(tile, "track NE-S", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x1a) {
		if (parse_railpro_tile(tile, "track N-SW", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x1b) {
		if (parse_railpro_tile(tile, "track N-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x1d) {
		if (parse_railpro_tile(tile, "station W-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x1e) {
		if (parse_railpro_tile(tile, "siding W-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x1f) {
		if (parse_railpro_tile(tile, "depot W-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x20) {
		if (parse_railpro_tile(tile, "points W-E+NW-SE W-SE NW-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x21) {
		if (parse_railpro_tile(tile, "points NW-E W-E+NW-SE W-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x22) {
		if (parse_railpro_tile(tile, "points W-SE NW-E W-E+NW-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x23) {
		if (parse_railpro_tile(tile, "points N-S+SW-NE SW-N S-NE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x24) {
		if (parse_railpro_tile(tile, "points SW-N S-NE N-S+SW-NE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x25) {
		if (parse_railpro_tile(tile, "points S-NE N-S+SW-NE SW-N", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x26) {
		if (parse_railpro_tile(tile, "track NW-SE", world) != SEM_OK) return SEM_ERROR; // TODO: underneath flyover
	} else if (code == 0x27) {
		if (parse_railpro_tile(tile, "track NW-SE", world) != SEM_OK) return SEM_ERROR; // TODO: flyover entrance at SE
	} else if (code == 0x28) {
		if (parse_railpro_tile(tile, "track NE-SW", world) != SEM_OK) return SEM_ERROR; // TODO: upper level of flyover
	} else if (code == 0x29) {
		if (parse_railpro_tile(tile, "track NW-SE", world) != SEM_OK) return SEM_ERROR; // TODO: flyover entrance at NW
	} else if (code == 0x2a) {
		if (parse_railpro_tile(tile, "track NW-SE+W-E", world) != SEM_OK) return SEM_ERROR; // TODO: graded, W-E on top
	} else if (code == 0x2b) {
		if (parse_railpro_tile(tile, "track W-E+NW-SE", world) != SEM_OK) return SEM_ERROR; // TODO: graded, NW-SE on top
	} else if (code == 0x2c) {
		if (parse_railpro_tile(tile, "points NW-E W-E SW-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x2d) {
		if (parse_railpro_tile(tile, "points W-E SW-E NW-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x2e) {
		if (parse_railpro_tile(tile, "points SW-E NW-E W-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x30) {
		if (parse_railpro_tile(tile, "points W-E+SW-NE SW-E W-NE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x31) {
		if (parse_railpro_tile(tile, "points SW-E W-NE W-E+SW-NE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x32) {
		if (parse_railpro_tile(tile, "points W-NE W-E+SW-NE SW-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x33) {
		if (parse_railpro_tile(tile, "points N-S+NW-SE N-SE NW-S", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x34) {
		if (parse_railpro_tile(tile, "points N-SE NW-S N-S+NW-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x35) {
		if (parse_railpro_tile(tile, "points NW-S N-S+NW-SE N-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x36) {
		if (parse_railpro_tile(tile, "track NE-SW", world) != SEM_OK) return SEM_ERROR; // TODO: underneath flyover
	} else if (code == 0x37) {
		if (parse_railpro_tile(tile, "track NE-SW", world) != SEM_OK) return SEM_ERROR; // TODO: flyover entrance at SW
	} else if (code == 0x38) {
		if (parse_railpro_tile(tile, "track NE-SW", world) != SEM_OK) return SEM_ERROR; // TODO: upper level of flyover
	} else if (code == 0x39) {
		if (parse_railpro_tile(tile, "track NE-SW", world) != SEM_OK) return SEM_ERROR; // TODO: flyover entrance at NE
	} else if (code == 0x3a) {
		if (parse_railpro_tile(tile, "track NE-SW+W-E", world) != SEM_OK) return SEM_ERROR; // TODO: graded, W-E on top
	} else if (code == 0x3b) {
		if (parse_railpro_tile(tile, "track W-E+NE-SW", world) != SEM_OK) return SEM_ERROR; // TODO: graded, NE-SW on top
	} else if (code == 0x3c) {
		if (parse_railpro_tile(tile, "points W-NE W-E W-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x3d) {
		if (parse_railpro_tile(tile, "points W-E W-SE W-NE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x3e) {
		if (parse_railpro_tile(tile, "points W-SE W-NE W-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x40) {
		if (parse_railpro_tile(tile, "points W-E W-NE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x41) {
		if (parse_railpro_tile(tile, "points W-E NW-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x42) {
		if (parse_railpro_tile(tile, "points W-E W-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x43) {
		if (parse_railpro_tile(tile, "points W-E SW-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x44) {
		if (parse_railpro_tile(tile, "points N-S NE-S", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x45) {
		if (parse_railpro_tile(tile, "points N-S NW-S", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x46) {
		if (parse_railpro_tile(tile, "points N-S N-SW", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x47) {
		if (parse_railpro_tile(tile, "points N-S N-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x48) {
		if (parse_railpro_tile(tile, "points NW-SE W-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x49) {
		if (parse_railpro_tile(tile, "points NE-SW SW-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x4a) {
		if (parse_railpro_tile(tile, "points NE-SW W-NE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x4b) {
		if (parse_railpro_tile(tile, "points NW-SE NW-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x4c) {
		if (parse_railpro_tile(tile, "points NW-SE NW-S", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x4d) {
		if (parse_railpro_tile(tile, "points NE-SW NE-S", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x4e) {
		if (parse_railpro_tile(tile, "points NE-SW N-SW", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x4f) {
		if (parse_railpro_tile(tile, "points NW-SE N-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x50) {
		if (parse_railpro_tile(tile, "points W-NE W-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x51) {
		if (parse_railpro_tile(tile, "points NW-E W-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x52) {
		if (parse_railpro_tile(tile, "points W-SE W-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x53) {
		if (parse_railpro_tile(tile, "points SW-E W-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x54) {
		if (parse_railpro_tile(tile, "points NE-S N-S", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x55) {
		if (parse_railpro_tile(tile, "points NW-S N-S", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x56) {
		if (parse_railpro_tile(tile, "points N-SW N-S", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x57) {
		if (parse_railpro_tile(tile, "points N-SE N-S", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x58) {
		if (parse_railpro_tile(tile, "points W-SE NW-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x59) {
		if (parse_railpro_tile(tile, "points SW-E NE-SW", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x5a) {
		if (parse_railpro_tile(tile, "points W-NE NE-SW", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x5b) {
		if (parse_railpro_tile(tile, "points NW-E NW-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x5c) {
		if (parse_railpro_tile(tile, "points NW-S NW-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x5d) {
		if (parse_railpro_tile(tile, "points NE-S NE-SW", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x5e) {
		if (parse_railpro_tile(tile, "points N-SW NE-SW", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x5f) {
		if (parse_railpro_tile(tile, "points N-SE NW-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x60) {
		if (parse_railpro_tile(tile, "signal W-E red main manual", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x61) {
		if (parse_railpro_tile(tile, "signal W-E amber main manual", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x62) {
		if (parse_railpro_tile(tile, "signal W-E green main manual", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x63) {
		if (parse_railpro_tile(tile, "signal N-S red main manual", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x64) {
		if (parse_railpro_tile(tile, "signal N-S amber main manual", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x65) {
		if (parse_railpro_tile(tile, "signal N-S green main manual", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x70) {
		if (parse_railpro_tile(tile, "signal W-E red main auto", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x71) {
		if (parse_railpro_tile(tile, "signal W-E amber main auto", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x72) {
		if (parse_railpro_tile(tile, "signal W-E green main auto", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x73) {
		if (parse_railpro_tile(tile, "signal N-S red main auto", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x74) {
		if (parse_railpro_tile(tile, "signal N-S amber main auto", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x75) {
		if (parse_railpro_tile(tile, "signal N-S green main auto", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x80) {
		if (parse_railpro_tile(tile, "signal W-E red sub", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x81) {
		if (parse_railpro_tile(tile, "signal W-E amber sub", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x82) {
		if (parse_railpro_tile(tile, "signal W-E green sub", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x83) {
		if (parse_railpro_tile(tile, "signal N-S red sub", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x84) {
		if (parse_railpro_tile(tile, "signal N-S amber sub", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x85) {
		if (parse_railpro_tile(tile, "signal N-S green sub", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x88) {
		if (parse_railpro_tile(tile, "bell W-E", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x89) {
		if (parse_railpro_tile(tile, "bell N-S", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x8a) {
		if (parse_railpro_tile(tile, "bell NW-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x8b) {
		if (parse_railpro_tile(tile, "bell NE-SW", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x86) {
		if (parse_railpro_tile(tile, "bell W-E+NW-SE", world) != SEM_OK) return SEM_ERROR;
	} else if (code == 0x87) {
		if (parse_railpro_tile(tile, "bell W-E+NE-SW", world) != SEM_OK) return SEM_ERROR;
	}
	return SEM_OK;
}

sem_success parse_railpro_tile(sem_tile* tile, char* tile_description, sem_world* world) {
	char* key = strdup(tile_description);
	sem_tokenization tokens;
	sem_tokenization_init(&tokens, key, " ");
	if (sem_tile_parse(tile, &tokens, world->track_cache) != SEM_OK) return SEM_ERROR;
	free(key);
	return SEM_OK;
}
