# 2MAE501
Architecture &amp; Programming of Software Systems

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
