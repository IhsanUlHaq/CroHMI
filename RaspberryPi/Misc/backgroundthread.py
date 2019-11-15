import threading
import time
import queue
import copy

class dataPacket:
    r = 0.0
    g = 0.0
    b = 0.0


mylist = queue.Queue(20)

dataPacketObj = dataPacket()
dataPacketObj.r = 1.1
dataPacketObj.g = 1.1
dataPacketObj.b = 1.1

mylist.put(dataPacketObj)

dataPacketObj2 = dataPacket()
dataPacketObj2 = copy.deepcopy(dataPacketObj)


dataPacketObj2.r = 1.2
dataPacketObj2.g = 1.2
dataPacketObj2.b = 1.2

mylist.put(dataPacketObj2) 

print(mylist.qsize())

dataPacketObjCheck = dataPacket()

print("What popped out: ")

dataPacketObjCheck = mylist.get()

print(dataPacketObjCheck.r)
print(dataPacketObjCheck.g)
print(dataPacketObjCheck.b)

print(mylist.qsize())
dataPacketObjCheck = mylist.get()

print(dataPacketObjCheck.r)
print(dataPacketObjCheck.g)
print(dataPacketObjCheck.b)

print(mylist.qsize())


def runinbackground():
    while True:
        print("Doing something important in the background")
        time.sleep(1)


thread = threading.Thread(target=runinbackground, args=())
thread.daemon = True
thread.start()

time.sleep(3)
print('Checkpoint')
time.sleep(2)
print('Bye')