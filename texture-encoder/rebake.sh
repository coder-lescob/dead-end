#!/bin/sh

OUT=../assets/textures.bin
WALLS=../assets/walls.png
TITLE=../assets/title_screen.png
ENTY=../assets/monster1.png
HUD_BACK=../assets/dead-end-hud-background.png
NUMS=../assets/nums.png

if command -v uv >/dev/null && find texture_encoder.py >/dev/null; then
    uv run python texture_encoder.py --section=wall -v "$WALLS" --section=titl -size=320x240 "$TITLE" --section=enty -v -indicate-size "$ENTY" "--out=$OUT" --section=hudb "$HUD_BACK" --section=nums "$NUMS"
else
    echo error uv is not installed
fi
