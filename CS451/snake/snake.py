# Lorenzo Villani
import sys
import time
import copy
import random
import pygame
import tkinter as tk
from tkinter import messagebox
import search
from search import Directions

# globals
rows = 20
width = 500

red = (220, 0, 0)
green = (0, 200, 0)
sky = (0, 190, 255)
gray = (127, 127, 127)
charcoal = (25, 25, 25)
white = (255, 255, 255)
black = (0, 0, 0)


# Keeps track of when the player presses escape so the game can be exited
class Flag:
	def __init__(self):
		self.flag = True

	def set_flag(self, b):
		self.flag = b

	def get_flag(self):
		return self.flag


# A cube to represent each piece of the snake's body and the snack
class Cube(object):
	def __init__(self, start, dir_x=1, dir_y=0, color=red):
		self.pos = start
		self.dir_x = dir_x
		self.dir_y = dir_y
		self.color = color

	# Moves the cube on the screen, wraps around the edges
	def move(self, dir_x, dir_y):
		self.dir_x = dir_x
		self.dir_y = dir_y
		pos = [self.pos[0] + self.dir_x, self.pos[1] + self.dir_y]
		for i in [0, 1]:
			pos[i] %= 20
		self.pos = tuple(pos)

	# Draws cube in window
	def draw(self, surface, eyes=False):
		dis = width // rows
		i = self.pos[0]
		j = self.pos[1]

		pygame.draw.rect(surface, self.color, (i * dis + 1, j * dis + 1, dis - 2, dis - 2))
		if eyes:
			center = dis // 2
			radius = 3
			circle_middle = (i * dis + center - radius, j * dis + 8)
			circle_middle2 = (i * dis + dis - radius * 2, j * dis + 8)
			pygame.draw.circle(surface, (0, 0, 0), circle_middle, radius)
			pygame.draw.circle(surface, (0, 0, 0), circle_middle2, radius)


# Represents the snake
class Snake(object):
	body = []
	turns = {}

	def __init__(self, color, pos):
		self.color = color
		self.head = Cube(pos)
		self.body.append(self.head)
		self.dir_x = 0
		self.dir_y = 1

	# Move the snake on screen either from key presses in manual mode or from algorithm in AI mode
	def move(self, flag, mode=0, direction=None):
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				pygame.quit()
		keys = pygame.key.get_pressed()

		# Stops game if escape is pressed
		if keys[pygame.K_ESCAPE]:
			flag.set_flag(False)

		# get direction from keys pressed
		if mode == 0:
			if keys[pygame.K_LEFT]:
				direction = Directions.LEFT
			elif keys[pygame.K_RIGHT]:
				direction = Directions.RIGHT
			elif keys[pygame.K_UP]:
				direction = Directions.UP
			elif keys[pygame.K_DOWN]:
				direction = Directions.DOWN

		# sets movement direction and prevents snake from moving back into itself
		if direction == Directions.LEFT and self.dir_x != 1:
			self.dir_x = -1
			self.dir_y = 0
			self.turns[self.head.pos[:]] = [self.dir_x, self.dir_y]
		elif direction == Directions.RIGHT and self.dir_x != -1:
			self.dir_x = 1
			self.dir_y = 0
			self.turns[self.head.pos[:]] = [self.dir_x, self.dir_y]
		elif direction == Directions.UP and self.dir_y != 1:
			self.dir_x = 0
			self.dir_y = -1
			self.turns[self.head.pos[:]] = [self.dir_x, self.dir_y]
		elif direction == Directions.DOWN and self.dir_y != -1:
			self.dir_x = 0
			self.dir_y = 1
			self.turns[self.head.pos[:]] = [self.dir_x, self.dir_y]
		elif direction == Directions.NONE:
			self.turns[self.head.pos[:]] = [self.dir_x, self.dir_y]

		for i, c in enumerate(self.body):
			p = c.pos[:]
			if p in self.turns:
				turn = self.turns[p]
				c.move(turn[0], turn[1])
				if i == len(self.body) - 1:
					self.turns.pop(p)
			else:
				c.move(c.dir_x, c.dir_y)

	# resets snake position
	def reset(self, pos):
		self.head = Cube(pos)
		self.body = []
		self.body.append(self.head)
		self.dir_x = 0
		self.dir_y = 1

	def add_cube(self):
		tail = self.body[-1]
		dx, dy = tail.dir_x, tail.dir_y

		if dx == 1 and dy == 0:
			self.body.append(Cube((tail.pos[0] - 1, tail.pos[1])))
		elif dx == -1 and dy == 0:
			self.body.append(Cube((tail.pos[0] + 1, tail.pos[1])))
		elif dx == 0 and dy == 1:
			self.body.append(Cube((tail.pos[0], tail.pos[1] - 1)))
		elif dx == 0 and dy == -1:
			self.body.append(Cube((tail.pos[0], tail.pos[1] + 1)))

		self.body[-1].dir_x = dx
		self.body[-1].dir_y = dy

	# draws snake in window
	def draw(self, surface):
		for i, c in enumerate(self.body):
			if i == 0:
				c.draw(surface, True)
			else:
				c.draw(surface)


