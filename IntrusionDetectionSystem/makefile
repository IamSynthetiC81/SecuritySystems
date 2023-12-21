rule_path = Rules
log_path = Logs
alert_type = alert_fast

my_path = /usr/local/snort/etc/snort

# Snort command
snort = /usr/local/snort/bin/snort --daq-dir /usr/local/lib/daq_s3/lib/daq

all : ICMP Hello Unprivilaged_port Brute_force

ICMP : 
	@echo -n "Detect any ICMP connection attempt"
	@$(snort) -q -R $(rule_path)/icmp.rules -r test_pcap_5mins.pcap -A $(alert_type) > $(log_path)/ICMP.log

Hello:
	@echo "Detect any packets containing the word hello"
	@$(snort) -q -R $(rule_path)/hello.rules -r test_pcap_5mins.pcap -A $(alert_type) > $(log_path)/Hello.log



Unprivilaged_port :  
	@echo "Detect any connection attempt to a port above 1024"
	@$(snort) -q -R $(rule_path)/non_root_ports.rules -r test_pcap_5mins.pcap -A $(alert_type) > $(log_path)/Unprivilaged_port.log
	
Brute_force : 
	@echo "Detect ssh brute force attack"
	@$(snort) -q -R $(rule_path)/brute_force.rules -r sshguess.pcap -A $(alert_type) > $(log_path)/Bute_force.log


Community_rules :
	@echo "Run with community rules"
	@$(snort) -q --talos -c $(my_path)/snort.lua -R Rules/snort3_community.rules -r test_pcap_5mins.pcap -A $(alert_type) > $(log_path)/Community_rules.log

clean : 
	@echo "Cleaning all the log files"
	@rm -rfd $(log_path)/* *.log