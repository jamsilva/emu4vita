## Overlay description ##

Example:
[Gameboy Advance 3x]
image_name="gba_3x.png"
viewport_rotate=0
viewport_width=720
viewport_height=480
viewport_x=120
viewport_y=20
[Arisa Mizuhara's 4:3]
image_name="full_4-3.png"
viewport_width=726
viewport_height=544
[Super Nintendo 4:3]
image_name="snes_full_4-3.png"
viewport_width=720
viewport_height=520
viewport_y=10

Configuration file description:
[]                 --（required）Entry name（[name to present]）
image_name         --（required）File name ove the overlay image（the overlay image is stored in the same folder as the configuration file）
viewport_rotate    --（optional）Screen rotation（0: none，1: 90º， 2: 180º， 3: 270º）（automatic if absent）
viewport_width     --（optional）Screen width（automatic if absent）
viewport_height    --（optional）Screen height（automatic if absent）
viewport_x         --（optional）Screen X position（centered if absent）
viewport_y         --（optional）Screen Y position（centered if absent）
