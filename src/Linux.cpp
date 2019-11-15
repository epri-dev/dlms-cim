// ===========================================================================
// Copyright (c) 2018, Electric Power Research Institute (EPRI)
// All rights reserved.
//
// DLMS-COSEM ("this software") is licensed under BSD 3-Clause license.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// *  Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// *  Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// *  Neither the name of EPRI nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
// OF SUCH DAMAGE.
//
// This EPRI software incorporates work covered by the following copyright and permission
// notices. You may not use these works except in compliance with their respective
// licenses, which are provided below.
//
// These works are provided by the copyright holders and contributors "as is" and any express or
// implied warranties, including, but not limited to, the implied warranties of merchantability
// and fitness for a particular purpose are disclaimed.
//
// This software relies on the following libraries and licenses:
//
// ###########################################################################
// Boost Software License, Version 1.0
// ###########################################################################
//
// * asio v1.10.8 (https://sourceforge.net/projects/asio/files/)
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// 

#include <iostream>
#include <cstdio>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <iomanip>
#include <asio.hpp>
#include <algorithm>
#include <string>

#include "LinuxBaseLibrary.h"
#include "LinuxCOSEMServer.h"

#include "HDLCLLC.h"
#include "COSEM.h"
#include "serialwrapper/SerialWrapper.h"
#include "tcpwrapper/TCPWrapper.h"
#include "dlms-cimConfig.h"

using namespace std;
using namespace EPRI;
using namespace asio;

class AppBase
{
public:
    typedef std::function<void(const std::string&)> ReadLineFunction;

    AppBase(LinuxBaseLibrary& BL) : 
        m_Base(BL), m_Input(BL.get_io_service(), ::dup(STDIN_FILENO)), 
        m_Output(BL.get_io_service(), ::dup(STDOUT_FILENO))
    {
    }
    
    virtual void Run()
    {
        m_Base.Process();
    }
    
    virtual void PrintLine(const std::string& Line)
    {
        asio::write(m_Output, asio::buffer(Line));
    }
    
    virtual void ReadLine(ReadLineFunction Handler)
    {
        asio::async_read_until(m_Input,
            m_InputBuffer,
            '\n',
            std::bind(&AppBase::ReadLine_Handler,
                      this,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      Handler));
        
    }
    
    virtual std::string GetLine()
    {
        asio::read_until(m_Input, m_InputBuffer, '\n');
        return ConsumeStream();
    }
    
protected:
    void ReadLine_Handler(const asio::error_code& Error, size_t BytesTransferred, ReadLineFunction Handler)
    {
        if (!Error)
        {
            Handler(ConsumeStream());
        }
    }
    
    std::string ConsumeStream()
    {
        std::istream Stream(&m_InputBuffer);
        std::string  RetVal;
        std::getline(Stream, RetVal);
        return RetVal;
    }

    int GetNumericInput(const std::string& PromptText, int Default)
    {
        std::string RetVal;
        do
        {
            PrintLine(PromptText + ": ");
            RetVal = GetLine();
            try
            {
                if (RetVal.length())
                    return std::stoi(RetVal, nullptr, 0);	
                else 
                    return Default;
            }
            catch (const std::invalid_argument&)
            {
                PrintLine("Input must be numeric!\n\n");
            }
            catch (const std::out_of_range&)
            {
                PrintLine("Input is too large!\n\n");
            }
        
        } while (true);
    }

    std::string GetStringInput(const std::string& PromptText, const std::string& Default)
    {
        std::string RetVal;
        PrintLine(PromptText + ": ");
        RetVal = GetLine();
        if (RetVal.empty())
            RetVal = Default;
        return RetVal;
    }
    
    LinuxBaseLibrary&           m_Base;
    posix::stream_descriptor    m_Input;
    asio::streambuf             m_InputBuffer;
    posix::stream_descriptor    m_Output;
    
};

class LinuxClientEngine : public COSEMClientEngine
{
public:
    LinuxClientEngine() = delete;
    LinuxClientEngine(const Options& Opt, Transport * pXPort)
        : COSEMClientEngine(Opt, pXPort)
    {
    }
    virtual ~LinuxClientEngine()
    {
    }
    
    virtual bool OnOpenConfirmation(COSEMAddressType ServerAddress)
    {
        Base()->GetDebug()->TRACE("\n\nAssociated with Server %d...\n\n",
            ServerAddress);
        return true;
    }

