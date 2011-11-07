#!/usr/bin/env python
from ants import *
import logging as log

# define a class with a do_turn method
# the Ants.run method will parse and update bot input
# it will also run the do_turn method for us
class MyBot:
    def __init__(self):
        # define class level variables, will be remembered between turns
        self.debug = True
        if self.debug:
            self.logger = log.getLogger("ants")
            fp = log.FileHandler('debug.log')
            formatter = log.Formatter('%(asctime)s %(message)s')
            fp.setFormatter(formatter)
            self.logger.addHandler(fp)
            self.logger.setLevel(log.INFO)

    def info(self, *msg):
        if self.debug:
            self.logger.info(msg)
        else:
            return

    def do_setup(self, ants):
        self.obsmap = [[1 for col in range(ants.cols)] for row in range(ants.rows)]
        self.foodtargets = []
        self.foodmap = [[0 for col in range(ants.cols)] for row in range(ants.rows)]
        self.rows = ants.rows
        self.cols = ants.cols

    def neighbors(self, loc_list, target):
        adj = []
        old_x, old_y = target
        for x,y in [(0,1),(1,0),(0,-1),(-1,0)]:
            new_x = x + old_x
            if new_x > self.cols-1:
                new_x -= self.cols
            if new_x < 0:
                new_x += self.cols
            new_y = y + old_y
            if new_y > self.rows-1:
                new_y -= self.rows
            if new_y < 0:
                new_y += self.rows
            if (new_x,new_y) in loc_list:
                adj.append((new_x, new_y))
        return adj
    
    def BFS(self, map, target):
        visited = []
        visiting = []
        remaining = []
    
        # Create x,y tuple for any loc that isn't 0
        # 0 denotes non-existant nodes
        for x in range(self.rows):
            for y in range(self.cols):
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
                for x in self.neighbors(remaining, node):
                    seen.append(x)
                    remaining.remove(x)
            visited.append(visiting)
            visiting = seen
    
        return visited

    def do_turn(self, ants):
        self.info("Begin Turn")
        self.info((ants.cols, ants.rows))
        
        # Remove discovered WATER nodes from the obstacle map
        for row in range(ants.rows):
            for col in range(ants.cols):
                if ants.map[row][col] == -4:
                    self.obsmap[row][col] = 0

        # Update food lists
        new_food = ants.food_list
        for old_food in self.foodtargets:
            if ants.visible(old_food) and old_food not in new_food:
                self.foodtargets.remove(old_food)
        for food in new_food:
            if food not in self.foodtargets:
                self.foodtargets.append(food)

        for food in self.foodtargets:
            start_time = time.time()
            bfs = self.BFS(self.obsmap, food)
            self.info("bfs took {0}".format(time.time()-start_time))
            for level in range(len(bfs)):
                for x,y in bfs[level]:
                    self.foodmap[x][y] += len(bfs)-level

        self.info("Turn over with {0}ms remaining".format(ants.time_remaining()))





if __name__ == '__main__':
    # psyco will speed up python a little, but is not needed
    try:
        import psyco
        psyco.full()
    except ImportError:
        pass
    
    try:
        # if run is passed a class with a do_turn method, it will do the work
        # this is not needed, in which case you will need to write your own
        # parsing function and your own game state class
        Ants.run(MyBot())
    except KeyboardInterrupt:
        print('ctrl-c, leaving ...')
