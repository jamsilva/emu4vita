##遮罩图片说明##

示例：
[GBA 3倍大小]
image_name="gba_2x.png"
viewport_rotate=0
viewport_width=720
viewport_height=480
viewport_x=120
viewport_y=20
[4:3铺满屏幕 (永爱水原ありさ)]
image_name="full_4-3.png"
viewport_width=726
viewport_height=544
[SFC 4:3铺满屏幕]
image_name="sfc_full_4-3.png"
viewport_width=720
viewport_height=520
viewport_y=10

配置文件说明:
[]                 --（必有）遮罩条目名称（[要显示的名称]）
image_name         --（必有）遮罩图文件名称（遮罩图放于配置文件同目录下）
viewport_rotate    --（可无）画面旋转（顺时针：0 无旋转，1 旋转90度， 2 旋转180度， 3 旋转270度）（无时：随图形设置）
viewport_width     --（可无）画面宽度（无时：随图形设置）
viewport_height    --（可无）画面高度（无时：随图形设置）
viewport_x         --（可无）画面x轴（无时：居中）
viewport_y         --（可无）画面y轴（无时：居中）