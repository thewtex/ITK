The Modules/ThirdParty/pygccxml/src/pygccxml
subdirectory contains a reduced distribution of the pygccxml source tree with
just the library source.

This module is used by WrapITK to build the wrappings. We distribute our own
version so that the user does not have to install this dependency.
This module does not need to be compiled as it is a pure python module.
We do not allow (for the moment) the usage the systems pygccxml and rely
on the one shipped here.

See Modules/ThirdParty/VNL/src/README-ITK.txt for update procedure of the
snapshot using a subtree merge.
