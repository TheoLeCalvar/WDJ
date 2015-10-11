#!/usr/bin/python3

from sys import argv, exit
import os

def generate_zoom(zoom_val, path):
    coords = {
            'path' : path.rstrip('/'),
            'z'    : zoom_val,
            'z1'   : zoom_val + 1
            }
    params = {}

    for x in range(pow(2, zoom_val)):
        for y in range(pow(2, zoom_val)):
            coords['x']  = x
            coords['y']  = y
            coords['x1'] = 2 * x
            coords['x2'] = 2 * x + 1
            coords['y1'] = 2 * y
            coords['y2'] = 2 * y + 1

            params['file_0'] = "%(path)s/%(z1)d-%(x1)d-%(y1)d.png" % coords
            params['file_1'] = "%(path)s/%(z1)d-%(x1)d-%(y2)d.png" % coords
            params['file_2'] = "%(path)s/%(z1)d-%(x2)d-%(y1)d.png" % coords
            params['file_3'] = "%(path)s/%(z1)d-%(x2)d-%(y2)d.png" % coords
            params['output'] = "%(path)s/%(z)d-%(x)d-%(y)d.png"    % coords

            res = os.system("montage %(file_0)s %(file_1)s %(file_2)s %(file_3)s -tile 2x2 -geometry 256x256 %(output)s" % params)

            if res:
                print("! Error while creating %(output)s : check that \n\t* %(file_0)s\n\t* %(file_1)s\n\t* %(file_2)s\n\t* %(file_3)s\nexists and are readable" % params)
                exit(-1)
            else:
                print("> %(output)s" % params)


# Zoom levels :
# 0 -> 1 x 1
# 1 -> 2 x 2
# 2 -> 4 x 4
# 3 -> 8 x 8

if len(argv) != 3:
    print("Usage : %s <path_to_image_dir> <zoom_level>\n\nThe zoom level is the log2() of the current number of images / side\n\nzoom level\timages/side\n  0\t\t  1\n  1\t\t  2\n  2\t\t  4\n  3\t\t  8\n...\n")
    exit(-1)
for i in range(int(argv[2]) - 1, -1, -1):
    generate_zoom(i, argv[1])
