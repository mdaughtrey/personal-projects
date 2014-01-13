import math

class Logo:
    def __init__(self):
        self.x = 0
        self.y = 0
        self.degrees = 0
        self.stack = []

    def push(self):
        self.stack.append((self.x, self.y))

    def pop(self):
        (self.x, self.y) = self.stack.pop()

    def walk(self, steps):
        self.x = math.sin(math.radians(self.degrees)) * steps + self.x
        self.y= math.cos(math.radians(self.degrees)) * steps + self.y
        print "goto %u %u 0\r" % (round(self.x, 0), round(self.y, 0))

    def turnRight(self, degrees):
        self.degrees += degrees
        self.degrees %= 360

    def turnLeft(self, degrees):
        self.degrees += (360 - degrees)
        self.degrees %= 360

    def home(self):
        print "goto 0 0 0\r"

    def walkToPop(self):
        self.pop()
        print "goto %u %u 0\r" % (round(self.x, 0), round(self.y, 0))
