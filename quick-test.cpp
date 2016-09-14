
#include <iostream>
#include <dlfcn.h>

using namespace std;

int main(int, char **)
{
    string example = "./example.so";
    
    void *lib = dlopen(example.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!lib) {
	cerr << "failed to open " + example + ": " << dlerror() << endl;
	return 1;
    }

    typedef const char *(*RequestFn)(const char *);
    RequestFn reqFn = (RequestFn)dlsym(lib, "vampipeRequestJson");
    if (!reqFn) {
	cerr << "failed to find request function in " +
	    example + ": " << dlerror() << endl;
	return 1;
    }
    
    typedef void (*FreeFn)(const char *);
    FreeFn freeFn = (FreeFn)dlsym(lib, "vampipeFreeJson");
    if (!freeFn) {
	cerr << "failed to find free function in " +
	    example + ": " << dlerror() << endl;
	return 1;
    }

    string listRequest = "{\"type\": \"list\"}";
    const char *listResponse = reqFn(listRequest.c_str());
    cout << listResponse << endl;
    freeFn(listResponse);

    string loadRequest = "{\"type\":\"load\",\"content\": {\"pluginKey\":\"vamp-example-plugins:powerspectrum\",\"inputSampleRate\":44100,\"adapterFlags\":[\"AdaptAllSafe\"]}}";
    const char *loadResponse = reqFn(loadRequest.c_str());
    cout << loadResponse << endl;
    freeFn(loadResponse);

    string configRequest = "{\"type\":\"configure\",\"content\":{\"pluginHandle\":1,\"configuration\":{\"blockSize\":8,\"channelCount\":1,\"stepSize\":8}}}";
    const char *configResponse = reqFn(configRequest.c_str());
    cout << configResponse << endl;
    freeFn(configResponse);

    string processRequest = "{\"type\":\"process\",\"content\":{\"pluginHandle\":1,\"processInput\":{\"timestamp\":{\"s\":0,\"n\":0},\"inputBuffers\":[{\"values\":[0,1,0,-1,0,1,0,-1]}]}}}";
    const char *processResponse = reqFn(processRequest.c_str());
    cout << processResponse << endl;
    freeFn(processResponse);

    string b64processRequest = "{\"type\":\"process\",\"content\":{\"pluginHandle\":1,\"processInput\":{\"timestamp\":{\"s\":0,\"n\":0},\"inputBuffers\":[{\"b64values\":\"AAAAAAAAgD8AAAAAAACAvwAAAAAAAIA/AAAAAAAAgL8\"}]}}}";
    const char *b64processResponse = reqFn(b64processRequest.c_str());
    cout << b64processResponse << endl;
    freeFn(b64processResponse);
    
    string finishRequest = "{\"type\":\"finish\",\"content\":{\"pluginHandle\":1}}";
    const char *finishResponse = reqFn(finishRequest.c_str());
    cout << finishResponse << endl;
    freeFn(finishResponse);
}
