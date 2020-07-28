# Product Information

Open MPI is a modular MPI implementation. This repository contains materials
used in maintaining Open MPI on LANL HPC machines.

## Release Schedule

There is no set schedule to Open MPI's releases. There is, however, a definite
cycle to them. Normally, two release series are active at a given time. A
release series is made up of versions that share the same minor version number.

After v2.0, the major version number is in the first field of the version
string. The second fields designates that new features were added. The last
field designates a bug-fix release. All versions that share the same major
version number should be ABI-compatible. A major version release series will
have active development for about 1 year, and then have bug-fix support for
about a year after that.

Prior to v2.0, the major version was the first two fields in the version string.
All versions that share the same major version number should be ABI-compatible.
Releases were divided between stable and feature release series. New features
were added in the feature releases. The feature release series was then wrapped
up in to the following stable release series. The stable release series were
recommended for production use and had 1.<even> major version numbers; the
feature releases are really only to be used for development purposes and had
1.<odd> major version numbers. Stable and feature release series were not
generally worked on at the same time.

## Details about how we put Releases on our Machines

- **Core Product**: yes
- **Retrieval Schedule**: Release-based Retrieval Schedule
- **Promotion Schedule**: Pooling Promotion Schedule
- **Production Deprecation Schedule**: No Production Deprecation Schedule
- **Removal Schedule**: Standard Removal Schedule
- **Common or Machine-specific build**: machine
- **Multiple builds required**: yes

Every release from a stable series should go through the Production Candidate
testing process. Only the final stable release should be promoted to Production.
When that final stable release is promoted, all other older stable release
versions should be deprecated.

# Files

This directory will contain the master scripts and files for installing Open
MPI. The directories are for individual versions and are called version
directories. Everything needed for that version of Open MPI should be in that
version's version directory.

## openmpi_build.bash

Build_openmpi.bash will attempt to build Open MPI for every compiler it finds
in the modules environment. Invoke it from this directory. The only required
option for build_openmpi.bash is --version. This tells build_openmpi.bash
which version directory to use. Please see build_openmpi.bash's help for more
information on this and other command line parameters.

Openmpi_build.bash requires envmodules_queryfuncs.bash and
envmodules_usefuncs.bash to be available in the same directory that it is
invoked in. These scripts are available in the ptools' utilities repository.

For each successful build, openmpi_build.bash will place a summary file in the
installation directory that will be named "build_summary". As an
example, if Open MPI 1.6.0 is being built for gcc 4.6.1 and being installed
to /usr/projects/hpcsoft/openmpi, the build summary will be
/usr/projects/hpcsoft/openmpi/1.4.5-gcc-4.6.1/build_summary and the
installation directory will be /usr/projects/hpcsoft/openmpi/1.4.5-gcc-4.6.1.

Openmpi_build.bash will also tar up and move log files from the build in to
the installation directory. If a build fails, any log files will be moved to
a directory named "failed_build_logs" that will be located in the version
directory.

## version directories

At least two files are needed in each version directory:
- build_config: customizable build configuration script written in bash
- Open MPI platform file: the name of the platform file is specified in
  build_config

Additionally, an Open MPI source tarball will be needed and a path to find
that tarball will need to be passed on the openmpi_build.bash command line
using the --tarball-dir parameter.

The build_config file allows for the customization of a particular build
of Open MPI. It can define the following parameters:

- ompi_tarball: specify the tarball to use when building Open MPI. Required.
- ompi_tarball_md5sum: specify the expected md5sum of the Open MPI source
  tarball. Since the source tarball is located elsewhere and not under the
  control of these installation scripts, this is necessary to verify the
  integrity of the source. Required.
- ompi_src_dir: specify the name of the top-level directory created by
  untarring ompi_tarball. Required.