    virtual bool OnGetConfirmation(RequestToken Token, const GetResponse& Response)
    {
        Base()->GetDebug()->TRACE("\n\nGet Confirmation for Token %d...\n", Token);
        if (Response.ResultValid && Response.Result.which() == Get_Data_Result_Choice::data_access_result)
        {
            Base()->GetDebug()->TRACE("\tReturned Error Code %d...\n", 
                Response.Result.get<APDUConstants::Data_Access_Result>());
            return false;
        }
        
        if (CLSID_IData == Response.Descriptor.class_id)
        {
            IData     SerialNumbers;
            DLMSValue Value;
        
            SerialNumbers.value = Response.Result.get<DLMSVector>();
            if (COSEMType::VALUE_RETRIEVED == SerialNumbers.value.GetNextValue(&Value))
            {
                Base()->GetDebug()->TRACE("%s\n", DLMSValueGet<VISIBLE_STRING_CType>(Value).c_str());
            }
           
        }
        else if (CLSID_IAssociationLN == Response.Descriptor.class_id)
        {
            IAssociationLN CurrentAssociation;
            DLMSValue      Value;
            
            switch (Response.Descriptor.attribute_id)
            {
            case IAssociationLN::ATTR_PARTNERS_ID:
                {
                    CurrentAssociation.associated_partners_id = Response.Result.get<DLMSVector>();
                    if (COSEMType::VALUE_RETRIEVED == CurrentAssociation.associated_partners_id.GetNextValue(&Value) &&
                        IsSequence(Value))
                    {
                        DLMSSequence& Element = DLMSValueGetSequence(Value);
                        Base()->GetDebug()->TRACE("ClientSAP %d; ServerSAP %d\n", 
                            DLMSValueGet<INTEGER_CType>(Element[0]),
                            DLMSValueGet<LONG_UNSIGNED_CType>(Element[1]));
                    }
                }
                break;
            
            default:
                Base()->GetDebug()->TRACE("Attribute %d not supported for parsing.", Response.Descriptor.attribute_id);
                break;
            }
        }
        return true;
    }
    
    virtual bool OnSetConfirmation(RequestToken Token, const SetResponse& Response)
    {
        Base()->GetDebug()->TRACE("\n\nSet Confirmation for Token %d...\n", Token);
        if (Response.ResultValid)
        {
            Base()->GetDebug()->TRACE("\tResponse Code %d...\n", 
                Response.Result);
        }
        return true;
    }

    virtual bool OnActionConfirmation(RequestToken Token, const ActionResponse& Response)
    {
        Base()->GetDebug()->TRACE("\n\nAction Confirmation for Token %d...\n", Token);
        if (Response.ResultValid)
        {
            Base()->GetDebug()->TRACE("\tResponse Code %d...\n", 
                Response.Result);
        }
        return true;
    }
    
    virtual bool OnReleaseConfirmation()
    {
        Base()->GetDebug()->TRACE("\n\nRelease Confirmation from Server\n\n");
        return true;
    }
    
    virtual bool OnReleaseConfirmation(COSEMAddressType ServerAddress)
    {
        Base()->GetDebug()->TRACE("\n\nRelease Confirmation from Server %d\n\n", ServerAddress);
        return true;
    }

    virtual bool OnAbortIndication(COSEMAddressType ServerAddress)
    {
        if (INVALID_ADDRESS == ServerAddress)
        {
            Base()->GetDebug()->TRACE("\n\nAbort Indication.  Not Associated.\n\n");
        }
        else
        {
            Base()->GetDebug()->TRACE("\n\nAbort Indication from Server %d\n\n", ServerAddress);
        }
        return true;
    }

};

