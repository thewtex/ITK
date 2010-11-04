#==========================================================================
#
#   Copyright Insight Software Consortium
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#          http://www.apache.org/licenses/LICENSE-2.0.txt
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
#==========================================================================*/

#
#  Example on the use of the CurvatureAnisotropicDiffusionImageFilter
#
package require InsightToolkit

set reader [ itkImageFileReaderUS2_New ]
set writer [ itkImageFileWriterUS2_New ]

set inputCast  [ itkCastImageFilterUS2F2_New    ]
set outputCast [ itkRescaleIntensityImageFilterF2US2_New ]

set filter [ itkCurvatureAnisotropicDiffusionImageFilterF2F2_New ]

$inputCast  SetInput [ $reader     GetOutput ]
$filter     SetInput [ $inputCast  GetOutput ]
$outputCast SetInput [ $filter     GetOutput ]
$writer     SetInput [ $outputCast GetOutput ]


$reader SetFileName [lindex $argv 0]
$writer SetFileName [lindex $argv 1]


$outputCast SetOutputMinimum       0
$outputCast SetOutputMaximum   65535

set numberOfIterations [expr [lindex $argv 2]]
set timeStep           [expr [lindex $argv 3]]
set conductance        [expr [lindex $argv 4]]


$filter SetNumberOfIterations     $numberOfIterations
$filter SetTimeStep               $timeStep
$filter SetConductanceParameter   $conductance


$writer Update


exit

