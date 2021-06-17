# agro_mesh

Firmware of [std_board](https://github.com/thmalmeida/std_board) of Agro Project.

To download:

```
# Download from git
$ git clone --recursive https://github.com/rnascunha/agro_mesh.git
# Enter directory
$ cd agro_mesh
```
Configure project:

```
$ idf.py menuconfig
```

Go to *Agro Mesh Configuration* and configure to your envirioment. Than, just flash and compile:

```
$ idf.py build flash monitor
```
