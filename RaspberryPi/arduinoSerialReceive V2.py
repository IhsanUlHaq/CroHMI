#Raspberry Pi Code for Agricultural Remote Sensing
#Github Pull Check

#Rquired Libraries
import serial
import time
import sys
from struct import Struct
import threading
import time
import sqlite3
import Queue
from sqlite3 import Error
import requests

#Display
import Adafruit_SSD1306
from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont
import socket

###########################################################
#################### DISPLAY SETUP ########################
###########################################################

RST = 0

disp = Adafruit_SSD1306.SSD1306_128_64(rst=RST)
disp.begin()
disp.clear()
disp.display()

width = disp.width
height = disp.heightl

image1 = Image.new('1', (width, height))



def oledReady():
    
    draw = ImageDraw.Draw(image1)
    draw.rectangle((0, 0, width, height), outline=0, fill=0)

    padding = -2
    top = padding

    bottom = height-padding
    x=0
    font = ImageFont.truetype('fonts/Helvetica.ttf', 12)
    
    disp.clear()
    disp.display()
    draw.text((x, top+4), "Ready", font = font, fill = 255)
     
    if (isInternetConnected()):
         draw.rectangle((124,0,128,12), outline=0, fill=255)
         draw.rectangle((120,4,124,12), outline=0, fill=255)
         draw.rectangle((116,8,120,12), outline=0, fill=255)
    else:
        draw.rectangle((124,0,128,12), outline=0, fill=255)
        draw.rectangle((120,4,124,12), outline=0, fill=255)
        draw.rectangle((116,8,120,12), outline=0, fill=255)
        draw.rectangle((116,8,128,10), outline=0, fill=255)

    disp.image(image1)
    disp.display()

def oledData(dataFromNode, nodeNumber, errLDB, errRDB):

    draw = ImageDraw.Draw(image1)
    draw.rectangle((0, 0, width, height), outline=0, fill=0)

    padding = -2
    top = padding

    bottom = height-padding
    x=0
    font = ImageFont.truetype('fonts/Helvetica.ttf', 12)
    
    disp.clear()
    disp.display()
    draw.text((x, top+4), "Rcvd From: " + str(nodeNumber), font = font, fill = 255)
    draw.text((x, top+16), "Air: " + str(dataFromNode[3]) + "C, " + str(dataFromNode[4]) + "%", font = font, fill = 255)
    draw.text((x, top+28), "Soil: " + str(dataFromNode[2]) + "C, " + str(dataFromNode[1]) + "%", font = font, fill = 255)
    draw.text((x, top+40), "Battery Voltage: " + str(dataFromNode[0]) + "V", font = font, fill = 255)
    draw.text((x, top+52), "LDB: " + str(errLDB) + ", RDB: " + str(errRDB), font = font, fill = 255)
     
    if (isInternetConnected()):
         draw.rectangle((124,0,128,12), outline=0, fill=255)
         draw.rectangle((120,4,124,12), outline=0, fill=255)
         draw.rectangle((116,8,120,12), outline=0, fill=255)
    else:
         draw.rectangle((124,0,128,12), outline=0, fill=255)
         draw.rectangle((120,4,124,12), outline=0, fill=255)
         draw.rectangle((116,8,120,12), outline=0, fill=255)
         draw.rectangle((116,8,128,10), outline=0, fill=255)

    disp.image(image1)
    disp.display()


def isInternetConnected():
    try:
          # see if we can resolve the host name -- tells us if there is
          # a DNS listening
        host = socket.gethostbyname("www.google.com")
          # connect to the host -- tells us if the host is actually
          # reachable
        s = socket.create_connection((host, 80), 2)
        s.close()
        return True
    except:
        pass
    return False

def shouldBeStatusCode():
    if (isInternetConnected()):
        return 1
    else:
        return 0
     

###########################################################
###########################################################
###########################################################

# A blueprint of how the data should be received
class dataPacket:
     
     #All the required parameters
    nodeNumber = 0
    batteryVoltage = 0.0
    soilMoisture = 0.0
    soilTemperature = 0.0
    airTemperature = 0.0
    airMoisture = 0.0
    NH3 = 0.0
    CO = 0.0
    NO2 = 0.0
    C3H8 = 0.0
    C4H10 = 0.0
    CH4 = 0.0
    H2 = 0.0
    C2H5OH = 0.0

     #Assembling values coming from the Arduino
    def addValues(self, reconstructed):

        if len(reconstructed) == 14:             #Checks to see if the incoming bytes are correct in size
              self.nodeNumber = round(rec[0], 3)
              self.batteryVoltage = round(rec[1], 3)
              self.soilMoisture = round(rec[2], 3)
              self.soilTemperature = round(rec[3], 3)
              self.airMoisture = round(rec[4], 3)
              self.airTemperature = round(rec[5], 3)
              self.NH3 = round(rec[6], 3)
              self.CO = round(rec[7], 3)
              self.NO2 = round(rec[8], 3)
              self.C3H8 = round(rec[9], 3)
              self.C4H10 = round(rec[10], 3)
              self.CH4 = round(rec[11], 3)
              self.H2 = round(rec[12], 3)
              self.C2H5OH = round(rec[13], 3)

        else:
             print("Data Packets: Invalid size of received list")

