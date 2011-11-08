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
        self.obsmap = [[1 for c in xrange(ants.cols)] for c in xrange(ants.rows)]
        self.foodtargets = set()
        self.rows = ants.rows
        self.cols = ants.cols

    def neighbors(self, (row,col)):
        dir = [(-1,0),(0,1),(1,0),(0,-1)]
        return [((row+r)%self.rows,(col+c)%self.cols) for r,c in dir]

    def diffuse(self, map, target):
        x,y = target
        visiting = []
        visiting.append((1,target))
        map[x][y] = 1
        
        for level,(node_x,node_y) in visiting:
            for (child_x,child_y) in self.neighbors((node_x,node_y)):
                if map[child_x][child_y] == 0:
                    continue
                if map[child_x][child_y] == -1:
                    map[child_x][child_y] = level+1
                    visiting.append((level+1,(child_x,child_y))) 
        return map

    def do_turn(self, ants):
        self.info("Begin Turn")
        orders = {}
        self.foodmap = [[-1 for c in xrange(ants.cols)] for c in xrange(ants.rows)]
        
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
            bfs = self.diffuse(self.obsmap, food)
            self.info("bfs took {0}".format(time.time()-start_time))
            for r in xrange(self.rows):
                for c in xrange(self.cols):
                    self.foodmap[r][c] += bfs[r][c]

        def move_ant(self, ant, map):
            valid_moves = []
            for x,y in self.neighbors(ant):
                if map[x][y] > 0 and (x,y) not in orders:
                    valid_moves.append((map[x][y],(x,y))) 
            if valid_moves:
                valid_moves.sort()
                nada, loc = valid_moves[0]
                ants.issue_order((ant, ants.direction(ant, loc)[0]))
                self.info("Moving ant {0} {1}".format(ant, ants.direction(ant, loc)))
                orders[loc] = ant

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