# draws grid in window
def draw_grid(w, r, surface):
	size_between = w // r

	x = 0
	y = 0
	for i in range(r):
		x += size_between
		y += size_between

		pygame.draw.line(surface, gray, (x, 0), (x, w))
		pygame.draw.line(surface, gray, (0, y), (w, y))


# redraws window for each frame of the game
def redraw_window(surface, snake, snack):
	global rows, width
	surface.fill(charcoal)
	snake.draw(surface)
	snack.draw(surface)
	draw_grid(width, rows, surface)
	pygame.display.update()


# generates new random snack
def random_snack(r, item):
	positions = item.body
	while True:
		x = random.randrange(r)
		y = random.randrange(r)
		if len(list(filter(lambda z: z.pos == (x, y), positions))) > 0:
			continue
		else:
			break

	return x, y


# message window to notify player of score
def message_box(subject, content):
	root = tk.Tk()
	root.attributes("-topmost", True)
	root.withdraw()
	messagebox.showinfo(subject, content)
	root.destroy()


# runs game
def run(mode=0):
	global rows, width

	tick = 0
	if mode == 0:
		tick = 12
	elif mode == 1:
		tick = 20
	elif mode == 2:
		tick = 100

	win = pygame.display.set_mode((width, width))
	pygame.display.set_caption('Snake AI')
	s = Snake(red, (10, 10))
	snack = Cube(random_snack(rows, s), color=green)
	b = Bot(s)
	b.set_snack(snack)
	b.get_path()

	flag = Flag()

	clock = pygame.time.Clock()

	start = time.time()
	n = 5 if mode == 2 else 1
	i = 0
	while flag.get_flag():
		# pygame.time.delay(50)
		clock.tick(tick)
		s.move(flag) if mode == 0 else s.move(flag, mode, b.get_move())
		if s.body[0].pos == snack.pos:
			s.add_cube()
			snack = Cube(random_snack(rows, s), color=green)
			b.set_snack(snack)
			b.get_path()

		for x in range(len(s.body)):
			if s.body[x].pos in list(map(lambda z: z.pos, s.body[x + 1:])):
				message = 'Score: ' + str(len(s.body))
				if len(s.body) == rows ** 2:
					message_box('You Win!', message)
					return time.time() - start
				message += '\nPlay again...'
				message_box('You Lose!', message)
				s.reset((10, 10))
				start = time.time()
				break
		if i % n == 0:
			redraw_window(win, s, snack)
		i += 1


# calls run and sets mode from cli
def main():
	if len(sys.argv) == 2:
		mode = int(sys.argv[1])
	else:
		mode = 0
	win_time = run(mode)
	if win_time is not None:
		print("Victory in " + str(win_time))


# =========== AI ==============
# this class provides the AI functionality, in particular running A* search on each snack
class Bot(object):
	def __init__(self, snake):
		self.snake = snake
		self.snack = None
		self.moves = []

	def set_snack(self, current_snack):
		self.snack = current_snack

	# runs A* and gets sequence if moves
	def get_path(self):
		problem = search.SearchProblem(copy.copy(self.snake), self.snake.head.pos, self.snack.pos)
		self.moves = search.a_star_search(problem)

	# returns next move in sequence, recalculates if error occurred and list is empty
	def get_move(self):
		if len(self.moves) == 0:
			self.get_path()
		return self.moves.pop(0)

	"""
	Old method - inefficient
	def get_move(self, current_snack):
		problem = search.SearchProblem(copy.copy(self.snake), self.snake.head.pos, current_snack.pos)
		moves = search.a_star_search(problem)
		return moves[0]

	"""


main()
