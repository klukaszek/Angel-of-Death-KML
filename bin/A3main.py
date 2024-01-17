from tkinter import *
from tkinter import filedialog
from tkinter import scrolledtext

from tkinter.ttk import *

from os import getcwd
from pathlib import Path
from sys import float_info
from ctypes import *

#TODO: Work on editing structs within GUI, tree populates correctly as of now
#NOTE: Lecture said to use createValidKML()
#NOTE: isloop tolerance is 10-15m by default

class App(Tk):

    """
    This class is a simple Tk object that contains all the functions that the app needs to use.
    The only thing changed with this Tk object are callbacks for keyboard shortcuts.
    """

    #initialize app window as Tk object
    def __init__(self):
        super().__init__()
        self.initialize_gui()

    #initializes gui components
    def initialize_gui(self):

        # generate gui window
        self.title("KMLbrowser")
        self.geometry("800x600")
        self.resizable(True, True)

        #configure row and columns for grid
        self.rowconfigure(0, weight=1)
        self.columnconfigure(0, weight=1)

        # set icon for application
        self.set_icon(self)

        #center window on display
        self.center(self)

        #New class variables initialized to None just for understandability
        self.file_panel = None
        self.point_tab = None
        self.path_tab = None
        self.style_tab = None
        self.log = None

        #Current KML struct
        self.kml_schema = 'ogckml22.xsd'.encode('utf-8')
        self.kml = None
        self.num_points = 0
        self.num_paths = 0
        self.num_styles = 0

        #filepath is used for Save
        self.filepath = ''
        #filename is used for strings
        self.filename = ''

        # generate menu tab for window
        self.generate_menu_bar()
        #generate file view panel
        self.generate_file_panel()
        # generate log panel at bottom of screen
        self.generate_log_panel()
        #get inputs from user for menu bar macros
        self.enable_input_events()

        #set the [x] button functionality to open the exit window to ask the user if they want to exit
        self.protocol("WM_DELETE_WINDOW", self.exit_func)
        
        #call mainloop to run window
        self.mainloop()

    #Opens file explorer on system
    def open_func(self):

        #Get path and convert it to posix
        path = Path(getcwd())
        path = path.as_posix()

        #Open the file dialog window for opening files
        file = filedialog.askopenfilename(
            initialdir=path,
            title="Select a file to open",
            filetypes=(("KML File", "*.kml*"), ("All Files", "*.*"))
        )
        
        #this is to check if the user pressed cancel in the file dialog box
        if str(file) == "" or str(file) == "()":
            return

        #Check if file ends with .kml
        if(file[-4::] == '.kml'):
            
            #save file so that it can be referenced when saved
            self.filepath = file

            #get filename
            tokens = file.split('/')
            self.filename = tokens[len(tokens)-1]

            #Create a KML struct using createValidKML() here with the filename
            #Use create valid based on lecture answer
            self.print_to_log("Calling createValidKML()\n")

            self.kml = createValidKML(file.encode('utf-8'), self.kml_schema)

            #Print to log
            self.print_to_log("File {} succesfully opened\n".format(self.filename))

            #Change window title to include filename
            self.title("KMLbrowser: " + self.filename)

            #update filename in file view panel
            self.file_panel.heading('#0', text=self.filename)

            #populate tree so with data from kml file
            self.populate_tree()
        else:
            #Print to log
            self.print_to_log("Unable to open file {}\n".format(file))
            
    #General save function, takes a filename and then validates KML and then writes to file. Called by save_func() and save_as_func().
    def save(self, filename):
        #valid = function call to validateKML
        valid = False
        self.print_to_log("Calling validateKML()\n")
        valid = validateKML(self.kml, self.kml_schema)

        #if valid, check if file writes successfully, else print validation failure message
        if valid:
            #written = function call to writeKML()
            written = False
            self.print_to_log("Calling writeKML()\n")
            written = writeKML(self.kml, filename.encode('utf-8'))

            #if file is written successfully, print success message, else print failure message
            if written:
                self.print_to_log("File {} successfully validated and saved\n".format(filename))
                return True
            else:
                self.print_to_log("Unable to save file {}\n".format(filename))

        else:
            self.print_to_log("Unable to validate file {} before saving.\n".format(filename))

        return False
    
    #Saves current file with the name that it was loaded in with
    def save_func(self):
        self.save(self.filename)
    
    #Saves current file with new name by opening file dialog window
    def save_as_func(self):
        
        #Get path and convert it to posix
        path = Path(getcwd())
        path = path.as_posix()
        
        #Open the file dialog window for opening files
        file = filedialog.asksaveasfilename(
            initialdir=path,
            title="Enter a filename:",
            filetypes=(("KML File", "*.kml*"), ("All Files", "*.*"))
        )
        
        #this is to check if the user pressed cancel in the file dialog box
        if file == "":
            return

        #save file using filename
        success = self.save(file)
        
        #update gui if save_as is succesful
        if(success is True):
            self.filepath = file
            
            #get filename
            tokens = file.split('/')
            self.filename = tokens[len(tokens)-1]

            #Change window title to include filename
            self.title("KMLbrowser: " + self.filename)

            #update filename in file view panel
            self.file_panel.heading('#0', text=self.filename)

    #Opens the about window
    def about_func(self):

        #initialize window
        window = Toplevel(self)
        window.title("About")
        window.geometry("400x175")
        window.resizable(False, False)
        
        #set window icon
        self.set_icon(window)

        #center window on display
        self.center(window)

        #focus window
        window.focus()

        #Label with About information
        Label(window, 
        text="""
        About this app:
        This app is used to view and edit KML files.
        
        App developed by: Kyle Lukaszek

        Last Edit: November 26 2022""").pack()

        self.print_to_log("Opened the About menu.\n")

        Button(window, text="Close", command=window.destroy).place(x=150, y=125)
        
        #bind escape to close window
        window.bind('<Escape>', lambda event: window.destroy())

    #handles program shutdown
    def exit_func(self):

        self.print_to_log("Invoked exit command. Will use deleteKML() upon shutown if a KML file is loaded.\n")

        #initialize window
        window = Toplevel(self)
        window.title("Exit?")
        window.geometry("300x100")
        window.resizable(False, False)

        #set icon
        self.set_icon(window)
        
        #center window on display
        self.center(window)

        #focus window
        window.focus()

        #Label asking user if they want to exit
        Label(window, text="Are you sure you would like to exit?").pack(padx=20, pady=20)

        #make a shutdown function that uses the C library to free memory
        #Buttons for window
        Button(window, text="Yes", command=self.quit_program).place(x=50, y=50)
        Button(window, text="No", command=window.destroy).place(x=150, y=50)

        #bind escape to close window
        window.bind('<Escape>', lambda event: window.destroy())

    #calls delete KML if a file has been loaded in, and then exits the app
    def quit_program(self):
        if(self.kml != None):
            deleteKML(self.kml)
        self.quit()

    #generates top menu bar for window
    def generate_menu_bar(self):

        #create a menu bar for the root window
        menubar = Menu(self)
        file = Menu(menubar, tearoff=0)

        #set commands in file menu
        file.add_command(label="Open", command=self.open_func, accelerator='Ctrl+O')
        file.add_command(label="Save", command=self.save_func, accelerator='Ctrl+S')
        file.add_command(label="Save as...", command=self.save_as_func, accelerator='Ctrl+Alt+S')
        file.add_separator()
        file.add_command(label="Exit", command=self.exit_func, accelerator='Ctrl+X')

        #add file menu to menubar
        menubar.add_cascade(label="File", menu=file)

        #add about menu to menubar
        menubar.add_command(label="About", command=self.about_func)

        #add menubar to root window
        self.config(menu=menubar)

    #generates file view panel for window
    def generate_file_panel(self):
        
        #Create frame to put file panel with scrollbars and place it in the root window
        container = Frame(self)
        container.grid(row=0, column=0, sticky='nsew')

        #Initialize treeview widget
        file_panel = Treeview(container, columns=('Data'))
        file_panel.heading('#0', text='No File Open', anchor=W)
        file_panel.column('#0', minwidth=50, width=300, stretch=NO, )
        file_panel.heading('Data', text='Attribute Values', anchor=W)
        file_panel.column('Data', minwidth=100)

        #Configure Vertical scrollbar
        vsb = Scrollbar(container, orient='vertical')
        vsb.pack(side=RIGHT, fill=Y)
        vsb.configure(command=file_panel.yview)

        #Configure Horizontal scrollbar
        hsb = Scrollbar(container, orient='horizontal')
        hsb.pack(side=BOTTOM, fill=X)
        hsb.configure(command=file_panel.xview)
        
        #Set file panel scrolling commands
        file_panel.configure(xscrollcommand=hsb.set, yscrollcommand=vsb.set)

        #List that there are no points, no paths, and no styles at start of program
        point_tab = file_panel.insert(parent="", index=END, text="No Points")
        path_tab = file_panel.insert(parent="", index=END, text="No Paths")
        style_tab = file_panel.insert(parent="", index=END, text="No Styles")

        #Assign pack file panel into container
        file_panel.pack(fill='both', expand=True)

        #Add callback for double click text editing so that we can edit specific value entries on the tree
        file_panel.bind('<Double-Button-1>', self.edit_entry_callback)
        
        #Set class variables
        self.file_panel = file_panel
        self.point_tab = point_tab
        self.path_tab = path_tab
        self.style_tab = style_tab

    #this function populates the tree with the data from our KML object that is parsed from a file
    def populate_tree(self):
        
        #Start off by clearing the current data that is populating the tree
        self.file_panel.delete(self.point_tab)
        self.file_panel.delete(self.path_tab)
        self.file_panel.delete(self.style_tab)

        #populate respective tabs with expected data
        self.populate_points()
        self.populate_paths()
        self.populate_styles()

    #called by populate tree
    def populate_points(self):

        #Populate Points, if no points, indicate that in the tree
        if(getNumPoints(self.kml) == 0):
            self.point_tab = self.file_panel.insert(parent="", index=END, text="No Points")
        else:
            self.point_tab = self.file_panel.insert(parent="", index=END, text="Points")

            #get list of point placemarks and create iterator
            list = self.kml.contents.pointPlacemarks
            iter = createIterator(list)
            elem = nextElement(byref(iter))
            count = 0

            #iterate through list of point placemarks and populate tree view with point placemark data
            while elem is not None:
                
                count+=1
                name = ''
                if getPointName(elem) != None:
                    name = getPointName(elem).decode('utf-8')
                long = getPointLong(elem)
                lat = getPointLat(elem)
                alt = getPointAlt(elem)

                #check if alt is set to DBL_MAX in C
                if (alt == float_info.max): alt = 'N/A'

                #add info cells
                tmp = self.file_panel.insert(parent=self.point_tab, index=END, text='Point {}'.format(count))
                self.file_panel.insert(parent=tmp, index=END, text='Name:', values=[name])
                self.file_panel.insert(parent=tmp, index=END, text='Longitude:', values=[long])
                self.file_panel.insert(parent=tmp, index=END, text='Latitude:', values=[lat])
                self.file_panel.insert(parent=tmp, index=END, text='Altitude', values=[alt])

                elem = nextElement(byref(iter))

            self.num_points = count
                
    #called by populate tree
    def populate_paths(self):

        #Populate Paths, if no paths, indicate that in the tree
        if(getNumPaths(self.kml) == 0):
            self.path_tab = self.file_panel.insert(parent="", index=END, text="No Paths")
        else:
            self.path_tab = self.file_panel.insert(parent="", index=END, text="Paths")

            #get list of path placemarks and create iterator
            list = self.kml.contents.pathPlacemarks
            iter = createIterator(list)
            elem = nextElement(byref(iter))
            count = 0

            #iterate through list of path placemarks and populate tree view with path placemark data
            while elem is not None:
                
                count+=1
                name = ''
                if getPathName(elem) != None:
                    name = getPathName(elem).decode('utf-8')
                length = getPathLength(elem)
                is_loop = checkPathLoop(elem, 15.0)

                #add info cells
                tmp = self.file_panel.insert(parent=self.path_tab, index=END, text='Path {}'.format(count))
                self.file_panel.insert(parent=tmp, index=END, text='Name:', values=[name])
                self.file_panel.insert(parent=tmp, index=END, text='Length:', values=[length])
                self.file_panel.insert(parent=tmp, index=END, text='Is Loop:', values=[is_loop])

                elem = nextElement(byref(iter))
            
            self.num_paths = count

    #called by populate tree
    def populate_styles(self):

        #Populate Styles, if no styles, indicate that in the tree
        if(getNumStyles(self.kml) == 0):
            self.style_tab = self.file_panel.insert(parent="", index=END, text="No Styles")
        else:
            self.style_tab = self.file_panel.insert(parent="", index=END, text="Styles")

            #Get list of styles and create iterator
            list = self.kml.contents.styles
            iter = createIterator(list)
            elem = nextElement(byref(iter))
            count = 0

            #iterate through list of styles and populate tree view with style data
            while elem is not None:
                
                count+=1
                col = getStyleColour(elem).decode('utf-8')
                width = getStyleWidth(elem)
                fill = getStyleFill(elem)

                #check if width or fill are uninitialized
                if (width == -1): width = 'N/A'
                if (fill == -1): fill = 'N/A'

                #add info cells
                tmp = self.file_panel.insert(parent=self.style_tab, index=END, text='Style {}'.format(count))
                self.file_panel.insert(parent=tmp, index=END, text='Colour:', values=[col])
                self.file_panel.insert(parent=tmp, index=END, text='Width:', values=[width])
                self.file_panel.insert(parent=tmp, index=END, text='Fill:', values=[fill])

                elem = nextElement(byref(iter))
                
            self.num_styles = count

    
    #handle double clicks on cells in the tree
    def edit_entry_callback(self, event):
        
        #Get row and column of whatever was just clicked on
        row = self.file_panel.identify_row(event.y)
        col = self.file_panel.identify_column(event.x)

        #Check if user is clicking on left side of treeview
        #col is '#0' on the left column, so we return if that is what was clicked on since we don't change anything on the left side
        if col == '#0':
            return
        
        #try and except are here if the user clicks on a cell that does not exist
        try:
            #get position of cell from TreeView widget so that the temporary cell can exist above the original
            x, y, width, height = self.file_panel.bbox(row, col)
            pady = height // 2
        except:
            return

        #get name of row
        title = self.file_panel.item(row, 'text')

        #Assignment outline says that we can only edit names of Points, Paths, and we can edit the Width and Colour of a Style
        if title != 'Name:' and title != 'Width:' and title != 'Colour:':
            return

        #get text list from values of 'Data' column
        text = self.file_panel.item(row, 'values')

        #Check if text is empty, any empty cell should not be editable since we are using N/A when information is missing
        try:
            test = text[0]
        except:
            return

        #since there is only 1 column ('Data') we can write to, we take text[0] since that is the value of 'Data' on a specific row
        cur_text = text[0]

        #Check if width or colour are set to N/A, return if true since we can't edit empty ones
        if (title == 'Width:' or title == 'Colour:') and cur_text == 'N/A':
            return

        #Create temporary editable entry widget using TreeView and text
        self.editable_entry = EditEntry(self, self.file_panel, row, cur_text)
        self.editable_entry.place(x=x, y=y+pady, anchor=W, width=width, height=height)
        

    #generates log panel for window with clear button
    def generate_log_panel(self):
        #Create frame for scrolledtext and button. Put frame into root window grid
        container = Frame(self)
        container.grid(row=1, column=0, sticky='s')

        #Create output log using scrolledtext widget that the user cannot write to. pack it in the container
        log = scrolledtext.ScrolledText(container, width=self.winfo_width(), height=10, state=DISABLED, )
        log.pack(fill='both', expand=True)

        #Configure Horizontal scrollbar
        hsb = Scrollbar(container, orient='horizontal')
        hsb.pack(side=BOTTOM, fill=X)
        hsb.configure(command=log.xview)

        #Set file panel scrolling commands
        log.configure(xscrollcommand=hsb.set)

        #set self.log so we can print to log from within the class sc
        self.log = log

        #Create button to clear output log and pack it in the container
        clear = Button(container, text="Clear", command=self.clear_log)
        clear.pack(fill=X)
        
    
    #helper function to quickly print text to log panel
    def print_to_log(self, text):
        #set state to normal so text can be written to text box, write, then disable it again
        self.log.configure(state=NORMAL)
        self.log.insert(INSERT, text)
        self.log.configure(state=DISABLED)

    def clear_log(self):
        #set state to normal so text can be deleted from text box, then disable it again
        self.log.configure(state=NORMAL)
        self.log.delete('1.0', END)
        self.log.configure(state=DISABLED)

    #a frame is used so that the user doesn't accidentally use a keyboard shortcut when typing
    def enable_input_events(self):
        #Focus window
        self.focus()

        #Bind key events for keyboard shortcuts, we use lambda ignore to ignore the event being passed by bind so we can call our functions
        #NOTE: "lambda var: function()"  is used to execute a function while ignoring the event being passed to it from Tk.bind(), normally the event will be passed to the callback so we use a lambda to get around that
        
        #Control + 'o' = open
        self.bind("<Control-KeyPress-o>", lambda event: self.open_func())
        self.bind("<Control-KeyPress-O>", lambda event: self.open_func())

        #Control + 's' = save
        self.bind("<Control-KeyPress-s>", lambda event: self.save_func())
        self.bind("<Control-KeyPress-S>", lambda event: self.save_func())

        #Control + Alt + 's' = save as, (Control + Shift + S was not working since shift kept changing the letter case)
        self.bind("<Control-Alt-KeyPress-s>", lambda event: self.save_as_func())
        self.bind("<Control-Alt-KeyPress-S>", lambda event: self.save_as_func())

        #Control + 'x' = exit
        self.bind("<Control-KeyPress-x>", lambda event: self.exit_func())
        self.bind("<Control-KeyPress-X>", lambda event: self.exit_func())

    #helper function to center a tkinter window on a display from:
    #https://stackoverflow.com/questions/3352918/how-to-center-a-window-on-the-screen-in-tkinter
    def center(self, window):

        window.update_idletasks()
        
        #Get original window width
        width = window.winfo_width()

        #Calculations needed to find x
        frm_width = window.winfo_rootx() - window.winfo_x()
        window_width = width + 2 * frm_width

        #Get original window height
        height = window.winfo_height()

        #Calculations needed to find y
        titlebar_height = window.winfo_rooty() - window.winfo_y()
        window_height = height + titlebar_height + frm_width

        #Calculate x and y for top left corner of window so that it is centered on display
        x = window.winfo_screenwidth() // 2 - window_width // 2
        y = window.winfo_screenheight() // 2 - window_height // 2

        #Update geometry with correct padding
        window.geometry('{}x{}+{}+{}'.format(width, height, x, y))
        window.deiconify()

    #sets window icon for Tk object
    def set_icon(self, window):
        path = Path(getcwd())
        path = path.as_posix() + "/kml-icon.png"
        icon = PhotoImage(file = path)
        window.wm_iconphoto(False, icon)

