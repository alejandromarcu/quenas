# Quenas
QUENAS stands for QUeued Network Automatic Simulation, and I wrote it for my master thesis in college. 
Please see the file "Getting Started with Quenas.pdf" for more information

##Setup

Run at the shell:

```
$ make quenas
```

This will completely build an executable for your platform. Then, you are ready to run simulations.

You can first run the simulator in test mode to check that everything is in order:

```
$ quenas -test
```

Running a simulation is very easy; you just need to provide an input file (usually with sim extension),
and the name for an XML output file:

```
$ quenas example.sim ouput.xml
```

The output file can be seen with any text viewer or with a browser. The simulations can optionally
use XSLT templates to provide a nice view of the output.
