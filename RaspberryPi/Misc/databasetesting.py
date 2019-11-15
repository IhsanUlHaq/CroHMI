import sqlite3
from sqlite3 import Error
 
 
def create_connection(db_file):
    try:
        conn = sqlite3.connect(db_file)
        print(sqlite3.version)
        return conn
    except Error as e:
        print(e)

    return None

def create_table(conn, create_table_sql):

    try:
        c = conn.cursor()
        c.execute(create_table_sql)
    except Error as e:
        print(e)
 
def createtables():
 
    sql_create_node_one_table = """ CREATE TABLE IF NOT EXISTS nodeone (
                                        id integer PRIMARY KEY,
                                        timestamp DATETIME DEFAULT (datetime(CURRENT_TIMESTAMP, 'localtime')),
                                        batteryVoltage float,
                                        soilMoisture float,
                                        soilTemperature float,
                                        airMoisture float,
                                        airTemperature float
                                    ); """
 
    sql_create_node_two_table = """ CREATE TABLE IF NOT EXISTS nodetwo (
                                        id integer PRIMARY KEY,
                                        timestamp DATETIME DEFAULT (datetime(CURRENT_TIMESTAMP, 'localtime')),
                                        batteryVoltage float,
                                        soilMoisture float,
                                        soilTemperature float,
                                        airMoisture float,
                                        airTemperature float
                                    ); """
 
    sql_create_node_three_table = """ CREATE TABLE IF NOT EXISTS nodethree (
                                        id integer PRIMARY KEY,
                                        timestamp DATETIME DEFAULT (datetime(CURRENT_TIMESTAMP, 'localtime')),
                                        batteryVoltage float,
                                        soilMoisture float,
                                        soilTemperature float,
                                        airMoisture float,
                                        airTemperature float
                                    ); """
 
    sql_create_node_four_table = """ CREATE TABLE IF NOT EXISTS nodefour (
                                        id integer PRIMARY KEY,
                                        timestamp DATETIME DEFAULT (datetime(CURRENT_TIMESTAMP, 'localtime')),
                                        batteryVoltage float,
                                        soilMoisture float,
                                        soilTemperature float,
                                        airMoisture float,
                                        airTemperature float
                                    ); """
 
    sql_create_node_five_table = """ CREATE TABLE IF NOT EXISTS nodefive (
                                        id integer PRIMARY KEY,
                                        timestamp DATETIME DEFAULT (datetime(CURRENT_TIMESTAMP, 'localtime')),
                                        batteryVoltage float,
                                        soilMoisture float,
                                        soilTemperature float,
                                        airMoisture float,
                                        airTemperature float
                                    ); """
 

 
    # create a database connection
    if conn is not None:
        # create projects table
        create_table(conn, sql_create_node_one_table)
        create_table(conn, sql_create_node_two_table)
        create_table(conn, sql_create_node_three_table)
        create_table(conn, sql_create_node_four_table)
        create_table(conn, sql_create_node_five_table)

    else:
        print("Error! cannot create the database connection.")

def createentry(conn, nodeData):
    
    sql = ''' INSERT INTO nodeone(batteryVoltage,soilMoisture,soilTemperature,airMoisture,airTemperature)
              VALUES(?,?,?,?,?) '''
    cur = conn.cursor()
    cur.execute(sql, nodeData)
    conn.commit()

################### MAIN ########################

database = "pythonsqlite.db"
conn = create_connection(database)

createtables()

nodeData = (1.1, 1.2, 1.3, 1.4, 1.5)
createentry(conn, nodeData)

conn.close()