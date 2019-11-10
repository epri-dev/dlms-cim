# Design
The software is composed of a number of pieces, which together make up the simulation.  The components are:

    - Meter simulator(s)
    - Head-End System (HES) simulator
    - Meter Data Management System (MDMS) simulator

Each of the components is described in more detail below.

## Meter simulator

## HES simulator

## MDMS simulator


## How to add a COSEM object
Adding a new COSEM object to the code is best done in a series of steps, starting with an existing class.  For this explanation, we will add the Image Transfer class (`class_id` = 18) based on the code for the `LinuxDisconnect` object.

 1. Copy the `LinuxDisconnect` files.  In this case, copy `LinuxDisconnect.cpp` to `LinuxImageTransfer.cpp` and `include/LinuxDisconnect.h` to `include/LinuxImageTransfer.h`
 2. Add the files to `CMakeList.txt`.  Add the `LinuxImageTransfer.cpp` file to the list of sources used to build the executable.
 3. Add the new object to the `LinuxCOSEMServer.cpp` file as a `LOGICAL_DEVICE_OBJECT` and to the `LinuxCOSEMServer.h` file.
 4. Add the new include file to the `LinuxCOSEMServer.h` file.
 5. Within the new `LinuxImageTransfer.h` file, create the appropriate `ClassIDType`.  In this case we'll use `const ClassIDType CLSID_ImageTransfer = 18;`.
 6. Replace the class names `Disconnect`, `IDisconnect` and `LinuxDisconnect` with appropriate replacements (e.g. `ImageTransfer`, `IImageTransfer` and `LinuxImageTransfer`). 

At this point the code should compile, but it will effectively be a duplicate of the copied code.  That is, in this case, it will be a `Disconnect` COSEM object, but having a `class_id = 18`.  What's next is to add the specific functionality of the new class, which can be done in the following steps:

 1. Add the appropriate `COSEMAttribute` enum and declarations in the `.h` file and then the corresponding attributes in the `.cpp` file.  For now, it's sufficient to simply use `IntegerSchema` for all attributes.  This will be fixed later.
 2. Add the appropriate `COSEMMethod` enum and declarations in the `.h` file and into the `.cpp` file.  As above, one can use `IntegerSchema` for all methods.
 3. Add in the appropriate schemas for each attribute.  How this is done depends on the schema type.  For example, for a COSEM `enum` type, a C++ `class enum` is used.
 4. Add in the appropriate schemas for each method. This is exactly like the previous step; the only difference is in usage.
 5. Set the OBIS code for the class in the `.cpp` file.  For `ImageTransfer`, the OBIS code is `0-0.44.0.0*255`.
 6. Add class-specific behavior.  For example, the `image_verify` method might check a CRC of the received image and return a result.  This may also require some local data, which should be in the most derived class (in this case, the `LinuxImageTransfer` class.)

