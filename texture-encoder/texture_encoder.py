import pygame
import utils
import sys
from dataclasses import dataclass
from copy import deepcopy

class Iter:
    def __init__(self, obj):
        self.iter = iter(obj)
    
    def peek(self):
        iter_tmp = deepcopy(self.iter)
        val = self.next()
        self.iter = iter_tmp
        return val

    def next(self):
        try:
            return next(self.iter)
        except StopIteration:
            return None

@dataclass
class ImageParm:
    path: str
    column_major: bool
    indicate_size: bool
    dim: tuple[int, int] | None

class Section:
    section_name: str
    images: list[ImageParm]

    def parse(name, args) -> 'Section':
        section_name = name
        images = []

        while arg := args.peek():
            if arg.startswith("--"):
                break
            args.next()
            side = False # default horizontal
            image_size = (0, 0) # sentinel for nothing to change
            indicate_size = False

            while arg.startswith("-"):
                match arg.removeprefix("-").split("=", 1):
                    case ["v"]: side = True
                    case ["size", size]:
                        if len(dim := size.split("x", 1)):
                            image_size = (int(dim[0]), int(dim[1]))
                        else:
                            print("invalid format missing 'x' in multiplication")
                            exit(1)
                    case ["indicate-size"]:
                        indicate_size = True

                arg = args.next()
                if arg is None:
                    break

            images.append(ImageParm(arg, side, indicate_size, image_size))

        self = Section()
        self.section_name = section_name
        self.images = images

        return self

    def encode(self) -> bytearray:
        
        # for each file path encode it and add it to the section's data
        data = bytearray()

        for image_param in self.images:
            path = image_param.path
            vertical = image_param.column_major
            indicate_size = image_param.indicate_size
            dim = image_param.dim
            image = pygame.image.load(path)

            if dim != (0, 0):
                image = pygame.transform.scale(image, dim)

            if indicate_size:
                data.extend(utils.encode_u32(image.get_width() & 0xffff | image.get_height() << 16))

            image_data = utils.encode_image(vertical, image)
            data.extend(image_data)

        return data

    def __repr__(self):
        return f"<section {self.section_name} {self.images}, {self.sides}>"

def parse_argv(argv) -> (list[Section], str | None):
    args = Iter(argv)
    sections = []
    output_file = None

    while arg := args.next():
        if arg.startswith("--"):
            [keyword, name] = arg.removeprefix("--").split("=", 1) if "=" in arg else [arg, None]

            match keyword:
                case "section":
                    sections.append(Section.parse(name, args))
                case "out":
                    output_file = name
                case "help":
                    print("texture-encoder for the dead-end game")
                    print("Copyright (c) 2026 Gabriel LESCOB. All Rights Reserved.")
                    print("usage:")
                    print("texture-encoder [--help] [--section=<name> [-v] [-size=<w>x<h>] [-indicate-size] image1 [-v] [-size=<w>x<h>] [-indicate-size] image2 ...]...")
                    print("--help: show this help")
                    print("--section=<name>: create a new section in the texture file")

    return (sections, output_file)

def main(argv):
    header: dict[str, int] = {}
    (sections, output_file) = parse_argv(argv)
    sections_data = bytearray()

    for section in sections:
        utils.add_section(sections_data, header, section.section_name, section.encode())

    file_data = utils.encode_header(header)
    file_data.extend(sections_data)
    
    if output_file is None:
        for (i, byte) in enumerate(file_data):
            if i % 16 == 0:
                print(f"\n{i:08x} | ", end="")
            print(f"{byte:02x} ", end="")
        return

    with open(output_file, "wb") as file:
        file.write(file_data)

if __name__ == "__main__":
    main(sys.argv)
