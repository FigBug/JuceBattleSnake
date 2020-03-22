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
        
        return rd->callback (r);
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
