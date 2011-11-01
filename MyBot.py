#!/usr/bin/env python
from ants import *

# define a class with a do_turn method
# the Ants.run method will parse and update bot input
# it will also run the do_turn method for us
class MyBot:
    def __init__(self):
        # define class level variables, will be remembered between turns
        pass
    
    # do_setup is run once at the start of the game
    # after the bot has received the game settings
    # the ants class is created and setup by the Ants.run method
    def do_setup(self, ants):
        # initialize data structures after learning the game settings
        self.unseen = []
        self.hills = []
        for row in range(ants.rows):
            for col in range(ants.cols):
                self.unseen.append((row, col))
    
    # do turn is run once per turn
    # the ants class has the game state and is updated by the Ants.run method
    # it also has several helper methods to use
    def do_turn(self, ants):
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

        targets = {}
        # Keep track of one target moving towards a location
        # and moves the ant in that general direction
        def move_location(loc, dest):
            directions = ants.direction(loc, dest)
            for direction in directions:
                if move_direction(loc, direction):
                    targets[dest] = loc
                    return True
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
                move_location(ant_loc, food_loc)

        # Second move priority
        # Attack enemy hills
        for hill_loc, hill_owner in ants.enemy_hills():
            if hill_loc not in self.hills:
                self.hills.append(hill_loc)

        ant_dist = []
        for hill_loc in self.hills:
            for ant_loc in ants.my_ants():
                if ant_loc not in orders.values():
                    dist = ants.distance(ant_loc, hill_loc)
                    ant_dist.append((dist, ant_loc))
        ant_dist.sort()
        for dist, ant_loc in ant_dist:
            move_location(ant_loc, hill_loc)

        # Third move priority
        # Explore the map to reveal non-visible areas
        for loc in self.unseen[:]:
            if ants.visible(loc):
                self.unseen.remove(loc)

        for ant_loc in ants.my_ants():
            if ant_loc not in orders.values():
                unseen_dist = []
                for unseen_loc in self.unseen:
                    dist = ants.distance(ant_loc, unseen_loc)
                    unseen_dist.append((dist, unseen_loc))
                unseen_dist.sort()
                for dist, unseen_loc in unseen_dist:
                    if move_location(ant_loc, unseen_loc):
                        break

        # Last move priority
        # Move off the anthill if we are blocking it
        for hill_loc in ants.my_hills():
            if hill_loc in ants.my_ants() and hill_loc not in orders.values():
                for direction in ('s', 'e', 'w', 'n'):
                    if move_direction(hill_loc, direction):
                        break
            

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
