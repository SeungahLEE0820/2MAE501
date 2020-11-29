import tkinter as tk
import threading
import socket
import queue
import time

#Initializing coordinates
#Factory 1
x1 = 60
y1 = 85

xr1 = 120
yr1 = 70
xr2 = 240
yr2 = 100

host = '192.168.43.204'
port = 9090

mySocket = socket.socket()
mySocket.connect((host,port))

def runloop(thread_queue=None):
##After result is produced put it in queue
      while True:
           data = mySocket.recv(4).decode()
           print(data)
           thread_queue.put(data)
           time.sleep(2)

def sendCommand(thread_queue=None):
##After result is produced put it in queue
      while True:
           mySocket.sendall(b'a: 1')
           print("AlarmOn")

class MainApp():

   def __init__(self):
####### Do something ######
      self.thread_queue = queue.Queue()
      self.root = tk.Tk()
      self.root.title("Factories 4.0")
      self.c = tk.Canvas(self.root, bg="gray92", height=320, width=750)
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
      self.name = self.c.create_text(250, 205, font=("Helvetica", 13), text="°C")

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
      self.name2 = self.c.create_text(600, 205, font=("Helvetica", 13), text="°C"
      self.Alarm = tk.Button(self.root, text="Alarm",command=self.commandGUI)
      self.Alarm.place(x=680,y=50)
      
      self.c.pack()

      self.root.after(100, self.listen_for_result)
      self.root.mainloop()
      self.update_text()

   def update_text(self):
      i = 1
##Spawn a new thread for running long loops in background
      self.Temperature.config(text = "Counting")
      self.thread_queue = queue.Queue()
      self.new_thread = threading.Thread(target=runloop,kwargs={"thread_queue":self.thread_queue})
      self.new_thread.start()
      while True:
          if queue.Empty:
              print("Temperature")
              self.res1 = self.thread_queue.get(True)
              print("sockets", self.res1)
              self.root.update_idletasks()
              self.Temperature.config(text = self.res1)
              time.sleep(2)           
          else:
              print("Problems with the queue")
      self.root.after(1000, self.listen_for_result)


   def listen_for_result(self):
#Check if there is something in the queue
      try:
          self.res = self.thread_queue.get(0)
          time.sleep(1)
          self.Temperature.config(text = "Loop Terminated")
          self.root.after(100, self.listen_for_result)

      except queue.Empty:
          self.root.after(100, self.listen_for_result)

   def commandGUI (self):
##Spawn a new thread for running long loops in background
      self.Alarm.config(text = "ON")
      self.new_thread2 = threading.Thread(target=sendCommand)
      self.new_thread2.start()
      self.root.after(1000, self.listen_for_result)

if __name__ == "__main__":
     main_app = MainApp()
