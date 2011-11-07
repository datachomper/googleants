import networkx as nx
import time

# Takes a point and returns the neighbors
#def neighbors(map, target, level, stack):
#    MAX = len(map)
#    adj = []
#    old_x, old_y = target
#    map[old_x][old_y] = 0
#    for x,y in [(-1,0),(1,0),(0,-1),(0,1)]:
#        new_x = x + old_x
#        if new_x > MAX-1:
#            new_x -= MAX
#        if new_x < 0:
#            new_x += MAX
#        new_y = y + old_y
#        if new_y > MAX-1:
#            new_y -= MAX
#        if new_y < 0:
#            new_y += MAX
#        if map[new_x][new_y]:
#            adj.append((new_x, new_y))
#        map[new_x][new_y] = 0
#    print level, adj
#    if adj == []:
#        return
#    try:
#        stack[level].append(adj)
#    except KeyError:
#        stack[level] = adj
#    for loc in adj:
#        print "{0} to level {1}".format(loc, level)
#        neighbors(map, loc, level+1, stack)

#map = [[1 for cols in range(10)] for row in range(10)]
#orig = map
#stack = {}
#for row in orig:
#    print row
##map[5][6] = 0
#
#neighbors(map, (0,0), 1, stack)
#for row in stack:
#    print row, stack[row]
#
#for level in stack:
#    for x,y in stack[level]:
#        map[x][y] = len(stack) - level
#
#for row in map:
#    print row
#
# Takes a list of lists and a target tuple
# Creates a diffusion map with wrapped edges
#def diffuse(map, target, value):
#    # Assumes a perfect square
#    MAX = len(map)
#    seen = map
#
#    # BFS from target outwards wrapping edges
#def depth(map, node):
#    level = 0
#    while map.predecessors(node) != []:
#        node = map.predecessors(node)[0]
#        level += 1
#    return level
#
#def set_depth(map, edges, target):
#    leveled = []
#    # Set target depth to 0
#    # Put all children in list
#    # Set all its children to 1
#    # iterate through children, repeat
#    leveled.append((0,target))
#    for p,c in edges:
#        leveled.append((depth(map,c),c))
#    return leveled
#
#def print_map(map, value):
#    for x in range(9):
#        row = []
#        for y in range(9):
#            row.append(map.node[(x,y)][value])
#        print row
#
#map = nx.grid_graph([10,10], periodic=True)
#for node in map.nodes():
#    map[node]['cost'] = 0
#print_map(map, 'cost')

#target = (0,0)
#edges = nx.bfs_edges(map, target)
#bfs = nx.bfs_tree(map, target)
##value = 1000
##tmp.node[target]['derp'] = value
##for p,node in bfs:
##    value -= 1
##    tmp.node[node]['derp'] = value
#
##print tmp.nodes(data=True)
##print_map(tmp, 'derp')
#depth_chart = set_depth(bfs,edges,target)
#for depth,loc in depth_chart:
#    map.node[loc]['cost'] = 50-depth
#
#print depth_chart
#print_map(map, 'cost')

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