class ClientApp : public AppBase
{
public:
    ClientApp(LinuxBaseLibrary& BL)
        : AppBase(BL)
    {
        m_Base.get_io_service().post(std::bind(&ClientApp::ClientMenu, this));
    }

protected:
    void ClientMenu()
    {
        PrintLine("\nClient Menu:\n\n");
        PrintLine("\t0 - Exit Application\n");
        PrintLine("\tA - TCP Connect\n");
        PrintLine("\tB - HDLC Physical Connect\n");
        PrintLine("\tC - HDLC Identify\n");
        PrintLine("\tD - HDLC Datalink Connect\n");
        PrintLine("\tE - Serial Wrapper Connect\n");
        PrintLine("\n");
        PrintLine("\t1 - COSEM Open\n");
        PrintLine("\t2 - COSEM Get\n");
        PrintLine("\t3 - COSEM Set\n");
        PrintLine("\t4 - COSEM Action\n");
        PrintLine("\t5 - COSEM Release\n");
        PrintLine("\n");
        PrintLine("\tT - TCP Disconnect\n"); 
        PrintLine("\tU - HDLC Disconnect\n");
        PrintLine("\tV - Serial Wrapper Disconnect\n");
        PrintLine("\nSelect: ");
        ReadLine(std::bind(&ClientApp::ClientMenu_Handler, this, std::placeholders::_1));
    }
    
    bool IdentifyResponse(const BaseCallbackParameter& Parameters)
    {
        const DLIdentifyResponseParameter& Response = 
            dynamic_cast<const DLIdentifyResponseParameter&>(Parameters);
        Base()->GetDebug()->TRACE("\n\nIdentify Response (%d, %d, %d, %d)...\n\n",
            Response.SuccessCode, Response.ProtocolID, Response.ProtocolVersion, 
            Response.ProtocolRevision);
        return true;
    }
    
    bool DisconnectConfirm(const BaseCallbackParameter& Parameters)
    {
        const DLDisconnectConfirmOrResponse& Response = 
            dynamic_cast<const DLDisconnectConfirmOrResponse&>(Parameters);
        Base()->GetDebug()->TRACE("\n\nDisconnect Response from Server %d...\n\n",
            Response.DestinationAddress.LogicalAddress());

        Base()->GetCore()->GetSerial()->ReleaseSocket(m_pSerialSocket);
        m_pSerialSocket = nullptr;
        m_pSocket = nullptr;
        PrintLine("Serial released.");
        
        return true;
    }
    
