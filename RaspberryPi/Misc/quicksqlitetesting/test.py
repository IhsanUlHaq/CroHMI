from quicksqlite import Connection

con = Connection(path="Database.db", auto_commit=False, reconnects=5, auto_connect=True)


