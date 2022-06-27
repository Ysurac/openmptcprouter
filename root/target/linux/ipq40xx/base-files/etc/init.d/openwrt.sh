#!/bin/sh /etc/rc.common
# Copyright (C) 2006-2011 OpenWrt.org

START=99
STOP=98

rps_flow_cnt=4096
core_count=$(grep -c processor /proc/cpuinfo)
rps_sock_flow_ent=`expr $core_count \* $rps_flow_cnt`
queue_cores="0 1 2"
queue_irq_cores="1 2"
wifi_core="3"
usb_core="0"

############### util functions ###############

to_hex_list() {
	local cores=$1
	local converted=""
	for core in $(echo $cores | awk '{print}')
	do
		local hex="$(gen_hex_mask "$core")"
		converted="$converted $hex"
	done
	echo `echo $converted | sed 's/^ *//'`
}

gen_hex_mask() {
	local cores=$1
	local mask=0
	for core in $(echo $cores | awk '{print}')
	do
		local bit="$((1 << $core))"
		let "mask = mask + bit"
	done
	local hex="$(printf %x "$mask")"
	echo "$hex"
}

val_at_index() {
	local values=$1
	local idx=$2
	echo `echo $values | awk -v i=$idx '{print $i}'`
}

size_of_list() {
	local list=$1
	local spaces=`echo $list | grep -o ' ' | wc -l`
	echo `expr $spaces + 1`
}

set_core_mask() {
	local file=$1
	local cores=$2
	local mask="$(gen_hex_mask "$cores")"
	echo $mask > $file
}

set_core_mask_round() {
	local files=$1
	local cores=$2
	local step_size=$3
	[ ! -n "$3" ] && step_size=1
	
	local core_count="$(size_of_list "$cores")"
	local counter=0
	local idx=1
	local roof=`expr $core_count \* $step_size`
	for file in $(echo $files | awk '{print}')
	do
		let "idx = counter / step_size + 1"
		local core="$(val_at_index "$cores" $idx)"
		set_core_mask $file $core
		let "counter = counter + 1"
		[ $counter -ge $roof ] && counter=0
	done
}

############### assign network queues ###############

set_interface_round() {
	local interface=$1
	local cores=$2
	local step_size=$3
	[ ! -n "$3" ] && step_size=1
	
	echo "using round mask for interface: $interface, step size: $step_size"
	set_core_mask_round "$(ls /sys/class/net/$interface/queues/tx-*/xps_cpus)" "$cores" $step_size
	set_core_mask_round "$(ls /sys/class/net/$interface/queues/rx-*/rps_cpus)" "$cores" $step_size
	
	for file in /sys/class/net/$interface/queues/rx-[0-9]*/rps_flow_cnt
	do
		echo $rps_flow_cnt > $file
	done
}

set_interface() {
	local interface=$1
	local cores=$2
	
	echo "using cores: $cores for interface: $interface"
	
	for file in /sys/class/net/$interface/queues/rx-[0-9]*/rps_cpus
	do
		set_core_mask $file "$cores"
		echo $rps_flow_cnt > `dirname $file`/rps_flow_cnt
	done

	for file in /sys/class/net/$interface/queues/tx-[0-9]*/xps_cpus
	do
		set_core_mask $file "$cores"
	done
}

set_interface_queues() {
	echo "using cpu: $queue_cores for network queues"
	for dev in /sys/class/net/eth*
	do
		[ -d "$dev" ] || continue
		
		local interface=`basename $dev`
		
		set_interface $interface "$queue_cores"
	done

	for dev in /sys/class/net/wlan*
	do
		[ -d "$dev" ] || continue
		
		local interface=`basename $dev`
		
		set_interface $interface "$queue_cores"
	done
	
	for dev in /sys/class/net/eth*
	do
		local eth=`basename $dev`
		echo "enabling offload on $eth"
		ethtool -K $eth rx-checksum on >/dev/null 2>&1
		ethtool -K $eth tx-checksum-ip-generic on >/dev/null 2>&1 || (
		ethtool -K $eth tx-checksum-ipv4 on >/dev/null 2>&1
		ethtool -K $eth tx-checksum-ipv6 on >/dev/null 2>&1)
		ethtool -K $eth tx-scatter-gather on >/dev/null 2>&1
		ethtool -K $eth gso on >/dev/null 2>&1
		ethtool -K $eth gro on >/dev/null 2>&1
		ethtool -K $eth lro on >/dev/null 2>&1
		ethtool -K $eth tso on >/dev/null 2>&1
		ethtool -K $eth ufo on >/dev/null 2>&1
	done
	
	echo $rps_sock_flow_ent > /proc/sys/net/core/rps_sock_flow_entries
	sysctl -w net.core.rps_sock_flow_entries=$rps_sock_flow_ent
}

############### assign interrupts ###############
set_irq_cores() {
	local mask="$(gen_hex_mask "$2")"
	echo "set mask $mask for irq: $1"
	echo $mask > "/proc/irq/$1/smp_affinity"
}

set_irq_pattern() {
	local name_pattern="$1"
	local cores="$2"
	
	for irq in `grep "$name_pattern" /proc/interrupts | cut -d: -f1 | sed 's, *,,'`
	do
		set_irq_cores $irq "$cores"
	done
}

set_irq_interleave() {
	local name_pattern=$1
	local cores=$2
	local step_size=$3
	
	local files=""
	for irq in `grep "$name_pattern" /proc/interrupts | cut -d: -f1 | sed 's, *,,'`
	do
		files="$files\
		/proc/irq/$irq/smp_affinity"
	done
	
	set_core_mask_round "$files" "$cores" "$step_size"
}

set_irqs() {
	#dma
	set_irq_pattern bam_dma "$usb_core"

	#ethernet
	set_irq_interleave eth_tx "$queue_irq_cores" 4
	set_irq_interleave eth_rx "$queue_irq_cores" 1

	#pcie
	set_irq_pattern pcie "$wifi_core"

	#usb
	set_irq_pattern usb "$usb_core"
}

start() {
	set_interface_queues
	set_irqs
}
