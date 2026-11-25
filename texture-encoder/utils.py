import pygame

def rgb565(color: pygame.Color) -> int:
    return (int(0x1f * (color.r / 255.0)) << 11
            | int(0x3f * (color.g / 255.0)) << 5
            | int(0x1f * (color.b / 255.0)))

def rgb565_2_888(color: int) -> pygame.Color:
    r = (255 * ((color >> 11) & 0x1f)) // 0x1f # max of 5 bits 
    g = (255 * ((color >> 5) & 0x3f)) // 0x3f  # max of 6 bits
    b = (255 * (color & 0x1f))       // 0x1f

    return pygame.Color(r, g, b)

def image_convert_to_calculator_quality(image: pygame.Surface) -> pygame.Surface:
    # set whole image to quality rgb565
    for x in range(image.get_width()):
        for y in range(image.get_height()):
            col565 = rgb565(image.get_at((x, y)))
            image.set_at((x, y), rgb565_2_888(col565))
    return image

def encode_u32(u32: int) -> bytearray:
    """ u32 to little endian bytearray """

    data = bytearray()

    data.append(u32         & 0xff)
    data.append((u32 >> 8)  & 0xff)
    data.append((u32 >> 16) & 0xff)
    data.append((u32 >> 24) & 0xff)

    return data

def encode_image(vertical: bool, image: pygame.Surface) -> bytearray:
    data = bytearray()

    if vertical:
        for x in range(image.get_width()):
            for y in range(image.get_height()):
                color = rgb565(image.get_at((x, y)))
                data.append(color & 0xff)
                data.append((color >> 8) & 0xff)
    else:
        for y in range(image.get_height()):
            for x in range(image.get_width()):
                color = rgb565(image.get_at((x, y)))
                data.append(color & 0xff)
                data.append((color >> 8) & 0xff)

    return data

def encode_header(sections: dict[str, int]) -> bytearray:
    header = bytearray()

    header_size = len(sections) * 8 # section contains 4 byte id and 4 byte offset total 12 bytes
    header.extend(encode_u32(header_size))

    for section_name, section_offset in sections.items():
        if len(section_name) != 4:
            raise ValueError("section name must be 4")

        for ch in section_name.encode("ascii"):
            header.append(ch)

        # add the header size to the offset
        section_offset += header_size + 4 # + the length of the header size it self

        header.extend(encode_u32(section_offset))

    return header

def add_section(file_data: bytearray, header_dict: dict[str, int], section_name: str, data: bytearray):
    offset = len(file_data)
    if section_name in header_dict.keys():
        raise Exception(f"duplicated section {section_name}")
    header_dict[section_name] = offset
    print(f"section {section_name} offset {header_dict[section_name]} size {len(data)}")
    file_data.extend(data)

    return file_data
