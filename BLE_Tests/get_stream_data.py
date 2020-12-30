import serial
import time

# internal
com = None

def open_com():
    global com
    com = serial.Serial('COM13', 115200, timeout=1)
    #time.sleep(0.5)
    com.reset_input_buffer()
    com.reset_output_buffer()

def read():
    r = com.readline()
    com.reset_input_buffer()
    return r.decode('UTF-8').strip('\r\n')

def close_com():
    com.close()

try:
    open_com()
    data_log = open('acq_data.txt', 'w')
    ready = False
    start = time.perf_counter()

    while(True):
        line = read()

        if len(line) > 2:
            if line[1] == ':':
                if  not ready:
                    ready = True
                    start = time.perf_counter()
                    print("Logging started")
                else:
                    data_log.write(line + ':' + str(current_time)+'\n') 
        
        current_time = time.perf_counter()

        if current_time - start > 60:
            data_log.close() 
            print("Logging finished")
            break

finally:
    close_com()