import serial
import glob
import sys
import os


def serial_ports():
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    if result:
        return result
    else:
        return "No serial ports found"

def main():
    if len(sys.argv) == 2 and sys.argv[1] == "ls":
        print(f'PORTS:\n{serial_ports()}')
        exit()
    elif len(sys.argv) < 3:
        sys.stderr.write(f"Usage: {sys.argv[0]} r/w PORT [outfile/infile]")
        exit()
    
    ser = serial.Serial(port=sys.argv[2], baudrate=57600, timeout = 0.1, xonxoff = False, rtscts=False, dsrdtr=False)
    


    if sys.argv[1] == 'r':
        input("Wait for Led to turn on and press Enter")
        command = "dump " + input("How many bytes to read? (rounded to 16)\n:")
        command += str('\n')
        ser.write(command.encode(encoding = 'ascii', errors = 'strict'))
        
        if len(sys.argv) == 4:
            try:
                with open(sys.argv[3], 'w') as f:
                    f.write("----BEGIN EEPROM DUMP----\n")
                    run = True
                    while run:
                        v = ser.read_until(b'!')
                        v = v[:-2]
                        f.write(str(v, 'UTF-8'))
                        if len(v) == 3:
                            run = False
                            f.write("\n")
                    f.write("----END EEPROM DUMP----\n")
            except:
                sys.stderr.write(f"could not open file '{sys.argv[3]}'")
        else:
            run = True
            while run:
                v = ser.read_until(b'!')
                v = v[:-1]
                print(str(v, 'UTF-8'), end='')
                if len(v) == 4:
                    run = False
                
            exit()
                
    elif sys.argv[1] == 'w':
        input("Wait for Led to turn on and press Enter")
        if len(sys.argv) == 4:
            try:
                with open(sys.argv[3], 'rb') as f:
                    offset = input("Offset for file write (default=0)\n:")
                    if offset == '':
                        offset = 0
                    i = 0
                    while i < os.path.getsize(sys.argv[3]):
                        byte = ord(f.read(1))
                        command = f"write {i} {byte}\n"
                        ser.write(command.encode(encoding = 'ascii', errors = 'strict'))
                        v = ser.read(3)
                        print(v)
                        if str(v, 'UTF-8') == "ERR":
                            sys.stderr.write(f"something went wrong while writing byte {hex(byte)} to add {hex(i)}")
                        i += 1

                        
            except:
                sys.stderr.write(f"could not open file '{sys.argv[3]}'")
        else:
            addr = input("Input addr in hex\n:0x")
            hex_addr = int(addr, 16)
            val = input(f"Input value to set at addr {hex(hex_addr)}\n:0x")
            hex_val = int(val, 16)
            print(f"Setting byte at addr {hex(hex_addr)} to {hex(hex_val)}")

            command = f"write {int(hex_addr)} {int(hex_val)}\n" 
            ser.write(command.encode(encoding = 'ascii', errors = 'strict'))
            v = ser.read(3)
            print(f"Successfully wrote byte {hex(hex_val)}at addr {hex(hex_addr)}" if str(v, 'UTF-8') == "SUC" else "Something went wrong")
    else:
        sys.stderr.write(f"Usage: {sys.argv[0]} r/w PORT [outfile/infile]")
        exit()

if __name__ == '__main__':
    main()