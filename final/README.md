# 2MAE501
Architecture and Programming of Software Systems


---Program execution manual---

<<IoT>>
1. compile programs
    $ make

<<Data Manage>>
1. First make sure to that the IP address of the sockets defined insed DBserverThread.cc match the actual IP address of the raspberry that will execute the code. Use below command to check the address. 
        $ ifconfig

2. Then compile DBserverThread.cc with the following command to create the executable. :
	$ g++ -Wall -I/usr/local/include -c DBserverThread.cc
	$ g++ -L/usr/local/lib server.o -lgsl -lgslcblas -lm -pthread -lstdc++

3. Execute the program
	$ ./server

4. With the server running, you can see that the server waits for incoming communications and assigned handlers when either a factory or a GUI request a connection. At any time the records of the process can be observed in Data1.txt.


<<AI>>
The AI codes are already merged in the Data Maange server file (DBserverThread.cc). But also you can run the AI programs separately. 

1. The compiling step
  $g++ -Wall -I/usr/local/include -c test.cpp AI.cpp

2. The linking step
  $g++ -L/usr/local/lib tets.o AI.o -lgsl -lgslcblas -lm

3. Execute the program
  $./a.out

<<GUI>>

1. After runnning the executable of the server, open the final.py file and make sure to change the IP address
    in line 19 in order to match with the one typed in the server file.

2. Once the IP address matches, in another terminal change the directory to the path where the final.py file is
    found.

3. It is necessary to have installed python 3 version in order to run the script. Make sure to have it installed
    by typing 
    $python3 --version.
    
    If it corresponds to version 3, type in the next line of the terminal:
    $python3.py

4. The Graphical User Interface shall be deployed. In order to start seeing the data, click the Start button and
    it shall start to wait for the threads from the "Manage Communication" node. In order to confirm that it is
    working, open in the side the terminal where the command was run. A string of zeros shall appear every line.
    This means that the GUI is waiting to receive data from the factories.





---Github manual---

1. Creat a git folder in your computer


$  mkdir ${HOME}/git

<<Download>>

How to download all files of the git repository to your new local storage

1. if this is your first time to downlaod the designated folder in git, you should downlaod whole git repository : use "git clone"

$ git clone https://github.com/SeungahLEE0820/2MAE501

2. You already cloned the repository, and you want to download the latest version from the repository to your local storage: use "git pull"

$ git pull 

<<Upload>>

 How to upload files from your local storage (git folder) to the repository 

1. Check out the status of the git: use "git status"

$ git status

2. add new or modified files in the local storage :  use "git add"

$ git add <file or folder name>

If you want to put all updated files, use "*"

$ git add *

3. Add some comments: use "git commit"

$ git commit -m "<write a message about update. example  --[src] add new threding files in IoT subsystem>"

4. use "git push"

$git push 
