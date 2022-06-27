#!/usr/bin/env python3
# ==========================================================================
#
#   Copyright NumFOCUS
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#          https://www.apache.org/licenses/LICENSE-2.0.txt
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
# ==========================================================================*/

"""Generate a Clang module.modulemap file for an ITK module.

The return code is used to pass the number of modules generated.
"""

FAILURE_RETURN_CODE = 0

import sys
import networkx as nx
from pathlib import Path
import re

if len(sys.argv) < 4:
    print(f'Usage: {sys.argv[0]} <ModuleIncludeDir> <ModuleName> <CPlusPlusRequires>')
    sys.exit(FAILURE_RETURN_CODE)
include_dir = Path(sys.argv[1])
module_name = sys.argv[2]
cplusplus_requires = sys.argv[3]

headers = [h for h in include_dir.glob('*.h')]
nodes = set([str(h.name) for h in headers])
used_nodes = set()
excluded_headers = {
 # Do not include directly
 'itkExceptionObject.h',
 # Deprecated
 'itkOrientationAdapterBase.h',
 'itkEnableIf.h',
 'itkIsBaseOf.h',
 'itkIsConvertible.h',
 'itkIsSame.h',
 # Weird inconsistent definition errors
 'itkCurvatureFlowFunction.h',
 'itkBoxImageFilter.h',
 'itkPadImageFilterBase.h',
 'itkConstantPadImageFilter.h',
 'itkBSplineL2ResampleImageFilterBase.h',
 'itkBSplineResampleImageFilterBase.h',
 'itkBSplineCenteredResampleImageFilterBase.h',
}

regex = re.compile('^#\s*include\s+\"(itk\w+\.h)\"$')
edges = []
for header in headers:
    with open(header, 'r') as fp:
        for line in fp:
            match = regex.match(line)
            if match:
                dep = match.groups()[0]
                # The NumericTraits specializations have special include conditions
                if dep in nodes:
                    edges.append((header.name, match.groups()[0]))
    hxx_file = Path(str(header) + 'xx')
    if hxx_file.exists():
        with open(hxx_file, 'r') as fp:
            for line in fp:
                match = regex.match(line)
                if match:
                    dep = match.groups()[0]
                    # The NumericTraits specializations have special include conditions
                    if dep in nodes:
                        edges.append((header.name, match.groups()[0]))

graph = nx.DiGraph()
graph.add_edges_from(edges)

if not nx.is_directed_acyclic_graph(graph):
    print('Cycles found in module header dependencies:')
    for cycle in nx.simple_cycles(graph):
        print(cycle)
        sys.exit(FAILURE_RETURN_CODE)

num_modules = 0
generations = list(nx.topological_generations(graph))
generations.reverse()
with open(include_dir / 'module.modulemap', 'w') as fp:
    for generation in generations:
        num_modules += 1
        fp.write(f"module {module_name}{num_modules} " + "{\n")
        fp.write(f"  requires {cplusplus_requires}" + "\n")
        for header in generation:
            if header not in excluded_headers:
                used_nodes.add(header)
                fp.write(f'  header "{header}"' + "\n")
        fp.write("  export *\n")
        fp.write("}\n\n")

unused_nodes = nodes.difference(used_nodes)
with open(include_dir / 'module.modulemap', 'a') as fp:
    if len(unused_nodes):
        num_modules += 1
        fp.write(f"module {module_name}{num_modules} " + "{\n")
        fp.write(f"  requires {cplusplus_requires}" + "\n")
        for header in unused_nodes:
            if header not in excluded_headers:
                fp.write(f'  header "{header}"' + "\n")
        fp.write("  export *\n")
        fp.write("}\n\n")

sys.exit(num_modules)