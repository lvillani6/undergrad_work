"""
Much of this code comes from the Pacman project, credit to all parties who created that code.
Heuristic and Directions Enum class is original
"""
import heapq
from enum import Enum


# Utilities
class Directions(Enum):
	LEFT, RIGHT, UP, DOWN, NONE = range(5)


class PriorityQueue:
	"""
	Implements a priority queue data structure. Each inserted item
	has a priority associated with it and the client is usually interested
	in quick retrieval of the lowest-priority item in the queue. This
	data structure allows O(1) access to the lowest-priority item.
	"""

	def __init__(self):
		self.heap = []
		self.count = 0

	def push(self, item, priority):
		entry = (priority, self.count, item)
		heapq.heappush(self.heap, entry)
		self.count += 1

	def pop(self):
		(_, _, item) = heapq.heappop(self.heap)
		return item

	def isEmpty(self):
		return len(self.heap) == 0

	def update(self, item, priority):
		# If item already in priority queue with higher priority, update its priority and rebuild the heap.
		# If item already in priority queue with equal or lower priority, do nothing.
		# If item not in priority queue, do the same thing as self.push.
		for index, (p, c, i) in enumerate(self.heap):
			if i == item:
				if p <= priority:
					break
				del self.heap[index]
				self.heap.append((priority, c, item))
				heapq.heapify(self.heap)
				break
		else:
			self.push(item, priority)


class PriorityQueueWithFunction(PriorityQueue):
	"""
	Implements a priority queue with the same push/pop signature of the
	Queue and the Stack classes. This is designed for drop-in replacement for
	those two classes. The caller has to provide a priority function, which
	extracts each item's priority.
	"""

	def __init__(self, heuristic):
		""" priorityFunction (item) -> priority """
		self.priority_function = heuristic  # store the priority function
		PriorityQueue.__init__(self)  # super-class initializer

	def push(self, item):
		"Adds an item to the queue with priority from the priority function"
		PriorityQueue.push(self, item, self.priority_function(item))


def manhattan_distance(p1, p2):
	return abs(p1[0] - p2[0]) + abs(p1[1] - p2[1])


def euclidean_distance(p1, p2):
	return ((p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2) ** 0.5


class SearchProblem:
	def __init__(self, snake, start, end):
		self.snake = snake
		self.start = start
		self.end = end

	def get_start_state(self):
		"""
		Returns the start state for the search problem.
		"""
		return self.start
		# return self.snake.head.pos

	def get_goal_state(self):
		return self.end
		# return self.snack.pos

	def is_goal_state(self, state):
		"""
		state: Search state

		Returns True if and only if the state is a valid goal state.
		"""
		return state == self.end

	def get_successors(self, state):
		"""
		state: Search state

		For a given state, this should return a list of triples, (successor,
		action, stepCost), where 'successor' is a successor to the current
		state, 'action' is the action required to get there, and 'stepCost' is
		the incremental cost of expanding to that successor.
		"""
		successors = []
		succession = [(-1, 0, Directions.LEFT), (1, 0, Directions.RIGHT), (0, -1, Directions.UP), (0, 1, Directions.DOWN)]
		for dx, dy, direction in succession:
			next_x = (state[0] + dx)
			next_y = (state[1] + dy)
			successor = (next_x, next_y)
			# if is_within_bounds(successor):                    # use if you want to allow looping at edge of screen
			successors.append((successor, direction, 0))
		return successors

	# emergency function to get a viable move if no path is found, then it can retry
	def get_viable_successor(self, state):
		for successor in self.get_successors(state):
			if successor not in list(map(lambda z: z.pos, self.snake.body[:])):
				return successor
		return False


# can be used to prevent looping around the edge of the screen
def is_within_bounds(state):
	return 0 <= state[0] < 20 and 0 <= state[1] < 20


# Heuristic considers both manhattan and euclidean distance to snack, with max cost for moves that would cause the snake
# to collide with itself
def a_star_heuristic(path, problem):
	state = path[-1]
	current_path = path[:len(path) - 1]
	body_walls = list(map(lambda z: z.pos, problem.snake.body[:]))
	length = len(body_walls)
	path_walls = [ele for ele in reversed(current_path)][:len(current_path) - 1]
	walls = path_walls + body_walls
	walls = walls[:length]
	if state in walls:
		return float('inf')

	man_dis = manhattan_distance(state, problem.end)
	euc_dis = euclidean_distance(state, problem.end)
	val = man_dis + 1.2 * euc_dis
	return val


# Old heuristic, doesn't consider path in invalid spaces
# def a_star_heuristic(path, problem):
#     state = path[-1]
#     if state in list(map(lambda z: z.pos, problem.snake.body[:])):
#         return float('inf')
#     man_dis = manhattan_distance(state, problem.end)
#     euc_dis = euclidean_distance(state, problem.end)
#     val = man_dis+1.2*euc_dis
#     return val

# A* Algorithm, a general graph search using a priority queue for its fringe using heuristic for priority
def a_star_search(problem):
	"""Search the node that has the lowest combined cost and heuristic first."""
	"*** YOUR CODE HERE ***"
	"""A* priority queue -> fringe
		W/Fn: Defines cost function"""
	cost = lambda path: len(path) + a_star_heuristic([p[0] for p in path], problem)
	pqueue = PriorityQueueWithFunction(cost)  # A*: pqueue -> fringe
	return general_graph_search(problem, pqueue)


def general_graph_search(problem, structure):
	closed = []  # Nodes already explored
	structure.push([(problem.get_start_state(), Directions.NONE, 0)])  # Start by exploring the problem start state
	while True:
		if structure.isEmpty():  # If the structure is empty then there is no path
			return problem.get_viable_successor(
				problem.snake.head.pos)  # If no path to food, go to any viable direction and redo
		node = structure.pop()  # Explore the next node in the structure
		cur_state = node[-1][0]  # Gets the state (positional data) of the node
		if problem.is_goal_state(cur_state):  # If the state is the goal state, return moves
			return [x[1] for x in node][1:]
		if cur_state not in closed:  # If not already explored then it could lead to goal
			closed.append(cur_state)  # Denote as explored
			for child in problem.get_successors(cur_state):  # For each successor keep track of the path to it
				child_path = node[:]  # and explore each possible outward path
				child_path.append(child)
				structure.push(child_path)
