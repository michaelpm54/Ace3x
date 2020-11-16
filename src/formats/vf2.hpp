#ifndef ACE3X_FORMATS_VF2_HPP_
#define ACE3X_FORMATS_VF2_HPP_

/* The following license information is taken directly from
    https://github.com/rafalh/rf-reversed/blob/master/vf_format.h
    Ace3x: Naming changes only.
*/

/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        vf_format.h
*  PURPOSE:     VF (Volition Font) format documentation
*  DEVELOPERS:  Rafał Harabień
*
*****************************************************************************/

/**
 * VF is a proprietary Volition bitmap font file format used in Red Faction game.
 *
 * Note: all fields are little-endian. On big-endian architecture bytes needs to be swapped.
 *
 * Work is partially based on information from Roma Sorokin <sorok-roma@yandex.ru>
 **/

#define VF_SIGNATURE 0x544E4656 // 'VFNT'

struct Vf2Header {
    std::uint32_t signature;          // should be equal to VF_SIGNATURE
    std::uint32_t version;            // font version (0 or 1)
    std::uint32_t format;             // exists if version >= 1, else font has VF_FMT_MONO_4 format, for values
                                      // description see vf_format_t
    std::uint32_t num_chars;          // length of vf_file::chars array
    std::uint32_t first_ascii;        // ascii code of first character supported by this font, usually equals
                                      // 0x20 (space character)
    std::uint32_t default_spacing;    // spacing used for characters missing in the font (lower than first_ascii and
                                      // greater than first_ascii + num_chars)
    std::uint32_t height;             // font height (height is the same for all characters)
    std::uint32_t num_kern_pairs;     // length of vf_file::kern_pairs array
    std::uint32_t kern_data_size;     // exists if version == 0, unused by RF (can be calculated from num_kern_pairs)
    std::uint32_t char_data_size;     // exists if version == 0, unused by RF (can be calculated from num_chars)
    std::uint32_t pixel_data_size;    // size of vf_file::pixels array
};

struct Vf2KerningPair {
    std::uint8_t char_before_idx;    // index of character before spacing
    std::uint8_t char_after_idx;     // index of character after spacing
    std::int8_t offset;              // value added to vf_char_desc_t::spacing
};

struct Vf2CharDescriptor {
    std::uint32_t spacing;                // base spacing for this character (can be modified by kerning data), spacing is
                                          // similar to width but is used only during text rendering to update X coordinate
    std::uint32_t width;                  // character width in pixels, not to be confused with spacing
    std::uint32_t pixel_data_offset;      // offset in vf_file::pixels, all pixels for one characters are stored in one run,
                                          // total number of pixels for a character equals:
                                          // vf_char_desc_t::width * vf_header_t::height
    std::uint16_t first_kerning_entry;    // index in vf_file::kern_pairs array, entries can be checked from there because
                                          // array is sorted by character indices
    std::uint16_t user_data;
};

enum VfFormat {
    VfFormatMono4 = 0xF,             // 1 byte per pixel, monochromatic, only values in range 0-14 are used
    VfFormatRgba4444 = 0xF0F0F0F,    // 2 byte per pixel, RGBA 4444 (4 bits per channel)
    VfFormatIndexed = 0xFFFFFFF0     // 1 byte per pixel, indexed (palette is at the end of the file)
};

#endif    // ACE3X_FORMATS_VF2_HPP_
