import time
import numpy
ROWS = 10
COLS = 10

def neighbors((row,col)):
    dir = [(-1,0),(0,1),(1,0),(0,-1)]
    return [((row+r)%ROWS,(col+c)%COLS) for r,c in dir]

def diffuse(map, target):
    x,y = target
    visiting = []
    visiting.append((1,target))
    map[x][y] = 1
    
    for level,(node_x,node_y) in visiting:
        for (child_x,child_y) in neighbors((node_x,node_y)):
            if map[child_x][child_y] == 0:
                continue
            if map[child_x][child_y] == -1:
                map[child_x][child_y] = level+1
                visiting.append((level+1,(child_x,child_y))) 
    return map

map = [[-1 for col in range(COLS)] for row in range(ROWS)]
#map = numpy.ones((ROWS,COLS))
t = time.time()
costmap = diffuse(map, (0,0))
print time.time()-t
for row in costmap:
    line = []
    for col in row:
        line.append("%02d"%col)
    print line
