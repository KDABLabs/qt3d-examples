#!/usr/bin/env python

import sys
import requests

class TileDownloader:
    def __init__(self, url, level):
        self.url = url
        self.level = level
    
    
    def downloadTiles(self):
        tileCount = pow(2, 2 * self.level)
        print ("tileCount" + str(tileCount))
        
        maxTileOnAxis = pow(2, self.level) - 1
        for x in range(0, maxTileOnAxis + 1):
            for y in range(0, maxTileOnAxis + 1):
                # url scheme is z, x, y
                url = self.url % (self.level, x, y)
                print(url)
                response = requests.get(url)
                if response.status_code == requests.codes.ok:
                    # Save tile
                    with open('elevation_%d_%d_%d.png' % (self.level, x, y), 'w') as tileFile:
                        tileFile.write(response.content)
                else:
                    print("Failed to download tile x:" + str(x) + ", y:" + str(y))



if len(sys.argv) != 3:
    print("Usage: " + sys.argv[0] + " tileLevel mapZen-key")
    exit()

try:
    layerCount = int(sys.argv[1])
    mapZenKey = str(sys.argv[2])
    downloader = TileDownloader("https://tile.mapzen.com/mapzen/terrain/v1/terrarium/%d/%d/%d.png?api_key=" + mapZenKey, layerCount)
    downloader.downloadTiles()
except ValueError:
    print("tileLevel is expected to be an int")
