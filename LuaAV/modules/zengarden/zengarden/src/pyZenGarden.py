from ctypes import cdll, CFUNCTYPE, c_float, c_char, c_char_p
from os import path, sep
from sys import platform

class pyZenGardenException(Exception):
    pass

class pyZenGarden:
    def __init__(self, pdFile, libraryPath, blockSize, inChannels, outChannels, sampleRate):
        """
            Interface to the ZenGarden Pd patch processing library.
            
            >>> zg = pyZenGarden("../pd-patches/simple_osc.pd", "../pd-patches", 64, 2, 2, 22050)
            >>> zg.prepare()
            Ordered evaluation list (2/2 active objects):
            osc~
            dac~
            >>> zg.process()
            >>> print [s for s in zg.outBlock[:5]]
            [1.0, 0.99215036630630493, 0.96872472763061523, 0.93009084463119507, 0.87685519456863403]
        """
        if platform == "darwin":
            self.zg = cdll.LoadLibrary("libzengarden.dylib")
        elif platform == "linux2":
            self.zg = cdll.LoadLibrary("libzengarden.so")
        else:
            raise pyZenGardenException("Sorry, your platform '%s' doesn't seem to be supported yet" % platform)
        filename = path.basename(pdFile)
        directory = path.dirname(pdFile)
        self.g = self.zg.NewPdGraph(directory + sep, filename, libraryPath + sep, blockSize, inChannels, outChannels, sampleRate)
        t_inBlock = c_float * (inChannels * blockSize)
        self.inBlock = t_inBlock()
        t_outBlock = c_float * (outChannels * blockSize)
        self.outBlock = t_outBlock()
    
    def prepare(self):
        self.zg.PrepareForProcessingPdGraph(self.g)
    
    def process(self):
        self.zg.ProcessPdGraph(self.g, self.inBlock, self.outBlock)
    
    def setPrintHook(self, fn):
        PRINT_HOOK_FUNC = CFUNCTYPE(None, c_char_p)
        printHook = PRINT_HOOK_FUNC(fn)
        self.zg.SetPrintHook(self.g, printHook)

if __name__ == "__main__":
    import doctest
    doctest.testmod()

