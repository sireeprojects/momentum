import tkinter as tk
import tkinter.ttk as ttk
from tkinter import messagebox
from functools import partial
import sqlite3

def center(win):
    win.update_idletasks()
    width = win.winfo_width()
    height = win.winfo_height()
    x = (win.winfo_screenwidth()//2) - (width//2)
    y = (win.winfo_screenheight()//2) - (height//2)
    win.geometry('{}x{}+{}+{}'.format(width, height, x, y))

def logout_user():
    btn_addprod['state'] = tk.DISABLED
    btn_showinv['state'] = tk.DISABLED
    btn_logout['state'] = tk.DISABLED
    btn_buy['state'] = tk.DISABLED
    btn_sell['state'] = tk.DISABLED
    btn_showhist['state'] = tk.DISABLED
    btn_logout['state'] = tk.DISABLED
    #
    btn_admin['state'] = tk.NORMAL
    btn_agent['state'] = tk.NORMAL
    #
    tree.delete(*tree.get_children())
    tree['show'] = 'headings'
    tree["columns"]=()
    #
    global current_user
    current_user = ""
    return

def show_inventory():
    result = mydb.execute("SELECT * from products ORDER BY id DESC")
    #
    tree.delete(*tree.get_children())
    tree['show'] = 'headings'
    tree["columns"]=("1","2", "3", "4", "5")
    #
    tree.column("1", width=100)
    tree.column("2", width=150)
    tree.column("3", width=150)
    tree.column("4", width=50)
    tree.column("5", width=80)
    #
    tree.heading("1", text="Product ID")
    tree.heading("2", text="Product Name")
    tree.heading("3", text="Min Sell Quantity")
    tree.heading("4", text="Price")
    tree.heading("5", text="Available")
    #
    for row in result:
        tree.insert("",0,text="Line 1", values=(row[0],row[1],row[2],row[3],row[4]))
    return

def show_history():
    global current_user
    #
    result = mydb.execute("SELECT * from transactions WHERE agent=? ORDER BY txnid ASC", [current_user])
    tree.delete(*tree.get_children())
    tree['show'] = 'headings'
    tree["columns"]=("1","2", "3", "4", "5", "6", "7")
    #
    tree.column("1", width=100)
    tree.column("2", width=50)
    tree.column("3", width=150)
    tree.column("4", width=50)
    tree.column("5", width=80)
    tree.column("6", width=50)
    tree.column("7", width=50)
    #
    tree.heading("1", text="Agent Name")
    tree.heading("2", text="Product ID")
    tree.heading("3", text="Product Name")
    tree.heading("4", text="Type")
    tree.heading("5", text="Quantity")
    tree.heading("6", text="Cost")
    tree.heading("7", text="TxnID")
    #
    for row in result:
        tree.insert("",0,text="Line 1", values=(row[0],row[1],row[2],row[3],row[4],row[5],row[6]))
    return

def login_ok(name,pwd,p):
    global current_user
    uname = name.get()
    upwd = pwd.get()
    current_user = uname
    #
    # query the user table if the entered name and password
    result = mydb.execute("SELECT * from users WHERE name=? and password=?", (uname, upwd))
    avail = result.fetchone()
    #
    if(avail==None):
        messagebox.showerror("Error", "Invalid Username/password")
    else:
        if (avail[4]==1):
            current_user = 'admin'
            btn_addprod['state'] = tk.NORMAL
            btn_showinv['state'] = tk.NORMAL
            btn_logout['state'] = tk.NORMAL
            btn_admin['state'] = tk.DISABLED
            btn_agent['state'] = tk.DISABLED
            p.destroy()
        elif (avail[4]==2):
            btn_buy['state'] = tk.NORMAL
            btn_sell['state'] = tk.NORMAL
            btn_showhist['state'] = tk.NORMAL
            btn_logout['state'] = tk.NORMAL
            btn_admin['state'] = tk.DISABLED
            btn_agent['state'] = tk.DISABLED
            p.destroy()
    return

def check_pid_exist():
    global pid_var
    pid_to_check = pid_var.get()
    #
    # check if product ID already exists in the database
    result = mydb.execute("SELECT * from products WHERE id=?", [pid_to_check])
    avail = result.fetchone()
    #
    if(avail==None): # new product
        return True
    else:
        messagebox.showerror("Error", "Product already exists")
        return False

def check_pid_exist_bs():
    global pid_var_bs
    global pname_var_bs
    global pprice_var_bs
    pid_to_check = pid_var_bs.get()
    #
    # check if product ID already exists in the database
    result = mydb.execute("SELECT * from products WHERE id=?", [pid_to_check])
    avail = result.fetchone()
    #
    if(avail==None): # new product
        messagebox.showerror("Error", "Product does not exist")
        pname_var_bs.set ("")
        pprice_var_bs.set("")
        return False
    else:
        result = mydb.execute("SELECT * from products WHERE id=?", [pid_to_check])
        pname_var_bs.set (avail[1])
        pprice_var_bs.set(avail[3])
        return True

def check_qty():
    global pid_var_bs
    global pname_var_bs
    global pprice_var_bs
    global qty_var_bs
    global cost_var_bs
    pid_to_check = pid_var_bs.get()
    #
    # check if product ID already exists in the database
    result = mydb.execute("SELECT * from products WHERE id=?", [pid_to_check])
    avail = result.fetchone()
    #
    if (txn_type_gl=='sell'):
        if (int(qty_var_bs.get())<avail[2]):
            messagebox.showerror("Error", "Minimum Quantity that can be sold is %d" % (avail[2]))
            return False
    #
    if (int(qty_var_bs.get()) > avail[4]):
        messagebox.showerror("Error", "%d units of %s is ot available" % (int(qty_var_bs.get()), avail[1]))
        return False
    else:
        cost_var_bs.set(int(qty_var_bs.get()) * avail[3])
        global btn_confirm
        btn_confirm.config(state='normal')
        return True

def confirm_booking(window):
    global pid_var_bs
    global pname_var_bs
    global pprice_var_bs
    global qty_var_bs
    global cost_var_bs
    #
    # increment the last txnid before commiting new transaction
    result = mydb.execute("SELECT MAX(txnid) from transactions WHERE agent=?", [current_user])
    for row in result: maxtxnid = row[0]
    maxtxnid = maxtxnid+1
    #
    mydb.execute("INSERT INTO transactions (agent,id,name,txn_type,qty,cost,txnid) \
        values (?, ?, ?, ?, ?, ?, ?)", \
        (current_user, int(pid_var_bs.get()), pname_var_bs.get(), \
        txn_type_gl, int(qty_var_bs.get()), int(cost_var_bs.get()), maxtxnid))
    mydb.commit()
    #
    result = mydb.execute("SELECT * from products WHERE id=?", [int(pid_var_bs.get())])
    for row in result:pavail = row[4]
    pavail = pavail - int(qty_var_bs.get())
    mydb.execute("UPDATE products set avail=? WHERE id=?", (pavail,int(pid_var_bs.get())))
    mydb.commit()
    window.destroy()
    return

def add_new_prod(pid, pname, pprice, pminsell, pavail, window):
    mydb.execute("INSERT INTO products (id,name,minsell,price,avail) \
        values(?, ?, ?, ?, ?)", \
        (int(pid.get()), pname.get(), int(pminsell.get()), int(pprice.get()), int(pavail.get())))
    mydb.commit()
    window.destroy()
    return

def buysell(txn_type):
    global pid_var_bs
    global pname_var_bs
    global pprice_var_bs
    global qty_var_bs
    global cost_var_bs
    global btn_confirm
    global txn_type_gl
    #
    tree.delete(*tree.get_children())
    tree['show'] = 'headings'
    tree["columns"]=()
    #
    buysell_page = tk.Toplevel(win, borderwidth=5)
    txn_type_gl = tk.StringVar()
    txn_type_gl = txn_type
    #
    if (txn_type=="sell"):
        buysell_page.title("Sell Products")
    else:
        buysell_page.title("Buy Products")
    buysell_page.focus_set() # make active
    #
    lab_pid = tk.Label(buysell_page, text="Product ID")
    lab_pname = tk.Label(buysell_page, text="Product Name")
    lab_pprice = tk.Label(buysell_page, text="Product Price")
    lab_qty = tk.Label(buysell_page, text="Product Quantity")
    lab_cost = tk.Label(buysell_page, text="Total Cost")
    #
    pid_var_bs = tk.StringVar()
    pname_var_bs = tk.StringVar()
    pprice_var_bs =tk.StringVar()
    qty_var_bs = tk.StringVar()
    cost_var_bs = tk.StringVar()
    #
    inp_pid_bs = tk.Entry(buysell_page, textvariable=pid_var_bs, \
        validatecommand=check_pid_exist_bs, validate='focusout')
    inp_pname = tk.Entry(buysell_page, textvariable=pname_var_bs   )
    inp_pprice = tk.Entry(buysell_page, textvariable=pprice_var_bs  )
    inp_qty = tk.Entry(buysell_page, textvariable=qty_var_bs, 
        validatecommand=check_qty, validate='focusout')
    inp_cost = tk.Entry(buysell_page, textvariable=cost_var_bs  )
    #
    lab_pid.grid(row=0, column=0, pady=5, sticky='e')
    lab_pname.grid(row=1, column=0, pady=5, sticky='e')
    lab_pprice.grid(row=2, column=0, pady=5, sticky='e')
    lab_qty.grid(row=3, column=0, pady=5, sticky='e')
    lab_cost.grid(row=4, column=0, pady=5, sticky='e')
    #
    inp_pid_bs.grid(row=0, column=1, columnspan=2, pady=5)
    inp_pname.grid(row=1, column=1, columnspan=2, pady=5)
    inp_pprice.grid(row=2, column=1, columnspan=2, pady=5)
    inp_qty.grid(row=3, column=1, columnspan=2, pady=5)
    inp_cost.grid(row=4, column=1, columnspan=2, pady=5)
    #
    btn_confirm = tk.Button(buysell_page, text='Confirm', width=10, \
        state=tk.DISABLED, command=lambda:confirm_booking(buysell_page))
    btn_cancel = tk.Button(buysell_page, text='Cancel' , width=10, command=buysell_page.destroy)
    btn_cancel.grid(row=5, column=2, pady=5, sticky='e')
    btn_confirm.grid(row=5, column=1, pady=5, sticky='we')
    #
    inp_pid_bs.focus()
    center(buysell_page)
    buysell_page.grab_set() # modal
    buysell_page.resizable(width=False, height=False)
    buysell_page.transient(win) # single window on taskbar
    return #}


def add_product():
    tree.delete(*tree.get_children())
    tree['show'] = 'headings'
    tree["columns"]=()
    add_prod_page = tk.Toplevel(win, borderwidth=5)
    add_prod_page.title("Add a New Product")
    add_prod_page.focus_set() # make active
    #
    lab_pid = tk.Label(add_prod_page, text="Product ID")
    lab_pname = tk.Label(add_prod_page, text="Product Name")
    lab_pprice = tk.Label(add_prod_page, text="Product Price")
    lab_pminsell = tk.Label(add_prod_page, text="Product MinSellQuantity")
    lab_pavail = tk.Label(add_prod_page, text="Product Available Qty")
    #
    global pid_var
    pid_var = tk.StringVar()
    pname_var = tk.StringVar()
    pprice_var =tk.StringVar()
    pminsell_var = tk.StringVar()
    pavail_var = tk.StringVar()
    #
    inp_pid = tk.Entry(add_prod_page, textvariable=pid_var, \
        validatecommand=check_pid_exist, validate='focusout')
    inp_pname = tk.Entry(add_prod_page, textvariable=pname_var   )
    inp_pprice = tk.Entry(add_prod_page, textvariable=pprice_var  )
    inp_pminsell = tk.Entry(add_prod_page, textvariable=pminsell_var )
    inp_pavail = tk.Entry(add_prod_page, textvariable=pavail_var  )
    #
    lab_pid.grid(row=0, column=0, pady=5, sticky='e')
    lab_pname.grid(row=1, column=0, pady=5, sticky='e')
    lab_pprice.grid(row=2, column=0, pady=5, sticky='e')
    lab_pminsell.grid(row=3, column=0, pady=5, sticky='e')
    lab_pavail.grid(row=4, column=0, pady=5, sticky='e')
    #
    inp_pid.grid(row=0, column=1, columnspan=2, pady=5)
    inp_pname.grid(row=1, column=1, columnspan=2, pady=5)
    inp_pprice.grid(row=2, column=1, columnspan=2, pady=5)
    inp_pminsell.grid(row=3, column=1, columnspan=2, pady=5)
    inp_pavail.grid(row=4, column=1, columnspan=2, pady=5)
    #
    btn_ok = tk.Button(add_prod_page, text='OK', width=10, \
        command=partial(add_new_prod, pid_var, pname_var, \
        pprice_var, pminsell_var, pavail_var, add_prod_page))
    btn_cancel = tk.Button(add_prod_page, text='Cancel', \
        width=10, command=add_prod_page.destroy)
    btn_cancel.grid(row=5, column=2, pady=5, sticky='e')
    btn_ok.grid(row=5, column=1, pady=5, sticky='we')
    #
    inp_pid.focus()
    center(add_prod_page)
    add_prod_page.grab_set() # modal
    add_prod_page.resizable(width=False, height=False)
    add_prod_page.transient(win) # single window on taskbar
    return

def show_login():
    login_page = tk.Toplevel(win, borderwidth=5)
    login_page.title("Login")
    login_page.focus_set() # make active
    #
    lab_name = tk.Label(login_page, text="Name"    )
    lab_pwd = tk.Label(login_page, text="Password")
    inp_name = tk.Entry(login_page             )
    inp_pwd = tk.Entry(login_page  , show='*' )
    btn_ok = tk.Button(login_page, text='OK', width=10, command=partial(login_ok, inp_name, inp_pwd, login_page))
    btn_cancel = tk.Button(login_page, text='Cancel' , width=10, command=login_page.destroy)
    #
    lab_pwd.grid(row=1, column=0, pady=5)
    lab_name.grid(row=0, column=0, pady=5)
    btn_cancel.grid(row=3, column=2, pady=3, sticky='e')
    btn_ok.grid(row=3, column=1, pady=3, sticky='we')
    inp_name.grid(row=0, column=1, columnspan=2, ipady=2, sticky='we')
    inp_pwd.grid(row=1, column=1, columnspan=2, ipady=2, sticky='we')
    inp_name.focus()
    center(login_page)
    login_page.grab_set() # modal
    login_page.resizable(width=False, height=False)
    login_page.transient(win) # single window on taskbar

global win
global login_page
global add_prod_page
global buysell_page
global current_user
global pid_var
global pid_var_bs
global pname_var_bs
global pprice_var_bs
global qty_var_bs
global cost_var_bs
global txn_type_gl
global btn_confirm
global btn_admin    
global btn_agent    
global btn_showinv  
global btn_buy      
global btn_sell     
global btn_showhist 
global btn_logout   
global btn_exit     
global btn_addprod
global tree

global mydb
mydb = sqlite3.connect('products.db')

win = tk.Tk()
#
btn_admin = tk.Button(win, text='Admin', width=10, command=show_login)
btn_agent = tk.Button(win, text='Agent', width=10, command=show_login)
btn_addprod = tk.Button(win, text='Add Product', width=14, state=tk.DISABLED, command=add_product)
btn_showinv = tk.Button(win, text='Show Inventory', width=14, state=tk.DISABLED, command=show_inventory)
btn_buy = tk.Button(win, text='Buy', width=10, state=tk.DISABLED, command=partial(buysell, "buy"))
btn_sell = tk.Button(win, text='Sell', width=10, state=tk.DISABLED, command=partial(buysell, "sell"))
btn_showhist = tk.Button(win, text='Show History', width=14, state=tk.DISABLED, command=show_history)
btn_logout = tk.Button(win, text='Logout', width=10, state=tk.DISABLED, command=logout_user)
btn_exit = tk.Button(win, text='Exit', width=10, command=win.quit )
#
btn_addprod.grid(row=0, column=2, sticky='w')
btn_admin.grid(row=0, column=0, sticky='w')
btn_agent.grid(row=0, column=1, sticky='w')
btn_showinv.grid(row=0, column=3, sticky='w')
btn_buy.grid(row=0, column=4, sticky='we')
btn_sell.grid(row=0, column=5, sticky='we')
btn_showhist.grid(row=0, column=6, sticky='we')
btn_logout.grid(row=0, column=7, sticky='w')
btn_exit.grid(row=0, column=8, sticky='w')

tree = ttk.Treeview(win)
# ysb = tk.Scrollbar(win, orient='vertical', command=tree.yview)
# ysb.grid(row=1, column=9, sticky='ns')
# tree.configure(yscroll=ysb.set)
tree.grid(row=1, column=0, columnspan=9, sticky='nsew')

#disable resizing of columns
for i in range (0,9): win.columnconfigure (i,weight=10)

# show main window
win.title("Inventory Management Project")
win.configure(background='white')
win.resizable(width=False, height=False)
center(win)
win.mainloop()
