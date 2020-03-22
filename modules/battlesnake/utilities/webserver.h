#pragma once

class Webserver
{
public:
    struct Options
    {
        File docRoot     = { File::getCurrentWorkingDirectory () };
        Array<int> ports = { 80 };
    };
    
    struct Request
    {
        // Info
        String method;
        String uri;
        String body;
        
        StringPairArray headers;
        
        // Response
        String contentType;
        String response;
    };
    
    Webserver (const Options& o);
    ~Webserver();
    
    void addHandler (const String& uri, std::function<int (Request&)> handler);
    
private:
    struct Impl;
    struct RequestData;
    std::unique_ptr<Impl> impl;
};
