/*
 * Copyright (c) 20015 The Regents of the University of California
 * Valmon Leymarie <leymariv@berkeley.edu>
 * See LICENSE for details.
 */

#ifndef TOPOLOGY_H_
#define TOPOLOGY_H_

struct cpu_topology {
  int numa_id;
  int socket_id;
  int core_id;
  int thread_id;
};

extern struct cpu_topology cpu_topology[];
extern int os_coreid_lookup[];
extern int hw_coreid_lookup[];

void topology_init();
void print_cpu_topology();

#endif /* !TOPOLOGY_H_ */