- ompi_src_patches: specify patches that can be applied to the Open MPI
  source after untarring it. The patches themselves need to be created in such
  a way that patch -p1 can be used to apply the patch from within the Open MPI
  source directory. The type of this value is a bash array so that multiple
  patches can be applied. The patches will be applied in the order that they
  appear in ompi_src_patches and should located in the version directory.
- platform_file: specify the platform file to use for a particular build.
  Required.
- ompi_configure_envs: specify environment variables to be used when invoking
  configure. Typically, CFLAGS, CXXFLAGS, or FCFLAGS are specified here. They
  will not be exported as environment variables; instead, they will precede
  the invocation of configure like this:
  $> $ompi_configure_envs ./configure ...
  It would be useful to specify them in the following format:
  ompi_configure_envs="FOO=\"value1 value2\" BAR=\"value3 value4\""
  This parameter is optional.
- ompi_configure_options: specify flags for the configure process. The
  following configure flags will be determined by the installation scripts, so
  there is no need to specify them in the build configuration file:
  --prefix
  --with-platform
  This parameter is optional.
- ompi_make_options: specify make flags. By default, the installation scripts
  don't set any make flags. Optional.
- post_install_scripts: specify scripts that can be applied to an Open MPI
  installation after installation. The script will be passed the installation
  prefix of the build just done so that the script does not have to figure out
  where the Open MPI build is located. The script will be called from within
  the top level source directory (the directory that is created by untarring
  the source tarball), so any relative paths need to take that in to account.
  The type of this value is a bash array so that multiple post-install scripts
  can be applied. The scripts will be run in the order that they appear in
  post_install_scripts. Optional.

The build_config file will be sourced for every build attempt. That is,
it will be sourced once all compiler information for a specific build
has been discovered. This way, since the build configuration file is
essentially a bash script, it is possible to use code to dynamically decide
what to set values to based on the current build attempt. The following useful
variables are available:
- machine: the name of the machine that the installation script is
  running on.
- os: the name of the OS that the installation script is running in.
- ompi_version: the version of Open MPI that is being installed.
- comp_class: the type of compiler (e.g. gcc, intel, pgi, pathscale)
- comp_ver: the full version of the compiler being used
- comp_mver: the major version of the compiler being used.

All of these variables are reset before reading build_config so there should
be no need to include logic within build_config to keep one build attempt's
settings from interfering with another build attempt.

The version directory can also contain a build_multiple_config configuration
file that can be used in liu of the --compiler-modules command line parameter.
This configuration file can specify a list of compiler modulefiles to build
against. Build_multiple_config is only used when --compiler-modules is not used
on the command line. It should be written in bash syntax and will be sourced
by the installation script.

Build_multiple_config can define the following parameters:

- compiler_list: a comma-separated list of compiler modulefiles with which to
  attempt to build Open MPI against.

The following variables are set at the time build_multiple_config is sourced
and can be used in determining the appropriate settings:

- machine: the name of the machine that openmpi_build.sh is running on
- os: the OS name of the machine that openmpi_build.sh is running on

## openmpi_build_test.bash

Openmpi_build_test.bash will test Open MPI builds by running through all of
the compilers and loading Open MPI for each, then running some simple mpirun
tests. 

Openmpi_build_test.bash requires envmodules_helpers.bash which is available in
the ptools' utilities repository.

Openmpi_build_test.bash is designed to be run on the node(s) that need to be
tested. It can be run on either a compute node or a headnode. To run on an
interactive session on a compute node, do the following:

```
$> ./openmpi_build_test.bash --version=<version to test>
```

To run openmpi_build_test.bash on a headnode, use the `--headnode-mode`
command line parameter. This will set appropriate environment variables for
running Open MPI on a headnode (by default, Open MPI's configuration won't
allow a mpirun instance to run without a scheduler allocation).

## modulefile-universal

modulefile-universal is a modulefile that can be used to load Open MPI based
upon the compiler module loaded.

## hpc_doc_page.md

Markdown file for use with hpc.lanl.gov Open MPI documentation page:
https://hpc.lanl.gov/ompi
