# How to solve the programming tasks

There are basically three ways to build and execute the exercises. Each of the three options has its advantages and disadvantages:

- [Working with a Dev Container](https://gitlab.fhnw.ch/progalg/exercises/-/wikis/Working-with-DevContainer)
- [Working on the HPC Cluster](https://gitlab.fhnw.ch/progalg/exercises/-/wikis/Working-on-the-HPC-Cluster)
- [Working without a Dev Container](https://gitlab.fhnw.ch/progalg/exercises/-/wikis/Working-without-DevContainer)

Normally, we recommend developing and testing the solutions in the Dev Container and executing the performance tests on the HPC cluster. To execute on the HPC cluster, it makes sense to copy the whole project with its directory structure to the cluster, rebuild the programs there, and execute the performance tests.

Please note that the Dev Container configuration file must be slightly modified before it is created: If your computer **does not have an NVIDIA graphics card**, you must remove the execution arguments `"--gpus"` and `"all"` from the `.devcontainer/devcontainer.json` file.

