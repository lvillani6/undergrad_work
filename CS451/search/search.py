# search.py
# ---------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
#
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


"""
In search.py, you will implement generic search algorithms which are called by
Pacman agents (in searchAgents.py).
"""

import util

class SearchProblem:
    """
    This class outlines the structure of a search problem, but doesn't implement
    any of the methods (in object-oriented terminology: an abstract class).

    You do not need to change anything in this class, ever.
    """

    def getStartState(self):
        """
        Returns the start state for the search problem.
        """
        util.raiseNotDefined()

    def isGoalState(self, state):
        """
          state: Search state

        Returns True if and only if the state is a valid goal state.
        """
        util.raiseNotDefined()

    def getSuccessors(self, state):
        """
          state: Search state

        For a given state, this should return a list of triples, (successor,
        action, stepCost), where 'successor' is a successor to the current
        state, 'action' is the action required to get there, and 'stepCost' is
        the incremental cost of expanding to that successor.
        """
        util.raiseNotDefined()

    def getCostOfActions(self, actions):
        """
         actions: A list of actions to take

        This method returns the total cost of a particular sequence of actions.
        The sequence must be composed of legal moves.
        """
        util.raiseNotDefined()


def tinyMazeSearch(problem):
    """
    Returns a sequence of moves that solves tinyMaze.  For any other maze, the
    sequence of moves will be incorrect, so only use this for tinyMaze.
    """
    from game import Directions
    s = Directions.SOUTH
    w = Directions.WEST
    return  [s, s, w, s, w, w, s, w]

def depthFirstSearch(problem):
    """
    Search the deepest nodes in the search tree first.

    Your search algorithm needs to return a list of actions that reaches the
    goal. Make sure to implement a graph search algorithm.

    To get started, you might want to try some of these simple commands to
    understand the search problem that is being passed in:
    """
    # print("Start:", problem.getStartState())
    # print("Is the start a goal?", problem.isGoalState(problem.getStartState()))
    # print("Start's successors:", problem.getSuccessors(problem.getStartState()))
    "*** YOUR CODE HERE ***"
    stack = util.Stack()            # DFS: stack -> fringe
    return generalGraphSearch(problem, stack)

def breadthFirstSearch(problem):
    """Search the shallowest nodes in the search tree first."""
    "*** YOUR CODE HERE ***"
    queue = util.Queue()            # BFS: queue -> fringe
    return generalGraphSearch(problem, queue)

def uniformCostSearch(problem):
    """Search the node of least total cost first."""
    "*** YOUR CODE HERE ***"
    """UCS priority queue -> fringe
        W/Fn: Defines cost function"""
    cost = lambda path: problem.getCostOfActions(x[1] for x in path[1:])
    pqueue = util.PriorityQueueWithFunction(cost)  # UCS: pqueue -> fringe
    return generalGraphSearch(problem, pqueue)

def nullHeuristic(state, problem=None):
    """
    A heuristic function estimates the cost from the current state to the nearest
    goal in the provided SearchProblem.  This heuristic is trivial.
    """
    return 0

def aStarSearch(problem, heuristic=nullHeuristic):
    """Search the node that has the lowest combined cost and heuristic first."""
    "*** YOUR CODE HERE ***"
    """A* priority queue -> fringe
        W/Fn: Defines cost function"""
    # print(problem)
    cost = lambda path: problem.getCostOfActions(x[1] for x in path[1:]) + heuristic(path[-1][0], problem)
    pqueue = util.PriorityQueueWithFunction(cost)   # A*: pqueue -> fringe
    return generalGraphSearch(problem, pqueue)
    # util.raiseNotDefined()

"""
Parameters: problem     = A search problem
            structure   = A data structure that stores frontier nodes and defines search behavior
Returns:    List of actions to the problem goal state OR False (fails)
"""
def generalGraphSearch(problem, structure):
    closed = []                                             # Nodes already explored
    structure.push([(problem.getStartState(),'Stop',0)])    # Start by exploring the prolem start state
    while(True):
        if(structure.isEmpty()):                            # If the structure is empty then there is no path
            return False
        node = structure.pop()                              # Explore the next node in the structure
        curState = node[-1][0]                              # Gets the state (positional data) of the node
        if(problem.isGoalState(curState)):                  # If the state is the goal state, return moves
            return [x[1] for x in node][1:]
        if(curState not in closed):                         # If not already explored then it could lead to goal
            closed.append(curState)                         # Denote as explored
            for child in problem.getSuccessors(curState):   # For each successor keep track of the path to it
                childPath = node[:]                         #   and explore each possible outward path
                childPath.append(child)
                structure.push(childPath)


# Abbreviations
bfs = breadthFirstSearch
dfs = depthFirstSearch
astar = aStarSearch
ucs = uniformCostSearch
