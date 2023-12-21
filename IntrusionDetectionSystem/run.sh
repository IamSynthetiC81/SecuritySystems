#!/bin/bash

RED='\033[0;31m'                                    # Red
NC='\033[0m'                                        # No Color

rule_path=Rules                                     # Path to the rules
log_path=Logs                                       # Path to the logs
alert_type=alert_fast                               # Alert type

my_path=/usr/local/snort/etc/snort                  # Path to the snort files

# Snort config
snort_config=/usr/local/snort/etc/snort/snort.conf  # Path to the snort config file

ICMP(){
    # -- Report any ICMP connection attempt in the pcap file
    echo -n "Detect any ICMP connection attempt"
    /usr/local/snort/bin/snort --daq-dir /usr/local/lib/daq_s3/lib/daq -q -R $rule_path/icmp.rules -r test_pcap_5mins.pcap -A $alert_type > $log_path/ICMP.log
	if [ $? -eq 0 ]; then
        echo -e ":\t\t\t ${Green}Success${NC}"
    else
        echo -e ":\t ${RED}Failed${NC}"
    fi
	return
}

Hello(){
	# -- Report all packets containing the word "hello" 
    echo -n "Detect any packets containing the word hello"
    /usr/local/snort/bin/snort --daq-dir /usr/local/lib/daq_s3/lib/daq -q -R $rule_path/hello.rules -r test_pcap_5mins.pcap -A $alert_type > $log_path/Hello.log
    if [ $? -eq 0 ]; then
        echo -e ":\t\t ${Green}Success${NC}"
    else
        echo -e ":\t\t ${RED}Failed${NC}"
    fi
    return
}

Unprivilaged_port(){
    # -- Report any connection attempt to a port above 1024
	echo -n "Detect any connection attempt to a port above 1024"
    /usr/local/snort/bin/snort --daq-dir /usr/local/lib/daq_s3/lib/daq -q -R $rule_path/non_root_ports.rules -r test_pcap_5mins.pcap -A $alert_type > $log_path/Unprivilaged_port.log
    if [ $? -eq 0 ]; then
        echo -e ":\t ${Green}Success${NC}"
    else
        echo -e ":\t ${RED}Failed${NC}"
    fi
    return
}

Brute_force(){
    # -- Detect ssh brute force attack
	echo -n "Detect ssh brute force attack"
    /usr/local/snort/bin/snort --daq-dir /usr/local/lib/daq_s3/lib/daq -q -R $rule_path/brute_force.rules -r sshguess.pcap -A $alert_type > $log_path/Bute_force.log
    if [ $? -eq 0 ]; then
        echo -e ":\t\t\t\t ${Green}Success${NC}"
    else
        echo -e ":\t\t\t\t ${RED}Failed${NC}"
    fi
    return
}

Community_rules(){
    # -- Run with community rules
	echo -n "Run with community rules"
    /usr/local/snort/bin/snort --daq-dir /usr/local/lib/daq_s3/lib/daq -q  -c $my_path/snort.lua -R Rules/snort3_community.rules -r test_pcap_5mins.pcap -A $alert_type > $log_path/Community_rules.log
    if [ $? -eq 0 ]; then
        echo -e ":\t\t\t\t ${Green}Success${NC}"
    else
        echo -e ":\t\t\t\t ${RED}Failed${NC}"
    fi
    return
}

ICMP
Hello
Unprivilaged_port
Brute_force
Community_rules

echo -e "\n${Green}All tests completed${NC}"
echo -e 'All results are saved in the Logs folder'

exit 0