import os
import time

__author__ = 'vikram'
import serial

ser = serial.Serial('/dev/ttyACM10', 9600)
X = 0
Y = 0
old_list = []
count = 0
while True:
    line = ser.readline().rstrip()
    state = line.split("\t")
    moveX = 0
    GYROY = 6
    GYROX=4
    moveY=0
    try:
        if "\t" in line:
            if abs(X - int(state[GYROY])) > 20:
                moveX = int((int(state[GYROY])) * 0.1)
                old_list = state
            if abs(Y - int(state[GYROX])) > 20:
                moveY = int((int(state[GYROX])) * 0.1)
            # print line.split("\t")
            if (abs(moveX) > 0) | (abs(moveY)>0):
                #print X, state[4], moveX
                #print old_list
                #print state
                move = ' '.join(['xdotool mousemove_relative -- ', str(int(moveX) * -1), str(int(moveY) *-1)])
                #rint move
                os.system(move)  #sthash.0DxJxvqQ.dpuf
                count=0
            else:
                count+=1

            X = int(state[GYROY])
            Y = int(state[GYROX])
        elif "," in line:
            state = line.split(",")
            print int(state[1])
            if count>10:
                print int(state[1])
                if int(state[1])>30:
                    os.system("xdotool click 1")
                    #time.sleep(1)
                    count=0
            print state
            print line
        #print line
    except Exception as e:
        print state
        print e