#Class to help with treeview widget so that cells can be edited
class EditEntry(Entry):

    """
    This entry class is used to create a temporary treeview cell that can be edited when double clicked on
    and then discarded once the user hits enter/return or decides they do not want to edit the cell when they press escape
    """

    def __init__(self, app: App, tree: Treeview, row, text):
        super().__init__(tree)
        self.app = app
        self.file_panel = tree
        self.row = row

        #Set text in temporary Entry to match text from actual Entry
        self.insert(0, text) 

        #focus force fixes a typing problem
        self.focus_force()

        #NOTE: "lambda var: function()"  is used to execute a function while ignoring the event being passed to it from Tk.bind(), normally the event will be passed to the callback so we use a lambda to get around that

        #Set event bind for when a user changes the data in an entry with Enter/Return 
        self.bind("<Return>", lambda event: self.submit_edit())
        #Set event bind for when a user cancels whatever they were trying to edit with Escape
        self.bind("<Escape>", lambda event: self.destroy())

    #Set tree entry data when user presses enter
    def submit_edit(self):
        #Get original string
        old_text = self.file_panel.item(self.row, 'values')

        #get whatever is typed into the temporary entry
        value = self.get()

        # WRITE A FUNCTION THAT CHANGES THE INFORMATION OF WHATEVER WAS EDITED IN THE KML STRUCT SO THAT IT CAN BE VALIDATED AND REWRITTEN
        written = self.edit_struct(value)

        if(written):
            #change the contents of the original cell
            self.file_panel.item(self.row, values=[value])
        else:
            self.app.print_to_log("Could not write {} to row {}\n".format(value, self.row))
            self.destroy()
            return

        #destroy temporary entry when user enters new data
        self.destroy()

        #Create temporary editable entry widget using TreeView and text
        new_text = self.file_panel.item(self.row, 'values')
        self.app.print_to_log("Value {} in row {} was updated to {}\n".format(old_text[0], self.row, new_text[0]))

    #Called by submit edit, handles logic for editing struct data
    def edit_struct(self, value):
        #get parent of selected row to find out the index to edit in the item list
        parent_id = self.file_panel.parent(self.row)
        selection_parent = self.file_panel.item(parent_id)['text']

        #get top parent to determine which list to iterate through
        category = self.file_panel.parent(parent_id)
        category = self.file_panel.item(category)['text']

        #get index to figure out what is being edited
        child_index = self.file_panel.index(self.row)

        #get index from selection_parent string and subtract 1
        index = selection_parent[-1]
        index = int(index) - 1

        #Handle editing points
        if(category == 'Points'):
     
            #Get list of styles and create iterator
            list = self.app.kml.contents.pointPlacemarks
            iter = createIterator(list)
            elem = nextElement(byref(iter))
            count = 0

            #iterate through list of points until we get the right one
            while(elem is not None and count < index):
                elem = nextElement(byref(iter))
                count += 1

            #if child_index = 0 that means we are updating the name of the point
            if(child_index == 0):
                
                #set point name after getting pointPlacemark as void pointer
                setPointName(elem, str(value).encode('utf-8'))
        
        #Handle editing paths
        elif(category == 'Paths'):

            #Get list of styles and create iterator
            list = self.app.kml.contents.pathPlacemarks
            iter = createIterator(list)
            elem = nextElement(byref(iter))
            count = 0

            #iterate through list of points until we get the right one
            while(elem is not None and count < index):
                elem = nextElement(byref(iter))
                count += 1

            #if child_index = 0 that means we are updating the name of the path
            if(child_index == 0):
                #set path name after getting pathPlacemark struct as void pointer
                setPathName(elem, str(value).encode('utf-8'))
        
        #Handle editing styles
        elif(category == 'Styles'):

            #Get list of styles and create iterator
            list = self.app.kml.contents.styles
            iter = createIterator(list)
            elem = nextElement(byref(iter))
            count = 0

            #iterate through list of points until we get the right one
            while(elem is not None and count < index):
                elem = nextElement(byref(iter))
                count += 1

            #if child_index = 0 that means we are updating the colour of the style
            if(child_index == 0):
                #set style colour after getting style struct as void pointer
                setStyleColour(elem, str(value).encode('utf-8'))

            #if child_index = 1 that means we are updating the width of the style
            if(child_index == 1):
                try:
                    value = int(value)
                except:
                    return False
                #set style width after getting style struct as void pointer
                setStyleWidth(elem, value)
            
        return True

