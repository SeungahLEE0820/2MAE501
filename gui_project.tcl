#############################################################
# GUI.tcl
# File Tcl/Tk 
##############################################################


proc ShowInformationFact1 {parent} {
  # Affiche l'ecran d'alarme, au depart, l'alarme n'est pas affichee
  global industry4
  set w $parent.information
  canvas $w -background gray100 -width 640 -height 350 -borderwidth 0
  set industry4(informationcanvas) $w

  $w create text 160 20 -fill black -justify center -text "Factory 1" -font {Helvetica -20 bold}
  $w create text 160 50 -fill black -justify center -text "Sensors" -font {Helvetica -15}

  #Show temperature
  $w create text 50 75 -fill black -justify center -text "Temperature" -font {Helvetica -15} 
  set industry4(information) [$w create rectangle 100 60 220 90 -fill yellow2]
  set industry4(informationtext) [$w create text 160 75 -text Temperature] 
  $w create text 235 75 -fill black -justify center -text "°C" -font {Helvetica -15} 

  #Show pressure
  $w create text 50 115 -fill black -justify center -text "Pressure" -font {Helvetica -15}
  set industry4(information) [$w create rectangle 100 100 220 130 -fill yellow2]
  set industry4(informationtext) [$w create text 160 115 -text Pressure]
  $w create text 240 115 -fill black -justify center -text "Hpa" -font {Helvetica -15}

  #Show humidity
  $w create text 50 155 -fill black -justify center -text "Humidity" -font {Helvetica -15}
  set industry4(information) [$w create rectangle 100 140 220 170 -fill yellow2]
  set industry4(informationtext) [$w create text 160 155 -text Humidity]
  $w create text 235 155 -fill black -justify center -text "%" -font {Helvetica -15}

  #Show time
  $w create text 50 195 -fill black -justify center -text "Time" -font {Helvetica -15}
  set industry4(information) [$w create rectangle 100 180 220 210 -fill yellow2]
  set industry4(informationtext) [$w create text 160 195 -text Time]
  $w create text 260 195 -fill black -justify center -text "hh:mm:ss" -font {Helvetica -15}

  $w create text 160 230 -fill black -justify center -text "Actuators" -font {Helvetica -15}
  
  #Show LED
  $w create text 50 255 -fill black -justify center -text "LED" -font {Helvetica -15}
  set industry4(information) [$w create rectangle 100 240 220 270 -fill gray50]
  set industry4(informationtext) [$w create text 160 255 -text LED]

  #Show relay
  $w create text 50 295 -fill black -justify center -text "Relay" -font {Helvetica -15}
  set industry4(information) [$w create rectangle 100 280 220 310 -fill gray50]
  set industry4(informationtext) [$w create text 160 295 -text Relay]
  return $w
}


proc ShowInformationFact2 {parent} {
  # Affiche l'ecran d'alarme, au depart, l'alarme n'est pas affichee
  global industry4
  set industry4(information2canvas) .information

  .information create text 500 20 -fill black -justify center -text "Factory 2" -font {Helvetica -20 bold}
  .information create text 500 50 -fill black -justify center -text "Sensors" -font {Helvetica -15}

  #Show temperature
  .information create text 390 75 -fill black -justify center -text "Temperature" -font {Helvetica -15} 
  set industry4(information2) [.information create rectangle 440 60 560 90 -fill cyan3]
  set industry4(information2text) [.information create text 500 75 -text Temperature] 
  .information create text 575 75 -fill black -justify center -text "°C" -font {Helvetica -15} 

  #Show pressure
  .information create text 390 115 -fill black -justify center -text "Pressure" -font {Helvetica -15}
  set industry4(information2) [.information create rectangle 440 100 560 130 -fill cyan3]
  set industry4(information2text) [.information create text 500 115 -text Pressure]
  .information create text 580 115 -fill black -justify center -text "Hpa" -font {Helvetica -15}

  #Show humidity
  .information create text 390 155 -fill black -justify center -text "Humidity" -font {Helvetica -15}
  set industry4(information2) [.information create rectangle 440 140 560 170 -fill cyan3]
  set industry4(information2text) [.information create text 500 155 -text Humidity]
  .information create text 575 155 -fill black -justify center -text "%" -font {Helvetica -15}

  #Show time
  .information create text 390 195 -fill black -justify center -text "Time" -font {Helvetica -15}
  set industry4(information2) [.information create rectangle 440 180 560 210 -fill cyan3]
  set industry4(information2text) [.information create text 500 195 -text Time]
  .information create text 600 195 -fill black -justify center -text "hh:mm:ss" -font {Helvetica -15}

  .information create text 160 230 -fill black -justify center -text "Actuators" -font {Helvetica -15}
  
  #Show LED
  .information create text 390 255 -fill black -justify center -text "LED" -font {Helvetica -15}
  set industry4(information2) [.information create rectangle 440 240 560 270 -fill gray50]
  set industry4(information2text) [.information create text 500 255 -text LED]

  #Show relay
  .information create text 390 295 -fill black -justify center -text "Relay" -font {Helvetica -15}
  set industry4(information2) [.information create rectangle 440 280 560 310 -fill gray50]
  set industry4(information2text) [.information create text 500 295 -text Relay]
  return .information
}


proc ShowAlarm1 {parent} {
  # Show the alarm only; we will initialize it afterwards
  global industry4
  set w $parent.alarm
  canvas $w -width 340 -height 70 -borderwidth 0
  set industry4(alarmcanvas) $w
  set industry4(alarm) [$w create rectangle 130 0 230 60  -fill green]
  set industry4(alarmtext) [$w create text 180 30 -text AlarmOK]
  return $w
}

proc ShowAlarm2 {parent} {
  # Show the alarm only; we will initialize it afterwards
  global industry4
  set w $parent.alarm2
  canvas $w -width 340 -height 70 -borderwidth 0
  set industry4(alarm2canvas) $w
  set industry4(alarm2) [$w create rectangle 130 0 230 60  -fill green]
  set industry4(alarm2text) [$w create text 180 30 -text AlarmOK]
  return $w
}

proc ShowWindow {} {
  #Show the main window
  wm title . "Industry 4.0"
  wm protocol . WM_DELETE_WINDOW "set EXIT 1;destroy ."
  pack [ShowInformationFact1 ""] -side top
  pack [ShowInformationFact2 ""] -side top
  pack [ShowAlarm1 ""] -side left
  pack [ShowAlarm2 ""] -side right
#  pack [ShowScale ""] -side top
#  pack [ShowDebit ""] -side top
}


#console show
ShowWindow