# URL for HTTP GET Requests
URL = "http://crohmi.seecs.nust.edu.pk/datauploadscript.php"

# Make a FIFO Queue for the incomding Data Packets
dataPacketStack = Queue.Queue(0)

# Defines the structure of how the incoming data is arranged
structure = Struct('ifffffffffffff')

# Defines the Serial Port to listen to and what baudrate
ser = serial.Serial(
    port='/dev/ttyACM0',
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=None
    )

###########################################################
##################### DATABASE ############################
###########################################################

def createConnection(db_file):
    try:
        conn = sqlite3.connect(db_file)
        print(sqlite3.version)
        return conn
    except Error as e:
        print(e)

    return None

def createTable(conn, create_table_sql):

    try:
        c = conn.cursor()
        c.execute(create_table_sql)
    except Error as e:
        print(e)
 
def createTables(conn):
 
    sql_create_node_one_table = """ CREATE TABLE IF NOT EXISTS nodeone (
                                        id integer PRIMARY KEY,
                                        timestamp DATETIME DEFAULT (datetime(CURRENT_TIMESTAMP, 'localtime')),
                                        batteryVoltage float,
                                        soilMoisture float,
                                        soilTemperature float,
                                        airMoisture float,
                                        airTemperature float,
                                        NH3 float,
                                        CO float,
                                        NO2 float,
                                        C3H8 float,
                                        C4H10 float,
                                        CH4 float,
                                        H2 float,
                                        C2H5OH float
                                    ); """
 
    sql_create_node_two_table = """ CREATE TABLE IF NOT EXISTS nodetwo (
                                        id integer PRIMARY KEY,
                                        timestamp DATETIME DEFAULT (datetime(CURRENT_TIMESTAMP, 'localtime')),
                                        batteryVoltage float,
                                        soilMoisture float,
                                        soilTemperature float,
                                        airMoisture float,
                                        airTemperature float,
                                        NH3 float,
                                        CO float,
                                        NO2 float,
                                        C3H8 float,
                                        C4H10 float,
                                        CH4 float,
                                        H2 float,
                                        C2H5OH float
                                    ); """
 
    sql_create_node_three_table = """ CREATE TABLE IF NOT EXISTS nodethree (
                                        id integer PRIMARY KEY,
                                        timestamp DATETIME DEFAULT (datetime(CURRENT_TIMESTAMP, 'localtime')),
                                        batteryVoltage float,
                                        soilMoisture float,
                                        soilTemperature float,
                                        airMoisture float,
                                        airTemperature float,
                                        NH3 float,
                                        CO float,
                                        NO2 float,
                                        C3H8 float,
                                        C4H10 float,
                                        CH4 float,
                                        H2 float,
                                        C2H5OH float
                                    ); """
 
    sql_create_node_four_table = """ CREATE TABLE IF NOT EXISTS nodefour (
                                        id integer PRIMARY KEY,
                                        timestamp DATETIME DEFAULT (datetime(CURRENT_TIMESTAMP, 'localtime')),
                                        batteryVoltage float,
                                        soilMoisture float,
                                        soilTemperature float,
                                        airMoisture float,
                                        airTemperature float,
                                        NH3 float,
                                        CO float,
                                        NO2 float,
                                        C3H8 float,
                                        C4H10 float,
                                        CH4 float,
                                        H2 float,
                                        C2H5OH float
                                    ); """
 
    sql_create_node_five_table = """ CREATE TABLE IF NOT EXISTS nodefive (
                                        id integer PRIMARY KEY,
                                        timestamp DATETIME DEFAULT (datetime(CURRENT_TIMESTAMP, 'localtime')),
                                        batteryVoltage float,
                                        soilMoisture float,
                                        soilTemperature float,
                                        airMoisture float,
                                        airTemperature float,
                                        NH3 float,
                                        CO float,
                                        NO2 float,
                                        C3H8 float,
                                        C4H10 float,
                                        CH4 float,
                                        H2 float,
                                        C2H5OH float
                                    ); """
    sql_create_node_six_table = """ CREATE TABLE IF NOT EXISTS nodesix (
                                            id integer PRIMARY KEY,
                                            timestamp DATETIME DEFAULT (datetime(CURRENT_TIMESTAMP, 'localtime')),
                                            batteryVoltage float,
                                            soilMoisture float,
                                            soilTemperature float,
                                            airMoisture float,
                                            airTemperature float,
                                            NH3 float,
                                            CO float,
                                            NO2 float,
                                            C3H8 float,
                                            C4H10 float,
                                            CH4 float,
                                            H2 float,
                                            C2H5OH float
                                        );"""
    sql_create_node_seven_table = """ CREATE TABLE IF NOT EXISTS nodeseven (
                                                id integer PRIMARY KEY,
                                                timestamp DATETIME DEFAULT (datetime(CURRENT_TIMESTAMP, 'localtime')),
                                                batteryVoltage float,
                                                soilMoisture float,
                                                soilTemperature float,
                                                airMoisture float,
                                                airTemperature float,
                                                NH3 float,
                                                CO float,
                                                NO2 float,
                                                C3H8 float,
                                                C4H10 float,
                                                CH4 float,
                                                H2 float,
                                                C2H5OH float
                                            );"""
    sql_create_node_eight_table = """ CREATE TABLE IF NOT EXISTS nodeeight (
                                                    id integer PRIMARY KEY,
                                                    timestamp DATETIME DEFAULT (datetime(CURRENT_TIMESTAMP, 'localtime')),
                                                    batteryVoltage float,
                                                    soilMoisture float,
                                                    soilTemperature float,
                                                    airMoisture float,
                                                    airTemperature float,
                                                    NH3 float,
                                                    CO float,
                                                    NO2 float,
                                                    C3H8 float,
                                                    C4H10 float,
                                                    CH4 float,
                                                    H2 float,
                                                    C2H5OH float
                                                );"""

    # create a database connection
    if conn is not None:
        # create projects table
        createTable(conn, sql_create_node_one_table)
        createTable(conn, sql_create_node_two_table)
        createTable(conn, sql_create_node_three_table)
        createTable(conn, sql_create_node_four_table)
        createTable(conn, sql_create_node_five_table)
        createTable(conn, sql_create_node_six_table)
        createTable(conn, sql_create_node_seven_table)
        createTable(conn, sql_create_node_eight_table)

    else:
        print("Error! cannot create the database connection.")