#Node for list class
class Node(Structure):
    pass
            
Node._fields_ = [
    ("data", c_void_p),
    ("previous", POINTER(Node)),
    ("next", POINTER(Node))
]

#List class for C linked list implementation
class List(Structure):

    _fields_ = [
        ("head", POINTER(Node)),
        ("tail", POINTER(Node)),
        ("deleteData", c_void_p),
        ("compareData", POINTER(c_int)),
        ("printData", c_void_p)
    ]

#list iterator struct implementation
class ListIterator(Structure):

    _fields_ = [
        ("current", POINTER(Node))
    ]

#KML struct implementation
class KML(Structure):

    _fields_ = [
        ("namespaces", POINTER(List)),
        ("pointPlacemarks", POINTER(List)),
        ("pathPlacemarks", POINTER(List)),
        ("styles", POINTER(List)),
        ("styleMaps", POINTER(List))
    ]

#initialize shared library
libpath = './libkmlparser.so'
kmllib = CDLL(libpath)

#define createValidKML Parser Library function in Python
createValidKML = kmllib.createValidKML
createValidKML.argtypes = [c_char_p, c_char_p]
createValidKML.restype = POINTER(KML)

#define validateKML Parser Library function in Python
validateKML = kmllib.validateKML
validateKML.argtypes = [POINTER(KML), c_char_p]
validateKML.restype = c_bool

