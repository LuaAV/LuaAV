import sys
sys.path.append("src")

from time import sleep

from Numeric import zeros
import pygame
from pygame import locals

from pyZenGarden import pyZenGarden

BLOCKSIZE = 512
CHANNELS = 2

# initialise the pygame mixer
pygame.mixer.init(22050, -16, CHANNELS, BLOCKSIZE)
pygame.init()

# if the user gives us a pd file, load that instead
if len(sys.argv) == 2:
    filename = sys.argv[1]
else:
    filename = "pd-patches/simple_osc.pd"

# initialise zengarden with our Pd file
zg = pyZenGarden(filename, "pd-patches", BLOCKSIZE, CHANNELS, CHANNELS, 22050)
zg.prepare()

def myPrintHook(instring):
    print "Python from Pd -> ", instring,

zg.setPrintHook(myPrintHook)

s1 = pygame.sndarray.make_sound(zeros((BLOCKSIZE, CHANNELS)))
s2 = pygame.sndarray.make_sound(zeros((BLOCKSIZE, CHANNELS)))
o1 = pygame.sndarray.samples(s1)
o2 = pygame.sndarray.samples(s2)

sounds = [s1, s2]
arrays = [o1, o2]
current_buffer = 0

c = pygame.mixer.find_channel()
c.set_endevent(locals.USEREVENT)

s_ranger = xrange(BLOCKSIZE)
c_ranger = xrange(CHANNELS)

c.queue(sounds[current_buffer])
c.queue(sounds[current_buffer + 1])

try:
    while 1:
        if pygame.event.get(locals.USEREVENT):
            # start playing the next buffer
            zg.process()
            for x in c_ranger:
                for y in s_ranger:
                    arrays[current_buffer][y][x] = zg.outBlock[y + x * BLOCKSIZE] * (pow(2, 16) / 2 - 1)
            c.queue(sounds[current_buffer])
            current_buffer = (current_buffer + 1) % 2
except KeyboardInterrupt:
    print "done"
