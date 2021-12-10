# multiAgents.py
# --------------
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


from util import manhattanDistance
from game import Directions
import random, util

from game import Agent

class ReflexAgent(Agent):
    """
    A reflex agent chooses an action at each choice point by examining
    its alternatives via a state evaluation function.

    The code below is provided as a guide.  You are welcome to change
    it in any way you see fit, so long as you don't touch our method
    headers.
    """


    def getAction(self, gameState):
        """
        You do not need to change this method, but you're welcome to.

        getAction chooses among the best options according to the evaluation function.

        Just like in the previous project, getAction takes a GameState and returns
        some Directions.X for some X in the set {NORTH, SOUTH, WEST, EAST, STOP}
        """
        # Collect legal moves and successor states
        legalMoves = gameState.getLegalActions()

        # Choose one of the best actions
        scores = [self.evaluationFunction(gameState, action) for action in legalMoves]
        bestScore = max(scores)
        bestIndices = [index for index in range(len(scores)) if scores[index] == bestScore]
        chosenIndex = random.choice(bestIndices) # Pick randomly among the best

        "Add more of your code here if you want to"

        return legalMoves[chosenIndex]

    def evaluationFunction(self, currentGameState, action):
        """
        Design a better evaluation function here.

        The evaluation function takes in the current and proposed successor
        GameStates (pacman.py) and returns a number, where higher numbers are better.

        The code below extracts some useful information from the state, like the
        remaining food (newFood) and Pacman position after moving (newPos).
        newScaredTimes holds the number of moves that each ghost will remain
        scared because of Pacman having eaten a power pellet.

        Print out these variables to see what you're getting, then combine them
        to create a masterful evaluation function.
        """
        # Useful information you can extract from a GameState (pacman.py)
        successorGameState = currentGameState.generatePacmanSuccessor(action)
        newPos = successorGameState.getPacmanPosition()
        newFood = successorGameState.getFood()
        newGhostStates = successorGameState.getGhostStates()
        newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]

        "*** YOUR CODE HERE ***"
        # Uses distance to nearest food and to ghosts to determine value
        # Prioritizing capsules lets you ignore ghosts
        newFoodList = newFood.asList()
        oldFoodLen = currentGameState.getFood().count()
        newFoodLen = len(newFoodList)
        if newFoodLen == oldFoodLen:
            foodValue = 1000
            for food in newFoodList:
                toFood = manhattanDistance(food, newPos)
                if toFood < foodValue:
                    foodValue = toFood
        else:
            foodValue = 0

        oldCap = currentGameState.getCapsules()
        newCap = successorGameState.getCapsules()
        if len(newCap) == len(oldCap) and not len(oldCap) == 0:
            capValue = 1000
            for cap in newCap:
                toCap = manhattanDistance(cap, newPos)
                if toCap < capValue:
                    capValue = toCap
        else:
            capValue = 0

        # Ratio of how much cap is favored: 0-1
        capRatio = 2/3      # Seems to be optimal at least for q1
        value = ((1-capRatio)*foodValue) + (capRatio*capValue)

        for ghost in newGhostStates:
            if ghost.scaredTimer == 0:
                value += 4 ** (2 - manhattanDistance(ghost.getPosition(), newPos))
        return -value

def scoreEvaluationFunction(currentGameState):
    """
    This default evaluation function just returns the score of the state.
    The score is the same one displayed in the Pacman GUI.

    This evaluation function is meant for use with adversarial search agents
    (not reflex agents).
    """
    return currentGameState.getScore()

class MultiAgentSearchAgent(Agent):
    """
    This class provides some common elements to all of your
    multi-agent searchers.  Any methods defined here will be available
    to the MinimaxPacmanAgent, AlphaBetaPacmanAgent & ExpectimaxPacmanAgent.

    You *do not* need to make any changes here, but you can if you want to
    add functionality to all your adversarial search agents.  Please do not
    remove anything, however.

    Note: this is an abstract class: one that should not be instantiated.  It's
    only partially specified, and designed to be extended.  Agent (game.py)
    is another abstract class.
    """

    def __init__(self, evalFn = 'scoreEvaluationFunction', depth = '2'):
        self.index = 0 # Pacman is always agent index 0
        self.evaluationFunction = util.lookup(evalFn, globals())
        self.depth = int(depth)

class MinimaxAgent(MultiAgentSearchAgent):
    """
    Your minimax agent (question 2)
    """

    def getAction(self, gameState):
        """
        Returns the minimax action from the current gameState using self.depth
        and self.evaluationFunction.

        Here are some method calls that might be useful when implementing minimax.

        gameState.getLegalActions(agentIndex):
        Returns a list of legal actions for an agent
        agentIndex=0 means Pacman, ghosts are >= 1

        gameState.generateSuccessor(agentIndex, action):
        Returns the successor game state after an agent takes an action

        gameState.getNumAgents():
        Returns the total number of agents in the game

        gameState.isWin():
        Returns whether or not the game state is a winning state

        gameState.isLose():
        Returns whether or not the game state is a losing state
        """
        "*** YOUR CODE HERE ***"
        def maxValue(state, depth):
            if state.isWin() or state.isLose():
                return state.getScore()

            move = Directions.STOP
            max_value = float('-inf')
            for action in state.getLegalActions(0):
                value = minValue(state.generateSuccessor(0, action), depth, 1)    # Can't use getScore above max depth
                if value > max_value:
                    max_value = value
                    move = action
            if depth == 0:
                return move
            else:
                return max_value


        def minValue(state, depth, agent):
            if state.isLose() or state.isWin():
                return state.getScore()

            next_agent = (agent + 1) % state.getNumAgents()

            min_value = float('inf')
            for action in state.getLegalActions(agent):
                successor = state.generateSuccessor(agent, action)
                if next_agent == 0:
                    if depth == self.depth - 1:
                        value = self.evaluationFunction(successor)
                    else:
                        value = maxValue(successor, depth + 1)
                else:
                    value = minValue(successor, depth, next_agent)

                if value < min_value:
                    min_value = value
            return min_value

        return maxValue(gameState, 0)

