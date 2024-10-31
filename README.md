Welcome to GrowOS.  GrowOS hopes to improve the possibility of code reuse across Esphome device projects.

Esphome uses yaml to enable declarative system configuration AKA a "no-code" system. The use of yaml in Esphome makes configuration of simple systems concise and understandable.  However, for larger yaml configurations and when there are many yaml files included by a project or there are multiple projects sharing the same yaml code, something more is needed to manage the complexity.  GrowOS attempts to improve that.

GrowOS has the following goals for Esphome device projects: 
1. Separate hardware definition from functional configuration to assemble projects faster.
2. Enable conditional compilation capabilities to compose projects more easily.
3. Define a flexible project integration structure to enable code reuse.

GrowOS attempts to satisfy these goals together by using a macro processor that supports variables, conditionals and functions and by introducing a build system that allows hardware definition to be separated from functional using a specified project directory structure and project configuration mechanism.  One complication that has to also be considered is that the yaml map keys used by Esphome require their elements to be listed together, because map keys (e.g. “sensor:” or “switch:”) are required by yaml to be unique within a single yaml document, which is what Esphome compiles.  GrowOS manages this by adding a yaml merge step to the build process that assembles Esphome yaml correctly.

Spoiler: GrowOS uses the C preprocessor ‘cpp’ as the macros processor, ‘make’ as the build system, an open-source Yaml processor called yq to do the Yaml merging, and a GrowOS-specified directory structure as the project system that separates hardware definition, application implementation and project configuration.

GrowOS leverages a small project called [cpphash](https://github.com/maartenSXM/cpphash) that enables the C preprocessor to be used on espmake.yaml. Every large project has a smaller project struggling to get out and for GrowOS that was cpphash. GrowOS relies on cpphash’s support for [Esphome projects](https://github.com/maartenSXM/cpphash/blob/main/esphome.mk) to launch esphome compile commands while avoiding unnecessary rebuilds.  Conveniently, the cpphash author chose to support ‘make’ as its exemplary build system.  cpphash can be used without GrowOS however GrowOS depends on cpphash.  cpphash, in turn, can be used to enable cpp for various text file formats, not just yaml.

On top of the Esphome build features that cpphash offers, GrowOS adds GrowOS BSPs (Board Support Packages), GrowOS apps, GrowOS system services, and GrowOS projects.  A GrowOS BSP is the part of Esphome yaml that defines hardware, such as the definition of the Esphome “display:” component.  GrowOS system services are selected generic esphome features that are centralized for optional inclusion and for use in any GrowOS project.  A GrowOS application is the espmake.yaml that declares software features such as actions and lambdas -the functionality that the device will provide and how it is exposed to the user.  And, finally, a GrowOS project is a Makefile fragment that connects a GrowOS BSP to a GrowOS app and to a configuration file which is just a header file containing some cpp directives to setup the project. Conveniently, since the project configuration is a cpp header file, it can be included by both Esphome yaml code and by C/C++ code.

GrowOS was originally named that since the idea was to use Esphome and grow up from there by building on top. The first task was to implement a generic integration platform capability for esphome.

The GrowOS Vision: Increase portability by separating Esphome BSPs, system services and applications.

More formally, GrowOS was motivated by the following requirements:

REQ1. Shall provide a configuration mechanism to externally change,
      include and exclude esphome device code to avoid hardcoding.

REQ2. Shall separate out hardware-specific software to enable
      application software reuse across multiple hardware platforms.

REQ3. Shall separate out system software to enable system software
      reuse across multiple applications.

REQ4. Shall provide build-level integration platform features that can
      accommodate multiple board support packages (BSPs) and applications.

REQ5. Shall be intuitive and natural for old school C programmers, including
      those who are esphome newbies, to configure and build with GOS.

GOS attempts to satisfy the above requirements using these techniques:

 - GOS uses make and a tool called cpphash to construct a yaml file using
   the C preprocessor in such a way that it can be processed by esphome.
   This enables #define and #ifdef etc to co-exist with yaml # comments.

 - GOS uses a tool called yq for its yaml  merge feature which enables
   yaml map keys to be declared multiple times in separate files.  This
   enables, for example, a system level reboot switch: to be declared
   in a system level yaml file and an application switch: to be declared
   separately in a yaml file specific to an application.

   GOS extends yq's capability to also allow such declarations to be made
   multiple times in one GOS yaml file even though this technically breaks
   the yaml specification. However, once the espmake.yaml file is constructed
   by the GOS build system, the multiple yaml map key declarations are merged
   and the yaml is correct according to the yaml specification.
 
 - GOS has a notion of a board support package (BSP) for espmake.yaml.
   Each GOS BSP has a bsp.yaml that declares hardware and advertises
   (using #define) what hardware it supports. Each BSP also has exports a
   pins.h file for C or C++ code to use if needed. That pins.h file is also
   used by bsp.yaml so that hardware pins are only defined in one file.

 - GOS has a notion of a project. A GOS project is a configuration that
   connects an application to a BSP.  The configuration mechanism uses
   C preprocessor #defines and make variables to allow a project to
   configure BSP, application and system-level features at build time. 

 - GOS applications, configurations and system services attempt to
   adapt to the hardware or available and/or provide error messages
   such as indicating that it is not possible to run on such a BSP

 - GOS provides mechanisms for C, C++ and yaml code to adapt to the
   current project, BSP, application and username.
