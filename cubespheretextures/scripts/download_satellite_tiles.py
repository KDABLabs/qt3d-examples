#!/usr/bin/env python

import sys
import requests
from multiprocessing import Pool
from functools import partial

def downloadTile(url, level, x, y):
    # This layer has a scheme that is z, y, z
    url = url % (level, y, x)
    print(url)
    response = requests.get(url)
    if response.status_code == requests.codes.ok:
        # Save tile
        with open('satellite_%d_%d_%d.jpg' % (level, x, y), 'w') as tileFile:
            tileFile.write(response.content)
    else:
        print("Failed to download tile x:" + str(x) + ", y:" + str(y))


class TileDownloader:
    def __init__(self, url, level):
        self.url = url
        self.level = level
        self.currentX = 0

        
    def downloadTiles(self):
        tileCount = pow(2, 2 * self.level)
        print ("tileCount" + str(tileCount))
        
        maxTileOnAxis = pow(2, self.level) - 1
        p = Pool(12)

        for x in range(0, maxTileOnAxis + 1):
            downloadFunc = partial(downloadTile, self.url, self.level, x)
            p.map(downloadFunc, range(0, maxTileOnAxis + 1))


if len(sys.argv) != 2:
    print("Usage: " + sys.argv[0] + " tileLevel")
    exit()

try:
    layerCount = int(sys.argv[1])                
    downloader = TileDownloader("http://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/%d/%d/%d", layerCount)
    downloader.downloadTiles()
except ValueError:
    print("tileLevel is expected to be an int")
