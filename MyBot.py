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
        self.turn = 0
        self.obsmap = [[-1 for c in xrange(ants.cols)] for c in xrange(ants.rows)]
        self.foodtargets = set()
        self.rows = ants.rows
        self.cols = ants.cols
        self.image = 44
    
    def write_png(self, costmap):
        import Image
        pix = Image.new("RGB",(self.cols,self.rows))
        upper = 1
        lower = 0
        for r in range(self.rows):
            for c in range(self.cols):
                upper = max(upper, costmap[r][c])
        for r in range(self.rows):
            for c in range(self.cols):
                value = costmap[r][c]
                if value == 0:
                    color = (0,0,0)
                elif value == -1:
                    color = (240,240,240)
                else:
                    color = (255-int(255*((1.0*costmap[r][c]-lower)/(upper-lower))),255,255)
                pix.putpixel((c,r),color)
        pix.save("foo.bmp")

    def neighbors(self, (row,col)):
        dir = [(-1,0),(0,1),(1,0),(0,-1)]
        return [((row+r)%self.rows,(col+c)%self.cols) for r,c in dir]

    def diffuse(self, m, target):
        x,y = target
        visiting = []
        visiting.append((1,target))
        m[x][y] = 1
        
        for level,(node_x,node_y) in visiting:
            for (child_x,child_y) in self.neighbors((node_x,node_y)):
                if m[child_x][child_y] == 0:
                    continue
                if m[child_x][child_y] == -1:
                    m[child_x][child_y] = level+1
                    visiting.append((level+1,(child_x,child_y))) 

    def do_turn(self, ants):
        self.info("Begin Turn {0}".format(self.turn))
        orders = {}
        foodmap = [[255 for c in xrange(ants.cols)] for c in xrange(ants.rows)]
        
        # Remove discovered WATER nodes from the obstacle map
        for row in xrange(ants.rows):
            for col in xrange(ants.cols):
                if ants.map[row][col] == -4:
                    self.obsmap[row][col] = 0

        # Update food lists
        visible_food = set(ants.food())
        for unseen in (self.foodtargets-visible_food):
            if ants.visible(unseen):
                self.foodtargets.remove(unseen)
        for new_food in (visible_food-self.foodtargets):
            self.foodtargets.add(new_food)

        for food in self.foodtargets:
            start_time = time.time()
            bfs = [[self.obsmap[r][c] for c in xrange(self.cols)] for r in xrange(self.rows)]
            for x,y in ants.my_ants():
                bfs[x][y] = 0
            self.diffuse(bfs, food)
            self.info("bfs took {0} for food {1}".format(time.time()-start_time, food))
            for r in xrange(self.rows):
                for c in xrange(self.cols):
                    if bfs[r][c] > 0:
                        foodmap[r][c] = min(foodmap[r][c],bfs[r][c])
                        #foodmap[r][c] *= bfs[r][c]
                        #foodmap[r][c] += bfs[r][c]
                    else:
                        foodmap[r][c] = 0
        if (self.image == self.turn):
            self.write_png(foodmap)
            self.image = None

        def move_ant(self, ant, map):
            valid_moves = []
            for x,y in self.neighbors(ant):
                if map[x][y] > 0 and (x,y) not in orders:
                    valid_moves.append((map[x][y],(x,y))) 
            if valid_moves:
                valid_moves.sort()
                self.info("vmoves ant {0}:{1}".format(ant, valid_moves))
                nada, loc = valid_moves[0]
                ants.issue_order((ant, ants.direction(ant, loc)[0]))
                self.info("Moving ant {0} {1}".format(ant, ants.direction(ant, loc)))
                orders[loc] = ant

        for ant in ants.my_ants():
            move_ant(self, ant, foodmap)

        self.info("Turn over with {0}ms remaining".format(ants.time_remaining()))
        self.turn += 1





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