    void ClientMenu_Handler(const std::string& RetVal) 
    {
        if (RetVal == "0")
        {
            exit(0);
        }
        else if (toupper(RetVal[0]) == 'A')
        {
            int         SourceAddress = GetNumericInput("Client Address (Default: 1)", 1);
            std::string TCPAddress = GetStringInput("Destination TCP Address (Default: localhost)", "localhost");
    
            m_pClientEngine = new LinuxClientEngine(COSEMClientEngine::Options(SourceAddress), 
                new TCPWrapper((m_pSocket = Base()->GetCore()->GetIP()->CreateSocket(LinuxIP::Options(LinuxIP::Options::MODE_CLIENT, LinuxIP::Options::VERSION4)))));
            if (SUCCESSFUL != m_pSocket->Open(TCPAddress.c_str()))
            {
                PrintLine("Failed to initiate connect\n");
            }
        }
        else if (toupper(RetVal[0]) == 'B')
        {
            int         SourceAddress = GetNumericInput("Client Address (Default: 1)", 1);
            std::string SerialPort = GetStringInput("Serial Port (Default: /tmp/ttyS11)", "/tmp/ttyS11");
            
            PrintLine("Initial Baud Rate Selection\n");
            PrintLine("\t6  - 9600\n");
            PrintLine("\t10 - 115200\n");
            int         BaudRate = GetNumericInput("(Default: 6 - 9600)", 6);
            //
            // Engine
            //
            m_pClientEngine = new LinuxClientEngine(COSEMClientEngine::Options(SourceAddress), 
                (m_pHDLC = new HDLCClientLLC(HDLCAddress(SourceAddress), 
                    (m_pSerialSocket = 
                        Base()->GetCore()->GetSerial()->CreateSocket(LinuxSerial::Options(ISerial::Options::BaudRate(BaudRate)))),
                    HDLCOptions())));
            //
            // Physical
            //
            if (m_pSerialSocket->Open(SerialPort.c_str()) != SUCCESS)
            {
                PrintLine("Failed to initiate connect\n");
            }
            else
            {
                m_pSocket = m_pSerialSocket;
            }
        }  
        else if (toupper(RetVal[0]) == 'C')
        {
            if (m_pHDLC)
            {
                //
                // IDENTIFY
                //
                m_pHDLC->RegisterIdentifyConfirm(std::bind(&ClientApp::IdentifyResponse, 
                    this, std::placeholders::_1));
                m_pHDLC->IdentifyRequest(DLIdentifyRequestParameter(HDLCAddress()));
            }
            else
            {
                PrintLine("Physical Connection Not Established Yet!\n"); 
            }
            
        }
        else if (toupper(RetVal[0]) == 'D')
        {
            if (m_pSerialSocket && m_pHDLC && m_pSerialSocket->IsConnected())
            {
                int DestinationAddress = GetNumericInput("Destination Address (Default: 1)", 1);
                //
                // Datalink
                //
                m_pHDLC->ConnectRequest(DLConnectRequestOrIndication(HDLCAddress(DestinationAddress)));
            }
            else
            {
                PrintLine("Physical Connection Not Established Yet!\n");
            }
        }
        
        else if (toupper(RetVal[0]) == 'E')
        {
            int         SourceAddress = GetNumericInput("Client Address (Default: 1)", 1);
            std::string SerialPort = GetStringInput("Serial Port (Default: /tmp/ttyS11)", "/tmp/ttyS11");
            
            PrintLine("Initial Baud Rate Selection\n");
            PrintLine("\t6  - 9600\n");
            PrintLine("\t10 - 115200\n");
            int         BaudRate = GetNumericInput("(Default: 6 - 9600)", 6);
            
            m_pClientEngine = new LinuxClientEngine(COSEMClientEngine::Options(SourceAddress), 
                    new SerialWrapper(
                        (m_pSerialSocket = 
                            Base()->GetCore()->GetSerial()->CreateSocket(LinuxSerial::Options(ISerial::Options::BaudRate(BaudRate))))));
            if (m_pSerialSocket->Open(SerialPort.c_str()) != SUCCESS)
            {
                PrintLine("Failed to initiate connect\n"); 
            }
            m_pSocket = m_pSerialSocket;
            
        }            
        else if (RetVal == "1")
        {
            if (m_pSocket && m_pSocket->IsConnected() & m_pClientEngine->IsTransportConnected()) 
            {
                bool                 Send = true;
                int                  DestinationAddress = GetNumericInput("Server Address (Default: 1)", 1);
                COSEMSecurityOptions::SecurityLevel Security = (COSEMSecurityOptions::SecurityLevel)
                    GetNumericInput("Security Level [0 - None, 1 - Low, 2 - High] (Default: 0)", COSEMSecurityOptions::SECURITY_NONE);
                std::string          Password;
                COSEMSecurityOptions SecurityOptions; 
                //
                // Only supports LN at this time
                //
                SecurityOptions.ApplicationContextName = SecurityOptions.ContextLNRNoCipher;
                switch (Security)
                {
                case COSEMSecurityOptions::SECURITY_NONE: 
                    break;
                    
                case COSEMSecurityOptions::SECURITY_LOW_LEVEL:
                    SecurityOptions.MechanismName = SecurityOptions.MechanismNameLowLevelSecurity;
                    SecurityOptions.AuthenticationValue = GetStringInput("Password", "");
                    break;
                    
                case COSEMSecurityOptions::SECURITY_HIGH_LEVEL:
                default:
                    Send = false;
                    PrintLine("Security Level is Not Supported at This Time");
                    break;
                }
                if (Send)
                {
                    size_t APDUSize = GetNumericInput("APDU Size (Default: 640)", 640);
                    m_pClientEngine->Open(DestinationAddress,
                                          SecurityOptions, 
                                          xDLMS::InitiateRequest(APDUSize));
                }
            }
            else
            {
                PrintLine("Transport Connection Not Established Yet!\n");
            }
        }
        else if (RetVal == "2")
        {
            if (m_pSocket && m_pSocket->IsConnected() && m_pClientEngine->IsOpen())
            {
                Cosem_Attribute_Descriptor Descriptor;
                
                Descriptor.class_id = (ClassIDType) GetNumericInput("Class ID (Default: 1)", CLSID_IData);
                Descriptor.attribute_id = (ObjectAttributeIdType) GetNumericInput("Attribute (Default: 2)", 2);
                if (Descriptor.instance_id.Parse(GetStringInput("OBIS Code (Default: 0-0:96.1.0*255)", "0-0:96.1.0*255")))
                {
                    if (m_pClientEngine->Get(Descriptor,
                                             &m_GetToken))
                    {
                        PrintLine(std::string("\tGet Request Sent: Token ") + std::to_string(m_GetToken) + "\n");
                    }
                }
                else
                {
                    PrintLine("Malformed OBIS Code!\n");
                }
            }
            else
            {
                PrintLine("Not Connected!\n");
            }
            
        }
        else if (RetVal == "3")
        {
            if (m_pSocket && m_pSocket->IsConnected() && m_pClientEngine->IsOpen())
            {
                Cosem_Attribute_Descriptor Descriptor; 
                
                Descriptor.class_id = (ClassIDType) GetNumericInput("Class ID (Default: 1)", CLSID_IData);
                Descriptor.attribute_id = (ObjectAttributeIdType) GetNumericInput("Attribute (Default: 2)", 2);
                if (Descriptor.instance_id.Parse(GetStringInput("OBIS Code (Default: 0-0:96.1.0*255)", "0-0:96.1.0*255")))
                {
                    COSEMType MyData(COSEMDataType::VISIBLE_STRING, GetStringInput("Value (Default: LINUXDATA)", "LINUXDATA#"));
                    if (m_pClientEngine->Set(Descriptor,
                                             MyData,
                                             &m_SetToken))
                    {
                        PrintLine(std::string("\tSet Request Sent: Token ") + std::to_string(m_SetToken) + "\n");
                    }
                }
                else
                {
                    PrintLine("Malformed OBIS Code!\n");
                }
            }
            else
            {
                PrintLine("Not Connected!\n");
            }
            
        }    
        else if (RetVal == "4")
        {
            if (m_pSocket && m_pSocket->IsConnected() && m_pClientEngine->IsOpen())
            {
                Cosem_Method_Descriptor Descriptor;

                Descriptor.class_id = (ClassIDType) GetNumericInput("Class ID (Default: 8)", CLSID_IClock);
                Descriptor.method_id = (ObjectAttributeIdType) GetNumericInput("Method (Default: 1)", 1);
                if (Descriptor.instance_id.Parse(GetStringInput("OBIS Code (Default: 0-0:1.0.0*255)", "0-0:1.0.0*255")))
                {
                    COSEMType MyData(COSEMDataType::INTEGER, GetNumericInput("Parameter (Default: 1)", 1));
                    if (m_pClientEngine->Action(Descriptor,
                                            DLMSOptional<DLMSVector>(MyData),
                                            &m_ActionToken))
                    {
                        PrintLine(std::string("\tAction Request Sent: Token ") + std::to_string(m_ActionToken) + "\n");
                    }
                }
                else
                {
                    PrintLine("Malformed OBIS Code!\n");
                }
            }
            else
            {
                PrintLine("Not Connected!\n"); 
            }
            
        }         
        else if (RetVal == "5")
        {
            if (!m_pClientEngine->Release(xDLMS::InitiateRequest()))
            {
                PrintLine("Problem submitting COSEM Release!\n");
            }
        }
        else if (toupper(RetVal[0]) == 'T')
        {
            if (m_pSocket)
            {
                Base()->GetCore()->GetIP()->ReleaseSocket(m_pSocket);
                m_pSocket = nullptr;
                PrintLine("Socket released.\n");
            }
            else
            {
                PrintLine("TCP Not Opened!\n");
            }
        }
        else if (toupper(RetVal[0]) == 'U')
        {
            if (m_pHDLC && m_pHDLC->IsConnected())
            {
                m_pHDLC->RegisterDisconnectConfirm(std::bind(&ClientApp::DisconnectConfirm, 
                    this,
                    std::placeholders::_1));
                m_pHDLC->DisconnectRequest(DLDisconnectRequestOrIndication());
            }
            else
            {
                PrintLine("HDLC Not Connected!\n");
            }
        }
       
        m_Base.get_io_service().post(std::bind(&ClientApp::ClientMenu, this));
    }
    
