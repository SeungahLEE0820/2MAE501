# 2MAE501
Architecture &amp; Programming of Software Systems


Program execution manual

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

<<GUI>>















Github manual

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
