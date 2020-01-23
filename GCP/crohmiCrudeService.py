import paho.mqtt.client as paho
import time
import sys
from datetime import datetime
import serial
import requests

import pymongo


broker = "mqtt-dashboard.com"
topic = "Crohmi"
URL = "http://crohmi.seecs.nust.edu.pk/datauploadscript.php"
inCominglist = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]


#Creating MongoDB client
myclient = pymongo.MongoClient("mongodb://localhost:27017/")

 #Creating new database
mydb = myclient["CrohmiDB"]
print("databases created.")

#Creating new collection
mycol = mydb["CropHealth"]
print("collection created.")


def on_message(client , userdata , message):
    print("Received data is")
    inComingData = str(message.payload.decode("utf-8"))
    inCominglist = inComingData.split('|')
    print(inCominglist)
    #uploadDataToServer(inCominglist)
    insertToDb(inCominglist)
     #Printing Recive message
    print("")

client = paho.Client("CrohmiCrudeService")
client.on_message = on_message

print("Connecter to broker host", broker)
client.connect(broker) #Connection with broker

print("Subscribing begins")

client.subscribe(topic)


def uploadDataToServer(datalist):
    PARAMS = (
        ('dat' , 2019),
        ('lat' , 33.2),
        ('lng' , 33.2),
        ('airm' , float(datalist[6])),
        ('airt' , float(datalist[5])),
        ('soilm1' , float(datalist[3])),
        ('soilt1' , float(datalist[4])),
        ('pn1' , float(datalist[1])),
        ('nh3' , float(datalist[7])),
        ('co' , float(datalist[8])),
        ('no2' , float(datalist[9])),
        ('c3h8' , float(datalist[10])),
        ('c4h10' , float(datalist[11])),
        ('ch4' , float(datalist[12])),
        ('h2' , float(datalist[13])),
        ('c2h5oh' , float(datalist[14]))
        )

    try:
        r = requests.get(url = URL, params = PARAMS)

    except requests.ConnectionError:
        print("Unable to connect to server, check internet connection.")

def insertToDb(datalist):
    now = datetime.now()
    mydict = {
        "Node Number" : float(datalist[1]),
        "Date and Time" : now.strftime("%d/%m/%Y %H:%M:%S"),
        "Batery Voltage" : float(datalist[2]),
        "Soil Moisture" : float(datalist[3]),
        "Soil Temperature" : float(datalist[4]),
        "Air Moisture" : float(datalist[5]),
        "Air Temperature" : float(datalist[6]),
        "NH3" : float(datalist[7]),
        "CO" : float(datalist[8]),
        "NO2" : float(datalist[9]),
        "C3H8" : float(datalist[10]),
        "C4h10" : float(datalist[11]),
        "CH4" : float(datalist[12]),
        "H2" : float(datalist[13]),
        "C2H5OH" : float(datalist[14])
    }

    x = mycol.insert_one(mydict)
    print("Data is Inserted with id")
    print(x.inserted_id)

while 1:
    client.loop_start()
    
    
