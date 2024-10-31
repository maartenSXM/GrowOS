Welcome to GrowOS.  GrowOS hopes to improve code reuse across Esphome device projects.

Esphome uses yaml to enable declarative system configuration AKA a "no-code" system. The use of yaml in Esphome makes configuration of simple systems concise and understandable.  However, for larger yaml configurations and when there are many yaml files included by a project or there are multiple projects sharing the same yaml code, something more is needed to manage the complexity.  GrowOS attempts to improve that.

GrowOS has the following goals for Esphome device projects:
 1. Separate hardware definition from functional configuration to assemble projects faster.
 2. Enable conditional compilation capabilities to compose projects more easily.
 3. Define a flexible project integration structure to enable code reuse.

GrowOS attempts to satisfy these goals together by using a macro processor that supports variables, conditionals and functions and by introducing a build system that allows hardware definition to be separated from functional using a specified project directory structure and project configuration mechanism.  One complication that has to also be considered is that the yaml map keys used by Esphome require their elements to be listed together, because map keys (e.g. “sensor:” or “switch:”) are required by yaml to be unique within a single yaml document, which is what Esphome compiles.  GrowOS manages this by adding a yaml merge step to the build process that assembles Esphome yaml correctly.

Spoiler: GrowOS uses the C preprocessor ‘cpp’ as the macros processor, ‘make’ as the build system, an open-source Yaml processor called yq to do the Yaml merging, and a GrowOS-specified directory structure as the project system that separates hardware definition, application implementation and project configuration.

GrowOS leverages a small project called cpphah (https://github.com/maartenSXM/cpphash) that enables the C preprocessor to be used on esphome yaml. Every large project has a smaller project struggling to get out and for GrowOS that was Cpptext. GrowOS relies on cpptext’s support for Esphome projects (xxx link) to launch esphome compile commands while avoiding unnecessary rebuilds.  Conveniently, the cpphash author chose to support ‘make’ as its exemplary build system.  Cpphash can be used without GrowOS however GrowOS depends on Cpphash.  Cpphash, in turn, can be used to enable cpp for various text file formats, not just yaml.

On top of the Esphome build features that Cpphash offers, GrowOS adds GrowOS BSPs (Board Support Packages), GrowOS apps, GrowOS system services, and GrowOS projects.  A GrowOS BSP is the part of Esphome yaml that defines hardware, such as the definition of the Esphome “display:” component.

GrowOS system services are selected generic esphome features that are centralized for optional inclusion and for use in any GrowOS project.

A GrowOS application is the esphome yaml that declares software features such as actions and lambdas -the functionality that the device will provide and how it is exposed to the user.

And, finally, a GrowOS project is a Makefile fragment that connects a GrowOS BSP to a GrowOS app and to a configuration file which is just a header file containing some cpp directives to setup the project. Conveniently, since the project configuration is a cpp header file, it can be included by both Esphome yaml code and by C/C++ code.
