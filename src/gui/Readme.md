[EN]
The scripts are written in Processing Language 3.3.6 and implement the base station's graphical interface.
The parent script is cansat_ground.pde which we run when the base station board is connected to
one free USB port.
In line 34, we modify the code to connect to the serial port.
port = new Serial (this, Serial.list () [1], 115200);
Serial.list () [0] means COM1, Serial.list () [1] or COM2 etc. 
If the computer is a laptop and does not have physical serial ports, then we declare Serial.list () [0].

[GR]
Τα scripts είναι γραμμένα σε γλώσσα Processing 3.3.6 και υλοποιούν το γραφικό περιβάλλον του σταθμού βάσης.
Το γονικό script είναι το cansat_ground.pde το οποίο το εκτελούμε εφόσον έχει συνδεθεί η πλακέτα του σταθμού βάσης σε 
κάποια ελεύθερη θύρα USB.
Στην γραμμή 34 τροποποιούμε τον κώδικα ώστε να συνδεθεί με την σειριακή θύρα.
port = new Serial(this, Serial.list()[1], 115200);
Το Serial.list()[0] σημαίνει η COM1, Serial.list()[1] η COM2 κλπ. Αν είναι φορητός και δεν διαθέτει φυσικές σειριακές θύρες, τότε
δηλώνουμε Serial.list()[0].
