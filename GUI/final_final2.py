import tkinter as tk
import threading
import socket
import queue
import time
import numpy as np

#Initializing coordinates
#Factory 1

Fan1 = 0
Fan2 = 0

host = '192.168.1.138'
port = 9090

mySocket = socket.socket()
mySocket.connect((host,port))

def runloop(thread_queue=None):
##After result is produced put it in queue
      while True:
           data = mySocket.recv(200).decode()
           #print(data)
           thread_queue.put(data)
           Fan3 = str(Fan1)
           Fan4 = str(Fan2)
           Command = Fan3+Fan4
           print(Command)
           ce = Command.encode()
           mySocket.sendall(ce)
           time.sleep(2)

class MainApp(threading.Thread):

   def __init__(self):
####### Do something ######
      threading.Thread.__init__(self)
      self.start()
      self.thread_queue = queue.Queue()
      self.root = tk.Tk()
      self.root.title("Factories 4.0")
      self.c = tk.Canvas(self.root, bg="gray92", height=480, width=900)
      self.name = self.c.create_text(180, 20, font=("Helvetica", 18), text="Factory LeMonde")
      self.name = self.c.create_text(180, 55, font=("Helvetica", 13), text="Sensors")
      
      #Factory 1
      #Temperature
      self.name = self.c.create_text(60, 85, font=("Helvetica", 13), text="Temperature")
      self.Temperature = tk.Label(self.root, bg="sienna1", height = 2, width =12) # Element to be updated
      self.Temperature.place(x=130,y=70)
      self.Temperature.config(text="Temperature")
      self.name = self.c.create_text(250, 85, font=("Helvetica", 13), text="°C")

      #Pressure
      self.name = self.c.create_text(60, 145, font=("Helvetica", 13), text="Pressure")
      self.Pressure = tk.Label(self.root, bg="sienna1", height = 2, width =12) # Element to be updated
      self.Pressure.place(x=130,y=130)
      self.Pressure.config(text="Pressure")
      self.name = self.c.create_text(250, 145, font=("Helvetica", 13), text="Hpa")

      #Humidity
      self.name = self.c.create_text(60, 205, font=("Helvetica", 13), text="Humidity")
      self.Humidity = tk.Label(self.root, bg="sienna1", height = 2, width =12) # Element to be updated
      self.Humidity.place(x=130,y=190)
      self.Humidity.config(text="Humidity")
      self.name = self.c.create_text(250, 205, font=("Helvetica", 13), text="%")
 
      #Actuators
      self.actuators = self.c.create_text(180, 250, font=("Helvetica", 13), text="Actuators")
      self.name = self.c.create_text(60, 290, font=("Helvetica", 13), text="Alarm")
      self.Alarm = tk.Label(self.root, bg="green3", height = 2, width =12) # Element to be updated
      self.Alarm.config(text="OFF")
      self.Alarm.place(x=130,y=270)

      self.F = tk.Label(self.root, bg="green3", height = 2, width =12) # Element to be updated
      self.name = self.c.create_text(60, 350, font=("Helvetica", 13), text="Fan")
      self.F.config(text="OFF")
      self.F.place(x=130,y=330)

      #Signal 
      self.Signal = tk.Label(self.root, bg="gray", height = 3, width =15) # Element to be updated
      self.Signal.config(text="Signal OFF")
      self.Signal.place(x=120,y=390)

      #Factory 2
      self.name = self.c.create_text(530, 20, font=("Helvetica", 18), text="Factory Chocolatin")
      self.name = self.c.create_text(530, 55, font=("Helvetica", 13), text="Sensors")
      #Temperature
      self.name2 = self.c.create_text(410, 85, font=("Helvetica", 13), text="Temperature")
      self.Temperature2 = tk.Label(self.root, bg="cyan3", height = 2, width =12) # Element to be updated
      self.Temperature2.place(x=480,y=70)
      self.Temperature2.config(text="Temperature")
      self.name2 = self.c.create_text(600, 85, font=("Helvetica", 13), text="°C")

      #Pressure
      self.name2 = self.c.create_text(410, 145, font=("Helvetica", 13), text="Pressure")
      self.Pressure2 = tk.Label(self.root, bg="cyan3", height = 2, width =12) # Element to be updated
      self.Pressure2.place(x=480,y=130)
      self.Pressure2.config(text="Pressure")
      self.name2 = self.c.create_text(600, 145, font=("Helvetica", 13), text="Hpa")

      #Humidity
      self.name2 = self.c.create_text(410, 205, font=("Helvetica", 13), text="Humidity")
      self.Humidity2 = tk.Label(self.root, bg="cyan3", height = 2, width =12) # Element to be updated
      self.Humidity2.place(x=480,y=190)
      self.Humidity2.config(text="Humidity")
      self.name2 = self.c.create_text(600, 205, font=("Helvetica", 13), text="%")

      #Actuators
      self.actuators = self.c.create_text(530, 250, font=("Helvetica", 13), text="Actuators")
      self.name = self.c.create_text(410, 290, font=("Helvetica", 13), text="Alarm")
      self.Alarm2 = tk.Label(self.root, bg="green3", height = 2, width =12) # Element to be updated
      self.Alarm2.config(text="OFF")
      self.Alarm2.place(x=480,y=270)

      self.F2 = tk.Label(self.root, bg="green3", height = 2, width =12) # Element to be updated
      self.name = self.c.create_text(410, 350, font=("Helvetica", 13), text="Fan")
      self.F2.config(text="OFF")
      self.F2.place(x=480,y=330)

      #Signal 
      self.Signal2 = tk.Label(self.root, bg="gray", height = 3, width =15) # Element to be updated
      self.Signal2.config(text="Signal OFF")
      self.Signal2.place(x=470,y=390)

      #Commands
      self.Start = tk.Button(self.root, text="Start",command=self.update_text)
      self.Start.place(x=1,y=0)

      self.Stop = tk.Button(self.root, text="Stop",command=self.StopThreads)
      self.Stop.place(x=1,y=45)
      

      #Factory 1
      self.name = self.c.create_text(750, 100, font=("Helvetica", 13), text="Command to Le Monde")
      self.Fan= tk.Button(self.root, text="Fan OFF",command=self.commandGUI)
      self.Fan.place(x=720,y=120)


      #Factory 2
      self.name = self.c.create_text(750, 220, font=("Helvetica", 13), text="Command to Chocolatin")
      self.Fan2= tk.Button(self.root, text="Fan OFF",command=self.commandGUIC)
      self.Fan2.place(x=720,y=240)
     
      self.c.pack()
      #self.root.after(100, self.listen_for_result)  
      self.root.mainloop()
      
   def update_text(self):