def createEntryNodeOne(conn, nodeData):
    
    sql = ''' INSERT INTO nodeone(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3 float,CO float,NO2 float,C3H8 float,C4H10 float,CH4 float,H2 float,C2H5OH float)
              VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()

def createEntryNodeTwo(conn, nodeData):
    
    sql = ''' INSERT INTO nodetwo(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3 float,CO float,NO2 float,C3H8 float,C4H10 float,CH4 float,H2 float,C2H5OH float)
              VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()

def createEntryNodeThree(conn, nodeData):
    
    sql = ''' INSERT INTO nodethree(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3 float,CO float,NO2 float,C3H8 float,C4H10 float,CH4 float,H2 float,C2H5OH float)
              VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()

def createEntryNodeFour(conn, nodeData):
    
    sql = ''' INSERT INTO nodefour(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3 float,CO float,NO2 float,C3H8 float,C4H10 float,CH4 float,H2 float,C2H5OH float)
              VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()

def createEntryNodeFive(conn, nodeData):

    sql = ''' INSERT INTO nodefive(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3 float,CO float,NO2 float,C3H8 float,C4H10 float,CH4 float,H2 float,C2H5OH float)
              VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()


def createEntryNodeSix(conn, nodeData):
    sql = ''' INSERT INTO nodesix(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3 float,CO float,NO2 float,C3H8 float,C4H10 float,CH4 float,H2 float,C2H5OH float)
              VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()


def createEntryNodeSeven(conn, nodeData):
    sql = ''' INSERT INTO nodeseven(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3 float,CO float,NO2 float,C3H8 float,C4H10 float,CH4 float,H2 float,C2H5OH float)
              VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()


def createEntryNodeEight(conn, nodeData):
    sql = ''' INSERT INTO nodeeight(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3 float,CO float,NO2 float,C3H8 float,C4H10 float,CH4 float,H2 float,C2H5OH float)
              VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()

###########################################################
###########################################################
###########################################################



###########################################################
################# BACKGROUND THREAD #######################
###########################################################

