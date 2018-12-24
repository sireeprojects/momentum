import os       # for system()
import sqlite3  # for database

os.system('cls');
mydb = sqlite3.connect('products.db')

txntype_str=""
pid = input ("\nEnter Product ID: ")
txntype = input ("Transaction type 1->Buy 2->Sell: ")

if (txntype=='1'): txntype_str = "Buy"
elif (txntype=='2'): txntype_str = "Sell"
else: print ("Invalid Option. Returning to agent menu..")

result = mydb.execute("SELECT * from products WHERE id=?", (pid))
for row in result:
    print ("\nProuct Name: ", row[1])
    print ("Prouct Cost: ", row[3])
    pname = row[1]

pqty = input ("\nEnter quantity: ")
if (int(txntype)==2):
    if (int(pqty) < row[2]):
        print ("Minimum Quantity that can be sold is: ", row[2])

if (int(pqty) <= row[4]):
    print ("%d units of %s is available" % (int(pqty), row[1]))
else:
    print ("Sorry. %d units of %s is not available" % (int(pqty), row[1]))

total = int(pqty) * row[3]

print ("Total cost of %d units of %s is Rs.%d" % (int(pqty) ,row[1], total))
confirm = input ("Confirm Booking? 1=>yes 2->Cancel: ")
if (int(confirm)==1):
    print ("\nBooking Confirmed !")
    #add transaction to the history
    # get the lat txnid and increment it before commiting new transaction
    result = mydb.execute("SELECT MAX(txnid) from transactions WHERE agent=?", ['agent1'])
    for row in result: maxtxnid = row[0]
    maxtxnid = maxtxnid+1
    mydb.execute("INSERT INTO transactions (agent,id,name,txn_type,qty,cost,txnid) \
        values (?, ?, ?, ?, ?, ?, ?)", ('agent1', pid, pname, txntype_str, int(pqty), total, maxtxnid))
    mydb.commit()
else:
    print ("\nBooking Cancelled, Returning to agent menu...")
