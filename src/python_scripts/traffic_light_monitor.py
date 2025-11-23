import serial
import time
import re
from datetime import datetime

PORT = 'COM7'
BAUDRATE = 9600
TIMEOUT = 1

class Colors:
    RED = '\033[91m'
    YELLOW = '\033[93m'
    GREEN = '\033[92m'
    BLUE = '\033[94m'
    CYAN = '\033[96m'
    RESET = '\033[0m'
    BOLD = '\033[1m'

def log_traffic_state(message):
    """Parsează și loghează starea semafoarelor"""
    pattern = r'\[TRAFFIC\] State: (\w+) \| Left: (\w+) \| Right: (\w+)'
    match = re.search(pattern, message)
    
    if match:
        state = match.group(1).upper()
        left_state = match.group(2).upper()
        right_state = match.group(3).upper()
        
        timestamp = datetime.now().strftime("%H:%M:%S")
        
        color_map = {
            'RED': Colors.RED,
            'YELLOW': Colors.YELLOW,
            'GREEN': Colors.GREEN
        }
        color = color_map.get(state, Colors.RESET)
        
        print(f"{Colors.CYAN}[{timestamp}]{Colors.RESET} {Colors.BOLD}Traffic Lights:{Colors.RESET}")
        print(f"  {Colors.BLUE}State:{Colors.RESET} {color}{state}{Colors.RESET}")
        print(f"  {Colors.BLUE}Left Semafor:{Colors.RESET} {color_map.get(left_state, Colors.RESET)}{left_state}{Colors.RESET}")
        print(f"  {Colors.BLUE}Right Semafor:{Colors.RESET} {color_map.get(right_state, Colors.RESET)}{right_state}{Colors.RESET}")
        print(f"  {Colors.CYAN}Status:{Colors.RESET} {'✓ Synchronized' if left_state == right_state else '✗ Not synchronized'}")
        print()
        return True
    return False

def log_message(message):
    timestamp = datetime.now().strftime("%H:%M:%S")
    
    if message.startswith("[TRAFFIC]"):
        return log_traffic_state(message)
    elif message.startswith("ERR:"):
        print(f"{Colors.RED}[{timestamp}] ERROR: {message[4:]}{Colors.RESET}")
    elif message.startswith("[INFO]") or "initialized" in message.lower():
        print(f"{Colors.GREEN}[{timestamp}] {message}{Colors.RESET}")
    else:
        print(f"{Colors.CYAN}[{timestamp}] {message}{Colors.RESET}")
    return False

try:
    ser = serial.Serial(PORT, BAUDRATE, timeout=TIMEOUT)
    time.sleep(2) 
    print(f"{Colors.GREEN}[INFO]{Colors.RESET} Connected to {PORT} at {BAUDRATE} baud.")
    print(f"{Colors.GREEN}[INFO]{Colors.RESET} Traffic Light Monitor started.\n")
except serial.SerialException as e:
    print(f"{Colors.RED}[ERROR]{Colors.RESET} Could not open serial port: {e}")
    exit(1)

# === CLI  ===
def interactive_cli():
    print(f"\n{Colors.BOLD}=== Traffic Light Control CLI ==={Colors.RESET}")
    print("Available commands:")
    print(f"  {Colors.CYAN}night{Colors.RESET} - Activate night mode (pedestrian lights OFF, yellow blinking)")
    print(f"  {Colors.CYAN}day{Colors.RESET} - Deactivate night mode")
    print(f"  {Colors.CYAN}help{Colors.RESET} - Show all commands")
    print(f"  {Colors.CYAN}exit{Colors.RESET} - Exit program")
    print()

    while True:
        try:
            user_input = input(f"{Colors.BOLD}>> {Colors.RESET}").strip()
            
            if user_input.lower() == "exit":
                break

            if not user_input:
                continue

            ser.write((user_input + '\n').encode())
            time.sleep(0.1)

            while ser.in_waiting:
                try:
                    response = ser.readline().decode('utf-8', errors='replace').strip()
                    if response:
                        log_message(response)
                except Exception as e:
                    pass
                    
        except KeyboardInterrupt:
            print(f"\n{Colors.YELLOW}[INFO]{Colors.RESET} Interrupted by user.")
            break
        except Exception as e:
            print(f"{Colors.RED}[ERROR]{Colors.RESET} {e}")

def main():
    try:
        time.sleep(0.5)
        while ser.in_waiting:
            try:
                response = ser.readline().decode('utf-8', errors='replace').strip()
                if response:
                    log_message(response)
            except Exception as e:
                pass
        
        interactive_cli()
    except Exception as e:
        print(f"{Colors.RED}[ERROR]{Colors.RESET} {e}")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print(f"\n{Colors.YELLOW}[INFO]{Colors.RESET} Interrupted by user.")
    finally:
        ser.close()
        print(f"{Colors.GREEN}[INFO]{Colors.RESET} Serial connection closed.")




