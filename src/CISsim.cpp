#include "EndDeviceControls_USCOREBinding.nsmap"        // XML namespace mapping table (only needed once at the global level)
#include "soapEndDeviceControls_USCOREBindingProxy.h"
#include "soapEndDeviceControls_USCOREBindingService.h"
#include "UUID.h"
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>

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

void run_client(std::string serverUrl, bool reconnect) {
    EndDeviceControls_USCOREBindingProxy client(SOAP_XML_INDENT);
    std::string endpointURL{serverUrl};
    
    std::vector<std::string> meters{"M1001", "M1002"};
    ns2__EndDeviceControlsResponseMessageType response{};
    if (client.CreateEndDeviceControls(endpointURL.c_str(), endpointURL.c_str(), 
            connectCreateRequest(reconnect, client.soap, meters), 
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

static const std::vector<std::string> meters{"M1001", "M1002"};

void run_server(int port) {
    std::cout << "Starting Head End Server on port " << port << '\n';
    EndDeviceControls_USCOREBindingService hes(SOAP_XML_INDENT);
    hes.soap->send_timeout = 5; // send timeout is 5s
    hes.soap->recv_timeout = 5; // receive timeout is 5s
    if (hes.run(port) != SOAP_OK && hes.soap->error != SOAP_TCP_ERROR) {
        hes.soap_stream_fault(std::cerr);
    }
    hes.destroy(); 
    std::cout << "Shut down Head End Server on port " << port << '\n';
}

/// Web service operation 'CreateEndDeviceControls' implementation, should return SOAP_OK or error code
int EndDeviceControls_USCOREBindingService::CreateEndDeviceControls(ns2__EndDeviceControlsRequestMessageType *req, ns2__EndDeviceControlsResponseMessageType &resp) {
    static std::string disconnectRef{"3.31.0.23"};
    static std::string reconnectRef{"3.31.0.18"};
    // remember the ReplyAddress
    if (req->Header->Verb == _ns3__HeaderType_Verb::create) {
        std::cout << "We got a create message\n";
    }
    auto replyTo{req->Header->ReplyAddress};
    std::cout << "Reply to: " << *replyTo << '\n';
    
    // make sure the devices exist
    // if they do, queue up the outbound messages
    // and send the response
    resp.Header = soap_new_req_ns3__HeaderType(soap,
        _ns3__HeaderType_Verb::reply,   // Verb
        "EndDeviceControls",  // Noun
        std::chrono::system_clock::now(),  // Timestamp
        UUID::random(),   // MessageID
        req->Header->CorrelationID  // CorrelationID
    );
    resp.Reply = soap_new_req_ns3__ReplyType(soap,
        _ns3__ReplyType_Result::OK);
    /* If everything is OK, just send 0.0 */
    resp.Reply->Error.emplace_back(soap_new_req_ns3__ErrorType(soap, "0.0"));
    /* Otherwise, if there are meters specified that do not exist, return 0.1 
     * and send an error for each meter that does not exist:
     * <Error>
     *  <code>2.4</code>
     *  <level>FATAL</level>
     *  <reason>no such meter</reason>
     *  <ID kind="name" objectType="Meter">X002</ID>
     * </Error>
     */
    return SOAP_OK;
}

//
/// Web service operation 'ChangeEndDeviceControls' implementation, should return SOAP_OK or error code
int EndDeviceControls_USCOREBindingService::ChangeEndDeviceControls(ns2__EndDeviceControlsRequestMessageType *req, ns2__EndDeviceControlsResponseMessageType &resp) {
    if (req->Header->Verb == _ns3__HeaderType_Verb::change) {
        std::cout << "We got a change message\n";
    }
    resp.Header = soap_new_req_ns3__HeaderType(soap,
        _ns3__HeaderType_Verb::reply,   // Verb
        "EndDeviceControls",  // Noun
        std::chrono::system_clock::now(),  // Timestamp
        UUID::random(),   // MessageID
        req->Header->CorrelationID  // CorrelationID
    );
    resp.Reply = soap_new_req_ns3__ReplyType(soap,
        _ns3__ReplyType_Result::OK);
    
    return SOAP_OK;
}

//
/// Web service operation 'CancelEndDeviceControls' implementation, should return SOAP_OK or error code
int EndDeviceControls_USCOREBindingService::CancelEndDeviceControls(ns2__EndDeviceControlsRequestMessageType *req, ns2__EndDeviceControlsResponseMessageType &resp) {
    if (req->Header->Verb == _ns3__HeaderType_Verb::cancel) {
        std::cout << "We got a cancel message\n";
    }
    resp.Header = soap_new_req_ns3__HeaderType(soap,
        _ns3__HeaderType_Verb::reply,   // Verb
        "EndDeviceControls",  // Noun
        std::chrono::system_clock::now(),  // Timestamp
        UUID::random(),   // MessageID
        req->Header->CorrelationID  // CorrelationID
    );
    resp.Reply = soap_new_req_ns3__ReplyType(soap,
        _ns3__ReplyType_Result::OK);
    
    return SOAP_OK;
}

//
/// Web service operation 'CloseEndDeviceControls' implementation, should return SOAP_OK or error code
int EndDeviceControls_USCOREBindingService::CloseEndDeviceControls(ns2__EndDeviceControlsRequestMessageType *req, ns2__EndDeviceControlsResponseMessageType &resp) {
    if (req->Header->Verb == _ns3__HeaderType_Verb::close) {
        std::cout << "We got a close message\n";
    }
    resp.Header = soap_new_req_ns3__HeaderType(soap,
        _ns3__HeaderType_Verb::reply,   // Verb
        "EndDeviceControls",  // Noun
        std::chrono::system_clock::now(),  // Timestamp
        UUID::random(),   // MessageID
        req->Header->CorrelationID  // CorrelationID
    );
    resp.Reply = soap_new_req_ns3__ReplyType(soap,
        _ns3__ReplyType_Result::OK);
    
    return SOAP_OK;
}

//
/// Web service operation 'DeleteEndDeviceControls' implementation, should return SOAP_OK or error code
int EndDeviceControls_USCOREBindingService::DeleteEndDeviceControls(ns2__EndDeviceControlsRequestMessageType *req, ns2__EndDeviceControlsResponseMessageType &resp) {
    if (req->Header->Verb == _ns3__HeaderType_Verb::delete_) {
        std::cout << "We got a delete message\n";
    }
    resp.Header = soap_new_req_ns3__HeaderType(soap,
        _ns3__HeaderType_Verb::reply,   // Verb
        "EndDeviceControls",  // Noun
        std::chrono::system_clock::now(),  // Timestamp
        UUID::random(),   // MessageID
        req->Header->CorrelationID  // CorrelationID
    );
    resp.Reply = soap_new_req_ns3__ReplyType(soap,
        _ns3__ReplyType_Result::OK);
    
    return SOAP_OK;
}


void timer_start(std::function<void(void)> func, unsigned int seconds) {
    std::thread([func, seconds] { 
        while (true) { 
            auto x = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
            func();
            std::this_thread::sleep_until(x);
        }
    }).detach();
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " serverUrl\n";
        return 1;
    }
    timer_start(std::bind(run_client, std::string{argv[1]}, false), 10);
    // give the other thread a two second head start
    std::this_thread::sleep_for(std::chrono::seconds(2));
    timer_start(std::bind(run_client, std::string{argv[1]}, true), 10);
    while (true)
        ;
}
