import os
import serial

__author__ = 'vikram'
import numpy as nm


class BrainStack:
    def __init__(self):
        self.items = []
        self.length = 10
        self.alpha = 0.1
        self.crazy = 15

    def isEmpty(self):
        return self.items == []

    def push(self, item):
        self.items.append(item)
        if self.size() > self.length:
            self.items.pop(0)

    def pop(self):
        return self.items.pop()

    def peek(self):
        return self.items[len(self.items) - 1]

    def size(self):
        return len(self.items)

    def std(self):
        return nm.std(self.items)

    def mean(self):
        return nm.average(self.items)

    def check_spike(self, value):
        if (value - self.mean()) > (self.alpha * self.std()):
            return True
        else:
            return False

    def check_crazy(self):
        if (self.std() > self.crazy) & (self.size() == self.length):
            return True
        else:
            return False


brain = BrainStack()
ser = serial.Serial('/dev/ttyACM0', 9600)
X = 0
Y = 0
old_list = []
count = 0

GYROY = 6
GYROX = 4
while True:
    line = ser.readline().rstrip()
    state = line.split("\t")
    moveY = 0
    moveX = 0
    try:
        if "\t" in line:
            if abs(X - int(state[GYROY])) > 20:
                moveX = int((int(state[GYROY])) * 0.1)
                old_list = state
            if abs(Y - int(state[GYROX])) > 20:
                moveY = int((int(state[GYROX])) * 0.1)
            if (abs(moveX) > 1) | (abs(moveY) > 1):
                # print X, state[4], moveX
                # print old_list
                # print state
                move = ' '.join(['xdotool mousemove_relative -- ', str(int(moveX) * -1), str(int(moveY) * -1)])
                # rint move
                os.system(move)  # sthash.0DxJxvqQ.dpuf
                count = 0
            else:
                count += 0
            count += 1

            X = int(state[GYROY])
            Y = int(state[GYROX])
        elif "," in line:
            state = line.split(",")
            brain.push(int(state[1]))
            print int(state[1])
            print brain.items
            print brain.std(), brain.mean(), brain.check_spike(int(state[1]))
            if count > 10:
                if brain.check_spike(int(state[1])):
                    print "click"
                    os.system("xdotool click 1")
                    # time.sleep(1)
                    count = 0
            if brain.check_crazy():
                msg = "xmessage FOCUS BRO -timeout 2"
                os.system(msg)

    except Exception as e:
        print state
        print e
