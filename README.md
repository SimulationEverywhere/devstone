# devstone
DEVStone is a benchmark utility designed to compare performance of Discrete-Event Simulators.

Definition of DEVStone can be found in these publications:
* http://cell-devs.sce.carleton.ca/publications/2005/GW05a/
* http://cell-devs.sce.carleton.ca/publications/2008/GW08a/
* http://cell-devs.sce.carleton.ca/publications/2011/WGG11/
* http://cell-devs.sce.carleton.ca/publications/2015/VNWD15/

The DEVStone benchmark has 4 kind of models to be studied. 
* LI: these models have a low level of interconnections, i.e. one input and one output port, for each coupled model. The input port is connected to each component but only one component produces an output through the output port.
* HI: these models have a high level of input couplings. Each atomic component a connects its output port to the input port of the a+1th component. 
* HO: these are models with high level of coupling and numerous outputs. HO models have two inputs and two outputs at each level  where each one of the atomic components triggers the entire first set of (width-1) atomic models. 
* HOmod: these are models with high level of coupling and numerous outputs. HOmod have a second set of (width-1) models where each one of the atomic components triggers the entire first set of (width-1) atomic models. 

## Our goals
* Keep track of the performance characteristics of our simulator, Cadmium. 
* Easily compare performance characteristics against other well-known simulators.

## Supported simulators
At this time we support:
* Cadmium: https://github.com/SimulationEverywhere/cadmium
* CDBoost: https://gforge.inria.fr/projects/cdboost/
* aDEVS: http://web.ornl.gov/~nutarojj/adevs/

## Usage
Run the help of the DEVStone command to check the parameters required. 

    devstone --help

Set the parameters and simulator and run.

    devstone --simulator=cadmium --version=HEAD \
             --kind=LI --width=100 --depth=5 \
             --ext-cycles=100 --int-cycles=100 \
             --event-list=events_list.in \
             --output=devstone.out

## License disclaimer
This project license is BSD 2-clause. However, each simulator being benchmarked has each own license that should be accepted before benchmarking them. 
In addition, Dhrystone 2.1 is  used as part of this project. For convenience its files are pasted into the dhry directory. Its own license should be accepted to use this DEVStone implementation.

