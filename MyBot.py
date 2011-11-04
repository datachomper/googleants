#!/usr/bin/env python
from ants import *
from heapq import heappush, heappop
import logging as log
import networkx as nx

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

    def do_setup(self, ants):
        # initialize data structures after learning the game settings
        self.unseen = []
        self.hills = []
        self.routes = {}
        for row in range(ants.rows):
            for col in range(ants.cols):
                self.unseen.append((row, col))

        # Define a base graph for obsticle avoidance
        self.basegraph = nx.grid_graph(dim=[ants.rows, ants.cols],
                                       periodic=True)
    
    # do turn is run once per turn
    def do_turn(self, ants):
        self.logger.info("Begin Turn")
        
        # Remove discovered WATER nodes from the base graph
        for row in range(ants.rows):
            for col in range(ants.cols):
                if (row,col) in self.basegraph and ants.map[row][col] == -4:
                    self.basegraph.remove_node((row,col))

        orders = {}
        def move_direction(loc, direction):
            # This library call takes care of map wrapping
            new_loc = ants.destination(loc, direction)
            if (ants.unoccupied(new_loc) and new_loc not in orders):
                ants.issue_order((loc, direction))
                orders[new_loc] = loc
                return True
            else:
                return False 

        # Returns manhattan distance
        def manhattan_dist(a, b):
            (x1, y1) = a
            (x2, y2) = b
            return ((x1-x2)**2 + (y1-y2)**2)**0.5

        targets = {}
        # Keep track of one target moving towards a location
        # and moves the ant in that general direction
        # start == finish == tuple with x,y map coords
        def move_location(start, finish):
            if start == finish:
                return False

            if finish in self.routes and start in self.routes[finish]:
                # We've already made this path bro! Use that shit!
                idx = self.routes[finish].index(start)
                step = self.routes[finish][idx+1]
            else:
                # A* seems to take about 10ms per path on avg
                path = nx.astar_path(self.basegraph, start, finish, manhattan_dist)
                self.routes[finish] = path
                step = path[1]
            if move_direction(start, ants.direction(start, step)[0]):
                targets[finish] = start
                return True
            else:
                return False

        # Don't move onto an anthill ever
        for hill_loc in ants.my_hills():
            orders[hill_loc] = None

        # First move priority
        # Locate all visible food and send one ant towards it
        ant_dist = []
        for food_loc in ants.food():
            for ant_loc in ants.my_ants():
                dist = ants.distance(ant_loc, food_loc)
                ant_dist.append((dist, ant_loc, food_loc))
        ant_dist.sort()

        for dist, ant_loc, food_loc in ant_dist:
            if food_loc not in targets and ant_loc not in targets.values():
                self.logger.info("Moving {0} to food {1}".format(ant_loc, food_loc))
                move_location(ant_loc, food_loc)

        # Second move priority
        # Attack enemy hills
#        for hill_loc, hill_owner in ants.enemy_hills():
#            if hill_loc not in self.hills:
#                self.hills.append(hill_loc)
#
#        ant_dist = []
#        for hill_loc in self.hills:
#            for ant_loc in ants.my_ants():
#                if ant_loc not in orders.values():
#                    dist = ants.distance(ant_loc, hill_loc)
#                    ant_dist.append((dist, ant_loc))
#        ant_dist.sort()
#        for dist, ant_loc in ant_dist:
#            self.logger.info("Moving {0} to hill {1}".format(ant_loc, hill_loc))
#            move_location(ant_loc, hill_loc)

        # Third move priority
        # Explore the map to reveal non-visible areas
        for loc in self.unseen[:]:
            if ants.visible(loc):
                self.unseen.remove(loc)

        for ant_loc in ants.my_ants():
            tries = 0
            if ant_loc not in orders.values():
                unseen_dist = []
                for unseen_loc in self.unseen:
                    dist = ants.distance(ant_loc, unseen_loc)
                    unseen_dist.append((dist, unseen_loc))
                unseen_dist.sort()
                for dist, unseen_loc in unseen_dist:
                    self.logger.info("Moving {0} to explore {1}".format(ant_loc, unseen_loc))
                    if move_location(ant_loc, unseen_loc) or tries < 4:
                        break
                    else:
                        tries += 1

        # Last move priority
        # Move off the anthill if we are blocking it
        for hill_loc in ants.my_hills():
            if hill_loc in ants.my_ants() and hill_loc not in orders.values():
                for direction in ('s', 'e', 'w', 'n'):
                    if move_direction(hill_loc, direction):
                        self.logger.info("Moving {0} to off anthill {1}".format(ant_loc, direction))
                        break
            
        self.logger.info("Turn over with {0}ms remaining".format(ants.time_remaining()))

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