#define writeKML Parser Library function in Python
writeKML = kmllib.writeKML
writeKML.argtypes = [POINTER(KML), c_char_p]
writeKML.restype = c_bool

#define deleteKML Parser Library function in Python
deleteKML = kmllib.deleteKML
deleteKML.argtypes = [POINTER(KML)]

#define createIterator Parser Library function in Python
createIterator = kmllib.createIterator
createIterator.argtypes = [POINTER(List)]
createIterator.restype = ListIterator

#define nextElement Parser Library function in Python
nextElement = kmllib.nextElement
nextElement.argtypes = [POINTER(ListIterator)]
nextElement.restype = c_void_p

#define getNumPoints Parser Library function in Python
getNumPoints = kmllib.getNumPoints
getNumPoints.argtypes = [POINTER(KML)]
getNumPoints.restype = c_int

#define getNumPaths Parser Library function in Python
getNumPaths = kmllib.getNumPaths
getNumPaths.argtypes = [POINTER(KML)]
getNumPaths.restype = c_int

#define getNumStyles Parser Library function in Python
getNumStyles = kmllib.getNumStyles
getNumStyles.argtypes = [POINTER(KML)]
getNumStyles.restype = c_int

####### Helper functions for populating points tab
getPointName = kmllib.getPointName
getPointName.argtypes = [c_void_p]
getPointName.restype = c_char_p

