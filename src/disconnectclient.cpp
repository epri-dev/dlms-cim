#include "EndDeviceControls_USCOREBinding.nsmap"        // XML namespace mapping table (only needed once at the global level)
#include "soapEndDeviceControls_USCOREBindingProxy.h"
#include "UUID.h"
#include <string>

ns2__EndDeviceControlsRequestMessageType* connectCreateRequest(bool reconnect, struct soap* soap, const std::vector<std::string>& meters) {
    static std::string disconnectRef{"3.31.0.23"};
    static std::string reconnectRef{"3.31.0.18"};
    std::vector<ns4__EndDeviceControls *> controls{soap_new_req_ns4__EndDeviceControls(soap)};
    auto &vedc = controls.front()->EndDeviceControl;
    vedc.push_back(soap_new_ns4__EndDeviceControl(soap));
    vedc.back()->EndDeviceControlType.ref = reconnect ? &reconnectRef : &disconnectRef;
    auto dev{soap_new_req_ns4__EndDevice(soap)};
    for (const auto m : meters) {
        dev->Names.push_back(soap_new_req_ns4__Name(soap, m));
    }
    vedc.back()->EndDevices.emplace_back(dev);

    auto request = soap_new_req_ns2__EndDeviceControlsRequestMessageType(soap, 
        soap_new_req_ns3__HeaderType(soap,
            _ns3__HeaderType_Verb::create,   // Verb
            "EndDeviceControls",  // Noun
            std::chrono::system_clock::now(),  // Timestamp
            UUID::random(),   // MessageID
            UUID::random()    // CorrelationID
        )
    );
    request->Payload = soap_new_req_ns2__EndDeviceControlsPayloadType(soap, controls);
    request->Header->ReplyAddress = soap_new_std__string(soap);
    request->Header->ReplyAddress->append("http://localhost:8888/reply/");
    return request;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " serverUrl\n";
        return 0;
    }
    EndDeviceControls_USCOREBindingProxy client(SOAP_XML_INDENT);
    std::string endpointURL{argv[1]};
    
    std::vector<std::string> meters{"M1001", "M1002"};
    ns2__EndDeviceControlsResponseMessageType response{};
    if (client.CreateEndDeviceControls(endpointURL.c_str(), endpointURL.c_str(), 
            connectCreateRequest(false, client.soap, meters), 
            response
    ) == SOAP_OK) {
        std::cout << "Result = ";
        switch(response.Reply->Result) {
            case _ns3__ReplyType_Result::OK:
                std::cout << "OK";
                break;
            case _ns3__ReplyType_Result::PARTIAL:
                std::cout << "PARTIAL";
                break;
            case _ns3__ReplyType_Result::FAILED:
                std::cout << "FAILED";
                break;
            default:
                std::cout << "unknown result code";
                break;
        }
        std::cout << '\n';
    } else {
        client.soap_stream_fault(std::cerr);
    }
    client.destroy();
}
