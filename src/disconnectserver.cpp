#include "EndDeviceControls_USCOREBinding.nsmap"        // XML namespace mapping table (only needed once at the global level)
#include "soapEndDeviceControls_USCOREBindingService.h"
#include "UUID.h"
#include <cstdlib>
#include <iostream>

static const std::vector<std::string> meters{"M1001", "M1002"};

int main(int argc, char *argv[]) {
    int port;
    if (argc > 1) {
        port = std::atoi(argv[1]);
    }
    if (port <= 0) {
        port = 8080;
    }
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

