import subprocess
import time
import statistics

# Configuration
INTERVAL = 10  # Run for 10 seconds
SLEEP_TIME = 1  # Interval between measurements

# Function to extract RX/TX values from command output
def get_rx_tx_values(interface):
    cmd = f"ip -s link show {interface} | awk '/RX:/ {{getline; print $1, $2}} /TX:/ {{getline; print $1, $2}}'"
    result = subprocess.run(cmd, shell=True, text=True, capture_output=True)
    if result.returncode != 0:
        raise RuntimeError("Failed to execute command")
    
    values = list(map(int, result.stdout.split()))
    if len(values) != 4:
        raise ValueError("Unexpected output format")
    
    return {
        "rx_bytes": values[0],
        "rx_packets": values[1],
        "tx_bytes": values[2],
        "tx_packets": values[3],
    }

# Main function to calculate rates
def monitor_network(interface, interval=INTERVAL, sleep_time=SLEEP_TIME):
    prev_values = get_rx_tx_values(interface)
    rx_bytes_diffs, rx_packets_diffs = [], []
    tx_bytes_diffs, tx_packets_diffs = [], []

    for _ in range(interval):
        time.sleep(sleep_time)
        new_values = get_rx_tx_values(interface)

        rx_bytes_diffs.append(new_values["rx_bytes"] - prev_values["rx_bytes"])
        rx_packets_diffs.append(new_values["rx_packets"] - prev_values["rx_packets"])
        tx_bytes_diffs.append(new_values["tx_bytes"] - prev_values["tx_bytes"])
        tx_packets_diffs.append(new_values["tx_packets"] - prev_values["tx_packets"])

        prev_values = new_values  # Update previous values

    # Compute median rates
    print(f"Median RX Bytes/s:   {statistics.median(rx_bytes_diffs)}")
    print(f"Median RX Packets/s: {statistics.median(rx_packets_diffs)}")
    print(f"Median TX Bytes/s:   {statistics.median(tx_bytes_diffs)}")
    print(f"Median TX Packets/s: {statistics.median(tx_packets_diffs)}")

# Run the script
if __name__ == "__main__":
    monitor_network("veth-adv03")
