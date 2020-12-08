#Raspberry Pi Code for Agricultural Remote Sensing
#Github Pull Check

#Rquired Libraries
import serial
import time

from struct import Struct
import threading

import sqlite3
import Queue
from sqlite3 import Error
import requests

#Display

import socket

###########################################################
#################### DISPLAY SETUP ########################
###########################################################


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
              self.nodeNumber = rec[0]
              self.batteryVoltage = round(rec[1], 3)
              self.soilMoisture = abs(round(rec[2], 3))
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
#URL = "http://crohmi.seecs.nust.edu.pk/datauploadscript.php"
URL = "http://111.68.101.17/db/store.php"
# Make a FIFO Queue for the incomding Data Packets
dataPacketStack = Queue.Queue(0)

# Defines the structure of how the incoming data is arranged
structure = Struct('ifffffffffffff')

# Defines the Serial Port to listen to and what baudrate
try :
    ser = serial.Serial(
            port='/dev/ttyACM0',
            baudrate=9600,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
            timeout=None
            )
except:
    ser = serial.Serial(
            port='/dev/ttyACM1',
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

    sql_create_node_nine_table = """ CREATE TABLE IF NOT EXISTS nodenine (
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
        createTable(conn, sql_create_node_nine_table)
        
    else:
        print("Error! cannot create the database connection.")

def createEntryNodeOne(conn, nodeData):
    
    sql = ''' INSERT INTO nodeone(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3,CO,NO2,C3H8,C4H10,CH4,H2,C2H5OH)
              VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()

def createEntryNodeTwo(conn, nodeData):
    
    sql = ''' INSERT INTO nodetwo(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3,CO,NO2,C3H8,C4H10,CH4,H2,C2H5OH)
              VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()

def createEntryNodeThree(conn, nodeData):
    
    sql = ''' INSERT INTO nodethree(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3,CO,NO2,C3H8,C4H10,CH4,H2,C2H5OH)
              VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()

def createEntryNodeFour(conn, nodeData):
    
    sql = ''' INSERT INTO nodefour(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3,CO,NO2,C3H8,C4H10,CH4,H2,C2H5OH)
              VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()

def createEntryNodeFive(conn, nodeData):

    sql = ''' INSERT INTO nodefive(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3,CO,NO2,C3H8,C4H10,CH4,H2,C2H5OH)
              VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()


def createEntryNodeSix(conn, nodeData):
    sql = ''' INSERT INTO nodesix(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3,CO,NO2,C3H8,C4H10,CH4,H2,C2H5OH)
              VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()


def createEntryNodeSeven(conn, nodeData):
    sql = ''' INSERT INTO nodeseven(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3,CO,NO2,C3H8,C4H10,CH4,H2,C2H5OH)
              VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()


def createEntryNodeEight(conn, nodeData):
    sql = ''' INSERT INTO nodeeight(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3,CO,NO2,C3H8,C4H10,CH4,H2,C2H5OH)
              VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()
    
    
def createEntryNodeNine(conn, nodeData):
    sql = ''' INSERT INTO nodenine(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature,NH3,CO,NO2,C3H8,C4H10,CH4,H2,C2H5OH)
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
                         ('air_moisture' , nodeData[4]),
                         ('air_temperature' , nodeData[3]),
                         ('soil_moisture' , nodeData[1]),
                         ('soil_temperature' , nodeData[2]),
                         ('node_id' , currentStackObject.nodeNumber),
                         ('nh3' , nodeData[5]),
                         ('co' , nodeData[6]),
                         ('no2' , nodeData[7]),
                         ('c3h8' , nodeData[8]),
                         ('c4h10' , nodeData[9]),
                         ('ch4' , nodeData[10]),
                         ('h2' , nodeData[11]),
                         ('c2h5oh' , nodeData[12]),
                         ('battery_voltage' , nodeData[0])
                     )
             try:
                 r = requests.get(url = URL, params = PARAMS)
                 errRDB = True
             except requests.ConnectionError:
                 print("Unable to connect to server, check internet connection.")
                 errRDB = False

             database = "pythonsqlite_V5.db"
             conn = createConnection(database)

             createTables(conn)

             print("Stack is not empty")
             print("Node no:")
             print(currentStackObject.nodeNumber)
             
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
                 print("Entry created in SQLite Database for Node 6")
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

             elif currentStackObject.nodeNumber == 9:
                 print("Values received from node 9")
                 createEntryNodeNine(conn, nodeData)
                 print("Entry created in SQLite Database for Node 9")
                 errLDB = True
                 
             else:
                 print("Invalid Node Number")
                 errLDB = False

             conn.close()

             

        

        #print("Stack is empty, no object to process")
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


#def selfValuesInterrupt():
    #while(True):
          
        #time.sleep(60)
        #print("Asked for Self Values")
        #ser.write('x')
          
     

#selfValuesInterruptThread = threading.Thread(target=selfValuesInterrupt, args=())
#selfValuesInterruptThread.daemon = True
#selfValuesInterruptThread.start()

###########################################################
###########################################################
###########################################################

###########################################################
##################### MAIN THREAD #########################
###########################################################

while True:
     

    #Defining object according to the blueprint
    
    receivedValues = dataPacket()
    x = ser.read(56)
    print(str(x))#Reads values from Serial Port
    rec = structure.unpack_from(x)
    print("--------------------------")
    print("Rec Values")
    print(rec[0])#Reconstructs integers and floats from incoming bytes
    print(rec[1])
    print(rec[2])
    print(rec[3])
    print(rec[4])
    print(rec[5])
    print(rec[6])
    print(rec[7])


    print("--------------------------")
    receivedValues.addValues(rec)      #Assembles the data into a python class object

    dataPacketStack.put(receivedValues)

    print(dataPacketStack.qsize())

###########################################################
###########################################################
###########################################################

