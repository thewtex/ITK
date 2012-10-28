#!/usr/bin/python
#
# Usage: WhatModulesITK.py itkSourceTree applicationFiles...
#     Generate a FindPackage(ITK COMPONENTS) that lists all modules referenced by a set of files
#
#     For example:
#       Running from the ITK source,
#         ./Utilities/Maintenance/WhatModulesITK.py . Modules/Filtering/Smoothing/test/itkDiscreteGaussianImageFilterTest.cxx
#       Produces
#         Find_Package(ITK COMPONENTS
#           ITKImageFilterBase
#           ITKSmoothing
#           ITKTestKernel
#           )
#
# NOTE: The modules list is created by looking at the itk include
#       files used by the application files. Some programs do not include
#       files for every class they use. They assume the include files
#       they reference will reference the required includes. For
#       example, many applications do not include itkImage.h because
#       the itk files they do include reference itkImage.h. itkImage.h is
#       in the module ITKCommon and my not be listed.
#
# NOTE: IO modules, other than ITKIOImageBase, are not discovered
#       unless their include file is present in the application
#       code. If ITKIOImageBase is present, a cmake variable
#       ITK_IO_MODULES_USED is created and added to the module list.

import os, sys
import re

# Build a dict that maps include files to paths
def IncludesToPaths(path):
    includeToPath = dict()
    prog = re.compile(r"(itk.*\.h)")
    for root, dirs, files in os.walk(path):
        for f in files:
            if prog.match(f):
                includeFile = prog.findall(f)[0]
                parts = root.split("/")
                module = parts[len(parts)-3] + parts[len(parts)-2]
                includeToPath[includeFile] = module
    return includeToPath

# Build a dict that maps paths to modules
def FindModules(path):
    pathToModule = dict()
    fileProg = re.compile(r"itk-module.cmake")
    moduleProg = re.compile('.*itk_module[^(]*\(([^ \n]*)',re.S)
    for root, dirs, files in os.walk(path):
        for f in files:
            if fileProg.match(f):
                fid = open(root + "/" + f,"r")
                contents = fid.read()
                m = moduleProg.match(contents)
                if m:
                    moduleName = m.group(1)
                    parts = root.split("/")
                    pathToModule[parts[len(parts)-2] + parts[len(parts)-1]] = moduleName
                fid.close()
    return pathToModule

# Build a set that contains itk includes
def FindIncludes(path):
    includes = set()
    includeProg = re.compile(r"(itk.*\.h)")
    fid = open(path, "r")
    contents = fid.read()
    incs = includeProg.findall(contents)
    includes.update(incs)
    fid.close()
    return includes

# Start the program
if len(sys.argv) < 3:
    print "Usage: " + sys.argv[0] + " itkSourceTree files..."
    exit(0)

# Generate dict's for mapping includes to modules
includesToPaths = IncludesToPaths(sys.argv[1] + "/Modules")
pathsToModules = FindModules(sys.argv[1] + "/Modules")

# Build a set of includes for all command line files
allIncludes = set()
sys.argv.pop(0)
sys.argv.pop(0)
for f in sys.argv:
    if os.path.isfile(f):
        allIncludes.update(FindIncludes(f))

# Build a set that contains all modules referenced in command line files
allModules = set()
for inc in allIncludes:
    if inc in includesToPaths:
        module = includesToPaths[inc]
        allModules.add(pathsToModules[includesToPaths[inc]])

# Print a useful cmake command
print r'set(ITK_IO_MODULES_USED "")'
print "Find_Package(ITK COMPONENTS"
for module in sorted(allModules):
    print "  " + module
if "ITKIOImageBase" in allModules:
    print r"  ${ITK_IO_MODULES_USED}"
print ")"
