//==============================================================================
struct ServerInit
{
    ServerInit()
    {
        mg_init_library (MG_FEATURES_DEFAULT);
    }
    
    ~ServerInit()
    {
        mg_exit_library();
    }
};

//==============================================================================
struct Webserver::RequestData
{
    Webserver* self;
    std::function<int (Request&)> callback;
};

//==============================================================================
struct Webserver::Impl
{
    static int requestHandler (struct mg_connection* conn, void* cbdata)
    {
        auto rd = (RequestData*)cbdata;
        auto ri = mg_get_request_info (conn);
        
        Request r;
        r.method    = ri->request_method;
        r.uri       = ri->local_uri;
        
        for (int i = 0; i < ri->num_headers; i++)
            r.headers.set (ri->http_headers[i].name, ri->http_headers[i].value);
        
        int sz;
        char buffer[1024];
        while ((sz = mg_read (conn, buffer, sizeof (buffer))) > 0)
            r.body += String::fromUTF8 (buffer, sz);
        
        r.contentType = "text/plain";
        
        int ret = rd->callback (r);
        
        mg_printf (conn,
                   "HTTP/1.1 200 OK\r\n"
                   "Content-Length: %d\r\n"
                   "Content-Type: %s\r\n"
                   "Connection: close\r\n\r\n",
                   int (r.response.getNumBytesAsUTF8()),
                   r.contentType.toRawUTF8());
        
        mg_write (conn, r.response.toRawUTF8(), r.response.getNumBytesAsUTF8());
        
        return ret;
    }
    
    SharedResourcePointer<ServerInit> serverInit;
    OwnedArray<RequestData> requestData;
    mg_context* context = nullptr;
};

//==============================================================================
Webserver::Webserver (const Options& o)
{
    impl = std::make_unique<Impl> ();
 
    auto root = o.docRoot.getFullPathName();
    StringArray ports;
    for (auto p : o.ports)
        ports.add (String (p));
    auto portString = ports.joinIntoString (",");
    
    const char* config[] =
    {
        "document_root",    root.toRawUTF8(),
        "listening_ports",  portString.toRawUTF8(),
        nullptr
    };
    
    mg_callbacks callbacks;
    memset (&callbacks, 0, sizeof (callbacks));
    
    impl->context = mg_start (&callbacks, this, config);
    jassert (impl->context != nullptr);
}

Webserver::~Webserver()
{
    if (impl->context)
        mg_stop (impl->context);
}


void Webserver::addHandler (const String& uri, std::function<int (Request& r)> handler)
{
    auto rd = new RequestData();
    rd->self     = this;
    rd->callback = handler;
    
    impl->requestData.add (rd);
    
    mg_set_request_handler (impl->context, uri.toRawUTF8(), Impl::requestHandler, rd);
}
