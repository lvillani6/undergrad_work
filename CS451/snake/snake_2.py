# Lorenzo Villani
import sys
import time
import copy
import random
import pygame
import tkinter as tk
from tkinter import messagebox
from enum import Enum

# globals
red = (220, 0, 0)
green = (0, 200, 0)
sky = (0, 190, 255)
gray = (127, 127, 127)
charcoal = (25, 25, 25)
white = (255, 255, 255)
black = (0, 0, 0)



class Flag:
	def __init__(self):
		self.flag = True

	def set_flag(self, b):
		self.flag = b

	def get_flag(self):
		return self.flag


class Directions(Enum):
	LEFT, RIGHT, UP, DOWN, NONE = range(5)


class Cube(object):
	rows = 20
	w = 500

	def __init__(self, start, dir_x=1, dir_y=0, color=red):
		self.pos = start
		self.dir_x = dir_x
		self.dir_y = dir_y
		self.color = color

	def move(self, dir_x, dir_y):
		self.dir_x = dir_x
		self.dir_y = dir_y
		pos = [self.pos[0] + self.dir_x, self.pos[1] + self.dir_y]
		for i in [0, 1]:
			pos[i] %= 20
		self.pos = tuple(pos)

	def draw(self, surface, eyes=False):
		dis = self.w // self.rows
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


class Snake(object):
	body = []
	turns = {}

	def __init__(self, color, pos):
		self.color = color
		self.head = Cube(pos)
		self.body.append(self.head)
		self.dir_x = 0
		self.dir_y = 1

	def move(self, flag, mode=0, direction=None):
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				pygame.quit()
		keys = pygame.key.get_pressed()

		if keys[pygame.K_ESCAPE]:
			flag.set_flag(False)

		if mode == 0:
			if keys[pygame.K_LEFT]:
				direction = Directions.LEFT
			elif keys[pygame.K_RIGHT]:
				direction = Directions.RIGHT
			elif keys[pygame.K_UP]:
				direction = Directions.UP
			elif keys[pygame.K_DOWN]:
				direction = Directions.DOWN

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

	def draw(self, surface):
		for i, c in enumerate(self.body):
			if i == 0:
				c.draw(surface, True)
			else:
				c.draw(surface)


def draw_grid(w, rows, surface):
	size_between = w // rows

	x = 0
	y = 0
	for l in range(rows):
		x += size_between
		y += size_between

		pygame.draw.line(surface, gray, (x, 0), (x, w))
		pygame.draw.line(surface, gray, (0, y), (w, y))


def redraw_window(surface):
	global rows, width
	surface.fill(charcoal)
	s.draw(surface)
	snack.draw(surface)
	draw_grid(width, rows, surface)
	pygame.display.update()


def random_snack(rows, item):
	positions = item.body
	while True:
		x = random.randrange(rows)
		y = random.randrange(rows)
		if len(list(filter(lambda z: z.pos == (x, y), positions))) > 0:
			continue
		else:
			break

	return x, y


def message_box(subject, content):
	root = tk.Tk()
	root.attributes("-topmost", True)
	root.withdraw()
	messagebox.showinfo(subject, content)
	root.destroy()


def run(mode=0):
	global rows, width, s, snack

	if mode == 0:
		tick = 12
	elif mode == 1:
		tick = 20
	elif mode == 2:
		tick = 100

	width = 500
	rows = 20
	win = pygame.display.set_mode((width, width))
	pygame.display.set_caption('Snake AI')
	s = Snake(red, (10, 10))
	snack = Cube(random_snack(rows, s), color=green)
	b = Bot(s)
	flag = Flag()

	clock = pygame.time.Clock()

	start = time.time()
	n = 5 if mode == 2 else 1
	i = 0
	while flag.get_flag():
		# pygame.time.delay(50)
		clock.tick(tick)
		s.move(flag, mode, b.get_move(snack)) if mode > 0 else s.move(flag)

		if s.body[0].pos == snack.pos:
			s.add_cube()
			snack = Cube(random_snack(rows, s), color=green)

		for x in range(len(s.body)):
			if s.body[x].pos in list(map(lambda z: z.pos, s.body[x + 1:])):
				message = 'Score: ' + str(len(s.body))
				if len(s.body) == rows**2:
					message_box('You Win!', message)
					return time.time()-start
				message += '\nPlay again...'
				message_box('You Lose!', message)
				s.reset((10, 10))
				start = time.time()
				break
		if i%n==0:
			redraw_window(win)
		i+=1


def main():
	mode = int(sys.argv[1])
	win_time = run(mode)
	if win_time is not None:
		print("Victory in " + win_time)


# =========== AI ==============

def print_dir(dx, dy):
	if dx == -1:
		print(Directions.LEFT)
	elif dx == 1:
		print(Directions.RIGHT)
	elif dy == -1:
		print(Directions.UP)
	elif dy == 1:
		print(Directions.DOWN)


def manhattan_distance(p1, p2):
	return abs(p1[0] - p2[0]) + abs(p1[1] - p2[1])


def euclidean_distance(p1, p2):
	return ((p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2) ** 0.5


def get_dir_points(p1, p2):
	if p1[0] - p2[0] == 1:
		return Directions.LEFT
	elif p1[0] - p2[0] == -1:
		return Directions.RIGHT
	elif p1[1] - p2[1] == 1:
		return Directions.UP
	elif p1[1] - p2[1] == -1:
		return Directions.DOWN


def get_dir_dir(dx, dy):
	if dx == -1:
		return Directions.LEFT
	elif dx == 1:
		return Directions.RIGHT
	elif dy == -1:
		return Directions.UP
	elif dy == 1:
		return Directions.DOWN


def next_states(head_pos, dx, dy):
	successors = []
	if dx != 1:
		successors.append((head_pos[0] - 1, head_pos[1]))  # LEFT
	if dx != -1:
		successors.append((head_pos[0] + 1, head_pos[1]))  # RIGHT
	if dy != -1:
		successors.append((head_pos[0], head_pos[1] + 1))  # UP
	if dy != 1:
		successors.append((head_pos[0], head_pos[1] - 1))  # DOWN
	return successors


def max_move(move_list):
	best_val = float('-inf')
	best_move = Directions.NONE
	for move in move_list:
		val = sum(a[0] for a in move)
		if val > best_val:
			best_val = sum(a[0] for a in move)
			best_move = move[0][1]
	return best_move


class Bot(object):
	def __init__(self, snake):
		self.snake = snake

	def calc_val(self, state, current_snack):
		if state in list(map(lambda z: z.pos, self.snake.body[:])):
			return float('-inf'), get_dir_points(self.snake.head.pos, state)

		man_dis = manhattan_distance(state, current_snack.pos)
		euc_dis = euclidean_distance(state, current_snack.pos)
		val = 0.0 - (man_dis + 1.2*euc_dis)

		new_direction = get_dir_points(self.snake.head.pos, state)
		return val, new_direction

	def get_move(self, current_snack):
		successors = next_states(self.snake.head.pos, self.snake.dir_x, self.snake.dir_y)
		moves = []
		for successor in successors:
			for dx,dy in [(-1,0),(1,0),(0,-1),(0,1)]:
				successors2 = next_states(successor, dx, dy)
				for successor2 in successors2:
					moves.append([self.calc_val(successor, current_snack), self.calc_val(successor2, current_snack)])
		# print(max_move(moves))
		return max_move(moves)




# ==========================================

# TODO: Comments

main()
