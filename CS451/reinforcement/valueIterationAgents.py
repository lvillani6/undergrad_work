# valueIterationAgents.py
# -----------------------
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


# valueIterationAgents.py
# -----------------------
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


import mdp, util

from learningAgents import ValueEstimationAgent
import collections

class ValueIterationAgent(ValueEstimationAgent):
    """
        * Please read learningAgents.py before reading this.*

        A ValueIterationAgent takes a Markov decision process
        (see mdp.py) on initialization and runs value iteration
        for a given number of iterations using the supplied
        discount factor.
    """
    def __init__(self, mdp, discount = 0.9, iterations = 100):
        """
          Your value iteration agent should take an mdp on
          construction, run the indicated number of iterations
          and then act according to the resulting policy.

          Some useful mdp methods you will use:
              mdp.getStates()
              mdp.getPossibleActions(state)
              mdp.getTransitionStatesAndProbs(state, action)
              mdp.getReward(state, action, nextState)
              mdp.isTerminal(state)
        """
        self.mdp = mdp
        self.discount = discount
        self.iterations = iterations
        self.values = util.Counter() # A Counter is a dict with default 0
        self.runValueIteration()

    def runValueIteration(self):
        # Write value iteration code here
        "*** YOUR CODE HERE ***"
        for i in range (self.iterations):
            values = self.values.copy()
            for state in self.mdp.getStates():
                self.values[state] = -float('inf')
                for action in self.mdp.getPossibleActions(state):
                    value = 0
                    for successor,prob in self.mdp.getTransitionStatesAndProbs(state, action):
                        value += prob * (self.mdp.getReward(state, action, successor) + self.discount * values[successor])  # Value iteration state update function
                    self.values[state] = max(self.values[state], value)
                if self.values[state] == -float('inf'):
                    self.values[state] = 0.0


    def getValue(self, state):
        """
          Return the value of the state (computed in __init__).
        """
        return self.values[state]


    def computeQValueFromValues(self, state, action):
        """
          Compute the Q-value of action in state from the
          value function stored in self.values.
        """
        "*** YOUR CODE HERE ***"
        value = 0
        for successor, prob in self.mdp.getTransitionStatesAndProbs(state, action):
            value += prob * (self.mdp.getReward(state, action, successor) + self.discount * self.values[successor])  # Value iteration state update function
        return value

    def computeActionFromValues(self, state):
        """
          The policy is the best action in the given state
          according to the values currently stored in self.values.

          You may break ties any way you see fit.  Note that if
          there are no legal actions, which is the case at the
          terminal state, you should return None.
        """
        "*** YOUR CODE HERE ***"
        if self.mdp.isTerminal(state):
            return None
        maxnum = -float('inf')
        best_action = None
        for action in self.mdp.getPossibleActions(state):
            value = self.computeQValueFromValues(state, action)
            if value > maxnum:
                maxnum = value
                best_action = action
        return best_action

    def getPolicy(self, state):
        return self.computeActionFromValues(state)

    def getAction(self, state):
        "Returns the policy at the state (no exploration)."
        return self.computeActionFromValues(state)

    def getQValue(self, state, action):
        return self.computeQValueFromValues(state, action)

class AsynchronousValueIterationAgent(ValueIterationAgent):
    """
        * Please read learningAgents.py before reading this.*

        An AsynchronousValueIterationAgent takes a Markov decision process
        (see mdp.py) on initialization and runs cyclic value iteration
        for a given number of iterations using the supplied
        discount factor.
    """
    def __init__(self, mdp, discount = 0.9, iterations = 1000):
        """
          Your cyclic value iteration agent should take an mdp on
          construction, run the indicated number of iterations,
          and then act according to the resulting policy. Each iteration
          updates the value of only one state, which cycles through
          the states list. If the chosen state is terminal, nothing
          happens in that iteration.

          Some useful mdp methods you will use:
              mdp.getStates()
              mdp.getPossibleActions(state)
              mdp.getTransitionStatesAndProbs(state, action)
              mdp.getReward(state)
              mdp.isTerminal(state)
        """
        ValueIterationAgent.__init__(self, mdp, discount, iterations)

    def runValueIteration(self):
        states = self.mdp.getStates()
        for i in range(self.iterations):
            state = states[i %  len(states)]
            action = self.computeActionFromValues(state)
            self.values[state] = 0 if action is None else self.computeQValueFromValues(state, action)

class PrioritizedSweepingValueIterationAgent(AsynchronousValueIterationAgent):
    """
        * Please read learningAgents.py before reading this.*

        A PrioritizedSweepingValueIterationAgent takes a Markov decision process
        (see mdp.py) on initialization and runs prioritized sweeping value iteration
        for a given number of iterations using the supplied parameters.
    """
    def __init__(self, mdp, discount = 0.9, iterations = 100, theta = 1e-5):
        """
          Your prioritized sweeping value iteration agent should take an mdp on
          construction, run the indicated number of iterations,
          and then act according to the resulting policy.
        """
        self.theta = theta
        ValueIterationAgent.__init__(self, mdp, discount, iterations)

    def qValues(self, state):
        values = util.Counter()
        for action in self.mdp.getPossibleActions(state):
            values[action] = self.computeQValueFromValues(state, action)
        return values

    def runValueIteration(self):
        "*** YOUR CODE HERE ***"
        states = self.mdp.getStates()
        predecessors = dict()
        for state in states:
            predecessors[state] = set()
        for state in states:                        # Compute predecessors of all states
            for action in self.mdp.getPossibleActions(state):
                for successor, prob in self.mdp.getTransitionStatesAndProbs(state, action):
                    if prob > 0:
                        predecessors[successor].add(state)

        pqueue = util.PriorityQueue()               # Initialize an empty priority queue

        for state in states:
            qvalues = self.qValues(state)
            if len(qvalues) > 0:                    # Terminal states have no q values.
                diff = abs(self.values[state] - qvalues[qvalues.argMax()])  # Find the abs of the difference between current and max q values
                pqueue.push(state, -diff)           # Push s into the priority queue with priority -diff.

        for i in range(self.iterations):            # For iteration in self.iterations, do:
            if pqueue.isEmpty():                    # If the priority queue is empty then terminate
                return
            state = pqueue.pop()                    # Pop a state of the priority queue
            qvalues = self.qValues(state)
            self.values[state] = qvalues[qvalues.argMax()]      # Updates s's value in self.values
            for pred in predecessors[state]:                    # for each predecessor p of s do:
                predQvalues = self.qValues(pred)
                diff = abs(self.values[pred] - predQvalues[predQvalues.argMax()])  # Find the abs of the difference between current and max q values
                if diff > self.theta:
                    pqueue.update(pred, -diff)
