/*
 * Copyright (c) 2015 The Regents of the University of California
 * Valmon LEYMARIE <leymariv@berkeley.edu>
 * See LICENSE for details.
 */

#include <arch/topology.h>
#include <arch/arch.h>
#include <acpi.h>
#include <smp.h>

#include <stdio.h>

struct cpu_topology cpu_topology[MAX_NUM_CPUS];
int hw_coreid_lookup[MAX_NUM_CPUS] = {[0 ... (MAX_NUM_CPUS - 1)] -1};
int os_coreid_lookup[MAX_NUM_CPUS] = {[0 ... (MAX_NUM_CPUS - 1)] -1};

static void build_topology(uint32_t cpu_bits, uint32_t core_bits)
{
    struct Apicst *temp = apics->st;
    while (temp) {
	if (temp->type == ASlapic) {
	    int apic_id = temp->lapic.id;
	    uint32_t cpu_in_core = apic_id & ((1 << cpu_bits) -1);
	    uint32_t core_in_chip = (apic_id >> cpu_bits) & ((1 << core_bits)-1);
	    uint32_t chip_id = apic_id & ~((1 << (cpu_bits+core_bits)) -1);

	    /* TODO: Build numa topology properly */
	    cpu_topology[apic_id].numa_id = 0;
	    cpu_topology[apic_id].socket_id = chip_id;
	    cpu_topology[apic_id].core_id = core_in_chip;
	    cpu_topology[apic_id].thread_id = apic_id;
	}
	temp = temp->next;
    }
}


static void build_flat_topology()
{
    struct Apicst *temp = apics->st;
    while (temp) {
	if (temp->type == ASlapic) {
	    int apic_id = temp->lapic.id;
	    cpu_topology[apic_id].thread_id = apic_id;
	    cpu_topology[apic_id].core_id = 0;
	    cpu_topology[apic_id].socket_id = 0;
	    cpu_topology[apic_id].numa_id = 0;
	}
	temp = temp->next;
    }	    
}

void topology_init()
{
    uint32_t eax, ebx, ecx, edx;
    uint32_t cpu_bits, core_bits;
    int smt_leaf, core_leaf, num_cpus;
    
    eax = 0x0000000b;
    ecx = 0;
    cpuid(eax, ecx, &eax, &ebx, &ecx, &edx);
    cpu_bits = eax;

    smt_leaf = (ecx >>8)&0x00000001;
    /* If we are in the case system supports smt_leaf */
    if (smt_leaf == 1) {
    	eax = 0x0000000b;
    	ecx = 1;
    	cpuid(eax, ecx, &eax, &ebx, &ecx, &edx);
    	core_leaf = (ecx >>8)&0x00000002;
    	/* If we are in the case system supports core_leaf */
    	if (core_leaf == 2) {
    	    core_bits = eax - cpu_bits;
    	    build_topology(cpu_bits, core_bits);
    	}
    } else {
    	build_flat_topology();
    }
    return;   
}

/* Can only be called after kthread_init because we need num_cpus */
void print_cpu_topology() 
{
    int i = 0;
    while(i < MAX_NUM_CPUS) {
	if (os_coreid_lookup[i] != -1) {
	    int coreid = os_coreid_lookup[i];
	    printk("Numa Node: %d, Socket: %d, Core: %d, Thread: %d\n",
		   cpu_topology[coreid].numa_id, 
		   cpu_topology[coreid].socket_id,
		   cpu_topology[coreid].core_id, 
		   cpu_topology[coreid].thread_id);
	}
	i++;
    }    
}

