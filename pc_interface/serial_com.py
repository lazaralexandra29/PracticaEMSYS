import serial
import time

PORT = 'COM7'
BAUDRATE = 9600
TIMEOUT = 1

try:
    ser = serial.Serial(PORT, BAUDRATE, timeout=TIMEOUT)
    time.sleep(2) 
    print(f"[INFO] Connected to {PORT} at {BAUDRATE} baud.")
except serial.SerialException as e:
    print(f"[ERROR] Could not open serial port: {e}")
    exit(1)

# === CLI INTERACTIV ===
def interactive_cli():
    print("\n[CLI] Enter command:")
    print("  SetLED <index> <ON/OFF/TOGGLE>")
    print("  GetLEDState <index>")
    print("  ButtonInfo")
    print("  Help")
    print("  exit\n")

    while True:
        user_input = input(">> ").strip()
        if user_input.lower() == "exit":
            break

        ser.write((user_input + '\n').encode())
        time.sleep(0.1)

        while ser.in_waiting:
            response = ser.readline().decode().strip()
            print(f"[MCU] {response}")

def main():
    interactive_cli()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n[INFO] Interrupted by user.")
    finally:
        ser.close()
        print("[INFO] Serial connection closed.")