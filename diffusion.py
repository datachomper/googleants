def neighbors(loc_list, MAX, target):
    adj = []
    old_x, old_y = target
    for x,y in [(0,1),(1,0),(0,-1),(-1,0)]:
        new_x = x + old_x
        if new_x > MAX-1:
            new_x -= MAX
        if new_x < 0:
            new_x += MAX
        new_y = y + old_y
        if new_y > MAX-1:
            new_y -= MAX
        if new_y < 0:
            new_y += MAX
        if (new_x,new_y) in loc_list:
            adj.append((new_x, new_y))
    return adj

def BFS(map, target):
    visited = []
    visiting = []
    remaining = []

    # Create x,y tuple for any loc that isn't 0
    # 0 denotes non-existant nodes
    for x in range(len(map)):
        for y in range(len(map)):
            if (map[x][y]):
                remaining.append((x,y))

    if target not in remaining:
        return []

    visiting.append(target)
    remaining.remove(target)
    # Visit every node on current level, store its children
    # in seen list
    while visiting != []:
        seen = []
        for node in visiting:
            for x in neighbors(remaining, len(map), node):
                seen.append(x)
                remaining.remove(x)
        visited.append(visiting)
        visiting = seen

    return visited

map = [[1 for col in range(10)] for row in range(10)]
map[1][1] = 0
map[1][2] = 0
map[1][3] = 0
map[2][3] = 0
map[3][3] = 0
map[3][2] = 0
map[2][1] = 0
map[3][1] = 0
levels = BFS(map, (0,0))
print levels
for level in range(len(levels)):
    for x,y in levels[level]:
        map[x][y] = len(levels) - level
    
for row in map:
    print row