def runInBackground():

    currentOLEDStatusCode = 2
    count = 20

    oledReady()
    currentOLEDStatusCode = shouldBeStatusCode()

    errLDB = False
    errRDB = False

    while True:
        if (dataPacketStack.qsize()>0):

             currentStackObject = dataPacketStack.get()
               
             nodeData = [
                    currentStackObject.batteryVoltage,
                    currentStackObject.soilMoisture,
                    currentStackObject.soilTemperature,
                    currentStackObject.airMoisture,
                    currentStackObject.airTemperature,
                    currentStackObject.NH3,
                    currentStackObject.CO,
                    currentStackObject.NO2,
                    currentStackObject.C3H8,
                    currentStackObject.C4H10,
                    currentStackObject.CH4,
                    currentStackObject.H2,
                    currentStackObject.C2H5OH ]
               
             PARAMS = (
                         ('dat' , 2019),
                         ('lat' , 33.2),
                         ('lng' , 33.2),
                         ('airm' , nodeData[4]),
                         ('airt' , nodeData[3]),
                         ('soilm1' , nodeData[1]),
                         ('soilt1' , nodeData[2]),
                         ('pn1' , currentStackObject.nodeNumber)
                         )
             try:
                 r = requests.get(url = URL, params = PARAMS)
                 errRDB = True
             except requests.ConnectionError:
                 print("Unable to connect to server, check internet connection.")
                 errRDB = False

             database = "pythonsqlite_V2.db"
             conn = createConnection(database)

             createTables(conn)

             print("Stack is not empty")
               
             if currentStackObject.nodeNumber == 1:
                 print("Values received from node 1")
                 createEntryNodeOne(conn, nodeData)
                 print("Entry created in SQLite Database for Node 1")
                 errLDB = True

             elif currentStackObject.nodeNumber == 2:
                 print("Values received from node 2")
                 createEntryNodeTwo(conn, nodeData)
                 print("Entry created in SQLite Database for Node 2")
                 errLDB = True

             elif currentStackObject.nodeNumber == 3:
                 print("Values received from node 3")
                 createEntryNodeThree(conn, nodeData)
                 print("Entry created in SQLite Database for Node 3")
                 errLDB = True

             elif currentStackObject.nodeNumber == 4:
                  print("Values received from node 4")
                  createEntryNodeFour(conn, nodeData)
                  print("Entry created in SQLite Database for Node 4")
                  errLDB = True


             elif currentStackObject.nodeNumber == 5:
                 print("Values received from node 5")
                 createEntryNodeFive(conn, nodeData)
                 print("Entry created in SQLite Database for Node 5")
                 errLDB = True

             elif currentStackObject.nodeNumber == 6:
                 print("Values received from node 6")
                 createEntryNodeSix(conn, nodeData)
                 print("Entry created in SQLite Database for Node 5")
                 errLDB = True

             elif currentStackObject.nodeNumber == 7:
                 print("Values received from node 7")
                 createEntryNodeSeven(conn, nodeData)
                 print("Entry created in SQLite Database for Node 7")
                 errLDB = True

             elif currentStackObject.nodeNumber == 8:
                 print("Values received from node 8")
                 createEntryNodeEight(conn, nodeData)
                 print("Entry created in SQLite Database for Node 8")
                 errLDB = True

             else:
                 print("Invalid Node Number")
                 errLDB = False

             conn.close()

             oledData(nodeData, currentStackObject.nodeNumber, errLDB, errRDB)

             currentOLEDStatusCode = 2

             count = 30

        else:
            if (shouldBeStatusCode() != currentOLEDStatusCode):
                if (currentOLEDStatusCode == 2):
                    if (count == 0):
                            oledReady()
                            currentOLEDStatusCode = shouldBeStatusCode()
                    else:
                        count = count - 1
                else:
                     oledReady()
                     currentOLEDStatusCode = shouldBeStatusCode()

        print("Stack is empty, no object to process")
        time.sleep(0.5)

# Run the function in background
dataPushThread = threading.Thread(target=runInBackground, args=())
dataPushThread.daemon = True
dataPushThread.start()

###########################################################
###########################################################
###########################################################

###########################################################
################# SELF REQUEST THREAD #####################
###########################################################


def selfValuesInterrupt():
    while(True):
          
        time.sleep(300)
        print("Asked for Self Values")
        ser.write('x')
          
     

selfValuesInterruptThread = threading.Thread(target=selfValuesInterrupt, args=())
selfValuesInterruptThread.daemon = True
selfValuesInterruptThread.start()

###########################################################
###########################################################
###########################################################

###########################################################
##################### MAIN THREAD #########################
###########################################################

while True:
     

    #Defining object according to the blueprint
    receivedValues = dataPacket()

    x = ser.read(62)    #Reads values from Serial Port
    rec = structure.unpack_from(x)     #Reconstructs integers and floats from incoming bytes
    receivedValues.addValues(rec)      #Assembles the data into a python class object

    dataPacketStack.put(receivedValues)

    print(dataPacketStack.qsize())

###########################################################
###########################################################
###########################################################

