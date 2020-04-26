import math
import pdb

class Tension:
    TENSION_MAX = 50
    TENSION_MIN = 5
    DIA_MAX = 180
    DIA0_UPTAKE = 65
    FILM_THICKNESS = 0.14
    FRAMESPACE_8MM = 3.3
    FRAMESPACE_S8 = 4.01

    def do(self, startdia, enddia):
        (feedTurns, filmLength) = self.calcLength(enddia, startdia, Tension.FILM_THICKNESS)
        numFrames = math.floor(filmLength / Tension.FRAMESPACE_8MM)
        feedC = self.calcCircums(enddia, math.floor(feedTurns), Tension.FILM_THICKNESS)
        feedD = self.calcDia(enddia, math.floor(feedTurns), Tension.FILM_THICKNESS)
        tPerTurn = (Tension.TENSION_MAX - Tension.TENSION_MIN) / feedTurns * startdia / Tension.DIA_MAX
        #tensionPerC = [math.floor(Tension.TENSION_MIN + tPerTurn * t) for t in range(math.floor(feedTurns))][::-1]
        tensionPerC = [Tension.TENSION_MIN + math.tanh((t/2)/(enddia/2)) * (Tension.TENSION_MAX - Tension.TENSION_MIN)  for t in feedD]
        framesPerC = [math.floor(x/Tension.FRAMESPACE_8MM) for x in feedC][::-1]
        return (filmLength, numFrames, [tensionPerC[self.getindex(frame, framesPerC)] for frame in range(numFrames)])

    # Given an ID d0 and an OD d1 and a thickness, calculate the length
    def calcLength(self, d0, d1, thick):
        turns = (d1 - d0) / (2 * thick)
        length = math.pi * turns * (d0 + thick * (turns -1 ))
        return (turns, length)

    # Number of concentric circles
    def calcturns(self, d0, length, thick):
        a = (4 * thick * length) / math.pi
        b = math.pow(d0 - thick, 2) + a
        c = thick - d0 + math.sqrt(b)
        return c / (2 * thick)

    # Circumference of each concentric circle
    def calcCircums(self, d0, turns, thickness):
        return list([math.floor(math.pi * (d0 + x * 2 * thickness)) for x in range(turns)])

    # Diamater of each concentric circle
    def calcDia(self, d0, turns, thickness):
        return list([math.floor(d0 + x * 2 * thickness) for x in range(turns)])

    # Index of framesPerC of framenum
    def getindex(self, framenum, framesPerC):
        for ii in range(1, len(framesPerC)):
            if sum(framesPerC[:ii]) > framenum:
                return ii - 1
        return len(framesPerC)-1
