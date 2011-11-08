#!/usr/bin/env python
from ants import *
import logging as log
import numpy as np
from heapq import *

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
        self.obsmap = np.ones((ants.rows, ants.cols))
        self.foodtargets = set()
        self.foodmap = np.zeros((ants.rows, ants.cols))
        self.rows = ants.rows
        self.cols = ants.cols

    def neighbors(self, loc_list, target):
        adj = set()
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
                adj.add((new_x, new_y))
        return adj
    
    def diffuse(self, map, target):
        visited = []
        visiting = set()
        remaining = set()
    
        # Create x,y tuple for any loc that isn't 0
        # 0 denotes non-existant nodes
        for x in xrange(self.rows):
            for y in xrange(self.cols):
                if (map[x][y]):
                    remaining.add((x,y))
    
        if target not in remaining:
            return []
    
        visiting.add(target)
        remaining.remove(target)
        # Visit every node on current level, store its children
        # in seen list
        while visiting != set([]):
            seen = set()
            for node in visiting:
                for x in self.neighbors(remaining, node):
                    seen.add(x)
                    remaining.remove(x)
            visited.append(visiting)
            visiting = seen
    
        return visited

    def do_turn(self, ants):
        self.info("Begin Turn")
        orders = {}
        
        # Remove discovered WATER nodes from the obstacle map
        for row in xrange(ants.rows):
            for col in xrange(ants.cols):
                if ants.map[row][col] == -4:
                    self.obsmap[row][col] = 0

        # Update food lists
        visible_food = set(ants.food_list)
        for unseen in self.foodtargets - visible_food:
            if ants.visible(unseen):
                self.foodtargets.remove(unseen)
        for new_food in visible_food - self.foodtargets:
            self.foodtargets.add(new_food)

        for food in self.foodtargets:
            start_time = time.time()
            bfs = self.BFS(self.obsmap, food)
            self.info("bfs took {0}".format(time.time()-start_time))
            for level in xrange(len(bfs)):
                for x,y in bfs[level]:
                    self.foodmap[x][y] += len(bfs)-level

        def move_ant(self, ant, map):
            adj = []
            old_x, old_y = ant
            for x,y,direction in [(0,1,'e'),(1,0,'s'),(0,-1,'w'),(-1,0,'n')]:
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
                if map[new_x][new_y] and (new_x,new_y) not in orders:
                    adj.append((map[new_x][new_y], direction))

            adj.sort()
            self.info(adj)
            desire,direction = adj[-1]
            self.info("Moving ant {0} {1}".format(ant, direction))
            ants.issue_order((ant, direction))
            orders[ants.destination(ant, direction)] = ant

        for ant in ants.my_ants():
            move_ant(self, ant, self.foodmap)

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
