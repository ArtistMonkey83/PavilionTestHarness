# Quick Links

- [Overview](#overview)
- [Usage](#usage)
- [Tips, Tricks, and Troubleshooting](#tipstrickstrouble)

# <a name="overview"></a>Overview

---

Website: <https://www.open-mpi.org/>

FAQ: <https://www.open-mpi.org/faq/>

From the Open MPI website:

"The Open MPI Project is an open-source Message Passing Interface
implementation that is developed and maintained by a consortium of academic,
research, and industry partners. Open MPI is therefore able to combine the
expertise, technologies, and resources from all across the High Performance
Computing community in order to build the best MPI library available."

Open MPI is the combination and continuation of several open source MPI
projects including LA-MPI and LAM/MPI. As such, there is a good chance that
it will be the preferred MPI implementation for LANL HPC machines of the
future.

Open MPI is component based and a single installation is able to work with
multiple machine configurations, including hardware. There are many run-time
configuration options to tune the behavior of Open MPI. A good introduction to
Open MPI's framework is the run-time tuning
[FAQ entry](https://www.open-mpi.org/faq/?category=tuning).

# <a name="usage"></a>Usage

---

Open MPI is available via a modulefile. Loading an Open MPI modulefile will
detect if a compiler modulefile is already loaded and will supply an
installation that works with that compiler. To load the default Open MPI
modulefile, do the following:

```
$> module load openmpi
```

Use the Open MPI compiler wrappers when compiling code:
- mpicc
- mpic++/mpicxx
- mpifort
- mpif90
- mpif77

Applications compiled with Open MPI can be run with Slurm's srun or its own
mpirun:

```
$> srun -n 2 -N 2 ./hello_world
OR
$> mpirun -n 2 -N 2 ./hello_world
```

Please see the man pages for srun (`man srun`) and mpirun (`man mpirun`) for
more information on runtime parameters.

# <a name="tipstrickstrouble"></a>Tips, Tricks, and Troubleshooting

---

## Mapping, Ranking, and Binding

Open MPI has a three-stage procedure for assigning MPI process locations and
ranks to hardware resources when using mpirun (see srun's man page for analogous
options to srun). Depending on the application, modification of any or all of
these from the default behaviors might be necessary to achieve the best
performance.

The three-stage procedure is as follows:

1. MPI processes are mapped to hardware resources
1. Each MPI process gets a rank in MPI_COMM_WORLD
1. Each MPI process is constrained to run on specific processing elements

There are different algorithms that can be used to change how MPI processes are
mapped, ranked, and bound. The default mapping, ranking, and binding behaviors
can change between major versions of Open MPI, so please consult the man page for
mpirun to find out the current defaults.

It is possible that a local LANL site configuration over-rides the defaults. To
determine the current values for mapping, ranking, and binding settings, use
the `ompi_info` command to get all Open MPI settings and grep for the right
value:
- Current mapping policy: `ompi_info -a | grep rmaps_base_mapping_policy`
- Current ranking policy: `ompi_info -a | grep rmaps_base_ranking_policy`
- Current binding policy: `ompi_info -a | grep hwloc_base_binding_policy`
If the "current value" is just "" (an empty string), then the default behavior
specified in the mpirun man page will happen.

To see how Open MPI has allocated and bound MPI ranks for a given mpirun line,
use mpirun's `--report-bindings` command line parameter:

```
$> mpirun --report-bindings ...
```

## Modifying Mapping, Ranking, and Binding Behaviors

Suggested mpirun parameters will be different between MPI-everywhere and
threaded applications. Thus, each will be dealt with separately below.

### MPI-Everywhere

For MPI-everywhere applications that do not care how specific MPI ranks are
laid out relative to other MPI ranks, the default mpirun behavior should
suffice. That is, no command line options for mapping, ranking, or binding
should need to be given to mpirun.

If an MPI-everywhere application does care about specific MPI rank placement,
such as making sure consecutive MPI ranks are on the same host as MPI ranks
communicate most heavily with other MPI ranks close to their own numeric MPI
rank, the ranking policy should be set to rank by cores:

```
$> mpirun --rank-by core ...
```

This will ensure that MPI ranks are assigned in order of processing cores on a
host, filling up each host one at a time.

Sometimes, the default binding policy may not be the best option for some
applications. For example, the default policy might want to bind to sockets,
but an application really wants to be bound to cores so that they don't migrate
at all. Do this by the following:

```
$> mpirun --bind-to core ...
```

If an application does not want to use all of the cores on a compute node, it is
a good idea to distribute the MPI processes in such a way that there is a
balance across both nodes and sockets. The following example achieves this (as
well as ranking by core):

```
$> mpirun --map-by socket:span --rank-by core ...
```

If an application does not want to use all of the cores on a compute node with
load balance between nodes and sockets, but also wants to bind MPI processes to
cores, do the following:

```
$> mpirun --map-by socket:span --bind-to core ...
```

### Threaded Applications

There is no good default Open MPI configuration that would work well for all
threaded applications, so it is necessary for threaded applications to specify
their own mapping, ranking, and binding behavior to reflect the MPI process and
thread layout of the application that works best. The following are example
mpirun commands for use with some basic layouts.

- To run one MPI rank per compute node and allow threads to utilize other cores,
  use the following:
  ```
  $> mpirun -n <number of nodes in allocation> --map-by node --bind-to none ...
  ```
- To run two MPI ranks per compute node, one MPI rank per socket, and allow
  threads to utilize the other cores on a socket, use the following:
  ```
  $> mpirun -n <2 * number of nodes in allocation> --map-by socket:span --bind-to socket ...
  ```
- Running more than one MPI rank per socket requires more complicated parameters
  to --map-by. For cases like these, we recommend you contact
  [LANL HPC help](mailto:consult@lanl.gov) to make sure you construct the proper
  command. However, here are two examples of running two MPI ranks per socket on
  every compute node and binding those ranks to 8 cores each:
  ```
  $> mpirun --map-by ppr:2:socket:PE=8 --bind-to core ...
  OR
  $> mpirun -n <#> --map-by socket:PE=8 --bind-to core ...
  ```
  Note that the first command does not require the -n parameter. This command
  tells mpirun to create ( 2 * num of sockets in allocation ) MPI processes and
  bind them to 8 processing elements (cores).

To change the ranking of any of the above, just use the `--rank-by` command
line parameter.
