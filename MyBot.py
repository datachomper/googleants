#!/usr/bin/env python
from ants import *
from heapq import heappush, heappop

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
        # start == finish == tuple with x,y map coords
        def move_location(start, finish):
            # A* algorithm for pathfinding
            # Adapted from http://theory.stanford.edu/~amitp/GameProgramming/ImplementationNotes.html
            olist = []
            clist = []
            parent = {}
            
            # Push start onto the open list
            # Everything in the heap needs to have F,G,H appended
            # and F has to be specified, as the heap algorithm uses the
            # leftmost value to sort
            heappush(olist, (0,0,0,start))
            parent[start] = None

            # While lowest rank in olist is not the finish
            while olist:
                # Removed lowest rank item from open list
                current = heappop(olist)
                if current[3] == finish:
                    # We're done, return the path
                    break

                # Add current to the closed list
                heappush(clist, current)

                # For neighbors of current
                for direction in ['n','e','s','w']:
                    # Take care of map wrapping
                    neighbor_loc = ants.destination(current[3], direction)

                    # Ignore invalid adjacent moves
                    if not ants.unoccupied(neighbor_loc) :
                        continue

                    # cost = G of current + movement cost to the neighbor
                    # set movement cost to 1 for now, maybe make this dynamic later
                    cost = current[1] + 1

                    # ants.distance() returns the manhattan distance which
                    # should be suitable for a square grid with only four moves
                    neighbor_g = cost
                    neighbor_h = ants.distance(neighbor_loc, finish)
                    neighbor = (neighbor_h + neighbor_g, neighbor_g, neighbor_h, neighbor_loc)


                    if (neighbor in olist and cost < neighbor[1]):
                        # remove neighbor from olist because new path is better
                        olist.remove(neighbor)
                    if (neighbor in clist and cost < neighbor[1]):
                        # remove neighbor from clist
                        clist.remove(neighbor)
                    if (neighbor not in olist and neighbor not in clist):
                        # add neighbor to olist
                        heappush(olist, neighbor)
                        # set neighbor parent to current
                        parent[neighbor[3]] = current[3]
                
            # Reconstruct reverse path by following parents
            reversepath = []
            node = current[3]
            while node is not start:
                reversepath.push(parent[node])
                node = parent[node]

            # reversepath[0] should be the next move
            move_direction(start, reversepath[0])

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