setPointName = kmllib.setPointName
setPointName.argtypes = [c_void_p, c_char_p]

getPointLong = kmllib.getPointLong
getPointLong.argtypes = [c_void_p]
getPointLong.restype = c_double

getPointLat = kmllib.getPointLat
getPointLat.argtypes = [c_void_p]
getPointLat.restype = c_double

getPointAlt = kmllib.getPointAlt
getPointAlt.argtypes = [c_void_p]
getPointAlt.restype = c_double
####### End of Point helper functions

####### Helper functions for populating paths tab
getPathName = kmllib.getPathName
getPathName.argtypes = [c_void_p]
getPathName.restype = c_char_p

setPathName = kmllib.setPathName
setPathName.argtypes = [c_void_p, c_char_p]

getPathLength = kmllib.getPathLength
getPathLength.argtypes = [c_void_p]
getPathLength.restype = c_double

checkPathLoop = kmllib.checkPathLoop
checkPathLoop.argtypes = [c_void_p, c_double]
checkPathLoop.restype = c_bool
####### End of Path helper functions

####### Helper functions for populating styles tab
getStyleColour = kmllib.getStyleColour
getStyleColour.argtypes = [c_void_p]
getStyleColour.restype = c_char_p

setStyleColour = kmllib.setStyleColour
setStyleColour.argtypes = [c_void_p, c_char_p]

getStyleWidth = kmllib.getStyleWidth
getStyleWidth.argtypes = [c_void_p]
getStyleWidth.restype = c_int

setStyleWidth = kmllib.setStyleWidth
setStyleWidth.argtypes = [c_void_p, c_int]

getStyleFill = kmllib.getStyleFill
getStyleFill.argtypes = [c_void_p]
getStyleFill.restype = c_int
####### End of Style helper functions

#main program loop
def main():

    app = App()
    
main()


