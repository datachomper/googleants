import time
import numpy
ROWS = 40
COLS = 40

def neighbors((row,col)):
    dir = [(-1,0),(0,1),(1,0),(0,-1)]
    return [((row+r)%ROWS,(col+c)%COLS) for r,c in dir]

def neighbors2(map, row, col):
    return min([map[r][c] for r,c in neighbors((row,col))])

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

def diffuse2(map):
    bChanged = True
    iter = 1
    while bChanged:
        changed = 0
        bChanged = False
        for row in xrange(ROWS):
            for col in xrange(COLS):
                value = min(map[row][col], neighbors2(map, row, col)+1)
                if value < map[row][col]:
                    changed += 1
                    bChanged = True
                    map[row][col] = value
        iter += 1
        print changed
    print iter
    return map

map = [[-1 for col in range(COLS)] for row in range(ROWS)]
t = time.time()
costmap = diffuse(map, (0,0))
print time.time()-t
#for row in costmap:
#    line = []
#    for col in row:
#        line.append("%02d"%col)
#    print line

t = time.time()
map2 = [[255 for col in range(COLS)] for row in range(ROWS)]
map2[0][0] = 1
out = diffuse2(map2)
print time.time()-t
#for row in out:
#    line = []
#    for col in row:
#        line.append("%02d"%col)
#    print line
