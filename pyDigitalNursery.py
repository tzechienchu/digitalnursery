import serial
import json

def waitMsg(port,slaveMsg):
    msg = port.readline()
    #print msg
    if (slaveMsg in msg):
        print 'got '+slaveMsg
        return 1
    else :
        #print 'not got it'
        return 0

def sendHOST(port):
    print 'send Host'
    port.write('HOST\n')

def sendCommand(port,command):
    sendcomd = command + '\n'
    port.write(sendcomd)
    
def readSensorData(port):
    jdata = ''
    msg = port.readline()
    msg = msg.strip()
    while not ('}}' in msg):
        jdata += msg
        msg = port.readline()
        msg = msg.strip()
        
    jdata += msg
    return jdata

    
#Need to test 3 Arduno Board
sensor = serial.Serial('/dev/tty.usbmodemfd121',115200,timeout=1)
#Wait UNO Message
while (waitMsg(sensor,'Sensory 1') == 0):
    pass
    
sendHOST(sensor)
    
#Read Sensor
while (waitMsg(sensor,'Read Sensor') == 0):
    pass

sendCommand(sensor,'READ')

sData = readSensorData(sensor)
sData = sData.replace('READ','')
jData = json.loads(sData)

print jData
