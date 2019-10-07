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

#pragma once

#include "COSEM.h"
#include "COSEMDevice.h"
#include "interfaces/IData.h"

namespace EPRI
{
    const ClassIDType CLSID_Disconnect = 70;

    class Disconnect : public ICOSEMInterface
    {
        COSEM_DEFINE_SCHEMA(Control_State_Schema)
        COSEM_DEFINE_SCHEMA(Control_Mode_Schema)

    public :
        Disconnect();
        virtual ~Disconnect() = default;

        enum Attributes : ObjectAttributeIdType
        {
            ATTR_OUTPUT_STATE = 2,
            ATTR_CONTROL_STATE = 3,
            ATTR_CONTROL_MODE = 4,
        };

        enum ControlState : uint8_t
        {
            DISCONNECTED = 0,
            CONNECTED = 1,
            READY_FOR_RECONNECTION = 2,
        };

        enum ControlMode : uint8_t
        {
            ATTR_CONTROL_MODE_0 = 0,
            ATTR_CONTROL_MODE_1 = 1,
            ATTR_CONTROL_MODE_2 = 2,
            ATTR_CONTROL_MODE_3 = 3,
            ATTR_CONTROL_MODE_4 = 4,
            ATTR_CONTROL_MODE_5 = 5,
            ATTR_CONTROL_MODE_6 = 6
        };

        COSEMAttribute<ATTR_OUTPUT_STATE, BooleanSchema, 0x08> output_state;
        COSEMAttribute<ATTR_OUTPUT_STATE, Control_State_Schema, 0x10> control_state;
        COSEMAttribute<ATTR_OUTPUT_STATE, Control_Mode_Schema, 0x18> control_mode;

        enum Methods : ObjectAttributeIdType
        {
            METHOD_REMOTE_DISCONNECT = 1,
            METHOD_REMOTE_RECONNECT = 2
        };
        COSEMMethod<METHOD_REMOTE_DISCONNECT, IntegerSchema, 0x20>             remote_disconnect;
        COSEMMethod<METHOD_REMOTE_RECONNECT, IntegerSchema, 0x28>             remote_reconnect;
    };


// TODO: add object derived from Disconnect and ICOSEMObject as with IClock.h, line 142
    class IDisconnect : public Disconnect, public ICOSEMObject 
    {
    public:
        IDisconnect() = delete;
        IDisconnect(const COSEMObjectInstanceCriteria& OIDCriteria, 
                uint16_t ShortNameBase = std::numeric_limits<uint16_t>::max());
        virtual ~IDisconnect() = default;
    protected:
        virtual APDUConstants::Action_Result InternalAction(const AssociationContext& Context,
            ICOSEMMethod * pMethod, 
            const Cosem_Method_Descriptor& Descriptor, 
            const DLMSOptional<DLMSVector>& Parameters,
            DLMSVector * pReturnValue = nullptr) = 0;
    };

    class LinuxDisconnect : public IDisconnect
    {
    public:
        LinuxDisconnect();
  
    protected:
        virtual APDUConstants::Data_Access_Result InternalGet(const AssociationContext& Context,
            ICOSEMAttribute * pAttribute, 
            const Cosem_Attribute_Descriptor& Descriptor, 
            SelectiveAccess * pSelectiveAccess) final;
        virtual APDUConstants::Data_Access_Result InternalSet(const AssociationContext& Context,
            ICOSEMAttribute * pAttribute, 
            const Cosem_Attribute_Descriptor& Descriptor, 
            const DLMSVector& Data,
            SelectiveAccess * pSelectiveAccess) final;
        virtual APDUConstants::Action_Result InternalAction(const AssociationContext& Context,
            ICOSEMMethod * pMethod, 
            const Cosem_Method_Descriptor& Descriptor, 
            const DLMSOptional<DLMSVector>& Parameters,
            DLMSVector * pReturnValue = nullptr) final;
        
        std::string m_Values[10];
        
    };
}