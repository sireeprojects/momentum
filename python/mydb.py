import os       # for system()
import time     # for sleep()
import getpass  # for getting password without echo
import sqlite3  # for database
import msvcrt   # to get a keystroke without echo

mydb = sqlite3.connect('products.db')
current_user=""

def clear_screen():
    os.system('cls');
    return

def login_page(mode):
    global current_user;
    uname = input("\n\033[1;35;40mUsername: \033[1;32;0m")
    upwd = getpass.getpass("\033[1;35;40mPassword:\033[1;32;0m")
    current_user = uname

    # query the user table if the entered name and password
    result = mydb.execute("SELECT * from users WHERE name=? and password=?", (uname, upwd))

    # check if the query returned anything
    avail = result.fetchone()

    if(avail==None):
        print("Invalid Username/password. Try again...")
        time.sleep(2)
        clear_screen()
        main_page()
    else:
        print("Username/password Correct")
        time.sleep(2)
        if(mode==1):
            clear_screen()
            admin_page()
        else:
            clear_screen()
            agent_page()
    return

def main_page():
    print("\033[1;32;40mWelcome to My Database Project\n\033[1;32;0m")
    print("Main Menu:\n")
    print("   1. Admin Login")
    print("   2. Agent Login")
    print("   3. Exit\n")
    option = input("Enter your option: ")

    if(option == '1' or option == '2'):
        login_page(int(option))
    elif(option == '3'):
        print("\nThank You")
    else:
        print("Invalid Option. Try again...");
        time.sleep(2)
        clear_screen()
        main_page()
    return

def admin_page():
    print("\033[1;32;40mWelcome to My Database Project\n\033[1;32;0m")
    print("Options for Admin:\n")
    print("   1. Add Products")
    print("   2. Display Inventory Details")
    print("   3. Logout\n")
    option = input("Enter your option: ")

    if(option == '1'):
        add_product()
    elif(option == '2'):
        display_inventory()
    elif(option == '3'):
        print("Returning to main menu...");
        time.sleep(2)
        clear_screen()
        main_page()
    else:
        print("Invalid Option. Try again...");
        time.sleep(2)
        clear_screen()
        admin_page()
    return

def add_product():
    pid      = input("\nEnter Product ID: ")
    pname    = input("Enter Product Name: ")
    pprice   = input("Enter Product Price: ")
    pminsell = input("Enter Product MinSellQuantity: ")
    pavail   = input("Enter Product Available Qty: ")

    # check if product ID already exists in the database
    result = mydb.execute("SELECT * from products WHERE id=?", (pid))
    avail = result.fetchone()

    if(avail==None): # new product
        mydb.execute("INSERT INTO products (id,name,minsell,price,avail) \
            values(?, ?, ?, ?, ?)", \
            (pid, pname, pminsell, pprice, pavail))
        mydb.commit()
        print("\nNew product added. Returning to admin menu...")
        time.sleep(2)
        clear_screen()
        admin_page()
    else:
        print("ERROR: Product already exist. Retry..")
        time.sleep(2)
        clear_screen()
        admin_page()
    return

def display_inventory():
    result = mydb.execute("SELECT * from products")
    print()
    print("\033[1;37;44m%-5s%-25s%-16s%-10s%-4s" % ("Id", "Name", "MinSellQty", "Price", "Available Qty\033[1;32;0m"))
    for row in result:
        print("%-5s%-25s%-16s%-10s%-4s" % (row[0],row[1],row[2],row[3],row[4]))
    print("\nPress any key to go back to Admin Menu...")
    msvcrt.getch() # block until any key is pressed
    clear_screen()
    admin_page()
    return

def agent_page():
    print("\033[1;32;40mWelcome to My Database Project\n\033[1;32;0m")
    print("Options for Agent:\n")
    print("   1. Buy/Sell")
    print("   2. Show History")
    print("   3. Logout\n")
    option = input("Enter your option: ")

    if(option == '1'):
        buy_sell()
    elif(option == '2'):
        show_history(current_user)
    elif(option == '3'):
        print("Returning to main menu...");
        time.sleep(2)
        clear_screen()
        main_page()
    else:
        print("Invalid Option. Try again...");
        time.sleep(2)
        clear_screen()
        agent_page()
    return

