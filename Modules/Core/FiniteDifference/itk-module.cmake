set(DOCUMENTATION "This module contains the base classes needed to implement
fininte differences images filtes. They include both spase and dense finite
differences. Most of the classes in this module are abtract and therefore are
not intended to be instantiated by themselves.")

itk_module(ITK-FiniteDifference DEPENDS ITK-Common TEST_DEPENDS ITK-TestKernel DESCRIPTION "${DOCUMENTATION}")
