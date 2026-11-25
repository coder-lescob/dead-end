#!/bin/sh

OUT=../assets/textures.bin
WALLS=../assets/walls.png
TITLE=../assets/title_screen.png
ENTY=../assets/monster1.png

if command -v uv >/dev/null && find texture_encoder.py >/dev/null; then
    uv run python texture_encoder.py --section=wall -v "$WALLS" --section=titl "$TITLE" --section=enty -v "$ENTY" "--out=$OUT"
else
    echo error uv is not installed
fi
