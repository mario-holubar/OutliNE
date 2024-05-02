# OutliNE

![OutliNE screenshot](screenshot.png)

OutliNE is a neuroevolution engine.

It simulates a population of neural networks performing a task such as controlling a car on a racetrack based on distance sensors.
The individuals are assigned a fitness value (distance along the track + penalty for crashing) and ranked accordingly.
The fittest individuals are recombined and mutated to form a new generation.
By repeating this process, the networks evolve to perform better and better at the task.

Neuroevolution works without the need for continuous reward signals, addressing one of the main limitations of reinforcement learning.

OutliNE is designed to be easily extensible with new environments and neuroevolution algorithms.
It currently supports three algorithms, SANE, ESP and CoSyNE.
These control how individuals are combined to form new networks.
