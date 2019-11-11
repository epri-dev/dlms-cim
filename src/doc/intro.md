# Overview 

Electricity meters commonly implement Device Language Message Specification and Companion Specification for Energy Metering (DLMS/COSEM) standardized in the IEC 62056 series. Head-End Systems (HES) for metering systems often use the Common Information Model (CIM), standardized in IEC 61968-9, to communicate with other enterprise systems such as a Customer Information System (CIS) or Outage Management System (OMS).

Every system that uses both DLMS/COSEM for the meters and CIM for the enterprise interface must necessarily implement a mapping from one to the other. While this has been described in [IEC TS 62056-6-9](https://webstore.iec.ch/publication/24736), this work is to show a concrete example of how this mapping might be accomplished and to document some of the ambiguities and difficulties encountered along the way.

The purpose of this tool is to provide a reference implementation of a translation from DLMS/COSEM to the Common Information Model (CIM) as would be done in an Advanced Metering Infrastructure (AMI) Head-End System (HES).  It is intended not as an actual functional HES, but as an example of the mapping from DLMS/COSEM to CIM and back, and also to serve as a way to examine the communications flows among the simulated components as though they were part of a real AMI system.

In its default operating mode, the software will create two simulated meters, a simulated HES and a simulated CIS.  The interaction among the software pieces is entirely automated and periodic.  Every ten seconds, the CIS sends a disconnect request, encoded as CIM, to the HES simulator.  The HES simulator, in turn, translates this into disconnect message in DLMS format and relays these commands to the two simulated meters.  After the simulated meters receive the commands, they respond to indicate that they have performed the disconnect.  The HES recieves these responses, encapsulates them into a single CIM response and finally sends this response message back to the CIS.  The CIS then delays for two seconds and sends service reconnect messages to the HES which performs the actions as before, except with a reconnect command instead of a disconnect command.  

## Further reading

[How to build the software](@ref building)

[How to use the software](@ref using)
