# Mapping DLMS/COSEM (IEC 62056) to CIM (IEC 61968-9)

Electricity meters commonly implement Device Language Message Specification and Companion Specification for Energy Metering (DLMS/COSEM) standardized in the IEC 62056 series.  Head-End Systems (HES) for metering systems often use the Common Information Model (CIM), standardized in IEC 61968-9, to communicate with other enterprise systems such as a Customer Information System (CIS) or Outage Management System (OMS).  

Every system that uses both DLMS/COSEM for the meters and CIM for the enterprise interface must necessarily implement a mapping from one to the other.  While this has been described in [IEC TS 62056-6-9](https://webstore.iec.ch/publication/24736), this work is to show a concrete example of how this mapping might be accomplished and to document some of the ambiguities and difficulties encountered along the way.
