import serial

ser = serial.Serial(
        port = 'COM4',
        baudrate = 57600,
        timeout = 10,
        xonxoff = False
)

val = 0
command = ''
command = input("Enter command: ")
command += str('\n')
val = ser.write(command.encode(encoding = 'ascii', errors = 'strict'))


print("Bytes written: ", val)
v = ser.read_until(b'!')
print(v)


'''
if len(sys.argv) == 4:
            try:
                with open(sys.argv[3], 'wb') as f:
                    while running:
                        v = ser.readline()
                        if str(v, 'UTF-8') == "ENDMSG":
                            running = False
                            exit()
                        f.write(v)
            except:
                if running:
                    sys.stderr.write(f"could not open file '{sys.argv[3]}'")
                else:
                    pass
else:



'''