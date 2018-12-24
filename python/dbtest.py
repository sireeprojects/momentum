import os       # for system()
import sqlite3  # for database


os.system('cls');
mydb = sqlite3.connect('products.db')

result = mydb.execute("SELECT * from transactions WHERE agent=? ORDER BY txnid DESC", ['agent1'])
print()
print("\033[1;37;44m%-16s%-5s%-16s%-10s%-12s%-12s%-12s" % ("Agent", "Id", "Name", "TxnType", "Quantity", "Cost", "TxnID\033[1;32;0m"))
for row in result:
    print("%-16s%-5s%-16s%-10s%-12s%-12s%-12s" % (row[0],row[1],row[2],row[3],row[4],row[5],row[6]))

result = mydb.execute("SELECT SUM(cost) from transactions WHERE agent=?", ['agent1'])
for row in result: print("%-16s" % (row[0])) #FIX

result = mydb.execute("SELECT MIN(cost) from transactions WHERE agent=?", ['agent1'])
for row in result: print("%-16s" % (row[0])) #FIX

