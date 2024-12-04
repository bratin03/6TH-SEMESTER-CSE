# Import necessary modules
from scapy.all import *
import re

# Function to process DNS packets and write extracted information to a file
def process_dns(pkt, output_file):
    # Check if the packet is a DNS packet with a DNS query
    if DNS in pkt and pkt.haslayer(DNSQR):
        # Extract the query name from the DNS packet
        query = pkt[DNSQR].qname.decode('utf-8')
        responses = []

        # Check if there are DNS response records in the packet
        if DNSRR in pkt:
            # Split the DNS response records into strings using '|'
            dnsrr_strings = repr(pkt[DNSRR]).split("|")
            rrname_pattern = re.compile(r"rrname='(.*?)'")
            rdata_pattern = re.compile(r"rdata=(.*?) ")
            data_dict = {}

            # Extract RRname and RData information from each DNS response record
            for entry in dnsrr_strings:
                rrname_match = rrname_pattern.search(entry)
                rdata_match = rdata_pattern.search(entry)

                # Check if both RRname and RData are present in the entry
                if rrname_match and rdata_match:
                    rrname = rrname_match.group(1)
                    rdata = rdata_match.group(1)

                    # Update data_dict with RRname and corresponding RData values
                    if rrname in data_dict:
                        data_dict[rrname].append(rdata)
                    else:
                        data_dict[rrname] = [rdata]

            # Write the extracted values to the output file
            with open(output_file, 'a') as file:
                for rrname, rdata_list in data_dict.items():
                    file.write(f"Q: {rrname}\nA: {', '.join(rdata_list)}\n\n")

# Main function
def main():
    # Specify the path to the pcap file and the output file
    pcap_file = '21CS10016_CAPTURE.pcap'
    output_file = 'dns_results.txt'

    try:
        # Read pcap file using Scapy
        pkts = rdpcap(pcap_file)

        # Process each packet in the pcap file
        for pkt in pkts:
            process_dns(pkt, output_file)

        # Print a message indicating successful completion
        print(f"Results saved to {output_file}")

    # Handle file not found exception
    except FileNotFoundError:
        print(f"Error: File {pcap_file} not found.")

    # Handle Scapy exception
    except Scapy_Exception as e:
        print(f"Error: {e}")

# Run the main function if the script is executed
if __name__ == "__main__":
    main()

