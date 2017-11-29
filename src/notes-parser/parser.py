'''
 Copyright (c) 2017 Georg Alexander Murzik
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
'''

import argparse

class NoteParser():
    def __init__(self, inputFile, quietMode):
        self.quietMode = quietMode
        self.notes = { "P" : 0,
            "C8" : 4186,
            "B7" : 3951,
            "A#7" : 3729,
            "A7" : 3520,
            "G#7" : 3322,
            "G7" : 3136,
            "F#7" : 2960,
            "F7" : 2794,
            "E7" : 2637,
            "D#7" : 2489,
            "D7" : 2349,
            "C#7" : 2217,
            "C7" : 2093,
            "B6" : 1976,
            "A#6" : 1865,
            "A6" : 1760,
            "G#6" : 1661,
            "G6" : 1568,
            "F#6" : 1480,
            "F6" : 1397,
            "E6" : 1319,
            "D#6" : 1245,
            "D6" : 1175,
            "C#6" : 1109,
            "C6" : 1047,
            "B5" : 988,
            "A#5" : 932,
            "A5" : 880,
            "G#5" : 831,
            "G5" : 784,
            "F#5" : 740,
            "F5" : 698,
            "E5" : 659,
            "D#5" : 622,
            "D5" : 587,
            "C#5" : 554,
            "C5" : 523,
            "B4" : 494,
            "A#4" : 466,
            "A4" : 440,
            "G#4" : 415,
            "G4" : 392,
            "F#4" : 370,
            "F4" : 349,
            "E4" : 330,
            "D#4" : 311,
            "D4" : 294,
            "C#4" : 277,
            "C4" : 262,
            "B3" : 247,
            "A#3" : 233,
            "A3" : 220,
            "G#3" : 208,
            "G3" : 196,
            "F#3" : 185,
            "F3" : 175,
            "E3" : 165,
            "D#3" : 156,
            "D3" : 147,
            "C#3" : 139,
            "C3" : 131,
            "B2" : 123,
            "A#2" : 117,
            "A2" : 110,
            "G#2" : 104,
            "G2" : 98,
            "F#2" : 92,
            "F2" : 87,
            "E2" : 82,
            "D#2" : 78,
            "D2" : 73,
            "C#2" : 69,
            "C2" : 65,
            "B1" : 62,
            "A#1" : 58,
            "A1" : 55,
            "G#1" : 52,
            "G1" : 49,
            "F#1" : 46,
            "F1" : 44,
            "E1" : 41,
            "D#1" : 39,
            "D1" : 37,
            "C#1" : 35,
            "C1" : 33,
            "B0" : 31,
            "A#0" : 29,
            "A0" : 27
        }

        self.frequencies = []
        self.durations = []

        with open(inputFile) as fin:
            for line in fin:
                self.parseLine(line)

    def getFrequencyValue(self, frequency):
        return self.notes[frequency.strip()]
            
    def parseLine(self, line):
        line = line.strip()
        if line.startswith("//") or line == "":
            return

        if self.quietMode is False:
        	print(line)
        (frequency, duration, dotted) = line.split(",")
        
        frequency = frequency.strip()
        duration = int(duration) # its a duck!
        dotted = dotted.strip().lower()

        if frequency in self.notes:
            frequency = self.getFrequencyValue(frequency)

        else:
            raise ValueError("Error: Frequency '{}' not found in line '{}'".format(frequency, line))

        self.durations.append(duration)
        self.frequencies.append(frequency)
        
        if dotted.startswith("y") or dotted.startswith("x"):
            self.durations.append(duration * 2)
            self.frequencies.append(frequency)

    def dumpArry(self, array, identifier):
        print("uint16_t {}[] = ".format(identifier), end='')
        print("{", end='') # escaping { is hard
        for (i, element) in enumerate(array):
           if isinstance(element, int):
               print("{0:g}".format(element), end='')

           else:
               print("{}".format(element), end='')
           

           if i + 1 < len(array):
                       print(", ", end='')
        print("};")

    def dumpFrequencies(self):
        self.dumpArry(self.frequencies, "frequencies")

    def dumpDurations(self):
        self.dumpArry(self.durations, "durations")

    def dumpLength(self):
        print("uint16_t length = {};".format(len(self.durations)))

if (__name__ == "__main__"):
    description = "Converts note values to C arrays and prints the result"
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument("input", help="CSV file to parse, where a line represents a note in a tuple (scientific pitch notation, duration, dotted). Example: E4, 2, y")
    parser.add_argument("-q", dest='quiet', action='store_const', const=True, default=False, help="enable quite mode")
                    
    args = parser.parse_args()
    if args.input:
        noteParser = NoteParser(args.input, args.quiet)
        noteParser.dumpLength()
        noteParser.dumpFrequencies()
        noteParser.dumpDurations()