class AlphaBetaAgent(MultiAgentSearchAgent):
    """
    Your minimax agent with alpha-beta pruning (question 3)
    """

    def getAction(self, gameState):
        """
        Returns the minimax action using self.depth and self.evaluationFunction
        """
        "*** YOUR CODE HERE ***"
        def maxValue(state, alpha, beta, depth):
            if state.isWin() or state.isLose():
                return state.getScore()

            move = Directions.STOP
            max_value = float('-inf')
            for action in state.getLegalActions(0):
                value = minValue(state.generateSuccessor(0, action), alpha, beta, depth, 1)    # Can't use getScore above max depth
                if value > max_value:
                    max_value = value
                    move = action
                if max_value > beta:
                    return max_value
                else:
                    alpha = max(alpha, max_value)
            if depth == 0:
                return move
            else:
                return max_value


        def minValue(state, alpha, beta, depth, agent):
            if state.isLose() or state.isWin():
                return state.getScore()

            next_agent = (agent + 1) % state.getNumAgents()

            min_value = float('inf')
            for action in state.getLegalActions(agent):
                successor = state.generateSuccessor(agent, action)
                if next_agent == 0:
                    if depth == self.depth - 1:
                        value = self.evaluationFunction(successor)
                    else:
                        value = maxValue(successor, alpha, beta, depth + 1)
                else:
                    value = minValue(successor, alpha, beta, depth, next_agent)

                if value < min_value:
                    min_value = value
                if min_value < alpha:
                    return min_value
                else:
                    beta = min(beta, min_value)
            return min_value

        return maxValue(gameState, float('-inf'), float('inf'), 0)

class ExpectimaxAgent(MultiAgentSearchAgent):
    """
      Your expectimax agent (question 4)
    """

    def getAction(self, gameState):
        """
        Returns the expectimax action using self.depth and self.evaluationFunction

        All ghosts should be modeled as choosing uniformly at random from their
        legal moves.
        """
        "*** YOUR CODE HERE ***"
        def maxValue(state, depth):
            if state.isWin() or state.isLose():
                return state.getScore()

            move = Directions.STOP
            max_value = float('-inf')
            for action in state.getLegalActions(0):
                value = expValue(state.generateSuccessor(0, action), depth, 1)    # Can't use getScore above max depth
                if value > max_value:
                    max_value = value
                    move = action
            if depth == 0:
                return move
            else:
                return max_value


        def expValue(state, depth, agent):
            if state.isLose() or state.isWin():
                return state.getScore()

            next_agent = (agent + 1) % state.getNumAgents()

            exp_value = 0
            actions = state.getLegalActions(agent)
            for action in actions:
                p = 1.0/len(actions)        # Since the ghost just moves randomly the probability is essentially the same for all
                successor = state.generateSuccessor(agent, action)
                if next_agent == 0:
                    if depth == self.depth - 1:
                        value = self.evaluationFunction(successor)
                        exp_value += p*value
                    else:
                        value = maxValue(successor, depth + 1)
                        exp_value += p*value
                else:
                    value = expValue(successor, depth, next_agent)
                    exp_value += p*value
            return exp_value

        return maxValue(gameState, 0)

def betterEvaluationFunction(currentGameState):
    """
    Your extreme ghost-hunting, pellet-nabbing, food-gobbling, unstoppable
    evaluation function (question 5).

    DESCRIPTION: <write something here so we know what you did>
    This algorithm is based off of a few concepts:
    1   The only way to win is to eat all of the food.
    2   Touching a ghost means losing.
    3   Eating a power capsule lets you ignore ghosts.

    Since we can't look ahead a state we have to assume that having a ghost
    right next to pacman means losing.

    This results in a minimum score.

    Otherwise the value of the state is a linear combination of the number of
    caps and food left and the distance to the nearest food, subtracted from the
    score.

    Including distance to nearest food provides incentive to pursue food even if
    there is none immediately nearby.

    Including the number of food at a higher weight encourages eating adjacent
    food before food at a distance.

    Including the number of capsules provides incentive to eat if nearby but
    since they don't cause a win, we don't seek them out at great distance.
    """
    "*** YOUR CODE HERE ***"
    pos = currentGameState.getPacmanPosition()
    score = currentGameState.getScore()
    food = currentGameState.getFood().asList()
    ghosts = currentGameState.getGhostStates()
    caps = currentGameState.getCapsules()
    dis = util.manhattanDistance

    food_dis = min([dis(f, pos) for f in food])
    ghost_dis = min([dis(g.getPosition(), pos) if g.scaredTimer == 0 else float('inf') for g in ghosts])
    if ghost_dis < 2:
        return float('-inf')

    factors = [score, len(caps), len(food), food_dis]
    weights = [1, -5, -2, -1]
    return sum([f*w for f,w in zip(factors,weights)])

# Abbreviation
better = betterEvaluationFunction