##Spawn a new thread for running long loops in background
      self.thread_queue = queue.Queue()
      self.new_thread = threading.Thread(target=runloop,kwargs={"thread_queue":self.thread_queue})
      self.new_thread.start()
      self.root.after(1000)
      self.new_thread3 = threading.Thread(target=self.listen_for_result)
      self.new_thread3.start()

   
   def listen_for_result(self):
#Check if there is something in the queue
      while True:
          if queue:
              self.res1 = self.thread_queue.get(True)
              print(self.res1)
              x = self.res1.split(",")
              #print(x[1])
              #print(x])
              if x[0] == "0.00":
                  self.Signal.config(text = "Signal ON 1:\nNot receiving data",bg ="red2")
              else:
                  self.Signal.config(text = "Signal ON 1:\nReceiving data",bg ="green")
                  self.root.update_idletasks()
                  self.Temperature.config(text = x[0])
                  self.Pressure.config(text = x[1])
                  self.Humidity.config(text = x[2])
                  if x[3] == "0": 
                      self.Alarm.config(text = "Alarm OFF")
                  else:
                      self.Alarm.config(text = "Alarm ON", bg="red3")
                  if x[4] == "0":
                      self.F.config(text = "Fan OFF")
                  else:
                      self.F.config(text = "Fan ON", bg="red3")
              if x[5] == "0.00":
                  self.Signal2.config(text = "Signal ON 2:\nNot receiving data",bg ="red2")
              else:
                  self.Signal2.config(text = "Signal ON 2:\nReceiving data",bg ="green")
                  self.root.update_idletasks()
                  self.Temperature2.config(text = x[5])
                  self.Pressure2.config(text = x[6])
                  self.Humidity2.config(text = x[7])
                  if x[8] == "0": 
                      self.Alarm2.config(text = "Alarm OFF")
                  else:
                      self.Alarm2.config(text = "Alarm ON", bg="red3")
                  if x[9] == "0":
                      self.F2.config(text = "Fan OFF")
                  else:
                      self.F2.config(text = "Fan ON", bg="red3")
              time.sleep(1)           
          else:
              print("Problems with the queue")

   def StopThreads(self):
       self.new_thread.end()
       self.new_thread3.end()

   def commandGUI (self):
      global Fan1

      if Fan1 == 0:
          self.Fan.config(text = "Fan ON")
          Fan1 += 1
      else:
          self.Fan.config(text = "Fan OFF")
          Fan1 = 0


   def commandGUIC (self):
      global Fan2

      if Fan2 == 1:
          self.Fan2.config(text = "Fan OFF")
          Fan2 = 0
      else:
          self.Fan2.config(text = "Fan ON")
          Fan2 += 1



if __name__ == "__main__":
     main_app = MainApp()