    LinuxClientEngine *             m_pClientEngine = nullptr;
    ISocket *                       m_pSocket = nullptr;
    ISerialSocket *                 m_pSerialSocket = nullptr;
    HDLCClientLLC *                 m_pHDLC = nullptr;
    COSEMClientEngine::RequestToken m_GetToken;
    COSEMClientEngine::RequestToken m_SetToken;
    COSEMClientEngine::RequestToken m_ActionToken; 
   
};

class ServerApp : public AppBase
{
public:
    ServerApp(LinuxBaseLibrary& BL) : 
        AppBase(BL)
    {
        m_Base.get_io_service().post(std::bind(&ServerApp::ServerMenu, this));
    }

protected:
    void ServerMenu()
    {
        PrintLine("\nServer Menu:\n\n");
        PrintLine("\t0 - Exit Application\n");
        PrintLine("\t1 - TCP Server\n");
        PrintLine("\t2 - HDLC Server\n");
        PrintLine("\t3 - Serial Wrapper Server\n\n");
        PrintLine("Select: ");
        ReadLine(std::bind(&ServerApp::ServerMenu_Handler, this, std::placeholders::_1));
    }
    
    void ServerMenu_Handler(const std::string& RetVal)
    {
        if (RetVal == "0")
        {
            exit(0);
        }
        else if (RetVal == "1")
        {
            ISocket *   pSocket;
   
            PrintLine("\nTCP Server Mode - Listening on Port 4059\n");
            m_pServerEngine = new LinuxCOSEMServerEngine(COSEMServerEngine::Options(),
                new TCPWrapper((pSocket = Base()->GetCore()->GetIP()->CreateSocket(LinuxIP::Options()))));
            if (SUCCESSFUL != pSocket->Open())
            {
                PrintLine("Failed to initiate listen\n");
            }
        }
        else if (RetVal == "2")
        {
            ISerialSocket * pSocket;
            int             ServerAddress = GetNumericInput("Server Address (Default: 1)", 1);
            std::string     SerialPort = GetStringInput("Serial Port (Default: /tmp/ttyS10)", "/tmp/ttyS10");
            
            PrintLine("Initial Baud Rate Selection\n");
            PrintLine("\t6  - 9600\n");
            PrintLine("\t10 - 115200\n");
            int         BaudRate = GetNumericInput("(Default: 6 - 9600)", 6);
            
            PrintLine(std::string("\nHDLC Server Mode - Listening on ") + SerialPort + std::string("\n"));
            //
            // TODO - HDLCServerLLC ServerAddress should be able to handle multiple SAPs
            //
            m_pServerEngine = new LinuxCOSEMServerEngine(COSEMServerEngine::Options(),
                new HDLCServerLLC(HDLCAddress(ServerAddress), 
                    (pSocket = Base()->GetCore()->GetSerial()->CreateSocket(LinuxSerial::Options(ISerial::Options::BaudRate(BaudRate)))), 
                HDLCOptions())); 
            
            if (pSocket->Open(SerialPort.c_str()) != SUCCESS)
            {
                PrintLine("Failed to initiate connect\n");
            }
        } 
        else if (RetVal == "3")
        {
            ISerialSocket * pSocket;
            std::string     SerialPort = GetStringInput("Serial Port (Default: /tmp/ttyS10)", "/tmp/ttyS10");
            
            PrintLine("Initial Baud Rate Selection\n");
            PrintLine("\t6  - 9600\n");
            PrintLine("\t10 - 115200\n");
            int         BaudRate = GetNumericInput("(Default: 6 - 9600)", 6);
            
            PrintLine(std::string("\nSerial Wrapper Server Mode - Listening on ") + SerialPort + std::string("\n"));
            m_pServerEngine = new LinuxCOSEMServerEngine(COSEMServerEngine::Options(),
                new SerialWrapper((pSocket = Base()->GetCore()->GetSerial()->CreateSocket(LinuxSerial::Options(ISerial::Options::BaudRate(BaudRate))))));
            
            if (pSocket->Open(SerialPort.c_str()) != SUCCESS)
            {
                PrintLine("Failed to initiate connect\n");
            }
        }
        
        m_Base.get_io_service().post(std::bind(&ServerApp::ServerMenu, this));
    }
    
    LinuxCOSEMServerEngine * m_pServerEngine = nullptr;
   
};

int main(int argc, char *argv[])
{
    int                  opt;
    bool                 Server = false;
    LinuxBaseLibrary     bl;
    
    while ((opt =:: getopt(argc, argv, "S")) != -1)
    {
        switch (opt)
        {
        case 'S':
            Server = true;
            break;
        default:
            break;
        }
    }
    
    std::cout << "EPRI DLMS/COSEM " << (Server ? "Server" : "Client") << "Test Harness\n";

    if (Server)
    {
        ServerApp App(bl);
        App.Run();
       
    }
    else
    {
        ClientApp App(bl);
        App.Run();
    }

}
