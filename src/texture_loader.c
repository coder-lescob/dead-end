#include "texture_loader.h"

#include "platform.h"
#include "errors.h"
#include <stdio.h>
#include <string.h>

#ifdef PLATFORM_CALCULATOR

/**
 * load textures from texture file
 */
int load_textures(textures_t *textures) {

    header_t header = parse_header(eadk_external_data);
    int missing = check_for_missing_sections(&header);
    
    if (missing != 0) { return -1; }
    
    // convert offsets to pointers
    const char *wall_ptr = eadk_external_data + header.wall_offset;
    const char *titl_ptr = eadk_external_data + header.titl_offset;
    const char *enty_ptr = eadk_external_data + header.enty_offset;
    const char *hudb_ptr = eadk_external_data + header.hud_background;

    *textures = (textures_t){
        .entities_tex_w = (uint16_t)next_u16(&enty_ptr),
        .entities_tex_h = (uint16_t)next_u16(&enty_ptr),
        .wall_textures     = (color_t *)wall_ptr,
        .entities_textures = (color_t *)enty_ptr,
        .title_screen      = (color_t *)titl_ptr,
        .hud_background    = (color_t *)hudb_ptr,
    };
    return 0;
}

void unload_textures(textures_t *textures) {
    (void)textures;
    /* no texture unloading needed on calculator */
}

#define SECTION_NAME_LEN 4

/**
 * load a texture offset in the header by its name
 */
static void load_texture_offset(header_t *header, uint32_t name, uint32_t offset) {
    if (name == 'wall') {
        header->wall_offset = offset;
    }
    else if (name == 'titl') {
        header->titl_offset = offset;
    }
    else if (name == 'enty') {
        header->enty_offset = offset;
    }
    else if (name == 'hudb') {
        header->hud_background = offset;
    }
    else if (name == 'nums') {
        header->nums_offset = offset;
    }
    else {
        /**
         * TODO: display or log warning
         */
    }
}

/**
 * check for any missing section
 */
int check_for_missing_sections(header_t *header) {
    if (header->wall_offset == 0)
        return error_screen("unable to load wall texture:\n'wall' section not found");
    else if (header->titl_offset == 0) 
        return error_screen("unable to load title screen texture:\n'titl' section not found");
    else if (header->enty_offset == 0)
        return error_screen("unable to load entity textures:\n'enty' section not found");
    else if (header->hud_background == 0)
        return error_screen("unable to load hud background:\n'hudb' section not found");
    else if (header->nums_offset == 0)
        return error_screen("unable to load nums textures:\n'nums' section not found");

    // No problemo
    return 0;
}

/**
 * parses the header of the texture file
 */
header_t parse_header(const char *file_content) {
    header_t header = {
        .wall_offset = 0,
        .enty_offset = 0,
        .titl_offset = 0,
    };

    // parse the header length
    uint32_t header_len = next_u32(&file_content, 0);

    for (uint16_t i = 0; i < header_len; i += 8) {
        // read section name and offset
        uint32_t name   = next_u32(&file_content, 1);
        uint32_t offset = next_u32(&file_content, 0);

        load_texture_offset(&header, name, offset);
    }

    return header;
}

/**
 * reads the next uint32 from the file and increament the pointer by 4 bytes
 */
uint32_t next_u32(const char **file_ptr, int endianess) {
    if (file_ptr == NULL) return 0;
    if (*file_ptr == NULL) return 0;

    uint32_t u32;
    
    // default: little-endian
    if (endianess == 0) {
        u32 = ((**file_ptr) & 0xff)
        | ((*(*file_ptr + 1) & 0xff) << 8 )
        | ((*(*file_ptr + 2) & 0xff) << 16)
        | ((*(*file_ptr + 3) & 0xff) << 24);
    }
    else {
        u32 = (*(*file_ptr + 3) & 0xff)
        | ((*(*file_ptr + 2) & 0xff) << 8 )
        | ((*(*file_ptr + 1) & 0xff) << 16)
        | ((**file_ptr & 0xff) << 24);
    }

    (*file_ptr) += 4;

    return u32;
}

/**
 * reads the next uint16 from the file and increament the pointer by 2 bytes
*/
uint16_t next_u16(const char **file_ptr) {
    if (file_ptr == NULL) return 0;
    if (*file_ptr == NULL) return 0;
    
    // little-endian
    uint16_t u16 = ((**file_ptr) & 0xff)
    | ((*(*file_ptr + 1) & 0xff) << 8);

    (*file_ptr) += 2;

    return u16;
}

#else
#error PLATFROM NOT SUPPOTED
#endif