def buy_sell():
    txntype_str=""
    pid = input("\nEnter Product ID: ")
    result = mydb.execute("SELECT * from products WHERE id=?", [pid])
    avail = result.fetchone()
    if(avail==None):
        print("Product ID does not exist. Try again...")
        time.sleep(2)
        clear_screen()
        agent_page()

    txntype = input("Transaction type 1->Buy 2->Sell: ")
    
    if(txntype=='1'): 
        txntype_str = "Buy"
    elif(txntype=='2'): 
        txntype_str = "Sell"
    else: 
        print("Invalid Option. Returning to agent menu..")
        time.sleep(2)
        clear_screen()
        agent_page()
    
    result = mydb.execute("SELECT * from products WHERE id=?", [pid])
    for row in result:
        print("\nProuct Name: ", row[1])
        print("Prouct Cost: ", row[3])
        pname = row[1]
        pavail = row[4]
    
    pqty = input("\nEnter quantity: ")
    #if sell, check minsellquantity
    if(int(txntype)==2): 
        if(int(pqty) < row[2]): 
            print("Minimum Quantity that can be sold is: %d. Try again..." % (row[2]))
            time.sleep(2)
            clear_screen()
            agent_page()
    
    # check is requried quantity is available
    if(int(pqty) <= row[4]): 
        print("%d units of %s is available" % (int(pqty), row[1]))
    else:
        print("Sorry. %d units of %s is not available, Try again.." % (int(pqty), row[1]))
        time.sleep(2)
        clear_screen()
        agent_page()
    
    total = int(pqty) * row[3]
    print("Total cost of %d units of %s is Rs.%d" % (int(pqty) ,row[1], total))

    confirm = input("Confirm Booking? 1=>yes 2->Cancel: ")
    if(int(confirm)==1):
        # increment the last txnid before commiting new transaction
        result = mydb.execute("SELECT MAX(txnid) from transactions WHERE agent=?", [current_user])
        for row in result: maxtxnid = row[0]
        maxtxnid = maxtxnid+1

        mydb.execute("INSERT INTO transactions (agent,id,name,txn_type,qty,cost,txnid) \
            values (?, ?, ?, ?, ?, ?, ?)", (current_user, pid, pname, txntype_str, int(pqty), total, maxtxnid))
        mydb.commit()

        pavail = pavail - int(pqty)
        mydb.execute("UPDATE products set avail=? WHERE id=?", (pavail,pid))
        mydb.commit()

        print("\nBooking Confirmed. Returning to agent menu...")
        time.sleep(2)
        clear_screen()
        agent_page()
    else:
        print("\nBooking Cancelled, Returning to agent menu...")
        time.sleep(2)
        clear_screen()
        agent_page()
    return

def show_history(agent_name):
    result = mydb.execute("SELECT * from transactions WHERE agent=? ORDER BY txnid DESC", [agent_name])
    print()
    print("\033[1;37;44m%-16s%-5s%-16s%-10s%-12s%-12s%-12s" % ("Agent", "Id", "Name", "TxnType", "Quantity", "Cost", "TxnID\033[1;32;0m"))
    for row in result:
        print("%-16s%-5s%-16s%-10s%-12s%-12s%-12s" % (row[0],row[1],row[2],row[3],row[4],row[5],row[6]))

    result = mydb.execute("SELECT SUM(cost) from transactions WHERE agent=?", [agent_name])
    for row in result: print("\nCost of all transaction by %s is Rs.%d" % (current_user, row[0])) #FIX

    print("\nPress any key to go back to Agent Menu...")
    msvcrt.getch() # block until any key is pressed
    clear_screen()
    agent_page()
    return

def main():
    clear_screen()
    main_page()
    return

#show main menu
main()
